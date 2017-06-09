#include <xju/Subprocess.hh>
#include "xju/signal.hh" //impl
namespace xju
{
void Subprocess::kill(int signal) throw(
    xju::Exception)
  {
    xju::syscall(xju::kill,XJU_TRACED)(pid_,signal);
  }

  
Subprocess::~Subprocess() throw()
  {
    (*stop_)(pid_);
    xju::syscall(xju::waitpid,XJU_TRACED)(pid_,&exitStatus_,0);
  }
  

Subprocess::Stop::~Stop() noexcept
    {
    }
    
void Subprocess::Stop::operator()(pid_t pid) noexcept
    {
      xju::syscall(xju::kill,XJU_TRACED)(pid, 9);
    }
  
}
