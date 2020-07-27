#!/bin/env python

# Parse Sun java compiler errors to extract dependencies.
import sys
import re
import string
import traceback
import os

class Trace:
    def __init__(self, lines):
        self.totalLines=len(lines)
        pass
    def __call__(self, remainingLines, msg):
        lineNumber=self.totalLines-len(remainingLines)+1
        print('jdep@%(lineNumber)s: %(msg)s' % vars(),file=sys.stderr)
        pass
    pass

trace=lambda remainingLines, msg: None

class Malformed:
    def __init__(self, lines, reason):
        self.lines=lines
        self.reason=reason
        pass
    def __str__(self):
        return '%s:\n"%s"' % (self.reason, ''.join(self.lines[0:4]))
    pass

class ErrorBlock:
    def __init__(self, fileName, lineNumber, lines, lineNumberOfFirstLine):
        self.fileName=fileName
        self.lineNumber=lineNumber
        self.lines=lines
        self.lineNumberOfFirstLine=lineNumberOfFirstLine
        pass

    def __str__(self):
        return 'ErrorBlock file %(fileName), line %(lineNumber), lines "%(lines)"' % self.__dict__


ErrorBlock.re_1=re.compile('^([0-9a-zA-Z_.]*[.]java):([0-9][0-9]*): ')
ErrorBlock.re_2=re.compile('^([a-z][a-z]*:[0-9a-zA-Z_.\\\\]*[.]java):([0-9][0-9]*): ')

def requireMatch(r, t, lines):
    'require that string %(t)s matches compiled regular expression %(r)s'
    'returns the match, or raises Malformed'
    m=r.match(t)
    if not m:
        p=r.pattern
        raise Malformed(lines, '"%(t)s" does not match regular expression "%(p)s"' % vars())
    return m

def parseErrorBlock(lines, lineNumberOfFirstLine):
    'parse ErrorBlock from front of lines, returning ErrorBlock and remaining lines, or raise Malformed'
    try:
        m=requireMatch(ErrorBlock.re_1, lines[0], lines)
    except Malformed, e1:
        try:
            m=requireMatch(ErrorBlock.re_2, lines[0], lines)
        except Malformed, e2:
            raise Malformed(lines, str(e1)+", "+str(e2)) from None
    fileName=m.groups()[0]
    lineNumber=int(m.groups()[1])
    l=lines[0:1]
    lines=lines[1:]
    while len(lines):
        if ErrorBlock.re_1.match(lines[0]) or ErrorBlock.re_2.match(lines[0]):
            return ErrorBlock(fileName, lineNumber, l, lineNumberOfFirstLine), lines
        l.append(lines[0])
        lines=lines[1:]
        pass
    return ErrorBlock(fileName, lineNumber, l,  lineNumberOfFirstLine), lines

note_re=re.compile('^Note: ')

def parseErrorBlocks(lines):
    'parse all error blocks from given lines, returning sequence'
    end=re.compile('[0-9][0-9]* errors')
    result=[]
    totalLines=len(lines)
    while len(lines):
        if note_re.match(lines[0]):
            lines=lines[1:]
        else:
            e, lines=parseErrorBlock(lines, totalLines-len(lines)+1)
            result.append(e)
    return result

class Unresolved:
    def __init__(self, package, symbol):
        assert package is None or (len(package) and not package.endswith('.')), (package, symbol)
        assert not symbol.startswith('.'), (package, symbol)
        self.package=package
        self.symbol=symbol
        pass
    def __str__(self):
        return 'unresolved symbol %(symbol)s from package %(package)s' % self.__dict__
    pass

re_4=re.compile(r'[ \t]*import[ \t][ \t]*([^;*]*);')
re_5=re.compile(r'[ \t]*import[ \t][ \t]*([^;*]*[*])')
def parse_imported_symbol(lines):
    'parse error message associated with "import x.y.z;", returning "x.y.z"'
    pattern=re_4.pattern
    x=re_4.match(lines[0])
    if not x:
        raise Malformed(lines, 'line does not match %(pattern)s' %vars())
    return x.groups()[0]

def parse_imported_package(lines):
    'parse error message associated with "import x.y.*;", returning "x.y"'
    pattern=re_5.pattern
    x=re_5.match(lines[0])
    if not x:
        raise Malformed(lines, 'line does not match %(pattern)s' % vars())
    terms=x.groups()[0].split('.')
    return '.'.join([_.strip() for _ in terms[:-1]])

re_3=re.compile('symbol[ \t]*:[ \t]*variable ([^\r\n]*)')
re_3b=re.compile('symbol[ \t]*:[ \t]*class ([^\r\n]*)')
re_3c=re.compile('symbol[ \t]*:[ \t]*method ([^\r\n]*)')
re_3c2=re.compile('symbol[ \t]*:[ \t]*constructor ([^\r\n(]*)')

def parse_symbol(lines):
    'parse error message associated with symbol reference, returning one or more Unresolved, the last having package None'
    pattern=[re_3.pattern, re_3b.pattern]
    x=re_3.match(lines[0])
    if x is None: x=re_3b.match(lines[0])
    if x is None: x=re_3c2.match(lines[0])
    if not x:
        if re_3c.match(lines[0]):
            return []
        raise Malformed(lines, 'line does not match %(pattern)s' % vars())
    elements=x.groups()[-1].split('.')
    result=[Unresolved('.'.join(elements[0:i]),
                       elements[i])
            for i in range(1, len(elements))]+\
            [Unresolved(None, elements[0])]
    return result

re_3d=re.compile('location: package ([^\r\n]*)')
def parse_imported_symbol_2(lines):
    'parse error message associated with imported symbol where compiler has seen the package name already, return absolute symbol name like "a.b.C"'
    unresolved=parse_symbol(lines)
    if len(unresolved)==0:
        raise Malformed(lines, 'parse_imported_symbol returned empty')
    symbol=unresolved[-1].symbol
    pattern=re_3d.pattern
    x=re_3d.match(lines[1])
    if x is None:
        raise Malformed(lines, '2nd line does not match %(pattern)s' % vars())
    package=x.groups()[-1].strip()
    return package+'.'+symbol

re_6=re.compile('([.A-Za-z0-9_]*)')
def parse_implicitly_referenced_symbol(lines, name):
    'parse error message associated with symbol from package that hasn\'t been explicitly imported, eg x.y without import x.y or import x.*;, returns one or more Unresolved'
    pattern=re_6.pattern
    if not lines[1].strip()=='^':
        raise Malformed(lines[1:], 'line does not match %(pattern)s' % vars())
    start=lines[1].find('^')
    r=lines[0][start:]
    m=re_6.match(r)
    if not m:
        pattern=re_6.match(r)
        raise Malformed(lines, '%(r)s does not match %(pattern)s' % vars())
    name=name+m.groups()[0]
    elements=[_.strip() for _ in name.split('.')]
    result=[Unresolved('.'.join(elements[0:i]),
                       elements[i])
            for i in range(1, len(elements))]+\
           [Unresolved(None, elements[0])]
    return result

class E:
    def __init__(self, m):
        self.m=m
        pass
    def __str__(self):
        return self.m
    pass

getPackageOfFile_re=re.compile('^[ \t]*package[ \t][ \t]*([^;]*)')
def getPackageOfFile(f):
    for l in open(f).readlines():
        m=getPackageOfFile_re.match(l)
        if m: return m.groups()[-1]
    raise E('no line of file %(f)s matches regular expression %(getPackageOfFile_re)s' % vars())

re_1=re.compile('.* package (.*) does not exist')
re_2=re.compile('.*: cannot find symbol')

class SymbolAccumulator:
    def __init__(self, realFileName):
        self.realFileName=realFileName
        self.imported_symbols=set([])
        self.imported_packages=[getPackageOfFile(realFileName)]
        self.unresolved=[]
        pass

    def __str__(self):
        return 'accumulator for %(realFileName)s' % self.__dict__

    def add(self, errorBlock):
        lines=errorBlock.lines
        trace(lines, '"'+lines[0].strip('\n'+chr(13))+'"')
            
        m1=re_1.match(lines[0])
        if m1:
            trace(lines, '  ->matched re_1: ' + re_1.pattern)
            trace(lines[1:], '"'+lines[1].strip('\n'+chr(13))+'"')
            try:
                x=parse_imported_symbol(lines[1:])
                trace(lines, '  %(self)s imports symbol %(x)s' %vars())
                self.imported_symbols.add(x)
                y=x.split('.')
                for i in range(1, len(y)):
                    z=Unresolved('.'.join(y[0:-i]), y[-i])
                    trace(lines, '  %(self)s needs symbol %(z)s' % vars())
                    self.unresolved.append(z)
            except Malformed, e1:
                try:
                    x=parse_imported_package(lines[1:])
                    trace(lines, '  %(self)s imports package %(x)s' %vars())
                    self.imported_packages.append(x)
                except Malformed, e2:
                    try:
                        x=parse_implicitly_referenced_symbol(
                            lines[1:], m1.groups()[0])
                        xs=[str(_) for _ in x]
                        trace(lines, ' %(self)s needs symbols %(xs)s' %vars())
                        self.unresolved.extend(x)
                    except Malformed, e3:
                        msg=lines[0].split()[0]+" this style of package reference is not yet supported by Odin build tool java package's "+argv[0]+' because '+str([str(_) for _ in [e1, e2, e3]])+')'
                        trace(lines, '    ERROR: '+msg)
                        raise E(msg) from None
                    pass
                pass
            pass
        elif re_2.match(lines[0]):
            trace(lines, '  ->matched re_2: ' + re_2.pattern)
            trace(lines[1:], '"'+lines[1].strip('\n'+chr(13))+'"')
            try:
                x=parse_imported_symbol_2(lines[1:])
                trace(lines, '  %(self)s needs symbols %(x)s' %vars())
                self.imported_symbols.add(x)
                y=x.split('.')
                self.unresolved.append(
                    Unresolved('.'.join(y[0:-1]), y[-1]))
                pass
            except Malformed, e1:
                trace(lines, str(e1))
                try:
                    x=parse_symbol(lines[1:])
                    self.unresolved.extend(x)
                    if len(x):
                        xs=[str(_) for _ in x]
                        trace(lines, '  %(self)s needs symbols %(xs)s' %vars())
                        pass
                    pass
                except Malformed, e2:
                    msg=lines[0].split()[0]+" this style of package reference is not yet supported by Odin build tool java package's jdep.py because "+str([str(_) for _ in [e1, e2]])+')'
                    trace(lines, '    ERROR: '+msg)
                    raise E(msg) from None
                pass
            pass
        else:
            trace(lines, '  ->(skip)')
            pass
        pass

    def getRequiredSymbols(self):
        required_symbols=[]
        for x in self.unresolved:
            if x.package is None:
                matching_imported_symbols=[_ for _ in self.imported_symbols \
                                           if _.split('.')[-1]==x.symbol]
                assert len(matching_imported_symbols)<=1, (matching_imported_symbols, x)
                if len(matching_imported_symbols):
                    required_symbols.append(matching_imported_symbols[0])
                else:
                    required_symbols.extend([_+'.'+x.symbol for _ in \
                                             self.imported_packages])
            else:
                required_symbols.append(x.package+'.'+x.symbol)

        return required_symbols

    pass

windowsPath_re=re.compile('^/cygdrive/([a-z][a-z]*)/(.*)')

def windowsPath(absolute_cygpath):
    m=windowsPath_re.match(absolute_cygpath)
    assert m, '%s does not match %s' % (absolute_cygpath, windowsPath_re.pattern)
    return m.groups()[0]+':\\'+m.groups()[1].replace('/', '\\')

def main(argv):
    ODIN_sources, ODIN_raw_errors=argv[1:3]

    realFileNames=open(ODIN_sources).read().splitlines()

    lines=open(ODIN_raw_errors).readlines()
    
    if False or os.environ.get('JDEP_TRACE'):
        global trace
        trace=Trace(lines)
        pass

    errorBlocks=parseErrorBlocks(lines)

    #on windows the error messages come out with real file names even
    #though we point at the shortcut file (the isolated file)
    if 0 and realFileNames[0].startswith('/cygdrive/'):
        accumulators=dict(zip([windowsPath(_) for _ in realFileNames],
                              [SymbolAccumulator(_) for _ in realFileNames]))
    else:
        isolatedFileNames=[str(_+1)+'.java' for _ in range(0, len(realFileNames))]
        accumulators=dict(zip(isolatedFileNames,
                              [SymbolAccumulator(_) for _ in realFileNames]))
    
    for e in errorBlocks:
        accumulators[e.fileName].add(e)
        pass

    y=[]
    for a in accumulators.values():
        y.extend(a.getRequiredSymbols())
        pass
    y=set(y)
    y=list(y)
    y.sort()
    sys.stdout.write(''.join([_+'\n' for _ in y]))
    pass

if __name__=='__main__':
    try:
        main(sys.argv)
    except:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)
    pass
