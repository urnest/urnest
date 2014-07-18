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
#include "xju/Shared.hh"
#include <string>
#include <vector>

namespace hcp_ast
{
typedef xju::parse::IteratorAdaptor<std::string::const_iterator> I;


class Item
{
public:
  virtual ~Item() throw() {
  }
  virtual I begin() const throw() = 0;
  virtual I end() const throw() = 0;

  template<class T>
  bool isA() const throw() {
    return dynamic_cast<T const*>(this);
  }
  template<class T>
  T const& asA() const throw() {
    return *dynamic_cast<T const*>(this);
  }
  template<class T>
  T& asA() throw() {
    return *dynamic_cast<T*>(this);
  }
};

inline std::string reconstruct(Item const& x) throw() {
  return std::string(x.begin().x_, x.end().x_);
}

typedef xju::Shared<Item const> IR;
typedef std::vector<IR> IRs;

template<class T>
bool isA_(IR const& x) throw()
{
  return (*x).isA<T>();
}

std::string reconstruct(IRs const& x) throw();

class CompositeItem : public Item
{
public:
  CompositeItem() throw()
  {
  }
  explicit CompositeItem(std::vector<IR> const& items) throw():
    items_(items) 
  {
  }
  virtual ~CompositeItem() throw() {
  }
  
  // Item::
  virtual I begin() const throw() {
    return items_.front()->begin();
  }
  
  // Item::
  virtual I end() const throw() {
    return items_.back()->end();
  }

  // The items that make up this composite item, ie the
  // children of this AST node.
  std::vector<IR> items_;
};

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
};

class String : public Item
{
public:
  I begin_;
  I end_;
  
  virtual ~String() throw() {
  }
  
  explicit String(I begin, I end) throw():
    begin_(begin),
    end_(end) {
  }
  // Item::
  virtual I begin() const throw() {
    return begin_;
  }
  // Item::
  virtual I end() const throw() {
    return end_;
  }
};

class LineCommentTag{};
class BlockCommentTag{};
class CommentsTag{};
class StringLiteralTag{};
class S_CharsTag{};
class HashIncludeTag{};

typedef TaggedCompositeItem<LineCommentTag> LineComment;
typedef TaggedCompositeItem<BlockCommentTag> BlockComment;
typedef TaggedCompositeItem<CommentsTag> Comments;
typedef TaggedCompositeItem<StringLiteralTag> StringLiteral;
typedef TaggedCompositeItem<S_CharsTag> S_Chars;
typedef TaggedCompositeItem<HashIncludeTag> HashInclude;

class HashIncludeImplTag{};
typedef TaggedCompositeItem<HashIncludeImplTag> HashIncludeImpl;

class OtherPreprocessorTag{};
typedef TaggedCompositeItem<OtherPreprocessorTag> OtherPreprocessor;

class WhitespaceTag{};
typedef TaggedCompositeItem<WhitespaceTag> Whitespace;

class TypedefTag{};
typedef TaggedCompositeItem<TypedefTag> Typedef;

class UsingTag{};
typedef TaggedCompositeItem<UsingTag> Using;

class EnumDefTag{};
typedef TaggedCompositeItem<EnumDefTag> EnumDef;

class FunctionNameTag{};
typedef TaggedCompositeItem<FunctionNameTag> FunctionName;

class FunctionDeclTag{};
typedef TaggedCompositeItem<FunctionDeclTag> FunctionDecl;

class BlockTag{};
typedef TaggedCompositeItem<BlockTag> Block;

class InitListTag{};
typedef TaggedCompositeItem<InitListTag> InitList;

class FunctionDefTag{};
typedef TaggedCompositeItem<FunctionDefTag> FunctionDef;

class FunctionImplTag{};
typedef TaggedCompositeItem<FunctionImplTag> FunctionImpl;

class TemplateFunctionDefTag{};
typedef TaggedCompositeItem<TemplateFunctionDefTag> TemplateFunctionDef;

class ClassNameTag{};
typedef TaggedCompositeItem<ClassNameTag> ClassName;

class ClassDef : public CompositeItem
{
public:
  std::string const className_;
  
  explicit ClassDef(std::vector<IR> const& items) throw():
    CompositeItem(items),
    className_(getClassName(items))
  {
  }
  virtual ~ClassDef() throw() {
  }
  static std::string getClassName(std::vector<IR> const& items) throw();
};

class AttrDeclTag{};
typedef TaggedCompositeItem<AttrDeclTag> AttrDecl;

class AccessModifierTag{};
typedef TaggedCompositeItem<AccessModifierTag> AccessModifier;

class TemplateClassDefTag{};
typedef TaggedCompositeItem<TemplateClassDefTag> TemplateClassDef;

class ClassForwardDeclTag{};
typedef TaggedCompositeItem<ClassForwardDeclTag> ClassForwardDecl;

class AnonymousNamespaceTag{};
typedef TaggedCompositeItem<AnonymousNamespaceTag> AnonymousNamespace;

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
};

class NamespaceNameTag{};
typedef TaggedCompositeItem<NamespaceNameTag> NamespaceName;

class NamespaceMembersTag{};
typedef TaggedCompositeItem<NamespaceMembersTag> NamespaceMembers;

class EatWhiteTag{};
typedef TaggedCompositeItem<EatWhiteTag> EatWhite;

class FunctionQualifiersTag{};
typedef TaggedCompositeItem<FunctionQualifiersTag> FunctionQualifiers;

class EndOfFileTag{};
typedef TaggedCompositeItem<EndOfFileTag> EndOfFile;

class FileTag{};
typedef TaggedCompositeItem<FileTag> File;

}


#endif
