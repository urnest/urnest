// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/uri/parseURI.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace uri
{

void test1() {

  {
    const std::string s{"a8-._~%20!$&'()*+,;=:"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segment())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<SegmentItem>(r)),
      s);
  }
  {
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segment())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<SegmentItem>(r)),
      s);
  }
  try{
    const std::string s{"abc/22"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segment())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 4.");
  }

  {
    const std::string s{"a8-._~%20!$&'()*+,;="};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segmentNz())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<SegmentItem>(r)),
      s);
  }
  try{
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segmentNz())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse segment-nz at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"-._~0-9a-zA-Z!$&'()*+,;=:@\" or pct-encoded at line 1 column 1 because\nfailed to parse pct-encoded at line 1 column 1 because\nfailed to parse \"%\" at line 1 column 1 because\nline 1 column 1: end of input.");
  }
  try{
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segmentNzNc())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse segment-nz-nc at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"-._~0-9a-zA-Z\" or pct-encoded or one of chars \"!$&'()*+,;=\" or \"@\" at line 1 column 1 because\nfailed to parse \"@\" at line 1 column 1 because\nline 1 column 1: end of input.");
  }
  try{
    const std::string s{":z"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segmentNzNc())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse segment-nz-nc at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"-._~0-9a-zA-Z\" or pct-encoded or one of chars \"!$&'()*+,;=\" or \"@\" at line 1 column 1 because\nfailed to parse \"@\" at line 1 column 1 because\nline 1 column 1: expected '@' but found ':'.");
  }
  try{
    const std::string s{"a%2p"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segmentNzNc())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 2.");
  }
}

void test2() {

  {
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbEmpty())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathAbEmptyItem>(r)),
      s);
  }
  {
    const std::string s{"/fred/jock"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbEmpty())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathAbEmptyItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),2U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"fred");
    xju::assert_equal(hcp_ast::reconstruct(y[1].get()),"jock");
  }
  try{
    const std::string s{"abc/22"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbEmpty())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 1.");
  }

}

void test3() {

  {
    const std::string s{"/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbsolute())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathAbsoluteItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),0U);
  }
  {
    const std::string s{"/fred/jock"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbsolute())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathAbsoluteItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),2U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"fred");
    xju::assert_equal(hcp_ast::reconstruct(y[1].get()),"jock");
  }
  try{
    const std::string s{"abc/22"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbsolute())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse path-absolute at line 1 column 1 because\nfailed to parse \"/\" at line 1 column 1 because\nline 1 column 1: expected '/' but found 'a'.");
  }
  try{
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathAbsolute())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse path-absolute at line 1 column 1 because\nfailed to parse \"/\" at line 1 column 1 because\nline 1 column 1: end of input.");
  }

}

void test4() {

  {
    const std::string s{"abc"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathNoScheme())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathNoSchemeItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),1U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"abc");
  }
  {
    const std::string s{"fred/jock"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathNoScheme())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathNoSchemeItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),2U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"fred");
    xju::assert_equal(hcp_ast::reconstruct(y[1].get()),"jock");
  }
  try{
    const std::string s{"abc:22"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathNoScheme())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 4.");
  }
  try{
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathNoScheme())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse path-noscheme at line 1 column 1 because\nfailed to parse segment-nz-nc at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"-._~0-9a-zA-Z\" or pct-encoded or one of chars \"!$&'()*+,;=\" or \"@\" at line 1 column 1 because\nfailed to parse \"@\" at line 1 column 1 because\nline 1 column 1: end of input.");
  }

}

void test5() {

  {
    const std::string s{"abc"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathRootless())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathRootlessItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),1U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"abc");
  }
  {
    const std::string s{"fred/jock"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathRootless())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathRootlessItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),2U);
    xju::assert_equal(hcp_ast::reconstruct(y[0].get()),"fred");
    xju::assert_equal(hcp_ast::reconstruct(y[1].get()),"jock");
  }
  try{
    const std::string s{"abc:%"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathRootless())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 5.");
  }
  try{
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathRootless())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse path-rootless at line 1 column 1 because\nfailed to parse segment-nz at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"-._~0-9a-zA-Z!$&'()*+,;=:@\" or pct-encoded at line 1 column 1 because\nfailed to parse pct-encoded at line 1 column 1 because\nfailed to parse \"%\" at line 1 column 1 because\nline 1 column 1: end of input.");
  }

}

void test6() {

  {
    const std::string s{""};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathEmpty())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PathEmptyItem>(r)),
      s);
    auto const y{hcp_ast::findChildrenOfType<SegmentItem>(r)};
    xju::assert_equal(y.size(),0U);
  }
  try{
    const std::string s{"abc:%"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),pathEmpty())};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    xju::assert_equal(readableRepr(e),"Failed to parse path-empty at line 1 column 1 because\nfailed to parse !one of chars \"-._~0-9a-zA-Z!$&'()*+,;=:@\" or pct-encoded at line 1 column 1 because\nline 1 column 1: expected parse failure.");
  }

}

void test7()
{

  {
    const std::string s{"abc=/?2"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),query())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<QueryItem>(r)),
      s);
  }
  try
  {
    const std::string s{"abc#fred"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),query())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 4.");
  }
}

void test8()
{

  {
    const std::string s{"abc=/?2"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),fragment())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<FragmentItem>(r)),
      s);
  }
  try
  {
    const std::string s{"abc#fred"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),fragment())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 4.");
  }
}

void test9()
{

  {
    const std::string s{"q81-10"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),scheme())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<SchemeItem>(r)),
      s);
  }
  try
  {
    const std::string s{"9ab"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),scheme())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse scheme at line 1 column 1 because\nfailed to parse one of chars 'A'..'Z' at line 1 column 1 because\nline 1 column 1: '9' is not one of chars 'A'..'Z'.");
  }
}

void test10()
{

  {
    const std::string s{"fred:pwd"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),userInfo())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<UserInfoItem>(r)),
      s);
  }
  try
  {
    const std::string s{"9a@b"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),userInfo())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 3.");
  }
}


void test11()
{

  {
    const std::string s{"10.8.32.120"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV4Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV4AddressItem>(r)),
      s);
  }
  try
  {
    const std::string s{"256.8.32.1"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV4Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv4 Address at line 1 column 1 because\nfailed to parse \".\" at line 1 column 3 because\nline 1 column 3: expected '.' but found '6'.");
  }
  try
  {
    const std::string s{"25.8.a2.1"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV4Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv4 Address at line 1 column 1 because\nfailed to parse 0..255 at line 1 column 6 because\nfailed to parse one of chars '0'..'9' at line 1 column 6 because\nline 1 column 6: 'a' is not one of chars '0'..'9'.");
  }
  try
  {
    const std::string s{"25.8.17"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV4Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv4 Address at line 1 column 1 because\nfailed to parse \".\" at line 1 column 8 because\nline 1 column 8: end of input.");
  }
}

void test12()
{

  {
    const std::string s{"1234:5678:9abc:def0:0fed:cba9:8765:4321"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x0f,0xed,0xcb,0xa9,0x87,0x65,0x43,0x21}));
                                                                                                      
  }
  {
    const std::string s{"::1"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}));
                                                                                                      
  }
  {
    const std::string s{"::"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}));
                                                                                                      
  }
  {
    const std::string s{"1234:5678::"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({1,2,3,4,5,6,7,8,0,0,0,0,0,0,0,0}));
                                                                                                      
  }
  {
    const std::string s{"1234:5678::4321"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({1,2,3,4,5,6,7,8,0,0,0,0,4,3,2,1}));
                                                                                                      
  }
  {
    const std::string s{"1::3.5.18.2"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,xju::ip::v6::Address({1,0,0,0,0,0,0,0,0,0,0,0,3,5,18,2}));
                                                                               }
  try
  {
    const std::string s{"a::abcde"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv6 Address at line 1 column 1 because\nfailed to parse \":\"-separated list of hex-component terminated by URI-format IPv4 Address or !\":\" or one of chars '0'..'9' or one of chars 'a'..'f' or one of chars 'A'..'F' at line 1 column 4 because\nfailed to parse \":\" at line 1 column 8 because\nline 1 column 8: expected ':' but found 'e'.");
  }
  try
  {
    const std::string s{"2:::32"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv6 Address at line 1 column 1 because\nfailed to parse \":\"-separated list of hex-component terminated by URI-format IPv4 Address or !\":\" or one of chars '0'..'9' or one of chars 'a'..'f' or one of chars 'A'..'F' at line 1 column 4 because\nfailed to parse hex-component at line 1 column 4 because\nfailed to parse one of chars 'A'..'F' at line 1 column 4 because\nline 1 column 4: ':' is not one of chars 'A'..'F'.");
  }
  try
  {
    const std::string s{"a:b:c:d:25.8.17"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),ipV6Address())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse URI-format IPv6 Address at line 1 column 1 because\nline 1 column 11: IPv6 address has 5 components not 8.");
  }
}

void test13()
{

  {
    const std::string s{"fred.our.com"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),host())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<HostItem>(r)),
      s);
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<HostNameItem>(r)),
                      "fred.our.com");
  }
  {
    const std::string s{"fr%20ed.our.com"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),host())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<HostItem>(r)),
      s);
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<HostNameItem>(r)),
                      "fr%20ed.our.com");
  }
  {
    const std::string s{"18.18.9.1"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),host())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<HostItem>(r)),
      s);
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<IpV4AddressItem>(r)),
                      "18.18.9.1");
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV4AddressItem>(r).address_,
                      xju::ip::v4::Address("18.18.9.1"));
  }
  {
    const std::string s{"[1234:5678:9abc:def0:0fed:cba9:8765:4321]"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),host())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<HostItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<IpV6AddressItem>(r).address_,
                      xju::ip::v6::Address({0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x0f,0xed,0xcb,0xa9,0x87,0x65,0x43,0x21}));
  }
  try
  {
    const std::string s{"1234:5678:9abc:def0:0fed:cba9:8765:4321"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),host())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 5.");
  }
}

void test14()
{

  {
    const std::string s{"7786"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),port())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PortItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<PortItem>(r).port_,
                      xju::ip::Port(7786));
  }
  {
    const std::string s{"65535"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),port())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<PortItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<PortItem>(r).port_,
                      xju::ip::Port(65535));
  }
  try
  {
    const std::string s{"66536"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),port())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse port at line 1 column 1 because\nline 1 column 6: 66536 is not a valid i.e. 0..65535 IP port.");
  }
}

void test15()
{

  {
    const std::string s{"fred@host.com:3374"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),authority())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<AuthorityItem>(r)),
      s);
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<UserInfoItem>(r)),
                      "fred");
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<HostItem>(r)),
                      "host.com");
    xju::assert_equal(hcp_ast::reconstruct(
                        hcp_ast::findOnlyChildOfType<PortItem>(r)),
                      "3374");
  }
}

void test16()
{

  {
    const std::string s{"http://fred@host.com:3374/index.html?name=z#top"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<URIItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<URIItem>(r).uri_,
                      URI(
                        Scheme("http"),
                        Authority(Host(xju::HostName("host.com")),
                                  xju::ip::Port(3374),
                                  UserInfo("fred")),
                        Path({Segment(""),Segment("index.html")}),
                        Query("name=z"),
                        Fragment("top")));
  }
  {
    const std::string s{"http://fred@ho%20st.com:3374/index.html?name=z#top"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<URIItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<URIItem>(r).uri_,
                      URI(
                        Scheme("http"),
                        Authority(Host(xju::HostName("ho st.com")),
                                  xju::ip::Port(3374),
                                  UserInfo("fred")),
                        Path({Segment(""),Segment("index.html")}),
                        Query("name=z"),
                        Fragment("top")));
  }
  {
    const std::string s{"http:"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<URIItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<URIItem>(r).uri_,
                      URI(
                        Scheme("http"),
                        xju::Optional<Authority>(),
                        Path({}),
                        Query(""),
                        Fragment("")));
  }
  {
    const std::string s{"http:b/x.txt"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<URIItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<URIItem>(r).uri_,
                      URI(
                        Scheme("http"),
                        xju::Optional<Authority>(),
                        Path({Segment("b"),Segment("x.txt")}),
                        Query(""),
                        Fragment("")));
  }
  try
  {
    const std::string s{"/a"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse RFC 3986 URI at line 1 column 1 because\nfailed to parse scheme at line 1 column 1 because\nfailed to parse one of chars 'A'..'Z' at line 1 column 1 because\nline 1 column 1: '/' is not one of chars 'A'..'Z'.");
  }
  try
  {
    const std::string s{"http://fred:jock/a"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),parseURI())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 7.");
  }
}

}
}


using namespace xju::uri;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  test5(), ++n;
  test6(), ++n;
  test7(), ++n;
  test8(), ++n;
  test9(), ++n;
  test10(), ++n;
  test11(), ++n;
  test12(), ++n;
  test13(), ++n;
  test14(), ++n;
  test15(), ++n;
  test16(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

