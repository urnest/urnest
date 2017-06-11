#ifndef _XJU_SUBPROCESS_HH
#define _XJU_SUBPROCESS_HH
// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// Create, run and wait for a subprocess to exit, executing a specified
// function in that process, capturing the subprocess exit status.
//
// For example, to run command by a deadline, capturing its stdout,
// killing subprocess on timeout:
//
// std::string doCmd(std::vector<std::string> const& cmd,
//                   std::chrono::system_clock::timepoint const& deadline)
//   throw(std::bad_alloc,xju::Exception)
//   {
//     std::chrono::duration<REVISIT> const timeout(
//       deadline-std::chrono::system_clock::now());
//     try{
//       auto p(xju::pipe(true,false); // to capture output of subprocess
//       int exitStatus(0);
//       std::ostringstream output;
//       try {
//         Subprocess command([const&](){ 
//           p.second->useAsStdout(); // use writable end of pipe as stdout
//           xju::exec(cmd);
//         ));
//         p.second.reset();//close writable end of pipe in parent process
//         std::vector<char> buffer(512,0);
//         while(std::chrono::system_clock::now()<deadline){
//           buffer.resize(p.first->read(buffer.data(),512,dealine));
//           std::copy(buffer.begin(),buffer.end(),
//                     std::ostream_iterator(output));
//         }
//         throw xju::Exception("timeout",XJU_TRACED);
//
//         // note exiting this block will kill(9) the subprocess, but that
//         // has no effect if it has exited already. Exiting this block
//         // also "waits" for the subprocess, collecting its exit status
//       }
//       catch(xju::io::Input::Closed&)
//       {
//         // pipe closes when subprocess exits
//       }
//       if (exitStatus!=0){
//         std::ostringstream s;
//         s << "command failed with status " << exitStatus;
//         throw xju::Exception(s.str(),XJU_TRACED);
//       }
//       return output.str();
//     }
//     catch(xju::Exception& e){
//       std::ostringstream s;
//       s << "run " << xju::format::join(cmd," ") << " within "
//         xju::format::float(timeout.count()) << "s";
//       e.addContext(s.str(),XJU_TRACED);
//     }
//  }
#include "xju/Exception.hh"
#include "xju/unistd.hh"
#include "xju/wait.hh"
#include <memory>
#include <sys/types.h>
#include "xju/NonCopyable.hh"

namespace xju
{

class Subprocess : NonCopyable
{
public:
  // call start() in Subprocess with process id pid_, set exitStatus
  // to exit status of Subprocess, which is result of start() unless
  // Subprocess calls exec, is killed or aborts (see waitpid manpage
  // for interpretation of exitStatus via WIFEXITED, WEXITSTATUS etc)
  // - exitStatus is not valid until destuctor completes
  // - destructor issues kill(9) and waits until Subprocess exits
  // - exitStatus must outlive Subprocess
  // int start() throw()
  template<class F>
  Subprocess(int& exitStatus, F start) throw(
    std::bad_alloc,
    xju::Exception):
      exitStatus_(exitStatus),
      stop_(new Stop),
      pid_(xju::syscall(xju::fork,XJU_TRACED)())
  {
    if (!pid_) {
      ::exit(start());
    }
  }
  // as above, but destructor calls stop(pid_) before waiting for
  // Subprocess to terminate
  // 
  // int start() throw()
  // void stop(pid_t pid) throw()
  template<class F1, class F2>
  Subprocess(int& exitStatus, F1 start, F2 stop) throw(
    std::bad_alloc,
    xju::Exception):
      exitStatus_(exitStatus),
      stop_(new Stop2<F2>(stop)),
      pid_(xju::syscall(xju::fork,XJU_TRACED)())
  {
    if (!pid_) {
      ::exit(start());
    }
  }
  void kill(int signal) throw(
    xju::Exception);


  ~Subprocess() throw();

  
private:
  int& exitStatus_;

  class Stop
  {
  public:
    virtual ~Stop() noexcept;

    virtual void operator()(pid_t pid) noexcept;

  };
  

  std::unique_ptr<Stop> stop_;
  pid_t const pid_;

  template<class F>
  class Stop2: public Stop
  {
  public:
    Stop2(F stop) noexcept:
      stop_(stop)
    {
    }
    virtual void operator()(pid_t pid) noexcept
    {
      stop_(pid);
    }
    F stop_;
  };
};

  
}
#endif
