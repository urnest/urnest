// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "p2.hh"

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include "xju/format.hh"
#include <typeinfo>
#include "xju/stringToInt.hh"
#include <stdlib.h>
#include "xju/Thread.hh"

xju::Exception translate(CORBA::Exception const& e) throw()
{
  std::ostringstream s;
  s << typeid(e).name();
  {
    CORBA::SystemException const* ee=
      dynamic_cast<CORBA::SystemException const*>(&e);
    if(ee) {
#ifdef minor
  // Digital Unix 3.2, and may be others as well, defines minor() as
  // a macro in its sys/types.h. Get rid of it!
#undef minor
#endif
      s << " minor " << ee->minor();
    }
  }
  {
    CORBA::PolicyError const* ee=
      dynamic_cast<CORBA::PolicyError const*>(&e);
    if(ee) {
      s << " reason " << ee->reason;
    }
  }
  return xju::Exception(s.str(), XJU_TRACED);
}

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

CORBA::ORB_var orbInit(int argc, char* argv[]) throw(
  xju::Exception)
{
  try {
    return CORBA::ORB_init(argc, argv);
  }
  catch (CORBA::Exception& ex) {
    throw translate(ex);
  }
}

void client_(CORBA::ORB_var orb, 
            int port, 
            std::string const& objectName) throw(
              xju::Exception)
{
  try {
    CORBA::Object_var obj = orb->string_to_object(
      makeURI(port, objectName).c_str());
    
    p2::F_var ref = p2::F::_narrow(obj);
    
    if (CORBA::is_nil(ref)) {
      throw xju::Exception(
        "Can't narrow reference to type p2::F (or it was nil).",
        XJU_TRACED);
    }
    
    ref->f1(1, 2, 3.4, "fred",'k',82,true);
    std::cout << ref->f2() << std::endl;
  }
  catch(CORBA::Exception& e) {
    throw translate(e);
  }
}

void client(int argc, char* argv[], 
            int port, 
            std::string const& objectName) throw(
              xju::Exception)
{
  std::ostringstream s;
  s << makeURI(port, objectName) << "->f1()";
  try {
    try {
      CORBA::ORB_var orb = orbInit(argc, argv);
      
      client_(orb,port,objectName);
    }
    catch(CORBA::Exception& e) {
      throw translate(e);
    }
  }
  catch(xju::Exception& e) {
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

class F_impl : public POA_p2::F
{
public:
  virtual void f1(CORBA::Short a, CORBA::Long b, CORBA::Double c, const char* d, CORBA::Octet e, CORBA::ULongLong f, CORBA::Boolean g)
  {
    std::cout << "F::f1(" 
              << a << ", "
              << b << ", "
              << c << ", "
              << d << ", "
              << e << ", "
              << f << ", "
              << g << ")" << std::endl;
  }
  virtual CORBA::Short f2()
  {
    std::cout << "F::f2(" << std::endl;
    return 22;
  }
};

  
void server(CORBA::ORB_var orb, std::string const& objectName) throw(
  xju::Exception)
{
  try {
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow(
      orb->resolve_initial_references("omniINSPOA"));
    PortableServer::POAManager_var pman = root_poa->the_POAManager();
    pman->activate();
    
    PortableServer::ObjectId_var oid =
      PortableServer::string_to_ObjectId(objectName.c_str());
    
    F_impl* f = new F_impl();
    
    root_poa->activate_object_with_id(oid, f);
    
    f->_remove_ref();
    
  }
  catch (CORBA::Exception& ex) {
    throw translate(ex);
  }
}

int main(int argc, char* argv[])
{
  try {
    if (argc != 3 || !(std::string("client")==argv[2]||
                       std::string("server")==argv[2]||
                       std::string("both")==argv[2])) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port> [client|server|both]" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p2");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      client(argc, argv, port, OBJECT_NAME);
    }
    else if (argv[2]==std::string("server")) {
      std::string orbEndPoint="giop:tcp::"+xju::format::str(port);
      ::setenv("ORBendPoint", orbEndPoint.c_str(), 1);
      
      CORBA::ORB_var orb = orbInit(argc, argv);
      
      server(orb, OBJECT_NAME);
      orb->run();
    }
    else
    {
      std::string orbEndPoint="giop:tcp::"+xju::format::str(port);
      ::setenv("ORBendPoint", orbEndPoint.c_str(), 1);
      
      CORBA::ORB_var orb = orbInit(argc, argv);
      
      server(orb, OBJECT_NAME);

      xju::Thread server_t(
        [orb](){ orb->run(); },
        [orb](){ orb->destroy(); });
      
      client_(orb, port, OBJECT_NAME);
    }
    
    return 0;
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}
