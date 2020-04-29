// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <iostream>
#include <xju/assert.hh>

#include <omnicxy/proto/CosNaming.sref.hh>
#include <omnicxy/proto/CosNaming.cref.hh>
#include <omnicxy/proto/p1.hh>
#include <omnicxy/proto/p1.cref.hh>
#include <omnicxy/proto/p1.sref.hh>
#include <chrono>
#include <xju/steadyNow.hh>
#include <cxy/Exception.hh>
#include <cxy/IOR.hh>
#include <string>
#include <sstream>
#include <xju/io/OStream.hh>
#include <vector>
#include <xju/format.hh>
#include <cxy/ORB.hh>
#include <xju/pipe.hh>
#include <xju/Subprocess.hh>
#include <tuple>
#include <xju/stringToUInt.hh>

class F_impl : public p1::F
{
public:
  ~F_impl() throw() {}
  
  virtual void f1() /*throw(cxy::Exception)*/ override
  {
  }
};


class NC1 : public CosNaming::NamingContext
{
public:
  explicit NC1(cxy::IOR< p1::F > x) noexcept
    : x_(x)
  {
  }
  cxy::IOR< p1::F > x_;

  virtual void bind(
    ::CosNaming::Name const& n,
    ::cxy::IOR< void > const& obj) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void rebind(
    ::CosNaming::Name const& n,
    ::cxy::IOR< void > const& obj) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual void bind_context(
    ::CosNaming::Name const& n,
    ::cxy::IOR< ::CosNaming::NamingContext > const& nc) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void rebind_context(
    ::CosNaming::Name const& n,
    ::cxy::IOR< ::CosNaming::NamingContext > const& nc) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< void > resolve(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_equal(n,CosNaming::Name({
          CosNaming::NameComponent(
            CosNaming::Istring("X"),CosNaming::Istring(""))}));
    return x_;
  }
  virtual void unbind(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< ::CosNaming::NamingContext > new_context() throw()
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< ::CosNaming::NamingContext > bind_new_context(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void destroy() /*throw(
    CosNaming::NamingContext::NotEmpty)*/
  {
    xju::assert_never_reached();
  }
  virtual std::tuple< ::CosNaming::BindingList,::cxy::IOR< ::CosNaming::BindingIterator > > list(
    uint32_t const& how_many) noexcept override
  {
    return std::make_tuple(
      CosNaming::BindingList({
        CosNaming::Binding(
          CosNaming::Name{
            CosNaming::NameComponent(
              CosNaming::Istring("X"),CosNaming::Istring(""))},
        CosNaming::BindingType::nobject),
          CosNaming::Binding(
            CosNaming::Name{
              CosNaming::NameComponent(
                CosNaming::Istring("Y"),CosNaming::Istring("Ty"))},
            CosNaming::BindingType::ncontext)}),
      ::cxy::IOR< ::CosNaming::BindingIterator >());
  }
};

class NC2 : public CosNaming::NamingContext
{
public:
  explicit NC2(cxy::IOR< CosNaming::NamingContext > nc1) noexcept
    : nc1_(nc1)
  {
  }
  cxy::IOR< CosNaming::NamingContext > nc1_;

  virtual void bind(
    ::CosNaming::Name const& n,
    ::cxy::IOR< void > const& obj) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void rebind(
    ::CosNaming::Name const& n,
    ::cxy::IOR< void > const& obj) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual void bind_context(
    ::CosNaming::Name const& n,
    ::cxy::IOR< ::CosNaming::NamingContext > const& nc) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void rebind_context(
    ::CosNaming::Name const& n,
    ::cxy::IOR< ::CosNaming::NamingContext > const& nc) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< void > resolve(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_equal(n,CosNaming::Name({
          CosNaming::NameComponent(
            CosNaming::Istring("NC1"),CosNaming::Istring(""))}));
    return nc1_;
  }
  virtual void unbind(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName)*/
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< ::CosNaming::NamingContext > new_context() throw()
  {
    xju::assert_never_reached();
  }
  virtual ::cxy::IOR< ::CosNaming::NamingContext > bind_new_context(
    ::CosNaming::Name const& n) /*throw(
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::AlreadyBound)*/
  {
    xju::assert_never_reached();
  }
  virtual void destroy() /*throw(
    CosNaming::NamingContext::NotEmpty)*/
  {
    xju::assert_never_reached();
  }
  virtual std::tuple< ::CosNaming::BindingList,::cxy::IOR< ::CosNaming::BindingIterator > > list(
    uint32_t const& how_many) noexcept
  {
    return std::make_tuple(
      CosNaming::BindingList({
        CosNaming::Binding(
          CosNaming::Name{
            CosNaming::NameComponent(
              CosNaming::Istring("NC1"),CosNaming::Istring(""))},
          CosNaming::BindingType::ncontext)}),
      ::cxy::IOR< ::CosNaming::BindingIterator >());
  }
};

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class Client
{
public:
  Client(std::string const& namingClientExe,
         xju::io::OStream& stdout,
         std::vector<std::string> const& args) noexcept
    : namingClientExe_(namingClientExe),
    stdout_(stdout),
    args_(args)
  {
  }
  std::string const namingClientExe_;
  xju::io::OStream& stdout_;
  std::vector<std::string> const args_;

  int operator()() noexcept
  {
    try{
      stdout_.useAsStdout();
      xju::exec(namingClientExe_,args_);
      return 0;
    }
    catch(xju::Exception& e){
      e.addContext(xju::format::join(args_.begin(),args_.end(),
                                     std::string(" ")),XJU_TRACED);
      std::cerr << readableRepr(e) << std::endl;
      return 1;
    }
  }
};

void test1(uint16_t const port, std::string const& namingClientExe)
{
  auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
  cxy::ORB<cxy::Exception> orb("giop:tcp::"+xju::format::str(port));

  F_impl x;
      
  cxy::sref<p1::F> const xs(orb, "x", x);

  NC1 nc1(xs.ior());

  cxy::sref<CosNaming::NamingContext> nc1s(orb, "nc1", nc1);
  
  NC2 nc2(nc1s.ior());

  cxy::sref<CosNaming::NamingContext> nc2s(orb, "nc2", nc2);

  {
    auto clientOutput{xju::pipe(true,false)};
    int exitStatus;
    std::string output;
    try
    {
      Client client(
        namingClientExe,*clientOutput.second,
        std::vector<std::string>({namingClientExe,makeURI(port,"nc2"),"NC1"}));
      xju::Subprocess p(exitStatus,client);
      clientOutput.second=std::unique_ptr<xju::io::OStream>(); //close write end of pipe now that child has started
      std::vector<char> buffer;
      while(xju::steadyNow()<deadline){
        buffer.resize(1024);
        buffer.resize(clientOutput.first->read(buffer.data(),buffer.size(),
                                               deadline));
        std::copy(buffer.begin(),buffer.end(),std::back_inserter(output));
      }
      xju::assert_never_reached(); //deadline reached
    }
    catch(xju::io::Input::Closed const&)
    {
    }
    xju::assert_equal(output,"NC1/X\nNC1/Y:Ty/\n");
  }
  {
    auto clientOutput{xju::pipe(true,false)};
    int exitStatus;
    std::string output;
    try
    {
      Client client(
        namingClientExe,*clientOutput.second,
        std::vector<std::string>(
          {namingClientExe,
           makeURI(port,"nc2"),
           "NC1/X"}));
      xju::Subprocess p(exitStatus,client);
      //close write end of pipe now that child has started
      clientOutput.second=std::unique_ptr<xju::io::OStream>();
      //read all output
      std::vector<char> buffer;
      while(xju::steadyNow()<deadline){
        buffer.resize(1024);
        buffer.resize(clientOutput.first->read(buffer.data(),buffer.size(),
                                               deadline));
        std::copy(buffer.begin(),buffer.end(),std::back_inserter(output));
      }
      xju::assert_never_reached(); //deadline reached
    }
    catch(xju::io::Input::Closed const&)
    {
    }
    xju::assert_startswith(output,std::string("NC1/X="));
  }
}

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(xju::stringToUInt(argv[1]),argv[2]), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

