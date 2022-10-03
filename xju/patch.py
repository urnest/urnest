# Copyright (c) 2022 Trevor Taylor
# coding: utf-8
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

from xju.assert_ import Assert

class PatchAttr:
    '''patch attribute {attr} of {x} for context duration with value {temporary_value}'''
    def __init__(self,x:object,attr:str,temporary_value):
        Assert(x).hasAttr(attr)
        self.x=x
        self.attr=attr
        self.temporary_value=temporary_value
        pass

    def __enter__(self):
        self.orig=getattr(self.x,self.attr)
        setattr(self.x,self.attr,self.temporary_value)
        return self

    def __exit__(self,t,e,b):
        setattr(self.x,self.attr,self.orig)
        pass
    pass
