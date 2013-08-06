# Classification: UNCLASSIFIED
#
# From code Copyright 2002 Trevor Taylor (with permission)
#
import string
import traceback
import sys
import types
import os
import fcntl

class IsType:
    def __init__(self, t):
        self.__t = t
        pass
    def __call__(self, x):
        return isinstance(x, self.__t)
    pass

class IsListType:
    def __init__(self, isType):
        self.__isType = isType
        pass
    def __call__(self, x):
        return filter(self.__isType, x) == x
    pass

class IsMapType:
    def __init__(self, isKeyType, isValueType):
        self.__isKeyType = isKeyType
        self.__isValueType = isValueType
        pass

    def __call__(self, m):
        return isinstance(m, types.DictType) and \
               filter(self.__isKeyType, m.keys()) == m.keys() and \
               filter(self.__isValueType, m.values()) == m.values()
    pass

class IsSetType:
    def __init__(self, isKeyType):
        self.__isKeyType = isKeyType
        pass
    def __call__(self, s):
        return isinstance(s, types.DictType) and \
               filter(self.__isKeyType, s.keys()) == s.keys() and \
               filter(lambda l: l == None, s.values()) == s.values()
    pass



# REVISIT: any way to get file and line number?
class Exception(Exception):
    """Exception allowing capture of cause and context.
    """
    def __init__(self, cause):
        self.cause = cause
        self.context = []
        pass

    def addContext(self, context):
        self.context.append(context)
        pass
    
    def __str__(self):
        result = ''
        if len(self.context):
            x = self.context[:]
            x.reverse()
            result = 'Failed to ' + \
                     string.join(x,
                                 ' because\nfailed to ') + \
                                 ' because\n'
        else:
            result = 'Failed because\n'
            pass
        result = result + self.cause + '.'
        return result


def makeException(exceptionInfo, context):
    e = Exception(string.join(traceback.format_exception(
        exceptionInfo[0],
        exceptionInfo[1],
        exceptionInfo[2])))
    e.addContext(context)
    return e


def inContext(context, exceptionInfo):
    """Make a mob.Exception that includes exception info and context.
    If exceptionInfo[1] is already a mob.Exception just add context,
    otherwise use exceptionInfo as cause for a new mob.Exception.

    exceptionInfo is as returned by sys.exc_info()
    """
    if (isinstance(exceptionInfo[1], Exception)):
        exceptionInfo[1].addContext(context)
        return exceptionInfo[1]
    
    return makeException(exceptionInfo, context)

if sys.platform == 'win32':
    def doCommand(command):
        '''perform command "%(command)s", return combined stderr and stdout'''
        try:
            io=os.popen4(command)
	    io[0].close()
	    output=io[1].read()
	    status=io[1].close()
	    if status:
	        raise Exception(
	            'command failed with status %s and output "%s"' % (
	            status, output))
	        pass
	    assert type(output) is types.StringType
	    return output
        except:
            raise inContext(doCommand.__doc__ % vars(), sys.exc_info())
        pass
else:
    import popen2
    def doCommand(command):
        '''perform command "%(command)s", return combined stderr and stdout'''
        try:
            x=popen2.Popen4(command)
	    x.tochild.close()
	    output=x.fromchild.read()
	    status=x.wait()
	    if status:
	        raise Exception(
	            'command failed with status %s and output "%s"' % (
	            status, output))
	        pass
	    assert type(output) is types.StringType
	    return output
        except:
            raise inContext(doCommand.__doc__ % vars(), sys.exc_info())
        pass
    
def firstLineOf(x):
  return x.splitlines()[0]

def listCoreFiles(directoryName):
  'list the core files in the named directory %(directoryName)s'
  try:
    files=os.listdir(directoryName)
    return [ (x, os.stat(x)) for x in files if x.startswith('core') ]
  except:
    raise inContext(firstLineOf(listCoreFiles.__doc__)%vars(),
                    sys.exc_info())
  pass

def verifyCommandAborts(command):
    'verify that shell command "%(command)s" aborts'
    try:
        corePre=listCoreFiles('.')
        x=doCommand(command)
    except Exception, e:
        corePost=listCoreFiles('.')
        coreFiles = [x[0] for x in corePost if not x in corePre]
        assert len(coreFiles)==1, str(coreFiles)
        return str(e), coreFiles[0]
    else:
        raise Exception('command "%(command)s" should have failed, but it succeeded with output "%(x)s"' % vars())
        pass
    pass

class FileLock:
    def __init__(self, fileName):
        'take out an exclusive lock file %(fileName)s, waiting till it is available if necessary'
        '- fileName need not exist (it will be created if necessary)'
        try:
            self.fd = open(fileName, 'w')
            fcntl.lockf(self.fd, fcntl.LOCK_EX)
        except:
            raise mob.inContext(firstLineOf(FileLock.__init__.__doc__)%vars(),
                                sys.exc_info())
        pass

    def __del__(self):
        fcntl.lockf(self.fd, fcntl.LOCK_UN)
        pass
    pass
