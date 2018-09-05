#include "x.hh"

#include <xju/assert.hh>

int main(int argc, char* argv[])
{
  A::B b;
  xju::assert_equal(get(b),b.get());
  return 0;
}
