// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/interop/a.hh>
#include <omnicxy/proto/interop/a.cref.hh>
#include <omnicxy/proto/interop/a.sref.hh>
#include <omnicxy/proto/interop/a.any.hh>
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

class server_impl : public omnicxy::proto::interop::a::server
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
  
  //omnicxy::proto::interop::a::server::
  virtual void run(
    cxy::IOR<omnicxy::proto::interop::a::reflect> const& f_) throw()
  {
    cxy::cref<omnicxy::proto::interop::a::reflect> f(orb_,f_);

    if (step_==1) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((int16_t)-997)));
      auto const z(y.get<int16_t>());
      xju::assert_equal(z,(int16_t)-997);
    }
    if (step_==2) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((int32_t)-100997)));
      auto const z(y.get<int32_t>());
      xju::assert_equal(z,(int32_t)-100997);
    }
    if (step_==3) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((uint16_t)997)));
      auto const z(y.get<uint16_t>());
      xju::assert_equal(z,(uint16_t)997);
    }
    if (step_==4) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((uint32_t)100997)));
      auto const z(y.get<uint32_t>());
      xju::assert_equal(z,(uint32_t)100997);
    }
    if (step_==5) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((float)997.5e-1)));
      auto const z(y.get<float>());
      xju::assert_equal(z,(float)997.5e-1);
    }
    if (step_==6) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>((double)100997.7e-3)));
      auto const z(y.get<double>());
      xju::assert_equal(z,(double)100997.7e-3);
    }
    if (step_==7) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>('g')));
      auto const z(y.get<char>());
      xju::assert_equal(z,'g');
    }
    if (step_==8) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>(true)));
      auto const z(y.get<bool>());
      xju::assert_equal(z,true);
    }
    if (step_==9) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>(std::string("fred"))));
      auto const z(y.get<std::string>());
      xju::assert_equal(z,"fred");
    }
    if (step_==10) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>(
                                omnicxy::proto::interop::a::S(5,"fred"))));
      auto const z(y.get<omnicxy::proto::interop::a::S>());
      xju::assert_equal(z,omnicxy::proto::interop::a::S(5,"fred"));
    }
    if (step_==11) {
      cxy::Any<> const y(
        f->f(
          step_,
          cxy::Any<>(
            std::shared_ptr<omnicxy::proto::interop::a::U1 const>(
              new omnicxy::proto::interop::a::U1::V<1>(5)))));
      auto const z(y.get<std::shared_ptr<omnicxy::proto::interop::a::U1 const> >());
      xju::assert_equal(*z,omnicxy::proto::interop::a::U1::V<1>(5));
    }
    if (step_==12) {
      cxy::Any<> const y(
        f->f(
          step_,
          cxy::Any<>(
            std::shared_ptr<omnicxy::proto::interop::a::U2 const>(
              new omnicxy::proto::interop::a::U2::EV1(5)))));
      auto const z(y.get<std::shared_ptr<omnicxy::proto::interop::a::U2 const> >());
      xju::assert_equal(*z,omnicxy::proto::interop::a::U2::EV1(5));
    }
    if (step_==13) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>(
                                cxy::createTypeCodeOf<uint32_t>())));
      auto const z(y.get< cxy::TypeCode >());
      xju::assert_equal(z,cxy::createTypeCodeOf<uint32_t>());
    }
    if (step_==14) {
       //test cxx->cxy direction
      omnicxy::proto::interop::a::Tree const x(
        "root",
        {omnicxy::proto::interop::a::Tree("fred",{})});
      cxy::Any<> const y(f->f(step_,cxy::Any<>(1)));
      auto const z(y.get< omnicxy::proto::interop::a::Tree >());
      xju::assert_equal(z,x);
    }
    if (step_==15) {
      omnicxy::proto::interop::a::Tree const x(
        "root",
        {omnicxy::proto::interop::a::Tree("fred",{})});
      cxy::Any<> const y(f->f(step_,cxy::Any<>(x)));
      auto const z(y.get< omnicxy::proto::interop::a::Tree >());
      xju::assert_equal(z,x);
    }
    if (step_==16) {
      omnicxy::proto::interop::a::X x(
        "file not found",
        cxy::Exception("file not found", {__FILE__,__LINE__}));
      cxy::Any<> const y(f->f(step_,cxy::Any<>(x)));
      auto const z(y.get< omnicxy::proto::interop::a::X >());
      xju::assert_equal(z.message_,x.message_);
    }
    if (step_==17) {
      cxy::Any<> const y(f->f(step_,cxy::Any<>(f_)));
      auto const z(y.get< cxy::IOR<omnicxy::proto::interop::a::reflect> >());
      xju::assert_equal(z,f_);
    }
    if (step_==18) {
      xju::Array<omnicxy::proto::interop::a::S,3> const x{
        omnicxy::proto::interop::a::S(5,"fred"),
        omnicxy::proto::interop::a::S(6,"jock"),
        omnicxy::proto::interop::a::S(2,"sally")};
          
      cxy::Any<> const y(f->f(step_,cxy::Any<>(x)));
      auto const z(y.get< xju::Array<omnicxy::proto::interop::a::S,3> >());
      xju::assert_equal(z,x);
    }
    done_=true;
  }
};


int main_(uint16_t const port, std::string const& testAnyCxxExe) throw(
  cxy::Exception)
{
  try {
    std::string const OBJECT_NAME("client");

    std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
    cxy::ORB<cxy::Exception> orb(orbEndPoint);

    // we run a new subprocess for each step because if we do them all
    // in a single subprocess the subprocess gets corruption somewhere - we're
    // not really interested in debugging cxx mapping code
    for (auto step: {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18}){
      server_impl x(orb,step);
    
      cxy::sref<omnicxy::proto::interop::a::server> const xa(orb, OBJECT_NAME, x);
      int exitStatus(0);
      {
        auto p(xju::pipe(true,false));
        xju::Subprocess sp(
          exitStatus,
          [&](){
            xju::exec(
              testAnyCxxExe, {testAnyCxxExe, makeURI(port,OBJECT_NAME)});
            return 0;
          },
          [&](pid_t pid){
            // parent process "stop" function - the child process should
            // exit of its own accord straight away, closing its end
            // of the pipe
            auto const deadline(std::chrono::steady_clock::now()+
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
                << argv[0] << " <test-Any-cxx-exe>" << std::endl;
      return 1;
    }
    
    for(uint16_t port=3387; port!=4387; ++port) {
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
