//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// AST produced by parsing C++, note that the tree defined
// here is unconstrained, eg a StringLiteral node could contain
// a ClassDef node (which is obviously not valid C++). Constraints
// are imposed by the parser (see parser.hh)
//
#ifndef HCP_AST_HH
#define HCP_AST_HH

#include <xju/parse.hh>
#include <memory>
#include <string>
#include <vector>
#include "xju/assert.hh"
#include <typeinfo>
#include <iostream>
#include <functional>

namespace hcp_ast
{
typedef xju::parse::IteratorAdaptor<std::string::const_iterator> I;


class Item
{
public:
  //pre: items.size()
  explicit Item(std::vector<std::shared_ptr<Item> > const& children) throw():
      items_(children),
      begin_(items_.front()->begin()),
      end_(items_.back()->end())
  {
    xju::assert_not_equal(items_.size(),0U);
  }
  explicit Item(I begin, I end) throw():
      begin_(begin),
      end_(end)
  {
  }
  virtual ~Item() throw() {
  }
  // Item::
  virtual I begin() const throw() {
    return items_.size()?items_.front()->begin():begin_;
  }
  
  // Item::
  virtual I end() const throw() {
    return items_.size()?items_.back()->end():end_;
  }

  std::vector<std::shared_ptr<Item> > const& items() const noexcept {
    return items_;
  }
  
  template<class T>
  bool isA() const throw() {
    CheckNotItem<T>::check;
    return dynamic_cast<T const*>(this);
  }
  template<class T>
  T const& asA() const throw() {
    CheckNotItem<T>::check;
    T const* result=dynamic_cast<T const*>(this);
    if (!result) {
      xju::assert_equal(typeid(*this).name(), typeid(T).name());
    }
    return *result;
  }
  template<class T>
  T& asA() throw() {
    T* result=dynamic_cast<T*>(this);
    if (!result) {
      xju::assert_equal(typeid(*this).name(), typeid(T).name());
    }
    return *result;
  }

  virtual std::string str() const throw();

private:
  // The items that make up this composite item, ie the
  // children of this AST node.
  std::vector<std::shared_ptr<Item> > items_;
  
  
  I begin_;
  I end_;

  template<class T>
  class CheckNotItem
  {
  public:
    static void check();
  };
    
  friend std::ostream& operator<<(std::ostream& s, Item const& x) throw()
  {
    return s << x.str();
  }
};

template<>
class Item::CheckNotItem<Item>
{
private:
  static void check(); //rejected cast to Item
};

inline std::string reconstruct(Item const& x) throw() {
  return std::string(x.begin().x_, x.end().x_);
}

typedef std::shared_ptr<Item> IR;
inline std::string reconstruct(IR const& x) throw() {
  return reconstruct(*x);
}

typedef std::vector<IR> IRs;

template<class T>
bool isA_(IR const& x) throw()
{
  return (*x).isA<T>();
}

template<class T>
T& asA_(IR const& x) throw()
{
  return (*x).asA<T>();
}

template<class T, class ... Us>
struct IsAnyOf
{
  IsAnyOf(IR const& x) throw():
      val_(isA_<T>(x)||IsAnyOf<Us...>(x)){
  }
  operator bool() const throw(){
    return val_;
  }
  bool const val_;
};
template<class T>
struct IsAnyOf<T>
{
  IsAnyOf(IR const& x) throw():
      val_(isA_<T>(x)) {
  }
  operator bool() const throw(){
    return val_;
  }
  bool const val_;
};

std::string reconstruct(IRs const& x) throw();

// pre: begin() <= i < end()
std::vector<Item const*> getContextAt(
  I i, Item const& within) throw();
  

// A convenient way to create Item subclasses
// (see below for subclasses that the parser creates).
template<class Tag>
class TaggedItem : public Item
{
public:
  using Item::Item;
  virtual ~TaggedItem() throw() {
  }

  virtual std::string str() const throw()
  {
    return typeid(Tag).name() + std::string(" ") + Item::str();
  }
};

// recursive search; does not search children that are of type T
// i.e. returns first returnAtMost x where:
//   x is a T
//   parent is an ancestor of x
//   lineage from x to parent has no Ts
//
template<class T>
std::vector<std::reference_wrapper<T const> > findChildrenOfType(
  Item const& parent,
  size_t const returnAtMost=SIZE_MAX)
    throw()
{
  std::vector<std::reference_wrapper<T const> > result;
  for(auto const ir: parent.items()) {
    if (ir->isA<T>()) {
      result.push_back(ir->asA<T>());
      if (result.size()==returnAtMost){
        return result;
      }
    }
    else {
      std::vector<std::reference_wrapper<T const> > descendents(
        findChildrenOfType<T>(*ir,
                              returnAtMost-result.size()));
      std::copy(descendents.begin(),descendents.end(),
                std::back_inserter(result));
      if (result.size()==returnAtMost) {
        return result;
      }
    }
  }
  return result;
}

//pre: parent has only one child of type T that has no ancestors of type T
//     up to parent
template<class T>
T const& findOnlyChildOfType(Item const& parent) throw()
{
  auto const result(findChildrenOfType<T>(parent,2));
  xju::assert_equal(result.size(),1U);
  return result[0];
}
    
template<class T,class F>
std::vector<std::reference_wrapper<T const> > filter(
  std::vector<std::reference_wrapper<T const> > const&  items,
  F f) throw()
{
  std::vector<std::reference_wrapper<T const> > result;
  for(auto x: items) {
    if (f(x)){
      result.push_back(x);
    }
  }
  return result;
}

class WhiteSpaceTag{};
class LineCommentTag{};
class BlockCommentTag{};
class CommentsTag{};
class StringLiteralTag{};
class S_CharsTag{};
class HashIncludeTag{};

typedef TaggedItem<WhiteSpaceTag> WhiteSpace;
typedef TaggedItem<LineCommentTag> LineComment;
typedef TaggedItem<BlockCommentTag> BlockComment;
typedef TaggedItem<CommentsTag> Comments;
typedef TaggedItem<StringLiteralTag> StringLiteral;
typedef TaggedItem<S_CharsTag> S_Chars;
typedef TaggedItem<HashIncludeTag> HashInclude;

class HashIncludeImplTag{};
typedef TaggedItem<HashIncludeImplTag> HashIncludeImpl;

class TargetOfHashIncludeTag{};
typedef TaggedItem<TargetOfHashIncludeTag> TargetOfHashInclude;
  
class OtherPreprocessorTag{};
typedef TaggedItem<OtherPreprocessorTag> OtherPreprocessor;

class WhitespaceTag{};
typedef TaggedItem<WhitespaceTag> Whitespace;

class TypedefTag{};
typedef TaggedItem<TypedefTag> Typedef;

class DefinedTypeTag{};
typedef TaggedItem<DefinedTypeTag> DefinedType;

class UsingTag{};
typedef TaggedItem<UsingTag> Using;

class EnumDefTag{};
typedef TaggedItem<EnumDefTag> EnumDef;

class EnumNameTag{};
typedef TaggedItem<EnumNameTag> EnumName;

class FunctionNameTag{};
typedef TaggedItem<FunctionNameTag> FunctionName;

class FunctionDeclTag{};
typedef TaggedItem<FunctionDeclTag> FunctionDecl;

class BlockOpenTag{};
typedef TaggedItem<BlockOpenTag> BlockOpen;

class BlockTag{};
typedef TaggedItem<BlockTag> Block;

class InitListTag{};
typedef TaggedItem<InitListTag> InitList;

class InitListOpenTag{};
typedef TaggedItem<InitListOpenTag> InitListOpen;

class FunctionDefTag{};
typedef TaggedItem<FunctionDefTag> FunctionDef;

class FunctionImplTag{};
typedef TaggedItem<FunctionImplTag> FunctionImpl;

class TemplateFunctionDefTag{};
typedef TaggedItem<TemplateFunctionDefTag> TemplateFunctionDef;

class TemplatePreambleTag{};
typedef TaggedItem<TemplatePreambleTag> TemplatePreamble;
  
class TemplateEmptyPreambleTag{};
typedef TaggedItem<TemplateEmptyPreambleTag> TemplateEmptyPreamble;
  
class ClassNameTag{};
typedef TaggedItem<ClassNameTag> ClassName;

class ClassDef : public Item
{
public:
  std::string const className_;
  bool const isTemplateSpecialisation_;
  
  explicit ClassDef(std::vector<IR> const& items) throw():
    Item(items),
    className_(getClassName(items)),
    isTemplateSpecialisation_(getIsTemplateSpeicialisation(items))
  {
  }
  virtual ~ClassDef() throw() {
  }
  static std::string getClassName(std::vector<IR> const& items) throw();
  static bool getIsTemplateSpeicialisation(std::vector<IR> const& items) throw();
  virtual std::string str() const throw()
  {
    return typeid(ClassDef).name() + std::string(" ") + Item::str();
  }
};

class BaseSpecifierTag{};
typedef TaggedItem<BaseSpecifierTag> BaseSpecifier;

class ClassMembersTag{};
typedef TaggedItem<ClassMembersTag> ClassMembers;

class AttrDeclTag{};
typedef TaggedItem<AttrDeclTag> AttrDecl;

class VarNameTag{};
typedef TaggedItem<VarNameTag> VarName;

class KeywordStaticTag{};
typedef TaggedItem<KeywordStaticTag> KeywordStatic;

class KeywordExternTag{};
typedef TaggedItem<KeywordExternTag> KeywordExtern;

class KeywordFriendTag{};
typedef TaggedItem<KeywordFriendTag> KeywordFriend;

class KeywordPublicTag{};
typedef TaggedItem<KeywordPublicTag> KeywordPublic;

class KeywordPrivateTag{};
typedef TaggedItem<KeywordPrivateTag> KeywordPrivate;

class KeywordProtectedTag{};
typedef TaggedItem<KeywordProtectedTag> KeywordProtected;

class KeywordVirtualTag{};
typedef TaggedItem<KeywordVirtualTag> KeywordVirtual;

class KeywordExplicitTag{};
typedef TaggedItem<KeywordExplicitTag> KeywordExplicit;

class KeywordOverrideTag{};
typedef TaggedItem<KeywordOverrideTag> KeywordOverride;

class KeywordFinalTag{};
typedef TaggedItem<KeywordFinalTag> KeywordFinal;

class KeywordNoexceptTag{};
typedef TaggedItem<KeywordNoexceptTag> KeywordNoexcept;

class KeywordThrowTag{};
typedef TaggedItem<KeywordThrowTag> KeywordThrow;

class KeywordTryTag{};
typedef TaggedItem<KeywordTryTag> KeywordTry;

class KeywordCatchTag{};
typedef TaggedItem<KeywordCatchTag> KeywordCatch;

class StaticVarDefTag{};
typedef TaggedItem<StaticVarDefTag> StaticVarDef;

class GlobalVarDefTag{};
typedef TaggedItem<GlobalVarDefTag> GlobalVarDef;

class ExternVarDefTag{};
typedef TaggedItem<ExternVarDefTag> ExternVarDef;

// marks the '=' in var initialiser
class VarInitialiserOpenTag{};
typedef TaggedItem<VarInitialiserOpenTag> VarInitialiserOpen;
  
class VarInitialiserTag{};
typedef TaggedItem<VarInitialiserTag> VarInitialiser;

class AccessModifierTag{};
typedef TaggedItem<AccessModifierTag> AccessModifier;

class TemplateClassDefTag{};
typedef TaggedItem<TemplateClassDefTag> TemplateClassDef;

class ClassForwardDeclTag{};
typedef TaggedItem<ClassForwardDeclTag> ClassForwardDecl;

class AnonymousNamespaceTag{};
typedef TaggedItem<AnonymousNamespaceTag> AnonymousNamespace;
class AnonymousNamespaceOpenTag{};
typedef TaggedItem<AnonymousNamespaceOpenTag> AnonymousNamespaceOpen;

class NamespaceDef : public Item
{
public:
  std::string const namespaceName_;
  
  explicit NamespaceDef(std::vector<IR> const& items) throw():
    Item(items),
    namespaceName_(getNamespaceName(items))
  {
  }
  virtual ~NamespaceDef() throw() {
  }
  static std::string getNamespaceName(std::vector<IR> const& items) throw();

  virtual std::string str() const throw()
  {
    return typeid(NamespaceDef).name() + std::string(" ") + Item::str();
  }
};

class NamespaceNameTag{};
typedef TaggedItem<NamespaceNameTag> NamespaceName;

class NamespaceMembersTag{};
typedef TaggedItem<NamespaceMembersTag> NamespaceMembers;

class EatWhiteTag{};
typedef TaggedItem<EatWhiteTag> EatWhite;

class ReturnTypeTag{};
typedef TaggedItem<ReturnTypeTag> ReturnType;

class OperatorNameTag{};
typedef TaggedItem<OperatorNameTag> OperatorName;

class ParamTag{};
typedef TaggedItem<ParamTag> Param;

class VarNonFpTag{};
typedef TaggedItem<VarNonFpTag> VarNonFp;

class ArrayDeclTag{};
typedef TaggedItem<ArrayDeclTag> ArrayDecl;

class FunctionQualifiersTag{};
typedef TaggedItem<FunctionQualifiersTag> FunctionQualifiers;

class TypeRefTag{};
typedef TaggedItem<TypeRefTag> TypeRef;

class ScopedNameTag{};
typedef TaggedItem<ScopedNameTag> ScopedName;

class ConstQualTag{};
typedef TaggedItem<ConstQualTag> ConstQual;

class VolatileQualTag{};
typedef TaggedItem<VolatileQualTag> VolatileQual;

class MoveQualTag{};
typedef TaggedItem<MoveQualTag> MoveQual;

class RefQualTag{};
typedef TaggedItem<RefQualTag> RefQual;

class PointerQualTag{};
typedef TaggedItem<PointerQualTag> PointerQual;

class ElipsesQualTag{};
typedef TaggedItem<ElipsesQualTag> ElipsesQual;

class FunctionPostQualifiersTag{};
typedef TaggedItem<FunctionPostQualifiersTag> FunctionPostQualifiers;

class VirtSpecifierSeqTag{};
typedef TaggedItem<VirtSpecifierSeqTag> VirtSpecifierSeq;

class EndOfFileTag{};
typedef TaggedItem<EndOfFileTag> EndOfFile;

class FileTag{};
typedef TaggedItem<FileTag> File;

}


#endif
