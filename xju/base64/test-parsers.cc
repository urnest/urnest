// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/base64/parsers.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace base64
{
namespace parsers
{

void test1() {
  std::string const x=
R"--(MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8N
Yn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYn
pOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8
jYopyHnOnq0OSdDrMujZRB19rapg+dA=)--";
  try{
    auto const y(hcp_parser::parseString(x.begin(),x.end(),string()));
  }
  catch(xju::Exception const& e){
    std::cerr << readableRepr(e) << std::endl;
    xju::assert_never_reached();
  }
}

void test2() {
  std::string const x=
R"--(AAAAB3NzaC1yc2EAAAADAQABAAABAQDC01TZzrY8c8DQaMAjrsvZYxK40rxaf/kxNkdkViA6GkY1pn6l5yQ4V7Unm99qSyUIgqwrIZieeLXv5PK0IjFRaNUCD4q4/zGqI7XcoK6p0NF1PCWj5Aek9vxQkTpKZhlSZDrwtBcdI55f2EJJ/4RoVohJGXXgAoh/xVZJW7yuW9qsUgJwOuMKgmY/d63fOjimqLqHrv7cXDY2MMaK7/VeG+AhLAqKCIxFq7ySV/nhklsxF6AP0ATYi+b+Q87CQhI+aFEP/xx5vIS1aM5VYacRRCK4dmY3nQM33NYRnxK4r6Ygt39beJ0aqL2TblAFT4U2/ZMtqHXdbiF0jXR3U+PN)--";
  try{
    auto const y(hcp_parser::parseString(x.begin(),x.end(),string()));
  }
  catch(xju::Exception const& e){
    std::cerr << readableRepr(e) << std::endl;
    xju::assert_never_reached();
  }
}

}
}
}

using namespace xju::base64::parsers;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

