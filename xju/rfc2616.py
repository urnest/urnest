# coding: utf-8

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
from xju.xn import in_function_context,in_context
from xju import assert_

CTLs=set.union(set([chr(n) for n in range(0,32)]),set([chr(127)]))
separators=set('()<>@,;:\\"{} \t')

def validateToken(name):
    '''validate RFC2616 token {name!r}'''
    try:
        if not len(name): raise Exception(f'{name!r} is empty')
        for i,c in enumerate(name):
            o=ord(c)
            try:
                if ord(c)<0 or ord(c)>127:
                    raise Exception(f'{o} not 0..127')
                if c in CTLs:
                    raise Exception(f'{c!r} is a control character')
                if c in separators:
                    raise Exception(f'{c!r} is a separator')
            except:
                rest=name[i:]
                raise in_context(f'validate first char' if i==0 else f'validate char at ...{rest!r}') from None
            pass
        return name
    except:
        raise in_function_context(validateToken,vars()) from None
    pass

