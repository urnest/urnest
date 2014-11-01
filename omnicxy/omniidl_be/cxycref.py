from omniidl import idlast
from omniidl import idltype

import sys
import os.path

operation_t='''
void %(name)s() throw(
  // ipc failure
  cxy::Exception)
{
  xju::assert_not_equal(obj_, (void*)0);
  xju::assert_not_equal(true, obj_->_NP_is_nil());
  obj_->%(name)s();
}
'''

interface_t='''\
template<>
class cref< ::%(fqn)s>
{
public:
  explicit cref(cxy::ORB<cxy::Exception>& orb, std::string const& uri) throw(
    // no object with specified uri, including server
    // not reachable and server does not know name
    cxy::Exceptions<cxy::Exception>::NoSuchObject,
    // object with specified uri is not a %(fqn)s
    cxy::Exceptions<cxy::Exception>::WrongType,
    // other failure, eg communication failure
    cxy::Exception):
      uri_(uri),
      obj_((&cxy::pof< ::%(fqn)s>::me_(), // force init of static var,
            (cxy::objref< ::%(fqn)s>*)orb.locate(
              uri, cxy::cdr< ::%(fqn)s>::repoId)))
  {
  }
  
  cref(cref const& b) throw():
      uri_(b.uri_),
      obj_(b.obj_)
  {
    if (obj_ && !obj_->_NP_is_nil())  omni::duplicateObjRef(obj_);
  }

  cref< ::%(fqn)s>& operator=(cref< ::%(fqn)s> const& b) throw()
  {
    if (this != &b) {
      ::CORBA::release(obj_);
      uri_= b.uri_;
      obj_ = b.obj_;
      if (obj_ && !obj_->_NP_is_nil())  omni::duplicateObjRef(obj_);
    }
    return *this;
  }

  ~cref() throw()
  {
    ::CORBA::release(obj_);
  }
  
  %(fqn)s* operator->() throw()
  {
    return obj_;
  }
  %(fqn)s const* operator->() const throw()
  {
    return obj_;
  }
  %(content)s
private:
  std::string uri_;
  cxy::objref< ::%(fqn)s>* obj_;
};
'''

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

def gen(decl,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        content=''.join([gen(_,indent+'  ') for _ in decl.contents()])
        result=interface_t%vars()
    elif isinstance(decl, idlast.Operation):
        name=decl.identifier()
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.parameters())==0, 'parameters not yet implemented'
        assert len(decl.raises())==0, 'raises not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
        result=reindent(indent,operation_t%vars())
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxycref idl backend specifying 
// cxy::Exception from "cxy/Exception.hh" as base class for all exceptions
#include <cxy/cref.hh>
#include <cxy/Exceptions.hh>

#include "%(baseName)s.hh"

#include "%(baseName)s.objref.hh" // impl
#include "%(baseName)s.cdr.hh" // impl
#include <cxy/ORB.hh> // impl
#include <cxy/translateException.hh> // impl
#include <xju/format.hh> // impl
#include <xju/assert.hh> // impl
#include <string>

namespace cxy
{
template<class E>
class ORB;

template<class T>
class objref;

%(items)s
}
'''

def run(tree, args):
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_) for _ in tree.declarations() if _.mainFile()])
    print template % vars()
    pass
