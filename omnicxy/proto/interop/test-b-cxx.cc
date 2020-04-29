// Copyright (c) 2015 Trevor Taylor
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

#include "b.hh"
#include "xju/Exception.hh"
#include <typeinfo>

class reflect_impl : public POA_omnicxy::proto::interop::b::reflect
{
public:
  virtual ::CORBA::Long f(::CORBA::Long step,
                          const char* x,
                          ::CORBA::Long y,
                          ::CORBA::Boolean& eq,
                          ::omnicxy::proto::interop::b::T_out z) override
  {
    switch(step) {
    case 1:
    {
      auto z_=new ::omnicxy::proto::interop::b::T;
      z_->x_=32;
      z_->y_=CORBA::string_dup(x);
      eq=(std::string(x).size()==y);
      z=z_;
      return step+1;
    }
    default:
      xju::assert_never_reached();
    }
  }
};


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

CORBA::ORB_var orbInit(int argc, char* argv[]) /*throw(
  xju::Exception)*/
{
  try {
    return CORBA::ORB_init(argc, argv);
  }
  catch (CORBA::Exception& ex) {
    throw translate(ex);
  }
}

int main(int argc, char* argv[])
{
  if (argc!=2) {
    std::cerr << "usage: " << argv[0] << " a::server-uri" << std::endl;
    std::cerr << "(see test-Any-cxy)" << std::endl;
    return 1;
  }
  std::string server_uri(argv[1]);

  CORBA::ORB_var orb = orbInit(argc, argv);

  PortableServer::POA_var root_poa = PortableServer::POA::_narrow(
    orb->resolve_initial_references("omniINSPOA"));
  PortableServer::POAManager_var pman = root_poa->the_POAManager();
  pman->activate();
  
  PortableServer::ObjectId_var oid =
    PortableServer::string_to_ObjectId("reflect");
    
  reflect_impl* f = new reflect_impl();
    
  root_poa->activate_object_with_id(oid,f);
    
  f->_remove_ref();

  CORBA::Object_var obj = orb->string_to_object(server_uri.c_str());
  
  omnicxy::proto::interop::b::server_var server = omnicxy::proto::interop::b::server::_narrow(obj);
  
  if (CORBA::is_nil(server)) {
    throw xju::Exception(
      "Can't narrow reference to type a::server (or it was nil).",
      XJU_TRACED);
  }
  
  server->run(f->_this());
  
  return 0;
}
