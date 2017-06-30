#include <xju/DeadlineReached.hh>
namespace xju
{
DeadlineReached::DeadlineReached(xju::Exception const& x) throw():
      xju::Exception(x)
  {
  }
  
DeadlineReached::DeadlineReached(xju::Exception&& x) throw():
      xju::Exception(x)
  {
  }
  

}
