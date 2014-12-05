from omniidl import idlast
from omniidl import idltype

import sys
import os.path

from cxy import ptype,unqualifiedType

interface_t='''\
template<>
class cref< ::%(fqn)s>
{
public:
  explicit cref(cxy::ORB<%(eclass)s>& orb, std::string const& uri) throw(
    // no object with specified uri, including server
    // not reachable and server does not know name
    cxy::Exceptions<%(eclass)s>::NoSuchObject,
    // object with specified uri is not a %(fqn)s
    cxy::Exceptions<%(eclass)s>::WrongType,
    // other failure, eg communication failure
    %(eclass)s):
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

def gen(decl,eclass,eheader,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_,eclass,eheader) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        content=''.join([gen(_,eclass,eheader,indent+'  ') for _ in decl.contents()])
        result=interface_t%vars()
    elif isinstance(decl, idlast.Operation):
        pass
    elif isinstance(decl, idlast.Typedef):
        pass
    elif isinstance(decl, idlast.Struct):
        pass
    elif isinstance(decl, idlast.Exception):
        pass
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxycref idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all exceptions
#include <cxy/cref.hh>
#include <cxy/Exceptions.hh>
#include %(eheader)s

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
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args if _.startswith('-e')]+[('cxy::Exception','cxy/Exception.hh')])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    print template % vars()
    pass
