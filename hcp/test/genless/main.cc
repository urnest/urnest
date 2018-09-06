#include "x.hh"

#include <xju/assert.hh>

int main(int argc, char* argv[])
{
  hcp::test::genless::X const x{1,"fred"};
  hcp::test::genless::X const y{1,"jock"};
  xju::assert_less(x,y);
  xju::assert_less_equal(x,y);
  xju::assert_less_equal(x,x);
  xju::assert_not_equal(x,y);
  xju::assert_equal(x,x);
  xju::assert_greater(y,x);
  xju::assert_greater_equal(y,x);
  return 0;
}
