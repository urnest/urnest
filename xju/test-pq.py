#!/usr/bin/env python3
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
# and output as HTML text
#
# see test1() near the bottom for usage examples
#
# REVISIT: need better diags, eg:
#  track selectors used to find/filter nodes and report them
#  on failure eg replace/html
#

from xju.pq import parse,hasClass,encodeEntities,isEntityRef,tagName,Selection
from xju.assert_ import Assert

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
    Assert(str(s))==html1
    s2=s.find(hasClass('list'))
    s2.html(parse(newitems, 'newitems'))
    Assert(str(s))==html2
    s2.html(newitems)
    Assert(str(s))==html2
    s3=s.find(hasClass('item5'))
    parse(newitem5, 'newitem5').replace(s3)
    Assert(str(s))==html3

html4='''<html>
<body>
<p>The best thing about html is simplicity

</body>
</html>'''

def test2():
    s=parse(html1, 'html1')
    sc=s.clone()
    sc.find(hasClass('list')).detach()
    Assert(str(s))==html1
    Assert(str(sc))==html4

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
    Assert(str(s2.appendTo(s.find(hasClass('list')))))==newitem5
    Assert(str(s))==html5

script='''
$(document).ready(function(){
});
'''
def test4():
    a=parse('<head></head>')
    parse(encodeEntities(script)).appendTo(a)
    Assert(str(a))=='<head>'+script+'</head>'
    pass

def test5():
    s=parse('<p>fred</p>')
    s.text('jock')
    Assert(str(s))==u'<p>jock</p>'
    pass

def test6():
    s=parse('<p>fred</p>')
    s.text(u'30x40”')
    Assert(str(s))==u'<p>30x40&rdquo;</p>'
    pass

def test7():
    s=parse('<p>fred</p>')
    s.text(u'30x40”')
    Assert(str(s))==u'<p>30x40&rdquo;</p>'
    pass

def test8():
    s=parse('<html><p>fred</p><p>jock</p></html>')
    s=s.children()
    assert len(s)==2, str(s).encode('utf-8')
    Assert(str(s.first()))==u'<p>fred</p>'
    pass

def test9():
    s=parse('<td><a href="fred">jock</a> and fred</td>')
    Assert(s.text())==u'jock and fred'
    pass

def test10():
    s=parse('<td><a href="fred">jock</a>&nbsp;and fred</td>')
    Assert(s.text())==u'jock\xa0and fred'
    s1=parse('<td><a href="fred">jock</a>&nbsp;')
    s2=parse('and fred</td>')
    Assert((s1+s2).text())==u'jock\xa0and fred'
    pass

def test12():
    s=parse('<ul><li class="a">1<li class="b">2</ul>')
    parse('<li>1.5').addBefore(s.find(hasClass('b')))
    Assert(str(s))==u'<ul><li class="a">1<li>1.5<li class="b">2</ul>'
    
def test13():
    s=parse('<ul><li class="a">1<li class="b">2</ul>')
    parse('<li>0').addBefore(s.find(hasClass('a')))
    Assert(str(s))==u'<ul><li>0<li class="a">1<li class="b">2</ul>'

def test14():
    s=parse('<li>')
    s.attr('x','"fred&jock"')
    Assert(str(s))==u'<li x="&quot;fred&amp;jock&quot;">'
    Assert(s.attr('x'))=='"fred&jock"'
    s=parse('<div><p a="fred"><bold>a</bold>x</p><p a="jock">b</p></div>').find(tagName('p'))
    assert len(s)==2, s.utf8()
    Assert(s.attr('a'))=='fredjock'
    Assert(s.attrs('a'))==['fred','jock']
    pass

def test15():
    s=parse('&lambda;')
    Assert(s.text())==chr(955), s.text()
    s=parse('&#955;')
    Assert(s.text())==chr(955), s.text()

def test16():
    s=parse('<div><p><bold>a</bold>x</p><p>b</p></div>').find(tagName('p'))
    Assert(len(s))==2
    Assert(Selection(s[0]).find(tagName('bold')).text())=='a'
    Assert(s[0].find(tagName('bold')).text())=='a'
    pass

def test17():
    s=parse('<div>fred &#x00022;jock&#34;</div>')
    Assert(Selection(s[0]).find(tagName('div')).text())=='fred "jock"'
    pass

def test18():
    s=parse('<body><div>a</div><div class="mid">b</div><div>c</div></body>')
    Assert(s.find(hasClass('mid')).predecessors().text())=='a',s.find(hasClass('mid')).predecessors()
    Assert(s.find(hasClass('mid')).successors().text())=='c',s.find(hasClass('mid')).predecessors()
    pass

if __name__=='__main__':
    tests=[var for name,var in list(vars().items())
           if name.startswith('test') and callable(var)]
    for t in tests:
        t()
        pass
    print('PASS - {0} steps'.format(len(tests)))
    pass
