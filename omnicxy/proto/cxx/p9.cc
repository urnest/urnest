// This file is generated by omniidl (C++ backend) - omniORB_4_3. Do not edit.

#include "p9.hh"
#include <omniORB4/IOP_S.h>
#include <omniORB4/IOP_C.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/callHandle.h>
#include <omniORB4/objTracker.h>


OMNI_USING_NAMESPACE(omni)

static const char* _0RL_library_version = omniORB_4_3;



p9::F_ptr p9::F_Helper::_nil() {
  return ::p9::F::_nil();
}

::CORBA::Boolean p9::F_Helper::is_nil(::p9::F_ptr p) {
  return ::CORBA::is_nil(p);

}

void p9::F_Helper::release(::p9::F_ptr p) {
  ::CORBA::release(p);
}

void p9::F_Helper::marshalObjRef(::p9::F_ptr obj, cdrStream& s) {
  ::p9::F::_marshalObjRef(obj, s);
}

p9::F_ptr p9::F_Helper::unmarshalObjRef(cdrStream& s) {
  return ::p9::F::_unmarshalObjRef(s);
}

void p9::F_Helper::duplicate(::p9::F_ptr obj) {
  if (obj && !obj->_NP_is_nil())  omni::duplicateObjRef(obj);
}

p9::F_ptr
p9::F::_duplicate(::p9::F_ptr obj)
{
  if (obj && !obj->_NP_is_nil())  omni::duplicateObjRef(obj);
  return obj;
}

p9::F_ptr
p9::F::_narrow(::CORBA::Object_ptr obj)
{
  if (!obj || obj->_NP_is_nil() || obj->_NP_is_pseudo()) return _nil();
  _ptr_type e = (_ptr_type) obj->_PR_getobj()->_realNarrow(_PD_repoId);
  return e ? e : _nil();
}


p9::F_ptr
p9::F::_unchecked_narrow(::CORBA::Object_ptr obj)
{
  if (!obj || obj->_NP_is_nil() || obj->_NP_is_pseudo()) return _nil();
  _ptr_type e = (_ptr_type) obj->_PR_getobj()->_uncheckedNarrow(_PD_repoId);
  return e ? e : _nil();
}

p9::F_ptr
p9::F::_nil()
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

const char* p9::F::_PD_repoId = "IDL:p9/F:1.0";


p9::_objref_F::~_objref_F() {
  
}


p9::_objref_F::_objref_F(omniIOR* ior, omniIdentity* id) :
   omniObjRef(::p9::F::_PD_repoId, ior, id, 1)
   
   
{
  _PR_setobj(this);
}

void*
p9::_objref_F::_ptrToObjRef(const char* id)
{
  if (id == ::p9::F::_PD_repoId)
    return (::p9::F_ptr) this;
  
  if (id == ::CORBA::Object::_PD_repoId)
    return (::CORBA::Object_ptr) this;

  if (omni::strMatch(id, ::p9::F::_PD_repoId))
    return (::p9::F_ptr) this;
  
  if (omni::strMatch(id, ::CORBA::Object::_PD_repoId))
    return (::CORBA::Object_ptr) this;

  return 0;
}


//
// Code for p9::F::f1

// Proxy call descriptor class. Mangled signature:
//  _wvoid_i_cstring
class _0RL_cd_c6f737071337bfb6_00000000
  : public omniCallDescriptor
{
public:
  inline _0RL_cd_c6f737071337bfb6_00000000(LocalCallFn lcfn, const char* op_, size_t oplen, _CORBA_Boolean upcall=0)
    : omniCallDescriptor(lcfn, op_, oplen, 1, _user_exns, 0, upcall)
  {
    
  }
  
  void marshalArguments(cdrStream&);
  void unmarshalArguments(cdrStream&);

    
  
  static const char* const _user_exns[];

  ::CORBA::String_var arg_0_;
  const char* arg_0;
};

void _0RL_cd_c6f737071337bfb6_00000000::marshalArguments(cdrStream& _n)
{
  _n.marshalString(arg_0,0);

}

void _0RL_cd_c6f737071337bfb6_00000000::unmarshalArguments(cdrStream& _n)
{
  arg_0_ = _n.unmarshalString(0);
  arg_0 = arg_0_.in();

}

const char* const _0RL_cd_c6f737071337bfb6_00000000::_user_exns[] = {
  0
};

// Local call call-back function.
static void
_0RL_lcfn_c6f737071337bfb6_10000000(omniCallDescriptor* cd, omniServant* svnt)
{
  _0RL_cd_c6f737071337bfb6_00000000* tcd = (_0RL_cd_c6f737071337bfb6_00000000*)cd;
  p9::_impl_F* impl = (p9::_impl_F*) svnt->_ptrToInterface(p9::F::_PD_repoId);
  impl->f1(tcd->arg_0);


}

void p9::_objref_F::f1(const char* x)
{
  _0RL_cd_c6f737071337bfb6_00000000 _call_desc(_0RL_lcfn_c6f737071337bfb6_10000000, "f1", 3);
  _call_desc.arg_0 = x;

  _invoke(_call_desc);



}

p9::_pof_F::~_pof_F() {}


omniObjRef*
p9::_pof_F::newObjRef(omniIOR* ior, omniIdentity* id)
{
  return new ::p9::_objref_F(ior, id);
}


::CORBA::Boolean
p9::_pof_F::is_a(const char* id) const
{
  if (omni::ptrStrMatch(id, ::p9::F::_PD_repoId))
    return 1;
  
  return 0;
}

const p9::_pof_F _the_pof_p9_mF;

p9::_impl_F::~_impl_F() {}


::CORBA::Boolean
p9::_impl_F::_dispatch(omniCallHandle& _handle)
{
  const char* op = _handle.operation_name();

  if (omni::strMatch(op, "f1")) {

    _0RL_cd_c6f737071337bfb6_00000000 _call_desc(_0RL_lcfn_c6f737071337bfb6_10000000, "f1", 3, 1);
    
    _handle.upcall(this,_call_desc);
    return 1;
  }


  return 0;
}

void*
p9::_impl_F::_ptrToInterface(const char* id)
{
  if (id == ::p9::F::_PD_repoId)
    return (::p9::_impl_F*) this;
  
  if (id == ::CORBA::Object::_PD_repoId)
    return (void*) 1;

  if (omni::strMatch(id, ::p9::F::_PD_repoId))
    return (::p9::_impl_F*) this;
  
  if (omni::strMatch(id, ::CORBA::Object::_PD_repoId))
    return (void*) 1;
  return 0;
}

const char*
p9::_impl_F::_mostDerivedRepoId()
{
  return ::p9::F::_PD_repoId;
}

POA_p9::F::~F() {}

