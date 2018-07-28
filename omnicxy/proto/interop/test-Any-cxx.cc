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

#include "a.hh"
#include "xju/Exception.hh"
#include <typeinfo>

class reflect_impl : public POA_omnicxy::proto::interop::a::reflect
{
public:
  virtual ::CORBA::Any* f(CORBA::Long step, const ::CORBA::Any& x)
  {
    switch(step) {
    case 1:
    {
      CORBA::Short y;
      x >>= y;
      xju::assert_equal(y, -997);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 2:
    {
      CORBA::Long y;
      x >>= y;
      xju::assert_equal(y, -100997);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 3:
    {
      CORBA::UShort y;
      x >>= y;
      xju::assert_equal(y, 997);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 4:
    {
      CORBA::ULong y;
      x >>= y;
      xju::assert_equal(y, 100997);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 5:
    {
      CORBA::Float y;
      x >>= y;
      xju::assert_equal(y, 997.5e-1);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 6:
    {
      CORBA::Double y;
      x >>= y;
      xju::assert_equal(y, 100997.7e-3);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 7:
    {
      CORBA::Char y;
      x >>= CORBA::Any::to_char(y);
      xju::assert_equal(y, 'g');
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= CORBA::Any::from_char(y);
      return result;
    }
    case 8:
    {
      CORBA::Boolean y;
      x >>= CORBA::Any::to_boolean(y);
      xju::assert_equal(y, 1);
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= CORBA::Any::from_boolean(y);
      return result;
    }
    case 9:
    {
      char* y(0);
      x >>= CORBA::Any::to_string(y,0);
      xju::assert_equal(y, std::string("fred"));
      char* z(CORBA::string_dup(y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= CORBA::Any::from_string(z,0);
      return result;
    }
    case 10:
    {
      omnicxy::proto::interop::a::S* y(0);
      x >>= y;
      xju::assert_equal(y->x_, 5);
      xju::assert_equal(std::string(y->y_), std::string("fred"));
      omnicxy::proto::interop::a::S* z(
        new omnicxy::proto::interop::a::S(*y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= z;
      return result;
    }
    case 11:
    {
      omnicxy::proto::interop::a::U1* y(0);
      x >>= y;
      xju::assert_equal(y->_d(), 1);
      xju::assert_equal(y->x_(), 5);
      omnicxy::proto::interop::a::U1* z(
        new omnicxy::proto::interop::a::U1(*y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= z;
      return result;
    }
    case 12:
    {
      omnicxy::proto::interop::a::U2* y(0);
      x >>= y;
      xju::assert_equal(y->_d(), omnicxy::proto::interop::a::EV1);
      xju::assert_equal(y->x_(), 5);
      omnicxy::proto::interop::a::U2* z(
        new omnicxy::proto::interop::a::U2(*y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= z;
      return result;
    }
    case 13:
    {
      CORBA::TypeCode* y(0);
      x >>= y;
      //note cannot copy CORBA::TypeCode but can copy
      //CORBA::Any containing TypeCode
      CORBA::Any* result(new CORBA::Any(x));
      return result;
    }
    case 14:
    {
      CORBA::Long y_;
      x >>= y_;
      xju::assert_equal(y_, 1);
      omnicxy::proto::interop::a::Tree* y;
      y=new omnicxy::proto::interop::a::Tree;
      y->name_=CORBA::string_dup("root");
      y->children_.length(1);
      y->children_[0]=omnicxy::proto::interop::a::Tree();
      y->children_[0].name_=CORBA::string_dup("fred");
      x >>= y;
      xju::assert_equal(y->name_.in(), std::string("root"));
      xju::assert_equal(y->children_.length(), 1);
      xju::assert_equal(y->children_[0].name_.in(), std::string("fred"));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 15:
    {
      omnicxy::proto::interop::a::Tree* y(0);
      x >>= y;
      xju::assert_equal(y->name_.in(), std::string("root"));
      xju::assert_equal(y->children_.length(), 1);
      xju::assert_equal(y->children_[0].name_.in(), std::string("fred"));
      omnicxy::proto::interop::a::Tree* z(
        new omnicxy::proto::interop::a::Tree(*y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= z;
      return result;
    }
    case 16:
    {
      omnicxy::proto::interop::a::X const* y(0);
      x >>= y;
      xju::assert_equal(y->message_.in(), std::string("file not found"));
      omnicxy::proto::interop::a::X* z(
        new omnicxy::proto::interop::a::X(*y));
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= z;
      return result;
    }
    case 17:
    {
      omnicxy::proto::interop::a::reflect_ptr y(0);
      x >>= y;
      CORBA::Any* result(new CORBA::Any);
      (*result) <<= y;
      return result;
    }
    case 18:
    {
      omnicxy::proto::interop::a::ThreeSs_forany y;
      x >>= y;
      CORBA::Any* result(new CORBA::Any);
      omnicxy::proto::interop::a::ThreeSs_forany z(
        omnicxy::proto::interop::a::ThreeSs_dup(y));
      (*result) <<= z;
      return result;
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
  
  omnicxy::proto::interop::a::server_var server = omnicxy::proto::interop::a::server::_narrow(obj);
  
  if (CORBA::is_nil(server)) {
    throw xju::Exception(
      "Can't narrow reference to type a::server (or it was nil).",
      XJU_TRACED);
  }
  
  server->run(f->_this());
  
  return 0;
}
