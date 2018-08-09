// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/interop/b.hh>
#include <omnicxy/proto/interop/b.cref.hh>
#include <omnicxy/proto/interop/b.sref.hh>
#include "cxy/ORB.hh"
#include "xju/unistd.hh"
#include "xju/Subprocess.hh"
#include <thread>
#include "xju/signal.hh"
#include "xju/pipe.hh"

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class server_impl : public omnicxy::proto::interop::b::server
{
public:
  explicit server_impl(cxy::ORB<cxy::Exception>& orb,
                       int step) throw():
      orb_(orb),
      step_(step),
      done_(false)
  {
  }
  cxy::ORB<cxy::Exception>& orb_;
  int const step_;
  bool done_;
  
  ~server_impl() throw()
  {
  }
  
  //omnicxy::proto::interop::b::server::
  virtual void run(
    cxy::IOR<omnicxy::proto::interop::b::reflect> const& f_) throw()
  {
    cxy::cref<omnicxy::proto::interop::b::reflect> f(orb_,f_);

    if (step_==1) {
      std::tuple<int32_t,bool,omnicxy::proto::interop::b::T> y(
        f->f(step_,"fred",32));
      xju::assert_equal(std::get<0>(y),step_+1);
      xju::assert_equal(std::get<1>(y),false);
      xju::assert_equal(std::get<2>(y),omnicxy::proto::interop::b::T(
                          32,"fred"));
    }
    done_=true;
  }
};


int main_(uint16_t const port, std::string const& testBCxxExe) throw(
  cxy::Exception)
{
  try {
    std::string const OBJECT_NAME("client");

    std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
    cxy::ORB<cxy::Exception> orb(orbEndPoint);

    // we run a new subprocess for each step because if we do them all
    // in a single subprocess the subprocess gets corruption somewhere - we're
    // not really interested in debugging cxx mapping code
    for (auto step: {1}){
      server_impl x(orb,step);
    
      cxy::sref<omnicxy::proto::interop::b::server> const xa(
        orb, OBJECT_NAME, x);
      int exitStatus(0);
      {
        auto p(xju::pipe(true,false));
        xju::Subprocess sp(
          exitStatus,
          [&](){
            xju::exec(
              testBCxxExe, {testBCxxExe, makeURI(port,OBJECT_NAME)});
            return 0;
          },
          [&](pid_t pid){
            // parent process "stop" function - the child process should
            // exit of its own accord straight away, closing its end
            // of the pipe
            auto const deadline(std::chrono::system_clock::now()+
                                std::chrono::seconds(5));
            try {
              char c;
              size_t const x(p.first->read(&c,1,deadline));
              xju::assert_never_reached();
            }
            catch(xju::io::Input::Closed const&) {
              // child should already be exited: if it has this kill
              // will have no effect
              xju::syscall(xju::kill,XJU_TRACED)(pid,6);
            }
          });
        p.second.reset(); // close pipe write end in parent
      }
      xju::assert_equal(exitStatus,0);
      xju::assert_equal(x.done_,true);
    }
    return 0;
  }
  catch(cxy::Exception& e) {
    std::ostringstream s;
    s << "run using port " << port;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "usage:  " 
                << argv[0] << " <test-b-cxx-exe>" << std::endl;
      return 1;
    }
    
    for(uint16_t port=3487; port!=4587; ++port) {
      try {
        return main_(port,argv[1]);
      }
      catch(cxy::PortInUse&) {
      }
    }
    
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
  catch(cxy::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
