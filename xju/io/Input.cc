#include <xju/io/Input.hh>
#include <iostream> //impl


namespace xju
{
namespace io
{
Input::~Input() throw()
  {
  }
  // in-line human readable description
  
Input::Closed::Closed(Input const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
std::ostream& operator<<(std::ostream& s, Input const& x) throw()
{
  return s << x.str();
}

}
}
