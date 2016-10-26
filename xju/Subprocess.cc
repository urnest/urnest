#include <xju/Subprocess.hh>
#line 1 "/home/xju/urnest/xju/Subprocess.hcp"
#line 64
#include "xju/signal.hh" //impl
namespace xju
{
#line 113
void Subprocess::kill(int signal) throw(
    xju::Exception)
  {
    xju::syscall(xju::kill,XJU_TRACED)(pid_,signal);
  }

  
#line 119
Subprocess::~Subprocess() throw()
  {
    (*stop_)(pid_);
    xju::syscall(xju::waitpid,XJU_TRACED)(pid_,&exitStatus_,0);
  }
  

#line 131
Subprocess::Stop::~Stop() noexcept
    {
    }
    
#line 134
void Subprocess::Stop::operator()(pid_t pid) noexcept
    {
      xju::syscall(xju::kill,XJU_TRACED)(pid, 9);
    }
  
}
