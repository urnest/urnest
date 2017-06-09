#include <xju/io/Output.hh>
#include <iostream> //impl


namespace xju
{
namespace io
{
Output::~Output() throw()
  {
  }
  // in-line human readable description
  
Output::Closed::Closed(Output const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
std::ostream& operator<<(std::ostream& s, Output const& x) throw()
{
  return s << x.str();
}


}
}
