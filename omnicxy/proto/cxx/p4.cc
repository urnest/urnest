// This file is generated by omniidl (C++ backend)- omniORB_4_2. Do not edit.

#include "p4.hh"
#include <omniORB4/IOP_S.h>
#include <omniORB4/IOP_C.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/callHandle.h>
#include <omniORB4/objTracker.h>


OMNI_USING_NAMESPACE(omni)

static const char* _0RL_library_version = omniORB_4_2;



void
p4::XS1::operator>>= (cdrStream &_n) const
{
  a_ >>= _n;
  _n.marshalString(b_,0);

}

void
p4::XS1::operator<<= (cdrStream &_n)
{
  (p3::MyInt&)a_ <<= _n;
  b_ = _n.unmarshalString(0);

}

p4::F_ptr p4::F_Helper::_nil() {
  return ::p4::F::_nil();
}

::CORBA::Boolean p4::F_Helper::is_nil(::p4::F_ptr p) {
  return ::CORBA::is_nil(p);

}

void p4::F_Helper::release(::p4::F_ptr p) {
  ::CORBA::release(p);
}

void p4::F_Helper::marshalObjRef(::p4::F_ptr obj, cdrStream& s) {
  ::p4::F::_marshalObjRef(obj, s);
}

p4::F_ptr p4::F_Helper::unmarshalObjRef(cdrStream& s) {
  return ::p4::F::_unmarshalObjRef(s);
}

void p4::F_Helper::duplicate(::p4::F_ptr obj) {
  if (obj && !obj->_NP_is_nil())  omni::duplicateObjRef(obj);
}

p4::F_ptr
p4::F::_duplicate(::p4::F_ptr obj)
{
  if (obj && !obj->_NP_is_nil())  omni::duplicateObjRef(obj);
  return obj;
}

p4::F_ptr
p4::F::_narrow(::CORBA::Object_ptr obj)
{
  if (!obj || obj->_NP_is_nil() || obj->_NP_is_pseudo()) return _nil();
  _ptr_type e = (_ptr_type) obj->_PR_getobj()->_realNarrow(_PD_repoId);
  return e ? e : _nil();
}


p4::F_ptr
p4::F::_unchecked_narrow(::CORBA::Object_ptr obj)
{
  if (!obj || obj->_NP_is_nil() || obj->_NP_is_pseudo()) return _nil();
  _ptr_type e = (_ptr_type) obj->_PR_getobj()->_uncheckedNarrow(_PD_repoId);
  return e ? e : _nil();
}

p4::F_ptr
p4::F::_nil()
{
#ifdef OMNI_UNLOADABLE_STUBS
  static _objref_F _the_nil_obj;
  return &_the_nil_obj;
#else
  static _objref_F* _the_nil_ptr = 0;
  if (!_the_nil_ptr) {
    omni::nilRefLock().lock();
    if (!_the_nil_ptr) {
      _the_nil_ptr = new _objref_F;
      registerNilCorbaObject(_the_nil_ptr);
    }
    omni::nilRefLock().unlock();
  }
  return _the_nil_ptr;
#endif
}

const char* p4::F::_PD_repoId = "IDL:p4/F:1.0";


p4::_objref_F::~_objref_F() {
  
}


p4::_objref_F::_objref_F(omniIOR* ior, omniIdentity* id) :
   omniObjRef(::p4::F::_PD_repoId, ior, id, 1)
   
   
{
  _PR_setobj(this);
}

void*
p4::_objref_F::_ptrToObjRef(const char* id)
{
  if (id == ::p4::F::_PD_repoId)
    return (::p4::F_ptr) this;
  
  if (id == ::CORBA::Object::_PD_repoId)
    return (::CORBA::Object_ptr) this;

  if (omni::strMatch(id, ::p4::F::_PD_repoId))
    return (::p4::F_ptr) this;
  
  if (omni::strMatch(id, ::CORBA::Object::_PD_repoId))
    return (::CORBA::Object_ptr) this;

  return 0;
}


//
// Code for p4::F::f1

// Proxy call descriptor class. Mangled signature:
//  _cp4_mXS1_i_cp4_mXS1
class _0RL_cd_c6f796071337bfb6_00000000
  : public omniCallDescriptor
{
public:
  inline _0RL_cd_c6f796071337bfb6_00000000(LocalCallFn lcfn, const char* op_, size_t oplen, _CORBA_Boolean upcall=0)
    : omniCallDescriptor(lcfn, op_, oplen, 0, _user_exns, 0, upcall)
  {
    
  }
  
  void marshalArguments(cdrStream&);
  void unmarshalArguments(cdrStream&);

  void unmarshalReturnedValues(cdrStream&);
  void marshalReturnedValues(cdrStream&);
  
  
  static const char* const _user_exns[];

  p4::XS1_var arg_0_;
  const p4::XS1* arg_0;
  p4::XS1_var result;
};

void _0RL_cd_c6f796071337bfb6_00000000::marshalArguments(cdrStream& _n)
{
  (const p4::XS1&) *arg_0 >>= _n;

}

void _0RL_cd_c6f796071337bfb6_00000000::unmarshalArguments(cdrStream& _n)
{
  arg_0_ = new p4::XS1;
  (p4::XS1&)arg_0_ <<= _n;
  arg_0 = &arg_0_.in();

}

void _0RL_cd_c6f796071337bfb6_00000000::marshalReturnedValues(cdrStream& _n)
{
  (const p4::XS1&) result >>= _n;

}

void _0RL_cd_c6f796071337bfb6_00000000::unmarshalReturnedValues(cdrStream& _n)
{
  result = new p4::XS1;
  (p4::XS1&)result <<= _n;

}

const char* const _0RL_cd_c6f796071337bfb6_00000000::_user_exns[] = {
  0
};

// Local call call-back function.
static void
_0RL_lcfn_c6f796071337bfb6_10000000(omniCallDescriptor* cd, omniServant* svnt)
{
  _0RL_cd_c6f796071337bfb6_00000000* tcd = (_0RL_cd_c6f796071337bfb6_00000000*)cd;
  p4::_impl_F* impl = (p4::_impl_F*) svnt->_ptrToInterface(p4::F::_PD_repoId);
  tcd->result = impl->f1(*tcd->arg_0);


}

p4::XS1* p4::_objref_F::f1(const ::p4::XS1& y)
{
  _0RL_cd_c6f796071337bfb6_00000000 _call_desc(_0RL_lcfn_c6f796071337bfb6_10000000, "f1", 3);
  _call_desc.arg_0 = &(::p4::XS1&) y;

  _invoke(_call_desc);
  return _call_desc.result._retn();


}

p4::_pof_F::~_pof_F() {}


omniObjRef*
p4::_pof_F::newObjRef(omniIOR* ior, omniIdentity* id)
{
  return new ::p4::_objref_F(ior, id);
}


::CORBA::Boolean
p4::_pof_F::is_a(const char* id) const
{
  if (omni::ptrStrMatch(id, ::p4::F::_PD_repoId))
    return 1;
  
  return 0;
}

const p4::_pof_F _the_pof_p4_mF;

p4::_impl_F::~_impl_F() {}


::CORBA::Boolean
p4::_impl_F::_dispatch(omniCallHandle& _handle)
{
  const char* op = _handle.operation_name();

  if (omni::strMatch(op, "f1")) {

    _0RL_cd_c6f796071337bfb6_00000000 _call_desc(_0RL_lcfn_c6f796071337bfb6_10000000, "f1", 3, 1);
    
    _handle.upcall(this,_call_desc);
    return 1;
  }


  return 0;
}

void*
p4::_impl_F::_ptrToInterface(const char* id)
{
  if (id == ::p4::F::_PD_repoId)
    return (::p4::_impl_F*) this;
  
  if (id == ::CORBA::Object::_PD_repoId)
    return (void*) 1;

  if (omni::strMatch(id, ::p4::F::_PD_repoId))
    return (::p4::_impl_F*) this;
  
  if (omni::strMatch(id, ::CORBA::Object::_PD_repoId))
    return (void*) 1;
  return 0;
}

const char*
p4::_impl_F::_mostDerivedRepoId()
{
  return ::p4::F::_PD_repoId;
}

POA_p4::F::~F() {}

