from omniidl import idlast
from omniidl import idltype

import sys
import os.path

operation_t='''
class %(name)s 
{
public:
  static void lcfn(omniCallDescriptor*, omniServant* svnt) throw()
  {
    %(fqn)s* impl = (%(fqn)s*) svnt->_ptrToInterface(cxy::cdr< ::%(fqn)s>::repoId);
    impl->%(name)s();
  }
  static const char* const _user_exns[] = {
    0
  };
};
'''

dispatcher_t='''
  if (omni::strMatch(op, "%(name)s")) {
    omniCallDescriptor c(%(name)s::lcfn, "%(name)s", 3, 0, %(name)s::_user_exns, 0, 1);
    _handle.upcall(impl_, c);
    return 1;
  }
'''

interface_t='''\
%(forward)s

namespace cxy
{
class sref_impl;

template<class BaseException>
class ORB;

template<>
class sref< ::%(fqn)s> :  private sref_if
{
public:
  // pre: lifetime(x) includes lifetime(this)
  // pre: lifetime(orb) includes lifetime(this)
  sref(cxy::ORB<%(eclass)s>& orb, 
       std::string const& name,
       ::%(fqn)s& x) throw(
         cxy::Exceptions<%(eclass)s>::DuplicateName) try:
      name_(name),
      x_(x),
      c_(guard_),
      impl_deleted_(false),
      impl_(new cxy::sref_impl(orb.impl_, 
                               name, 
                               cxy::cdr< ::%(fqn)s>::repoId, 
                               *this))
  {
  }
  catch(cxy::Exceptions<cxy::Exception>::DuplicateName& e)
  {
    if (typeid(%(eclass)s)==typeid(cxy::Exception)) {
      throw;
    }
    cxy::Exceptions<%(eclass)s>::DuplicateName ee(
      e.cause_.first, e.cause_.second);
    for (std::vector<std::pair<std::string, cxy::Exception::FileAndLine> >::const_iterator i=
           e.context_.begin();
         i != e.context_.end();
         ++i) {
      ee.addContext((*i).first, (*i).second);
    }
    throw ee;
  }
  ~sref() throw()
  {
    xju::mt::Lock l(guard_);
    // impl might be deleted during deactivate or sometime after, which
    // is why we use a separate thread
    xju::mt::Thread<sref_impl> t(*impl_, &sref_impl::deactivate);
    while(!impl_deleted_) {
      c_.wait(l);
    }
  }

  std::string const name_;
  ::%(fqn)s& x_;
  
private:
  %(operations)s

  // sref_if::
  virtual bool _dispatch(omniCallHandle& _handle) throw()
  {
    const char* op = _handle.operation_name();
    
    %(dispatchers)s
    return 0;
  }

  // sref_if::
  virtual void* _ptrToInterface(const char* id) throw()
  {
    if (id == cxy::cdr< ::%(fqn)s>::repoId) {
      return &x_;
    }
    if (id == ::CORBA::Object::_PD_repoId) {
      return (void*) 1;
    }
    if (omni::strMatch(id, cxy::cdr< ::%(fqn)s>::repoId)) {
      return &x_;
    }
    if (omni::strMatch(id, ::CORBA::Object::_PD_repoId)) {
      return (void*) 1;
    }
    return 0;
  }
  
  // sref_if::
  virtual void impl_deleted() throw()
  {
    xju::mt::Lock l(guard_);
    impl_deleted_=true;
    c_.signal(l);
  }

  xju::mt::Mutex guard_;
  xju::mt::Condition c_;
  bool impl_deleted_;
  cxy::sref_impl* impl_;
  
};
}'''

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

def genOperation(decl,eclass,eheader,indent,fqn):
    result=''
    assert isinstance(decl, idlast.Operation), repr(decl)
    name=decl.identifier()
    assert not decl.oneway(), 'oneway not yet implemented'
    assert len(decl.parameters())==0, 'parameters not yet implemented'
    assert len(decl.raises())==0, 'raises not yet implemented'
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
    
    result=reindent(indent,operation_t%vars())
    return result

def genDispatcher(decl,eclass,eheader,indent,fqn):
    assert isinstance(decl, idlast.Operation), repr(decl)
    name=decl.identifier()
    assert not decl.oneway(), 'oneway not yet implemented'
    assert len(decl.parameters())==0, 'parameters not yet implemented'
    assert len(decl.raises())==0, 'raises not yet implemented'
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
    
    result=reindent(indent,dispatcher_t%vars())
    return result

def genForward(scopedName):
    if len(scopedName)==1:
        return 'class %s;\n' % scopedName[0]
    return 'namespace %s'%scopedName[0]+' {\n'+\
        genForward(scopedName[1:])+\
        '}\n'

def gen(decl,eclass,eheader,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(
            gen(_,eclass,eheader,indent) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        forward=genForward(decl.scopedName())
        operations=''.join(
            [genOperation(_,eclass,eheader,indent+'  ',fqn) \
                 for _ in decl.contents() \
                 if isinstance(_,idlast.Operation)])
        dispatchers=''.join(
            [genDispatcher(_,eclass,eheader,indent+'  ',fqn) \
                 for _ in decl.contents()\
                 if isinstance(_,idlast.Operation)])
        result=interface_t%vars()
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxysref idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all exceptions
#include <cxy/sref.hh>
#include <cxy/sref_if.hh>
#include <cxy/Exceptions.hh>
#include %(eheader)s

#include "p1.hh" // impl
#include "p1.cdr.hh" // impl

#include <cxy/ORB.hh> // impl
#include <cxy/sref_impl.hh> // impl
#include <omniORB4/callDescriptor.h> // impl
#include <omniORB4/callHandle.h> // impl
#include <omniORB4/omniServant.h> // impl

#include <xju/mt.hh>
#include <string>

class omniCallHandle;
class omniCallDescriptor;
class omniServant;

%(items)s
'''

def run(tree, args):
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args if _.startswith('-e')]+[('cxy::Exception','cxy/Exception.hh')])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join(
        [gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    print template % vars()
    pass
