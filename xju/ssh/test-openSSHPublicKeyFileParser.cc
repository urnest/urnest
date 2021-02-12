// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/openSSHPublicKeyFileParser.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/istream.hh>
#include <xju/ssh/decode.hh>

namespace xju
{
namespace ssh
{

void test1() {
  std::string const x("ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDKIk2gmAJL+vKD6WLDQYvBHSlcfKTZxlHnoOTZwX02nifUDGxJkC7QYuAQJg/dwI+7TdU/WJhwrZ/cfLHDNCsYY6o83AdbMZLEacQoCdGA6ORkAte6AosTKFx3K4BPH9BYvSrFVCyURTZFzs/ZYA4uhgY07j/+4CugID5nC7ERJ/SlOldvO52XyD+jo/obyFizQH31A0E2jyZAiYAxF7u092s+Qn7QFKdH+0UuhfUPqN0dpv4TFeQjhljG3tTm/q4OS2enuQAO/CzY5WQIKRrOhplAhANqRtqTIllPxoPDilGMnK4/hyLFdIN5VCigjYDYCnDZ6aXZYih6G8vV8m5t xju@xjutv");
  auto const r(hcp_parser::parseString(x.begin(),x.end(),
                                       openSSHPublicKeyFileParser()));
  auto const i(hcp_ast::findOnlyChildOfType<xju::ssh::open_ssh_public_key_file_parser::Item>(r));
  xju::assert_equal(std::get<0>(i.get()),KeyTypeName("ssh-rsa"));
  xju::assert_equal(std::get<2>(i.get()),std::string("xju@xjutv"));
  auto const encodedKey(std::get<1>(i.get()));
  xju::MemIBuf b(encodedKey.encodedValue_);
  xju::net::istream s(b);
  auto const k(decode<xju::crypt::rsa::PublicKey>(s));
}

}
}

using namespace xju::ssh;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

