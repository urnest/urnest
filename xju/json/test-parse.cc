// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/json/parse.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/utf8/encodeCodePoint.hh>

namespace xju
{
namespace json
{

void testNull()
{
  {
    auto const x{parse(Utf8String(" null "))};
    xju::assert_equal(x->isNull(),true);
  }
  try{
    auto const x{parse(Utf8String("nil"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse null at line 1 column 1 because
failed to parse "null" at line 1 column 1 because
line 1 column 2: expected 'u' but found 'i'.
)--");
  }
  {
    auto const x{parse(Utf8String("1.06e10"))};
    xju::assert_equal(x->isNull(),false);
  }
}

void testTrue()
{
  {
    auto const x{parse(Utf8String(" true "))};
    xju::assert_equal(x->asBool(),true);
  }
  try{
    auto const x{parse(Utf8String("tru"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse true at line 1 column 1 because
failed to parse "true" at line 1 column 1 because
line 1 column 4: end of input.
)--");
  }
  {
    auto const x{parse(Utf8String("1.06e10"))};
    try{
      x->asBool();
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to get 1.06e10 (at line 1 column 1) as a bool because
1.06e10 (at line 1 column 1) is not true or false.
)--");
    }
  }
}

void testFalse()
{
  {
    auto const x{parse(Utf8String(" false "))};
    xju::assert_equal(x->asBool(),false);
  }
  try{
    auto const x{parse(Utf8String("fasle"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse false at line 1 column 1 because
failed to parse "false" at line 1 column 1 because
line 1 column 3: expected 'l' but found 's'.
)--");
  }
  {
    auto const x{parse(Utf8String("1.06e10"))};
    try{
      x->asBool();
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to get 1.06e10 (at line 1 column 1) as a bool because
1.06e10 (at line 1 column 1) is not true or false.
)--");
    }
  }
}

void testNumber() {
  {
    auto const x{parse(Utf8String("3.45"))};
    xju::assert_equal(x->asDouble(),3.45);
  }
  {
    auto const x{parse(Utf8String("-3e-2 "))};
    xju::assert_equal(x->asDouble(),-3e-2);
  }
  {
    auto const x{parse(Utf8String(" 0"))};
    xju::assert_equal(x->asDouble(),0.0);
  }
  {
    auto const x{parse(Utf8String("0.6e+1"))};
    xju::assert_equal(x->asDouble(),0.6e+1);
  }
  {
    auto const x{parse(Utf8String("1.06e10"))};
    xju::assert_equal(x->asDouble(),1.06e+10);
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asInt();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get 1.06e10 (at line 1 column 1) as an int because\nfailed to convert \"1.06e10\" to an integer because\ncharacter 2 ('.') of \"1.06e10\" unexpected.");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asBool();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get 1.06e10 (at line 1 column 1) as a bool because\n1.06e10 (at line 1 column 1) is not true or false.");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asString();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to get 1.06e10 (at line 1 column 1) as a string because
1.06e10 (at line 1 column 1) is not a string.
)--");
  }

}

void testString()
{
  {
    auto const x{parse(Utf8String("\"fred\""))};
    xju::assert_equal(x->asString(),xju::Utf8String("fred"));
  }
  {
    auto const x{parse(Utf8String(R"--("fr\"\\\/\a\b\f\n\r\ted")--"))};
    xju::assert_equal(x->asString(),xju::Utf8String("fr\"\\/\a\b\f\n\r\ted"));
  }
  {
    auto const x{parse(Utf8String(R"--("fr\u20ACed")--"))};
    xju::assert_equal(x->asString(),
                      xju::Utf8String(
                        std::string{'f','r',
                            (char)0xE2,(char)0x82,(char)0xAC,
                            'e','d'}));
  }
  {
    //surrogate pair - see xju/utf8/test-surrogate.cc
    auto const x{parse(Utf8String(R"--("fr\uD801\uDC37ed")--"))};
    xju::assert_equal(x->asString(),
                      xju::Utf8String("fr")+
                      xju::Utf8String(xju::utf8::encodeCodePoint(0x10437))+
                      xju::Utf8String("ed"));
  }

  try{
    auto const x{parse(Utf8String(R"--("fred")--"))};
    x->asInt();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to get "fred" (at line 1 column 1) as an int because
"fred" (at line 1 column 1) is not a number.
)--");
  }

  try{
    auto const x{parse(Utf8String(R"--("fre\d")--"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse JSON string at line 1 column 1 because
line 1 column 6: expected one of ", \, /, a, b, f, n, r, t, u, got (c-escaped) "d".
)--");
  }

  try{
    auto const x{parse(Utf8String(R"--("fre\uD7ff\uDC00d")--"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse JSON string at line 1 column 1 because
line 1 column 16: 0x0000dc00 is not a valid Unicode character because it is in the utf-16 surrogate pair reserved range 0xD800..0xDFFF.
)--");
  }
  try{
    auto const x{parse(Utf8String(R"--("fre\uD800\uDb00d")--"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse JSON string at line 1 column 1 because
line 1 column 16: failed to decode 0xd800 and 0xdb00 assuming they are a UTF16 surrogate pair (high,low) because
0xdb00 is not in range 0xDC00..0xDFFF.
)--");
  }

  try{
    auto const x{parse(Utf8String(R"--("fred)--"))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse JSON string at line 1 column 1 because
line 1 column 6: end of input.
)--");
  }


}

void testArray()
{
  {
    std::string const s{" [ 1,2 , 3] "};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asArray()};
    xju::assert_equal(y.size(),3);
    xju::assert_equal(y[0]->asDouble(),1.0);
    xju::assert_equal(y[1]->asDouble(),2.0);
    xju::assert_equal(y[2]->asDouble(),3.0);
  }
  {
    std::string const s{" [ ] "};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asArray()};
    xju::assert_equal(y.size(),0U);
  }
  try{
    std::string const s{" [ 3 4] "};
    auto const x{parse(Utf8String(s))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e),R"--(
Failed to parse array at line 1 column 2 because
failed to parse "," then optional whitespace-separated list of element (recurse) commencing with "[" then optional whitespace and terminated by "]" then optional whitespace at line 1 column 2 because
failed to parse "," at line 1 column 6 because
line 1 column 6: expected ',' but found '4'.)--");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asArray();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e),R"--(
Failed to get 1.06e10 (at line 1 column 1) as an array because
1.06e10 (at line 1 column 1) is not an Array.)--");
  }
  {
    std::string const s{};
    auto const x{parse(Utf8String(" [ 1,2 , 3] "))};
    auto const y{parse(Utf8String(" [ 1,2 , 3,4] "))};
    xju::assert_less(*x,*y);
  }

}

void testObject()
{
  {
    std::string const s{R"--( { "a":1,"b" :2 , "c" : 3 } )--"};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asObject()};
    xju::assert_equal(y.size(),3);
    xju::assert_equal((*y.find(Utf8String("a"))).second->asInt(),1);
    xju::assert_equal((*y.find(Utf8String("b"))).second->asInt(),2);
    xju::assert_equal((*y.find(Utf8String("c"))).second->asInt(),3);
    xju::assert_equal(x->hasMember(Utf8String("a")),true);
    xju::assert_equal(x->getMember(Utf8String("a")).asInt(),1);
    xju::assert_equal(x->hasMember(Utf8String("d")),false);
  }
  {
    std::string const s{" { } "};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asObject()};
    xju::assert_equal(y.size(),0U);
  }
  try{
    std::string const s{R"--( { "a":1 "b" :2 , "c" : 3 } )--"};
    auto const x{parse(Utf8String(s))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to parse object at line 1 column 2 because
failed to parse "," then optional whitespace-separated list of object member commencing with "{" then optional whitespace and terminated by "}" then optional whitespace at line 1 column 2 because
failed to parse "," at line 1 column 10 because
line 1 column 10: expected ',' but found '"'.
)--");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asObject();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to get 1.06e10 (at line 1 column 1) as an object because
1.06e10 (at line 1 column 1) is not an Object.
)--");
  }
  {
    std::string const s1{R"--( { "a":1,"b" :1 , "c" : 3 } )--"};
    std::string const s2{R"--( { "a":1,"b" :2 , "c" : 3 } )--"};
    auto const x{parse(Utf8String(s1))};
    auto const y{parse(Utf8String(s2))};
    xju::assert_less(*x,*y);
  }

}

}
}

using namespace xju::json;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  testNull(), ++n;
  testTrue(), ++n;
  testFalse(), ++n;
  testNumber(), ++n;
  testString(), ++n;
  testArray(), ++n;
  testObject(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

