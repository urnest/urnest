#include <xju/io/Output.hh>
#line 1 "/home/xju/urnest/xju/io/Output.hcp"
#line 20
#include <iostream> //impl


namespace xju
{
namespace io
{
#line 31
Output::~Output() throw()
  {
  }
  // in-line human readable description
  
#line 40
Output::Closed::Closed(Output const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
#line 55
std::ostream& operator<<(std::ostream& s, Output const& x) throw()
{
  return s << x.str();
}


}
}
