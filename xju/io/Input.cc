#include <xju/io/Input.hh>
#line 1 "/home/xju/urnest/xju/io/Input.hcp"
#line 20
#include <iostream> //impl


namespace xju
{
namespace io
{
#line 31
Input::~Input() throw()
  {
  }
  // in-line human readable description
  
#line 40
Input::Closed::Closed(Input const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
#line 56
std::ostream& operator<<(std::ostream& s, Input const& x) throw()
{
  return s << x.str();
}

}
}
