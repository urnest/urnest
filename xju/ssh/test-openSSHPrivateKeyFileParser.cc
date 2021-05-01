// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/openSSHPrivateKeyFileParser.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/istream.hh>
#include <xju/ssh/decode.hh>
#include <xju/crypt/Signer.hh>

namespace xju
{
namespace ssh
{

void test1() {
  std::string const x(R"**(-----BEGIN OPENSSH PRIVATE KEY-----
b3BlbnNzaC1rZXktdjEAAAAABG5vbmUAAAAEbm9uZQAAAAAAAAABAAABFwAAAAdzc2gtcn
NhAAAAAwEAAQAAAQEAyiJNoJgCS/ryg+liw0GLwR0pXHyk2cZR56Dk2cF9Np4n1AxsSZAu
0GLgECYP3cCPu03VP1iYcK2f3HyxwzQrGGOqPNwHWzGSxGnEKAnRgOjkZALXugKLEyhcdy
uATx/QWL0qxVQslEU2Rc7P2WAOLoYGNO4//uAroCA+ZwuxESf0pTpXbzudl8g/o6P6G8hY
s0B99QNBNo8mQImAMRe7tPdrPkJ+0BSnR/tFLoX1D6jdHab+ExXkI4ZYxt7U5v6uDktnp7
kADvws2OVkCCkazoaZQIQDakbakyJZT8aDw4pRjJyuP4cixXSDeVQooI2A2Apw2eml2WIo
ehvL1fJubQAAA8Dhp5mD4aeZgwAAAAdzc2gtcnNhAAABAQDKIk2gmAJL+vKD6WLDQYvBHS
lcfKTZxlHnoOTZwX02nifUDGxJkC7QYuAQJg/dwI+7TdU/WJhwrZ/cfLHDNCsYY6o83Adb
MZLEacQoCdGA6ORkAte6AosTKFx3K4BPH9BYvSrFVCyURTZFzs/ZYA4uhgY07j/+4CugID
5nC7ERJ/SlOldvO52XyD+jo/obyFizQH31A0E2jyZAiYAxF7u092s+Qn7QFKdH+0UuhfUP
qN0dpv4TFeQjhljG3tTm/q4OS2enuQAO/CzY5WQIKRrOhplAhANqRtqTIllPxoPDilGMnK
4/hyLFdIN5VCigjYDYCnDZ6aXZYih6G8vV8m5tAAAAAwEAAQAAAQEAmtb6NioboE6E1g3D
LSWZw/3twD5vHa+w8Ems5S42F8/BXMuDbDq50cHcOCKgBtPKhsXRtCvO4onuKr2OgqUE2j
MGpoY6UF5COpl2vOEAjspYolAXJxNpq+xAIL2/FV2egUuvZufbGl2GfKU3TSIq2vKp8IQN
m9XnNUtd4hvvsN5Yas9Vx0ytTXTgmXEhT2+EJR9PlFPqoGE/j7VE7k/xf4wfIkviDcslp8
CQM5V2f12DNwxJzTX/e1ZbZ9t765dSw9uw2xFPVRrPp3n7ITYmAKFiYMRXcH0gNiR7AokR
+n3dveKGh12UF9H5uxbyiYqpD/ELvfwUSjJbX6WBpoIYQQAAAIBQhTw/HQVqD4lEhpTb/K
g+CISc3UHQxUSDS2BX9Ln+e5BYQF9ZBswLVHj0hC7LpibJMv1kcHmEry1N8D2LtsUeBEF6
osSsCcFGSWUk+ZCkZdp78gp7aEM6V7M2+p06ZAaGLqvh3xd3OlPAB9pAddLGk+HRq93ue+
7ZIbgewtuwrAAAAIEA8phqA0zTCGG5U1oAhoQg42RVeOj3xypy9O+P8i17DIuvmEHU4/Qn
T/XFAFoJc4UWwk+k5F4Gnc3tpZY7xkxp14d2NNU1rnd7w46dUFO0finOVcFTRM2X6Jh9oc
3avcF2I6CI1/CMw/2byVF4qFRp3jGuDwo1AmpuvtwLFA8C+V0AAACBANVNjPV2dTnd5Ugn
qxi9jZDu2Z4JuisvZyr7szy+X9sNsf+NQNDl4pBh/kIoQRQSCeEOs06Nx+LbBoepSr5TQZ
xNT3dAQloE0dFUY+VI90z5IGA+8234/jm8Kc7FOehJrxb4tQN71gKJlXWkJjpouEGh9anY
B1Eno5M3rVirhyhRAAAACXhqdUB4anV0dgE=
-----END OPENSSH PRIVATE KEY-----
)**");
  auto const r(hcp_parser::parseString(x.begin(),x.end(),
                                       openSSHPrivateKeyFileParser()));
  auto const i(hcp_ast::findOnlyChildOfType<open_ssh_private_key_file_parser::Item>(r));
  EncodedPrivateKey k(i.getEncodedPrivateKey());

  xju::MemIBuf b(k.value_);
  xju::net::istream s(b);
  auto const signer(decode<std::unique_ptr<xju::crypt::Signer>>(s));
  
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

