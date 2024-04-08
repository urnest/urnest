# coding: utf-8
#
# Copyright (c) 2018 Trevor Taylor
# 
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that all
# copyright notices and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# jquery-like python library
#
# parse a HTML text into a tree, with search, manipulation
# and re-output as HTML text
#
# parse() and parseFile() are the primary interface.
#
# REVISIT: need better diags, eg:
#  track selectors used to find/filter nodes and report them
#  on failure eg replace/html
#
from __future__ import annotations
from typing import Iterator
from html.parser import HTMLParser
from html.entities import entitydefs as htmlentitydefs
from html.entities import name2codepoint as entities
from html.entities import codepoint2name as reverseentities
import sys
import traceback
import string
from xju.xn import in_context,in_function_context

class Pos:
    def __init__(self, file, line, col):
        self.file=file
        self.line=line
        self.col=col
        return
    def __str__(self):
        return '%(file)s:%(line)s:%(col)s' % self.__dict__
    pass

class ParseFailed(Exception):
    def __init__(self, cause, pos):
        self.cause=cause
        self.pos=pos
        Exception.__init__(self,self.__str__())
        return
    def __str__(self):
        return 'failed to parse html at %(pos)s because\n%(cause)s'%self.__dict__
    pass


def encodeEntity(c):
    x=reverseentities.get(ord(c),None)
    if x is None: return c
    return u'&%(x)s;'%vars()

def encodeEntities(s:str)->str:
    if s is None: return u''
    x=u''.join([encodeEntity(_) for _ in s])
    return x

class Node:
    pos: Pos
    def __init__(self, pos: Pos, parent=None):
        self.pos = pos
        self.parent=parent
        if parent:
            parent.children.append(self)
            pass
        pass
    def predecessor(self):
        if self.parent:
            i=self.parent.children.index(self)
            if i>0:
                return [self.parent.children[i-1]]
            pass
        return []
    def successor(self):
        if self.parent:
            i=self.parent.children.index(self)+1
            if i<len(self.parent.children):
                return [self.parent.children[i]]
            pass
        return []
    
class Root(Node):
    def __init__(self, pos:Pos):
        Node.__init__(self,pos)
        self.children:list[Node]=[]
    def indexOf(self, child):
        return [ _[0] for _ in zip(range(0,len(self.children)),
                                   self.children) if _[1]==child][0]
    def remove(self, node):
        i=self.indexOf(node)
        self.children=self.children[0:i]+self.children[i+1:]
        node.parent=None
        return self
    pass

endOptional=frozenset([
        'body',
        'colgroup',
        'dd',
        'dt',
        'head',
        'html',
        'li',
        'option',
        'p',
        'tbody',
        'td',
        'tfoot',
        'th',
        'thead',
        'tr'])

emptyTags=frozenset([
        'area',
        'base',
        'basefont',
        'br',
        'col',
        'frame',
        'hr',
        'img',
        'input',
        'isindex',
        'link',
        'meta',
        'param'])

class Tag(Node):
    def __init__(self, tagName, attrs, parent, pos):
        Node.__init__(self, pos, parent)
        self.tagName=tagName
        self.attrs=dict(attrs)
        self.children:list[Node]=[]
        self.end=''
        self.classes=set(self.attrs.get('class','').split())
        return
    def __repr__(self):
        return '%(tagName)s at %(pos)s' % self.__dict__
    def __str__(self):
        encodedAttrs=['%s="%s"' % (_[0],encodeEntities(_[1])) for 
                      _ in self.attrs.items()]
        encodedAttrs.sort()
        start=' '.join([self.tagName]+encodedAttrs)
        if self.tagName in ['script','style']:
            content=self.text()
        else:
            content=''.join([str(_) for _ in self.children])
            pass
        end=self.end
        return '''<%(start)s>%(content)s%(end)s'''%vars()
    def hasClass(self,c):
        return c in self.classes
    def addClass(self,c):
        self.classes.add(c)
        self.attrs['class']=' '.join(self.classes)
        return self
    def removeClass(self,c):
        if c in self.classes:
            self.classes.remove(c)
            pass
        self.attrs['class']=' '.join(self.classes)
        if len(self.classes)==0:
            del self.attrs['class']
        return self
    def attr(self, a, val=None):
        if not val is None:
            self.attrs[a]=str(val)
        return self.attrs.get(a,'')
    def removeAttr(self, a):
        if a in self.attrs: del self.attrs[a]
        return
    def attrEquals(self, a, val):
        return self.attrs.get(a,None)==val
    def hasAttr(self, a):
        return a in self.attrs
    def indexOf(self, child):
        return [ _[0] for _ in zip(range(0,len(self.children)),
                                   self.children) if _[1]==child][0]
    def remove(self, node):
        i=self.indexOf(node)
        self.children=self.children[0:i]+self.children[i+1:]
        node.parent=None
        return self
    def replace(self, node, newNode):
        if newNode.parent:
            newNode.parent.remove(newNode)
        self.children[self.indexOf(node)]=newNode
        newNode.parent=self
        return self
    def clone(self, newParent):
        result=Tag(self.tagName, self.attrs.items(), newParent, self.pos)
        result.end=self.end
        result.classes=set(self.classes)
        for c in self.children:
            assert isinstance(c,Tag) or \
                isinstance(c,Data) or \
                isinstance(c,Comment) or \
                isinstance(c,Decl) or \
                isinstance(c,PI)
            c.clone(result)
        return result
    def text(self):
        if self.tagName=='br':
            return u'\n'
        if self.tagName=='p':
            return u''.join([_.text() for _ in self.children])+u'\n'
        if self.tagName=='li':
            return u''.join([_.text() for _ in self.children])+u'\n'
        return u''.join([_.text() for _ in self.children])
    pass

class Data(Node):
    '''CData, including <script> and <style> content'''
    def __init__(self, data, parent, pos):
        Node.__init__(self, pos, parent)
        self.data=data
        pass
    def __str__(self):
        return encodeEntities(self.data)
    def __repr__(self):
        return 'data at %(pos)s, %(data)r' % self.__dict__
    def clone(self, newParent):
        result=Data(self.data, newParent, self.pos)
        return result
    def text(self):
        return self.data
    pass

class Comment(Node):
    '''Comment'''
    def __init__(self, comment, parent, pos):
        Node.__init__(self, pos, parent)
        self.comment=comment
    def __str__(self):
        return '<!--%(comment)s-->' % self.__dict__
    def __repr__(self):
        return 'comment at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=Comment(self.comment, newParent, self.pos)
        return result
    def text(self):
        return u''
    pass

class Decl(Node):
    '''Decl'''
    def __init__(self, decl, parent, pos):
        Node.__init__(self, pos, parent)
        self.decl=decl
    def __str__(self):
        return '<!%(decl)s>' % self.__dict__
    def __repr__(self):
        return 'decl at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=Decl(self.decl, newParent, self.pos)
        return result
    def text(self):
        return u''
    pass

class PI(Node):
    '''Processing Instruction'''
    def __init__(self, pi, parent, pos):
        Node.__init__(self, pos, parent)
        self.pi=pi
    def __str__(self):
        return '<?%(pi)s>' % self.__dict__
    def __repr__(self):
        return 'processing instruction at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=PI(self.pi, newParent, self.pos)
        return result
    def text(self):
        return u''
    pass

class Parser(HTMLParser):
    def __init__(self, fileName):
        HTMLParser.__init__(self)
        self.fileName=fileName
        self.root=Root(Pos(fileName,1,1))
        self.current:Root|Tag=self.root
        return
    def pos(self):
        return Pos(self.fileName, *self.getpos())
    def handle_starttag(self, tag, attrs):
        if tag in emptyTags:
            Tag(tag, attrs, self.current, self.pos())
        else:
            #end-optional tag ends current tag of same name, eg
            #<li>x
            #<li>y
            if isinstance(self.current, Tag) and \
                    tag in endOptional and \
                    self.current.tagName == tag:
                self.current=self.current.parent
            self.current=Tag(tag, attrs, self.current, self.pos())
        return
    def handle_endtag(self, tag):
        current=self.current
        while not isinstance(current,Root) and current.tagName != tag:
            current=current.parent
            pass
        if not isinstance(current,Root):
            self.current=current
            self.current.end=u'</%(tag)s>'%vars()
            self.current=self.current.parent
        return
    def handle_data(self,data):
        Data(data, self.current, self.pos())
        return
    def handle_comment(self,comment):
        Comment(comment, self.current, self.pos())
        return
    def handle_decl(self,decl):
        Decl(decl,self.current, self.pos())
        return
    def handle_pi(self,pi):
        PI(pi,self.current, self.pos())
        return

def filter(node, predicate):
    '''return [node if matches predicate] + all children that match'''
    result=[_ for _ in [node] if predicate(_)]
    if isinstance(node,Tag):
        for c in node.children:
            result.extend(filter(c, predicate))
    return result
        
class Selection:
    nodeList:list[Node]
    def __init__(self, nodeList:Selection|Node|list[Node]):
        if isinstance(nodeList,Selection):
            self.nodeList=nodeList.nodeList
        elif isinstance(nodeList,Node):
            self.nodeList=[nodeList,]
        else:
            self.nodeList=nodeList[:]
            pass
        return
    def find(self, predicate) -> Selection:
        '''find all children of our nodes that match predicate'''
        result=[]
        for _ in self.nodeList:
            result.extend(filter(_,predicate))
        return Selection(result)
    def filter(self, predicate) -> Selection:
        '''nodes of ours that match predicate'''
        return Selection([_ for _ in self.nodeList if predicate(_)])
    def unless(self, predicate):
        '''nodes of ours that don't match predicate'''
        return Selection([_ for _ in self.nodeList if not predicate(_)])
    def html(self, nodes):
        '''replace our first node's children with the specified list of nodes/html string'''
        if type(nodes)==str:
            nodes=parse(nodes)
            pass
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
            pass
        for n in nodes:
            if n.parent: n.parent.remove(n)
        for n in self.nodeList[0:1]:
            n.children=nodes
            for c in n.children:
                c.parent=n
        return self
    def text(self, s=None):
        '''replace each of our node's children with the specified text string'''
        '''or return concatenation of text content of children'''
        if s is None:
            return u''.join([_.text() for _ in self.nodeList])
        for n in self.nodeList:
            ss=parse(encodeEntities(str(s)))
            Selection([n]).html(ss)
        return self
    def replace(self, nodes):
        '''replace first of specified nodes with first of our nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        nodes[0].parent.replace(nodes[0],self.nodeList[0])
        return self
    def appendTo(self, nodes):
        '''append our nodes to children of first of specified nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        for n in self.nodeList:
            n.parent=nodes[0]
            nodes[0].children.append(n)
        return self
    def addAfter(self, nodes):
        '''add our nodes after first of specified nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        parent=nodes[0].parent
        index=parent.indexOf(nodes[0])
        for n in self.nodeList:
            n.parent=parent
        parent.children[index+1:index+1]=self.nodeList
        return self
    def addBefore(self, nodes):
        '''add our nodes before first of specified nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        parent=nodes[0].parent
        index=parent.indexOf(nodes[0])
        for n in self.nodeList:
            n.parent=parent
        parent.children[index:index]=self.nodeList
        return self
    def detach(self):
        '''remove each of our nodes from its parent'''
        for n in self.nodeList:
            if n.parent:
                n.parent.remove(n)
        return self
    def remove(self):
        return self.detach()
    def empty(self):
        '''remove all children from each of our nodes'''
        for n in self.nodeList:
            if isinstance(n,Tag):
                for c in n.children:
                    c.parent=None
                    pass
                n.children=[]
                pass
            pass
        return self
    def first(self):
        '''return Selection containing first of our nodes'''
        return Selection(self.nodeList[0:1])
    def last(self):
        '''return Selection containing first of our nodes'''
        return Selection(self.nodeList[-1:])
    def children(self):
        '''return Selection containing children of our nodes'''
        nodeList:list[Node]=[]
        return Selection(sum([_.children for _ in self.nodeList],nodeList))
    def predecessors(self):
        '''return Selection containing predecessor of each of our nodes'''
        nodeList:list[Node]=[]
        return Selection(sum([_.predecessor() for _ in self.nodeList],nodeList))
    def successors(self):
        '''return Selection containing successor of each of our nodes'''
        nodeList:list[Node]=[]
        return Selection(sum([_.successor() for _ in self.nodeList],nodeList))
    def clone(self):
        '''return Selection containing a copy of our nodes'''
        return Selection([_.clone(None) for _ in self.nodeList])
    def addClass(self, name):
        '''add class %(name)s to each of our children'''
        for n in self.nodeList:
            if isinstance(n,Tag):
                n.addClass(name)
                pass
            pass
        return self
    def removeClass(self, name):
        '''remove class %(name)s from each of our children'''
        for n in self.nodeList:
            if isinstance(n,Tag):
                n.removeClass(name)
                pass
            pass
        return self
    def hasClass(self,c):
        '''True iff all our nodes have class c'''
        each=[True for _ in self.nodeList if isinstance(_,Tag) and _.hasClass(c)]
        return len(each)==len(self.nodeList)
    def attr(self, name, value=None, joiner=u''):
        '''attr('src') gets the values of the src attributes of our nodes and joins them with joiner, returning a single string'''
        """attr('src','fred') sets the src attribute of our only node to 'fred'"""
        if value is None:
            return joiner.join([_.attr(name, value) for _ in self.nodeList if isinstance(_,Tag)])
        [_.attr(name, value) for _ in self.nodeList if isinstance(_,Tag)]
        return self
    def attrs(self, name, value=None):
        '''attrs('src') lists the values of the src attributes of each of our nodes'''
        """attrs('src','fred.html') sets the src attribute of each of our nodes to 'html'"""
        if value is None:
            return [_.attr(name, value) for _ in self.nodeList if isinstance(_,Tag)]
        [_.attr(name, value) for _ in self.nodeList if isinstance(_,Tag)]
        return self
    def removeAttr(self, name):
        [_.removeAttr(name) for _ in self.nodeList if isinstance(_,Tag)]
        return self
    def join(self,nodes):
        '''return new Selection containing nodes with each separated by a clone of our nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
            pass
        resultNodes=[]
        for i,n in enumerate(nodes):
            resultNodes.append(n)
            if i+1 < len(nodes): resultNodes.extend(self.clone().nodeList)
            pass
        return Selection(resultNodes)
    def __str__(self):
        '''re-format as html'''
        return ''.join([str(_) for _ in self.nodeList])
    def __len__(self):
        return len(self.nodeList)
    def __iter__(self) -> Iterator[Selection]:
        return iter([Selection(n) for n in self.nodeList])
    def __getitem__(self, key):
        return Selection(self.nodeList.__getitem__(key))
    def __add__(self, b):
        if isinstance(b,Selection):
            return Selection(self.nodeList+b.nodeList)
        return NotImplemented
    def utf8(self):
        return str(self).encode('utf-8')
    pass

# basic predicates
def hasClass(c:str):
    return lambda node: isinstance(node, Tag) and node.hasClass(c)
def tagName(t:str):
    return lambda node: isinstance(node, Tag) and node.tagName==t
def attrEquals(attr:str,value:str):
    return lambda node: isinstance(node, Tag) and node.attrEquals(attr,value)
def hasAttr(attr):
    return lambda node: isinstance(node, Tag) and node.hasAttr(attr)

def parse(s:str, origin='unknown') -> Selection:
    '''parse HTML string "%(origin)s"'''
    parser=Parser(origin)
    try:
        u=str(s)
        parser.feed(u)
        parser.close()
        result=Selection(parser.root.children)
        result.detach()
        return result
    except:
        raise ParseFailed(str(''.join(traceback.format_tb(sys.exc_info()[2])))+'\n'+str(sys.exc_info()[1]), parser.pos()) from None
    pass

def parseFile(fileName,encoding='utf-8') -> Selection:
    '''parse {fileName} as HTML'''
    try:
        with open(fileName,encoding=encoding) as f:
            return parse(f.read(),fileName)
        pass
    except Exception:
        raise in_function_context(parseFile,vars())
    pass

# deprecated, use parseFile
def loadFile(fileName,encoding='utf-8'):
    return parseFile(fileName,encoding)
    pass
