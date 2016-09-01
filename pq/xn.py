import traceback
import sys
import string

class Xn(Exception):
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
    e = Xn(' '.join(traceback.format_exception(
        exceptionInfo[0],
        exceptionInfo[1],
        exceptionInfo[2])))
    e.addContext(context)
    return e


def inContext(context, exceptionInfo=None):
    """Make a Xn that includes exception info and context.
    If exceptionInfo[1] is already a Xn just add context,
    otherwise use exceptionInfo as cause for a new Xn.

    exceptionInfo is as returned by sys.exc_info()
    """
    if exceptionInfo is None: exceptionInfo=sys.exc_info()
    if (isinstance(exceptionInfo[1], Xn)):
        exceptionInfo[1].addContext(context)
        return exceptionInfo[1]
    return makeException(exceptionInfo, context)

def firstLineOf(x):
    return str(x).split('\n')[0]
