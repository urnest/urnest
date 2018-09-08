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

  // The items that make up this composite item, ie the
  // children of this AST node.
  std::vector<std::shared_ptr<Item> > items_;
  
  
  template<class T>
  bool isA() const throw() {
    return dynamic_cast<T const*>(this);
  }
  template<class T>
  T const& asA() const throw() {
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
  I begin_;
  I end_;
  
  friend std::ostream& operator<<(std::ostream& s, Item const& x) throw()
  {
    return s << x.str();
  }
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

class CompositeItem : public Item
{
public:
  explicit CompositeItem(std::vector<IR> const& items) throw():
      Item(items)
  {
  }
  virtual ~CompositeItem() throw() {
  }
  
};

// pre: begin() <= i < end()
std::vector<CompositeItem const*> getContextAt(
  I i, CompositeItem const& within) throw();
  

// A convenient way to create CompositeItem subclasses
// (see below for subclasses that the parser creates).
template<class Tag>
class TaggedCompositeItem : public CompositeItem
{
public:
  explicit TaggedCompositeItem(std::vector<IR> const& items) throw():
    CompositeItem(items)
  {
  }
  virtual ~TaggedCompositeItem() throw() {
  }

  virtual std::string str() const throw()
  {
    return typeid(Tag).name() + std::string(" ") + CompositeItem::str();
  }
};

template<class T>
std::vector<std::reference_wrapper<T const> > findChildrenOfType(
  Item const& parent,
  size_t const returnAtMost=SIZE_MAX)
    throw()
{
  std::vector<std::reference_wrapper<T const> > result;
  for(auto const ir: parent.items_) {
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

//pre: parent has only one child of type T
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

class String : public Item
{
public:
  virtual ~String() throw() {
  }
  
  explicit String(I begin, I end) throw():
      Item(begin,end)
  {
  }
};

class WhiteSpaceTag{};
class LineCommentTag{};
class BlockCommentTag{};
class CommentsTag{};
class StringLiteralTag{};
class S_CharsTag{};
class HashIncludeTag{};

typedef TaggedCompositeItem<WhiteSpaceTag> WhiteSpace;
typedef TaggedCompositeItem<LineCommentTag> LineComment;
typedef TaggedCompositeItem<BlockCommentTag> BlockComment;
typedef TaggedCompositeItem<CommentsTag> Comments;
typedef TaggedCompositeItem<StringLiteralTag> StringLiteral;
typedef TaggedCompositeItem<S_CharsTag> S_Chars;
typedef TaggedCompositeItem<HashIncludeTag> HashInclude;

class HashIncludeImplTag{};
typedef TaggedCompositeItem<HashIncludeImplTag> HashIncludeImpl;

class TargetOfHashIncludeTag{};
typedef TaggedCompositeItem<TargetOfHashIncludeTag> TargetOfHashInclude;
  
class OtherPreprocessorTag{};
typedef TaggedCompositeItem<OtherPreprocessorTag> OtherPreprocessor;

class WhitespaceTag{};
typedef TaggedCompositeItem<WhitespaceTag> Whitespace;

class TypedefTag{};
typedef TaggedCompositeItem<TypedefTag> Typedef;

class DefinedTypeTag{};
typedef TaggedCompositeItem<DefinedTypeTag> DefinedType;

class UsingTag{};
typedef TaggedCompositeItem<UsingTag> Using;

class EnumDefTag{};
typedef TaggedCompositeItem<EnumDefTag> EnumDef;

class EnumNameTag{};
typedef TaggedCompositeItem<EnumNameTag> EnumName;

class FunctionNameTag{};
typedef TaggedCompositeItem<FunctionNameTag> FunctionName;

class FunctionDeclTag{};
typedef TaggedCompositeItem<FunctionDeclTag> FunctionDecl;

class BlockOpenTag{};
typedef TaggedCompositeItem<BlockOpenTag> BlockOpen;

class BlockTag{};
typedef TaggedCompositeItem<BlockTag> Block;

class InitListTag{};
typedef TaggedCompositeItem<InitListTag> InitList;

class InitListOpenTag{};
typedef TaggedCompositeItem<InitListOpenTag> InitListOpen;

class FunctionDefTag{};
typedef TaggedCompositeItem<FunctionDefTag> FunctionDef;

class FunctionImplTag{};
typedef TaggedCompositeItem<FunctionImplTag> FunctionImpl;

class TemplateFunctionDefTag{};
typedef TaggedCompositeItem<TemplateFunctionDefTag> TemplateFunctionDef;

class TemplatePreambleTag{};
typedef TaggedCompositeItem<TemplatePreambleTag> TemplatePreamble;
  
class TemplateEmptyPreambleTag{};
typedef TaggedCompositeItem<TemplateEmptyPreambleTag> TemplateEmptyPreamble;
  
class ClassNameTag{};
typedef TaggedCompositeItem<ClassNameTag> ClassName;

class ClassDef : public CompositeItem
{
public:
  std::string const className_;
  bool const isTemplateSpecialisation_;
  
  explicit ClassDef(std::vector<IR> const& items) throw():
    CompositeItem(items),
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
    return typeid(ClassDef).name() + std::string(" ") + CompositeItem::str();
  }
};

class BaseSpecifierTag{};
typedef TaggedCompositeItem<BaseSpecifierTag> BaseSpecifier;

class ClassMembersTag{};
typedef TaggedCompositeItem<ClassMembersTag> ClassMembers;

class AttrDeclTag{};
typedef TaggedCompositeItem<AttrDeclTag> AttrDecl;

class VarNameTag{};
typedef TaggedCompositeItem<VarNameTag> VarName;

class KeywordStaticTag{};
typedef TaggedCompositeItem<KeywordStaticTag> KeywordStatic;

class KeywordExternTag{};
typedef TaggedCompositeItem<KeywordExternTag> KeywordExtern;

class KeywordFriendTag{};
typedef TaggedCompositeItem<KeywordFriendTag> KeywordFriend;

class KeywordPublicTag{};
typedef TaggedCompositeItem<KeywordPublicTag> KeywordPublic;

class KeywordPrivateTag{};
typedef TaggedCompositeItem<KeywordPrivateTag> KeywordPrivate;

class KeywordProtectedTag{};
typedef TaggedCompositeItem<KeywordProtectedTag> KeywordProtected;

class KeywordVirtualTag{};
typedef TaggedCompositeItem<KeywordVirtualTag> KeywordVirtual;

class KeywordExplicitTag{};
typedef TaggedCompositeItem<KeywordExplicitTag> KeywordExplicit;

class KeywordOverrideTag{};
typedef TaggedCompositeItem<KeywordOverrideTag> KeywordOverride;

class KeywordFinalTag{};
typedef TaggedCompositeItem<KeywordFinalTag> KeywordFinal;

class KeywordNoexceptTag{};
typedef TaggedCompositeItem<KeywordNoexceptTag> KeywordNoexcept;

class KeywordThrowTag{};
typedef TaggedCompositeItem<KeywordThrowTag> KeywordThrow;

class KeywordTryTag{};
typedef TaggedCompositeItem<KeywordTryTag> KeywordTry;

class KeywordCatchTag{};
typedef TaggedCompositeItem<KeywordCatchTag> KeywordCatch;

class StaticVarDefTag{};
typedef TaggedCompositeItem<StaticVarDefTag> StaticVarDef;

class GlobalVarDefTag{};
typedef TaggedCompositeItem<GlobalVarDefTag> GlobalVarDef;

class ExternVarDefTag{};
typedef TaggedCompositeItem<ExternVarDefTag> ExternVarDef;

// marks the '=' in var initialiser
class VarInitialiserOpenTag{};
typedef TaggedCompositeItem<VarInitialiserOpenTag> VarInitialiserOpen;
  
class VarInitialiserTag{};
typedef TaggedCompositeItem<VarInitialiserTag> VarInitialiser;

class AccessModifierTag{};
typedef TaggedCompositeItem<AccessModifierTag> AccessModifier;

class TemplateClassDefTag{};
typedef TaggedCompositeItem<TemplateClassDefTag> TemplateClassDef;

class ClassForwardDeclTag{};
typedef TaggedCompositeItem<ClassForwardDeclTag> ClassForwardDecl;

class AnonymousNamespaceTag{};
typedef TaggedCompositeItem<AnonymousNamespaceTag> AnonymousNamespace;
class AnonymousNamespaceOpenTag{};
typedef TaggedCompositeItem<AnonymousNamespaceOpenTag> AnonymousNamespaceOpen;

class NamespaceDef : public CompositeItem
{
public:
  std::string const namespaceName_;
  
  explicit NamespaceDef(std::vector<IR> const& items) throw():
    CompositeItem(items),
    namespaceName_(getNamespaceName(items))
  {
  }
  virtual ~NamespaceDef() throw() {
  }
  static std::string getNamespaceName(std::vector<IR> const& items) throw();

  virtual std::string str() const throw()
  {
    return typeid(NamespaceDef).name() + std::string(" ") + CompositeItem::str();
  }
};

class NamespaceNameTag{};
typedef TaggedCompositeItem<NamespaceNameTag> NamespaceName;

class NamespaceMembersTag{};
typedef TaggedCompositeItem<NamespaceMembersTag> NamespaceMembers;

class EatWhiteTag{};
typedef TaggedCompositeItem<EatWhiteTag> EatWhite;

class ReturnTypeTag{};
typedef TaggedCompositeItem<ReturnTypeTag> ReturnType;

class OperatorNameTag{};
typedef TaggedCompositeItem<OperatorNameTag> OperatorName;

class ParamTag{};
typedef TaggedCompositeItem<ParamTag> Param;

class VarNonFpTag{};
typedef TaggedCompositeItem<VarNonFpTag> VarNonFp;

class ArrayDeclTag{};
typedef TaggedCompositeItem<ArrayDeclTag> ArrayDecl;

class FunctionQualifiersTag{};
typedef TaggedCompositeItem<FunctionQualifiersTag> FunctionQualifiers;

class TypeRefTag{};
typedef TaggedCompositeItem<TypeRefTag> TypeRef;

class ScopedNameTag{};
typedef TaggedCompositeItem<ScopedNameTag> ScopedName;

class ConstQualTag{};
typedef TaggedCompositeItem<ConstQualTag> ConstQual;

class VolatileQualTag{};
typedef TaggedCompositeItem<VolatileQualTag> VolatileQual;

class MoveQualTag{};
typedef TaggedCompositeItem<MoveQualTag> MoveQual;

class RefQualTag{};
typedef TaggedCompositeItem<RefQualTag> RefQual;

class PointerQualTag{};
typedef TaggedCompositeItem<PointerQualTag> PointerQual;

class ElipsesQualTag{};
typedef TaggedCompositeItem<ElipsesQualTag> ElipsesQual;

class FunctionPostQualifiersTag{};
typedef TaggedCompositeItem<FunctionPostQualifiersTag> FunctionPostQualifiers;

class VirtSpecifierSeqTag{};
typedef TaggedCompositeItem<VirtSpecifierSeqTag> VirtSpecifierSeq;

class EndOfFileTag{};
typedef TaggedCompositeItem<EndOfFileTag> EndOfFile;

class FileTag{};
typedef TaggedCompositeItem<FileTag> File;

}


#endif
