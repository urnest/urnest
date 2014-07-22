#!/usr/bin/env python
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

class ParseFailed:
    def __init__(self, cause, pos):
        self.cause=cause
        self.line, self.col=pos
        return
    def __str__(self):
        return 'failed to parse html at line %(line)s col %(col)s because\n%(cause)s'%self.__dict__
    pass

entities=htmlentitydefs.entitydefs
reverseentities=dict((_[1],_[0]) for _ in entities.items())

def encodeEntities(s):
    x=''.join([reverseentities.get(_,_) for _ in s])
    return x

class Node:
    def __init__(self, parent=None):
        self.parent=parent
        if parent:
            parent.children.append(self)

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
        self.line, self.col=pos
        self.tagName=tagName
        self.attrs=dict(attrs)
        self.children=[]
        self.end=''
        self.classes=set(self.attrs.get('class','').split())
        return
    def __repr__(self):
        return '%(tagName)s at line %(line):%(col)s' % self.__dict__
    def __str__(self):
        encodedAttrs=['%s="%s"' % (_[0],encodeEntities(_[1])) for 
                      _ in self.attrs.items()]
        encodedAttrs.sort()
        start=' '.join([self.tagName]+encodedAttrs)
        content=''.join([str(_) for _ in self.children])
        end=self.end
        return '''<%(start)s>%(content)s%(end)s'''%vars()
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
    def attr(self, a, val):
        if not val is None:
            self.attrs[a]=val
        return self.attrs.get(a,'')
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
    pass

class Data(Node):
    '''CData, including <script> and <style> content'''
    def __init__(self, data, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self. data=data
        pass
    def __str__(self):
        return self.data
    def __repr__(self):
        return 'data at line %(line):%(col)s' % self.__dict__
    pass

class EntityRef(Node):
    '''Entity ref'''
    def __init__(self, name, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self.name=name
    def __str__(self):
        return '&%(name)s;' % self.__dict__
    def __repr__(self):
        return 'entity ref at line %(line):%(col)s' % self.__dict__
    pass

class CharRef(Node):
    '''Char ref'''
    def __init__(self, name, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self.name=name
    def __str__(self):
        return '&#%(name)s;' % self.__dict__
    def __repr__(self):
        return 'char ref at line %(line):%(col)s' % self.__dict__
    pass

class Comment(Node):
    '''Comment'''
    def __init__(self, comment, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self.comment=comment
    def __str__(self):
        return '<!-- %(comment)s -->' % self.__dict__
    def __repr__(self):
        return 'comment at line %(line):%(col)s' % self.__dict__
    pass

class Decl(Node):
    '''Comment'''
    def __init__(self, decl, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self.decl=decl
    def __str__(self):
        return '<!%(decl)s>' % self.__dict__
    def __repr__(self):
        return 'decl at line %(line):%(col)s' % self.__dict__
    pass

class PI(Node):
    '''Processing Instruction'''
    def __init__(self, pi, parent, pos):
        Node.__init__(self, parent)
        self.line, self.col=pos
        self.pi=pi
    def __str__(self):
        return '<?%(pi)s>' % self.__dict__
    def __repr__(self):
        return 'processing instruction at line %(line):%(col)s' % self.__dict__
    pass

class Parser(HTMLParser.HTMLParser):
    def __init__(self):
        HTMLParser.HTMLParser.__init__(self)
        self.root=Root()
        self.current=self.root
        return
    def handle_starttag(self, tag, attrs):
        if tag in emptyTags:
            Tag(tag, attrs, self.current, self.getpos())
        else:
            #end-optional tag ends current tag of same name, eg
            #<li>x
            #<li>y
            if isinstance(self.current, Tag) and \
                    tag in endOptional and \
                    self.current.tagName == tag:
                self.current=self.current.parent
            self.current=Tag(tag, attrs, self.current, self.getpos())
        return
    def handle_endtag(self, tag):
        while self.current.tagName != tag:
            self.current=self.current.parent
        self.current.end='</%(tag)s>'%vars()
        self.current=self.current.parent
        return
    def handle_data(self,data):
        Data(data, self.current, self.getpos())
        return
    def handle_entityref(self,name):
        EntityRef(name, self.current, self.getpos())
        return
    def handle_charref(self,name):
        CharRef(name, self.current, self.getpos())
        return
    def handle_comment(self,comment):
        Comment(comment, self.current, self.getpos())
        return
    def handle_decl(self,decl):
        Decl(decl,self.current, self.getpos())
        return
    def handle_pi(self,pi):
        PI(pi,self.current, self.getpos())
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
        self.nodeList=nodeList
        return
    def find(self, predicate):
        '''find all children of our nodes that match predicate'''
        result=[]
        for _ in self.nodeList:
            result.extend(filter(_,predicate))
        return Selection(result)
    def filter(self, predicate):
        '''nodes of ours that match predicate'''
        return Selection([_ for _ in nodeList if predicate(_)])
    def html(self, nodes):
        '''replace our first nodes content with the specified list of nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        for n in nodes:
            if n.parent:
                n.parent.remove(n)
        for n in self.nodeList[0:1]:
            n.children=nodes
            for c in n.children:
                c.parent=n
        return self
    def replace(self, nodes):
        '''replace first of specified nodes with first of our nodes'''
        if isinstance(nodes, Selection):
            nodes=nodes.nodeList
        nodes[0].parent.replace(nodes[0],self.nodeList[0])
        return self
    def __str__(self):
        return ''.join([str(_) for _ in self.nodeList])
    def __len__(self):
        return len(self.nodeList)
    pass

# basic predicates
def hasClass(c):
    return lambda node: isinstance(node, Tag) and node.hasClass(c)
def tagName(t):
    return lambda node: isinstance(node, Tag) and node.tagName==t

def parse(s):
    parser=Parser()
    try:
        parser.feed(s)
        parser.close()
        return Selection(parser.root.children)
    except:
        raise ParseFailed(str(''.join(traceback.format_tb(sys.exc_info()[2])))+'\n'+str(sys.exc_info()[1]), parser.getpos())
    pass

def assert_equal(a, b):
    assert a==b, ('%(a)s\n!=\n%(b)s' % vars())

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
    s=parse(html1)
    assert_equal(str(s), html1)
    s2=s.find(hasClass('list'))
    s2.html(parse(newitems))
    assert_equal(str(s), html2)
    s3=s.find(hasClass('item5'))
    parse(newitem5).replace(s3)
    assert_equal(str(s), html3)

if __name__=='__main__':
    try:
        test1()
    except:
        print >>sys.stderr, sys.exc_info()[1]
        sys.exit(1)
