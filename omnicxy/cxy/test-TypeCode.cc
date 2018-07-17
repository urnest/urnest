// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <cxy/TypeCode.hh>

#include <iostream>
#include <xju/assert.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/cdrStream.h>

namespace cxy
{

void testVoid() {
  cxy::MemCdrStream s;
  ::cxy::TypeCode const x(cxy::createTypeCodeOf<void>());
  ::cxy::cdr< ::cxy::TypeCode>::marshal(x,*s);

  ::cxy::TypeCode y(cdr< ::cxy::TypeCode >::unmarshalFrom(*s));
  xju::assert_equal(*x,*y);
  
}

template<class T>
void testx(T const v) {
  {
    cxy::MemCdrStream s;
    cdr<T>::marshal(v,*s);
    
    xju::assert_equal(cdr<T>::unmarshalFrom(*s),v);
  }
  cxy::MemCdrStream s;
  ::cxy::TypeCode const x(cxy::createTypeCodeOf<T>());
  ::cxy::cdr< ::cxy::TypeCode>::marshal(x,*s);
  cdr<T>::marshal(v,*s);

  ::cxy::TypeCode y(cdr< ::cxy::TypeCode >::unmarshalFrom(*s));
  xju::assert_equal(*x,*y);
  
  MemCdrStream t;
  y->copyValue(*s,*t);
  
  xju::assert_equal(cdr<T>::unmarshalFrom(*t),v);
}

class Tree
{
public:
  Tree(std::string const& name,
       std::vector<Tree> const& children) throw():
      name_(name),
      children_(children)
  {
  };
  std::string name_;
  std::vector<Tree> children_;
  friend bool operator==(Tree const& a, Tree const& b) throw(){
    return (a.name_ == b.name_) &&
      (a.children_==b.children_);
  }
};
template<>
class cdr<Tree>
{
public:
  static void marshal(Tree const& x, cdrStream& s)
  {
    ::cxy::cdr< std::string >::marshal(x.name_,s);
    ::cxy::cdr< std::vector<Tree> >::marshal(x.children_,s);
  }
  static Tree unmarshalFrom(cdrStream& s)
  {
    auto const name(::cxy::cdr< std::string >::unmarshalFrom(s));
    auto const children(
      ::cxy::cdr< std::vector<Tree> >::unmarshalFrom(s));
    return Tree(name,children);
  }
};
template<>
class TypeCodeOf<Tree>
{
public:
  static TypeCode create(cxy::TypeCodeRefIndex& index) throw(){
    TypeCode name(cxy::createTypeCodeOf< std::string >(index));
    TypeCode children(
      cxy::createTypeCodeOf< std::vector<Tree> >(index));
    return TypeCode(std::shared_ptr<TypeCode_>(
                      new cxy::StructTypeCode(
                        "Tree",
                        "Tree",
                        { {"name",name},
                          {"children",children} })));
  }
};
void test1()
{
  testVoid();
  testx((int16_t)27);
  testx((int32_t)-32);
  testx(std::vector<int16_t>{3});
  testx(Tree("fred",{Tree("jock",{})}));
}

}

using namespace cxy;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

