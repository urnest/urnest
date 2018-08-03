from omniidl import idlast
from omniidl import idltype

import sys
import os.path

from cxy import ptype, unqualifiedType,GenerateFailed,DIRECTION_OUT

objref_operation_t='''
// %(fqn)s::
%(returnType)s %(name)s(%(params)s) throw(
  %(eclass)s)
{
  try {
    try {
      calldesc::%(name)s c("%(name)s", %(nameLen)s+1, 0%(paramNames)s);
      _invoke(c);%(returnValue)s
    }
    catch(CORBA::Exception const& ee) {
      throw cxy::translateException< %(eclass)s >(
        translateCorbaException(ee));
    }
  }
  catch(%(eclass)s& e) {
    e.addContext(uri_+"->%(fqn)s::%(name)s()", std::make_pair(__FILE__, __LINE__));
    throw;
  }
}
'''

calldesc_userException_t='''
    if (omni::strMatch(repoId, cxy::cdr< ::%(fqn)s >::repoId)) {
      ::%(fqn)s _ex(cxy::cdr< ::%(fqn)s >::unmarshalFrom(s));
      if (iop_client){
        iop_client->RequestCompleted();
      }
      throw _ex;
    }
'''
calldesc_operation_t='''
class %(name)s : public omniCallDescriptor
{
public:
  %(name)s(
    const char* op_,
    size_t oplen,
    _CORBA_Boolean upcall%(params)s) throw():
      omniCallDescriptor(
        &%(name)s::lcfn, op_, oplen, %(oneway)s, _user_exns, %(user_exns_size)s, upcall)%(paramInits)s
  {
  }
  %(paramMembers)s%(returnMember)s

  // omniCallDescriptor::
  void marshalArguments(cdrStream& s) // REVISIT: throw
  {%(paramMarshals)s
  }
  void unmarshalReturnedValues(cdrStream& s) // REVISIT: throw
  {%(returnUnmarshal)s
  }
  void userException(cdrStream& s,
                     omni::IOP_C* iop_client,
                     const char* repoId)//REVISIT: throw
  {%(userExceptions)s
    if (iop_client) iop_client->RequestCompleted(1);
    OMNIORB_THROW(UNKNOWN,omni::UNKNOWN_UserException,
                  (::CORBA::CompletionStatus)s.completion());
  }

  static void lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
  {
    ::%(fqn)s* impl=(::%(fqn)s*)svnt->_ptrToInterface(cxy::cdr< ::%(fqn)s >::repoId);
    %(name)s* cd((%(name)s*)calldesc);
    %(callDescReturnValue)s impl->%(name)s(%(callDescInvocationParams)s);
  }
  static const char* const _user_exns[] = {%(user_exns)s
    0
  };
};
'''

interface_t='''\
template<>
class objref< ::%(fqn)s >:
  public virtual ::%(fqn)s,%(inherits)s
  public virtual ::CORBA::Object,
  public virtual omniObjRef,
  public virtual xju::NonCopyable
{
public:
  objref(): uri_("nil") { _PR_setobj(0); }  // nil
  objref(omniIOR* ior, omniIdentity* id) throw() :
      omniObjRef(cxy::cdr< ::%(fqn)s >::repoId, ior, id, 1)%(initinherits)s {
    _PR_setobj(this);
  }
  std::string uri_;

  IORImpl ior(std::string const& repoId) throw() {
    omniIOR_var const ior(_getIOR());
    return IORImpl(repoId,
                   cxy::TaggedProfileList(ior->iopProfiles()));
  }

  %(objref_content)s

protected:
  virtual ~objref() throw(){
  }
  
private:
  class calldesc
  {
  public:
    %(calldesc_content)s
  };
  // CORBA::Object::
  // result is a CORBA::Object* not a omniObjRef as the name suggest
  virtual void* _ptrToObjRef(const char* repoId)
  {
    if (repoId == cxy::cdr< ::%(fqn)s >::repoId)
      return (objref< ::%(fqn)s >*)this;%(inherit_cast_repoids)s

    if (omni::strMatch(repoId, cxy::cdr< ::%(fqn)s >::repoId))
      return (objref< ::%(fqn)s >*)this;%(inherit_cast_repoid_strs)s
    
    if (repoId == ::CORBA::Object::_PD_repoId)
      return (::CORBA::Object_ptr) this;
    
    if (omni::strMatch(repoId, ::CORBA::Object::_PD_repoId))
      return (::CORBA::Object_ptr) this;
    
    return 0;
  }
};

template<>
class pof< ::%(fqn)s > : public omni::proxyObjectFactory {
public:
  pof() : omni::proxyObjectFactory(cxy::cdr< ::%(fqn)s >::repoId){
  }
  virtual ~pof()
  {
  }
  
  virtual omniObjRef* newObjRef(omniIOR* ior,omniIdentity* id)
  {
    return new cxy::objref< ::%(fqn)s >(ior, id);
  }
  
  virtual _CORBA_Boolean is_a(const char* repoId) const
  {
    if (repoId==cxy::cdr< ::%(fqn)s >::repoId%(inherit_equal_repoids)s) {
      return 1;
    }
    if (omni::ptrStrMatch(repoId, cxy::cdr< ::%(fqn)s >::repoId)%(inherit_equal_repoid_strs)s) {
      return 1;
    }
    return 0;
  }

  static pof< ::%(fqn)s >& me_() throw()
  {
    return me;
  }
  static pof< ::%(fqn)s > me;
};
'''

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

def genCalldescUserException(decl):
    assert isinstance(decl, idlast.Exception),repr(decl)
    fqn='::'.join(decl.scopedName())
    return calldesc_userException_t%vars()

def genCalldesc(decl,eclass,eheader,indent,fqn):
    assert isinstance(decl, idlast.Operation),repr(decl)
    name=decl.identifier()
    nameLen=len(name)
    pns=['p%s'%i for i in range(1, len(decl.parameters())+1)]
    params=''.join([',\n    %s %s'%(ptype(p,eclass),n)
                    for p,n in zip(decl.parameters(),pns)
                    if p.direction()!=DIRECTION_OUT])
    callDescInvocationParams=','.join(['\n      cd->%s_'%n for n in pns])
    paramInits=''.join([',\n      %s_(%s)'%(n,n) for n in pns])
    paramMembers=''.join(
        ['\n  %s %s_;'%(unqualifiedType(p.paramType(),eclass),n)
         for p,n in zip(decl.parameters(),pns)])
    paramMarshals=''.join(
        ['\n    cxy::cdr< %s >::marshal(%s_, s);'%(
            unqualifiedType(p.paramType(),eclass),n)
         for p,n in zip(decl.parameters(),pns)])
    returnType=unqualifiedType(decl.returnType(),eclass)
    returnMember=''
    returnUnmarshal=''
    callDescReturnValue=''
    if returnType != 'void':
        returnMember='\n  xju::Optional< %(returnType)s > r_;'%vars()
        returnUnmarshal='\n    r_=cxy::cdr< %(returnType)s >::unmarshalFrom(s);'%vars()
        callDescReturnValue='\n    cd->r_='
        pass
    oneway=0
    if decl.oneway(): oneway=1
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    user_exns=''.join(['\n    "%s",'%_.repoId() for _ in decl.raises()])
    userExceptions=''.join([genCalldescUserException(_) for _ in decl.raises()])
    user_exns_size=len(decl.raises())
    result=reindent(indent,calldesc_operation_t%vars())
    return result

def genObjref(decl,eclass,eheader,indent,fqn):
    assert isinstance(decl, idlast.Operation), repr(decl)
    name=decl.identifier()
    nameLen=len(name)
    pns=['p%s'%i for i in range(1, len(decl.parameters())+1)]
    params=','.join(['\n  %s %s'%(ptype(p,eclass),n)
                     for p,n in zip(decl.parameters(),pns)
                     if p.direction()!=DIRECTION_OUT])
    paramNames=''.join([',\n      %s'%n for n in pns])
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    returnType=unqualifiedType(decl.returnType(),eclass)
    returnValue=''
    if returnType != 'void':
        returnValue='\n    return c.r_.value();'
        pass
    result=reindent(indent,objref_operation_t%vars())
    return result

def gen(decl,eclass,eheader,indent=''):
    try:
        result=''
        if isinstance(decl, idlast.Module):
            result=''.join(gen(_,eclass,eheader,indent) for _ in decl.definitions())
        elif isinstance(decl, idlast.Interface):
            fqn='::'.join(decl.scopedName())
            repoId=decl.repoId()
            calldesc_content=''.join(
                [genCalldesc(_,eclass,eheader,indent+'    ',fqn) \
                     for _ in decl.contents()\
                     if isinstance(_,idlast.Operation)])
            objref_content=''.join(
                [genObjref(_,eclass,eheader,indent+'  ',fqn)
                 for _ in decl.contents()
                 if isinstance(_,idlast.Operation)])
            inherit_fqns=['::'.join(_.scopedName()) for _ in decl.inherits()]
            inherits=''.join(
                ['\n  public virtual objref< ::%(_)s >,'%vars()
                 for _ in inherit_fqns])
            initinherits=''.join(
                [',\n      objref< ::%(_)s >(ior,id)'%vars()
                 for _ in inherit_fqns])
            inherit_cast_repoids=''.join(
                ['\n    if (repoId == cxy::cdr< ::%(_)s >::repoId)\n      return (objref< ::%(_)s >*)this;'%vars()
                 for _ in inherit_fqns])
            inherit_cast_repoid_strs=''.join(
                ['\n    if (omni::strMatch(repoId, cxy::cdr< ::%(_)s >::repoId))\n      return (objref< ::%(_)s >*)this;'%vars()
                 for _ in inherit_fqns])
            inherit_equal_repoids=''.join(
                [' ||\n       (repoId == cxy::cdr< ::%(_)s >::repoId)'%vars()
                 for _ in inherit_fqns])
            inherit_equal_repoid_strs=''.join(
                [' ||\n       (omni::strMatch(repoId, cxy::cdr< ::%(_)s >::repoId))'%vars()
                 for _ in inherit_fqns])
            result=interface_t%vars()
        elif isinstance(decl, idlast.Typedef):
            pass
        elif isinstance(decl, idlast.Struct):
            pass
        elif isinstance(decl, idlast.Exception):
            pass
        elif isinstance(decl, idlast.Enum):
            pass
        elif isinstance(decl, idlast.Const):
            pass
        elif isinstance(decl, idlast.Union):
            pass
        elif isinstance(decl, idlast.Forward):
            pass
        elif isinstance(decl, idlast.StructForward):
            pass
        else:
            assert False, repr(decl)
            pass
        return result
    except:
        raise GenerateFailed(decl,sys.exc_info())
    pass

template='''\
// generated from %(fileName)s by omnicxy cxyobjref idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all ipc exceptions

#include %(hhinc)s
#include %(cdrhhinc)s

#include <cxy/objref.hh>
#include <cxy/translateException.hh> //impl
#include <cxy/translateCorbaException.hh> //impl
#include <cxy/pof.hh>
#include <cxy/TaggedProfileList.hh>//impl

#include <xju/NonCopyable.hh>
#include <xju/Optional.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/IOP_C.h>
%(idlincludes)s

namespace cxy
{

%(items)s
}
'''

def includeSpec(fileName,hpath,hhext):
    if os.path.dirname(fileName)=='':
        if hpath=='':
            return '"%s"'%(os.path.splitext(fileName)[0]+'.objref.'+hhext)
        else:
            return '<%s%s>'%(hpath,os.path.splitext(fileName)[0]+'.objref.'+hhext)
    return '<%s>'%(os.path.splitext(fileName)[0]+'.objref.'+hhext)

def gen_idlincludes(fileNames,hpath,hhext):
    if not len(fileNames):
        return ''
    return '\n// included idl'+''.join(
        ['\n#include %s'%includeSpec(_,hpath,hhext)
         for _ in fileNames])

def run(tree, args):
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args if _.startswith('-e')]+[('cxy::Exception','cxy/Exception.hh')])[0]
    hhext=([_.split('-hhext=',1)[1] for _ in args \
            if _.startswith('-hhext')]+\
           ['hh'])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    hpath=([_.split('-hpath=',1)[1] for _ in args \
                if _.startswith('-hpath')]+\
               [''])[0]
    if len(hpath)>0 and not hpath.endswith('/'):
        hpath=hpath+'/'
    if len(hpath):
        hhinc='<%(hpath)s%(baseName)s.%(hhext)s>'%vars()
        cdrhhinc='<%(hpath)s%(baseName)s.cdr.%(hhext)s>'%vars()
    else:
        hhinc='"%(baseName)s.%(hhext)s"'%vars()
        cdrhhinc='"%(baseName)s.cdr.%(hhext)s"'%vars()
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() \
                                         if not _.mainFile()]),
                                hpath,
                                hhext)
    print template % vars()
    pass
