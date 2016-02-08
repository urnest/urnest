#!/usr/bin/env python
# coding: utf-8
# jquery-like python library
#
# parse a HTML text into a tree, with search, manipulation
# and output as HTML text
#
# see test1() near the bottom for usage examples
#
# REVISIT: need better diags, eg:
#  track selectors used to find/filter nodes and report them
#  on failure eg replace/html
#
import HTMLParser
import htmlentitydefs
import sys
import traceback
import types
import string
from xn import Xn,inContext

class Pos:
    def __init__(self, file, line, col):
        self.file=file
        self.line=line
        self.col=col
        return
    def __str__(self):
        return '%(file)s:%(line)s:%(col)s' % self.__dict__
    pass

class ParseFailed(Xn):
    def __init__(self, cause, pos):
        self.cause=cause
        self.pos=pos
        Xn.__init__(self,self.__str__())
        return
    def __str__(self):
        return 'failed to parse html at %(pos)s because\n%(cause)s'%self.__dict__
    pass

entities=htmlentitydefs.name2codepoint
reverseentities=htmlentitydefs.codepoint2name

def encodeEntities(s):
    if s is None: return u''
    x=u''.join([reverseentities.get(_,_) for _ in s])
    return x

class Node:
    def __init__(self, parent=None):
        self.parent=parent
        if parent:
            parent.children.append(self)

def unicodeOfElements(l):
        result=[]
        for i,c in enumerate(l):
            try:
                result.append(unicode(c))
            except:
                t=type(c).__name__
                if t=='instance':
                    t=c.__class__.__name__
                raise inContext('get unicode representation of child %(i)r, %(c)r, which is of type %(t)s'%vars())
            pass
        return result

class Root(Node):
    def __init__(self):
        Node.__init__(self)
        self.children=[]
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
    def __str__(self):
        return ''.join([str(_) for _ in self.children])
    def __unicode__(self):
        return u''.join(unicodeOfElements(self.children))
    def __repr__(self):
        return 'root node'
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
        Node.__init__(self, parent)
        self.pos=pos
        self.tagName=tagName
        self.attrs=dict(attrs)
        self.children=[]
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
        content=''.join([str(_) for _ in self.children])
        end=self.end
        return '''<%(start)s>%(content)s%(end)s'''%vars()
    def __unicode__(self):
        try:
            encodedAttrs=[u'%s="%s"' % (_[0],encodeEntities(_[1])) for 
                          _ in self.attrs.items()]
            encodedAttrs.sort()
            start=u' '.join([self.tagName]+encodedAttrs)
            content=u''.join(unicodeOfElements(self.children))
            end=self.end
            return u'''<%(start)s>%(content)s%(end)s'''%vars()
        except:
            raise inContext('get unicode representation of %(self)r'%vars())
        pass
    def hasClass(self,c):
        return c in self.classes
    def addClass(self,c):
        self.classes.add(c)
        self.attrs['class']=' '.join(self.classes)
        return self
    def removeClass(self,c):
        self.classes.remove(c)
        self.attrs['class']=' '.join(self.classes)
        if len(self.classes)==0:
            del self.attrs['class']
        return self
    def attr(self, a, val=None):
        if not val is None:
            self.attrs[a]=val
        return self.attrs.get(a,'')
    def attrEquals(self, a, val):
        return self.attrs.get(a,None)==val
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
            c.clone(result)
        return result
    def text(self):
        if self.tagName=='br':
            return u'\n'
        if self.tagName=='p':
            return u''.join([_.text() for _ in self.children])+u'\n'
        if self.tagName=='li':
            return u'\n'
        return u''.join([_.text() for _ in self.children])
    pass

class Data(Node):
    '''CData, including <script> and <style> content'''
    def __init__(self, data, parent, pos):
        Node.__init__(self, parent)
        self.pos=pos
        self.data=data
        pass
    def __str__(self):
        return self.data
    def __unicode__(self):
        if type(self.data) is types.UnicodeType:
            return self.data
        if type(self.data) is types.StringType:
            return unicode(self.data,'utf-8','strict')
        return unicode(self.data)
    def __repr__(self):
        return 'data at %(pos)s, %(data)r' % self.__dict__
    def clone(self, newParent):
        result=Data(self.data, newParent, self.pos)
        return result
    def text(self):
        return unicode(self)
    pass

class EntityRef(Node):
    '''Entity ref'''
    def __init__(self, name, parent, pos):
        Node.__init__(self, parent)
        self.pos=pos
        self.name=name
    def __str__(self):
        return '&%(name)s;' % self.__dict__
    def __unicode__(self):
        return u'&%(name)s;' % self.__dict__
    def __repr__(self):
        return 'entity ref %(name)r at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=EntityRef(self.name, newParent, self.pos)
        return result
    def text(self):
        if not self.name in entities:
            raise Xn('unknown entity %(name)s'%self.__dict__)
        return unichr(entities[self.name])
    pass

class CharRef(Node):
    '''Char ref'''
    def __init__(self, name, parent, pos):
        Node.__init__(self, parent)
        self.pos=pos
        self.name=name
    def __str__(self):
        return '&#%(name)s;' % self.__dict__
    def __unicode__(self):
        return u'&#%(name)s;' % self.__dict__
    def __repr__(self):
        return 'char ref at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=CharRef(self.name, newParent, self.pos)
        return result
    def text(self):
        assert 'text() not implemented for %(self)r' % vars()
    pass

class Comment(Node):
    '''Comment'''
    def __init__(self, comment, parent, pos):
        Node.__init__(self, parent)
        self.pos=pos
        self.comment=comment
    def __str__(self):
        return '<!-- %(comment)s -->' % self.__dict__
    def __unicode__(self):
        return u'<!-- %(comment)s -->' % self.__dict__
    def __repr__(self):
        return 'comment at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=Comment(self.comment, newParent, self.pos)
        return result
    def text(self):
        return u''
    pass

class Decl(Node):
    '''Comment'''
    def __init__(self, decl, parent, pos):
        Node.__init__(self, parent)
        self.pos=pos
        self.decl=decl
    def __str__(self):
        return '<!%(decl)s>' % self.__dict__
    def __unicode__(self):
        return u'<!%(decl)s>' % self.__dict__
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
        Node.__init__(self, parent)
        self.pos=pos
        self.pi=pi
    def __str__(self):
        return '<?%(pi)s>' % self.__dict__
    def __unicode__(self):
        return u'<?%(pi)s>' % self.__dict__
    def __repr__(self):
        return 'processing instruction at %(pos)s' % self.__dict__
    def clone(self, newParent):
        result=PI(self.pi, newParent, self.pos)
        return result
    def text(self):
        return u''
    pass

class Parser(HTMLParser.HTMLParser):
    def __init__(self, fileName):
        HTMLParser.HTMLParser.__init__(self)
        self.fileName=fileName
        self.root=Root()
        self.current=self.root
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
        while not current is self.root and current.tagName != tag:
            current=current.parent
            pass
        if not current is self.root:
            self.current=current
            self.current.end=u'</%(tag)s>'%vars()
            self.current=self.current.parent
        return
    def handle_data(self,data):
        Data(data, self.current, self.pos())
        return
    def handle_entityref(self,name):
        EntityRef(name, self.current, self.pos())
        return
    def handle_charref(self,name):
        CharRef(name, self.current, self.pos())
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
    def __init__(self, nodeList):
        if isinstance(nodeList,Node):
            self.nodeList=[nodeList,]
        else:
            self.nodeList=nodeList[:]
            pass
        return
    def find(self, predicate):
        '''find all children of our nodes that match predicate'''
        result=[]
        for _ in self.nodeList:
            result.extend(filter(_,predicate))
        return Selection(result)
    def filter(self, predicate):
        '''nodes of ours that match predicate'''
        return Selection([_ for _ in self.nodeList if predicate(_)])
    def html(self, nodes):
        '''replace our first node's children with the specified list of nodes/html string'''
        if type(nodes)==types.StringType:
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
        '''replace our first node's children with the specified text string'''
        '''or return unicode concatenation of text content of children'''
        if s is None:
            return u''.join([_.text() for _ in self.nodeList])
        if type(s) is types.StringType:
            s=unicode(s,'utf-8','strict')
        for n in self.nodeList:
            Selection([n]).html(parse(encodeEntities(s)))
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
            for c in n.children:
                c.parent=None
                pass
            n.children=[]
            pass
        return self
    def first(self):
        '''return Selection containing first of our nodes'''
        return Selection(self.nodeList[0:1])
    def children(self):
        '''return Selection containing children of our nodes'''
        return Selection(sum([_.children for _ in self.nodeList],[]))
    def clone(self):
        '''return Selection containing a copy of our nodes'''
        return Selection([_.clone(None) for _ in self.nodeList])
    def addClass(self, name):
        '''add class %(name)s to each of our children'''
        for n in self.nodeList:
            n.addClass(name)
        return self
    def removeClass(self, name):
        '''remove class %(name)s from each of our children'''
        for n in self.nodeList:
            n.removeClass(name)
        return self
    def attr(self, name, value=None):
        '''attr('src') lists the values of the src attributes of each of our nodes'''
        """attr('src','fred.html') sets the src attribute of each of our nodes to 'html'"""
        if value is None:
            return [_.attr(name, value) for _ in self.nodeList]
        [_.attr(name, value) for _ in self.nodeList]
        return self
    def __str__(self):
        return ''.join([str(_) for _ in self.nodeList])
    def __unicode__(self):
        return u''.join(unicodeOfElements(self.nodeList))
    def __len__(self):
        return len(self.nodeList)
    def __getitem__(self, key):
        return self.nodeList[key]
    def __getslice__(self, i, j):
        return Selection(self.nodeList[i:j])
    def __add__(self, b):
        assert isinstance(b,Selection), repr(b)
        return Selection(self.nodeList+b.nodeList)
    pass

# basic predicates
def hasClass(c):
    return lambda node: isinstance(node, Tag) and node.hasClass(c)
def tagName(t):
    return lambda node: isinstance(node, Tag) and node.tagName==t
def attrEquals(attr,value):
    return lambda node: isinstance(node, Tag) and node.attrEquals(attr,value)
def isEntityRef(name):
    return lambda node: isinstance(node, EntityRef) and node.name==name

def parse(s, origin='unknown',encoding='utf-8'):
    '''parse HTML string "%(origin)s" assuming it has %(encoding)r encoding (per python unicode() function), returns a Selection'''
    parser=Parser(origin)
    try:
        u=s
        if type(s) is types.StringType:
            u=unicode(s,encoding,'strict')
            pass
        parser.feed(u)
        parser.close()
        result=Selection(parser.root.children)
        result.detach()
        return result
    except:
        raise ParseFailed(str(''.join(traceback.format_tb(sys.exc_info()[2])))+'\n'+str(sys.exc_info()[1]), parser.pos())
    pass

def parseFile(fileName,encoding='utf-8'):
    return parse(file(fileName).read(),fileName,encoding)

def loadFile(fileName,encoding='utf-8'):
    return parse(file(fileName).read(),fileName,encoding)

def assert_equal(a, b):
    assert a==b, (u'%(a)r\n!=\n%(b)r' % vars())

html1='''<html>
<body>
<p>The best thing about html is simplicity
<ul class="list">
<li class="i">item 1
<li class="i">item 2</li>
</ul>
</body>
</html>'''

#note the spacing is carefully chosen to get the format of html2 (below)
newitems='''
<li>item 4
<li class="item5">item 5
<li>item 6
'''

html2='''<html>
<body>
<p>The best thing about html is simplicity
<ul class="list">
<li>item 4
<li class="item5">item 5
<li>item 6
</ul>
</body>
</html>'''

html3='''<html>
<body>
<p>The best thing about html is simplicity
<ul class="list">
<li>item 4
<li>item &lt;b&gt;
<li>item 6
</ul>
</body>
</html>'''

#note the spacing is carefully chosen to get the format of html2 (below)
newitem5='''<li>item &lt;b&gt;
'''

def test1():
    s=parse(html1, 'html1')
    assert_equal(str(s), html1)
    s2=s.find(hasClass('list'))
    s2.html(parse(newitems, 'newitems'))
    assert_equal(str(s), html2)
    s2.html(newitems)
    assert_equal(str(s), html2)
    s3=s.find(hasClass('item5'))
    parse(newitem5, 'newitem5').replace(s3)
    assert_equal(str(s), html3)

html4='''<html>
<body>
<p>The best thing about html is simplicity

</body>
</html>'''

def test2():
    s=parse(html1, 'html1')
    sc=s.clone()
    sc.find(hasClass('list')).detach()
    assert_equal(str(s), html1)
    assert_equal(str(sc), html4)

html5='''<html>
<body>
<p>The best thing about html is simplicity
<ul class="list">
<li class="i">item 1
<li class="i">item 2</li>
<li>item &lt;b&gt;
</ul>
</body>
</html>'''

def test3():
    s=parse(html1, 'html1')
    s2=parse(newitem5, 'newitem5')
    assert_equal(str(s2.appendTo(s.find(hasClass('list')))),newitem5)
    assert_equal(str(s), html5)

script='''
$(document).ready(function(){
});
'''
def test4():
    a=parse('<head></head>')
    parse(encodeEntities(script)).appendTo(a)
    assert_equal(str(a), '<head>'+script+'</head>')
    pass

def test5():
    s=parse('<p>fred</p>')
    s.text('jock')
    assert_equal(unicode(s),u'<p>jock</p>')
    pass

def test6():
    s=parse('<p>fred</p>')
    s.text(u'30x40”')
    assert_equal(unicode(s),u'<p>30x40”</p>')
    pass

def test7():
    s=parse('<p>fred</p>')
    s.text('30x40”')
    assert_equal(unicode(s),u'<p>30x40”</p>')
    pass

def test8():
    s=parse('<html><p>fred</p><p>jock</p></html>')
    s=s.children()
    assert len(s)==2, unicode(s).encode('utf-8')
    assert_equal(unicode(s.first()),u'<p>fred</p>')
    pass

def test9():
    s=parse('<td><a href="fred">jock</a> and fred</td>')
    assert_equal(s.text(), u'jock and fred')
    pass

def test10():
    s=parse('<td><a href="fred">jock</a>&nbsp;and fred</td>')
    assert_equal(s.text(), u'jock\xa0and fred')
    s1=parse('<td><a href="fred">jock</a>&nbsp;')
    s2=parse('and fred</td>')
    assert_equal((s1+s2).text(), u'jock\xa0and fred')
    pass

def test11():
    s=parse('<a href="fred">John&nbsp;Walker</a>')
    s.find(isEntityRef('nbsp')).remove()
    assert str(s)=='<a href="fred">JohnWalker</a>',s.text()
    pass

def test12():
    s=parse('<ul><li class="a">1<li class="b">2</ul>')
    parse('<li>1.5').addBefore(s.find(hasClass('b')))
    assert_equal(unicode(s),u'<ul><li class="a">1<li>1.5<li class="b">2</ul>')
    
def test13():
    s=parse('<ul><li class="a">1<li class="b">2</ul>')
    parse('<li>0').addBefore(s.find(hasClass('a')))
    assert_equal(unicode(s),u'<ul><li>0<li class="a">1<li class="b">2</ul>')

if __name__=='__main__':
        test1()
        test2()
        test3()
        test4()
        test5()
        test6()
        test7()
        test8()
        test9()
        test10()
        test11()
        test12()
        test13()
