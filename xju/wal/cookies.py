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
# HTTP RFC6265 cookie stuff

from . import rfc2616
from xju.xn import inContext,firstLineOf as l1

def validateCookieName(name):
    '''validate RFC6265 cookie name {name!r}'''
    try:
        return rfc2616.validateToken(name)
    except:
        raise inContext(l1(validateCookieName.__doc__).format(**vars()))
    pass

def validateCookieValue(v):
    '''validate RFC6265 cookie-value {v!r}'''
    try:
        if v.startswith('"') and not v.endswith('"'):
            raise Exception('{v!r} startswith double quote but does not end with double quote'.format(**vars()))
        x=v[1:-1] if v.startswith('"') else v
        for i,c in enumerate(x):
            o=ord(c)
            if o<0x21 or \
               (o>0x21 and o<0x23) or \
               (o>0x2b and o<0x2d) or \
               (o>0x3a and o<0x3c) or \
               (o>0x4b and o<0x5d) or \
               (o>0x7e):
                rest=result[i:]
                raise Exception('invalid character at {rest!r}'.format(**vars()))
            pass
        return v
    except:
        raise inContext(l1(validateCookieValue.__doc__).format(**vars()))
    pass

def validateCookiePath(v):
    '''validate RFC6265 cookie Path value {v!r}'''
    try:
        for i,c in enumerate(v):
            try:
                if c in rfc2616.CTLs:
                    raise Exception('{c!r} is a RFC2616 control character'.format(**vars()))
                if c==';':
                    raise Exception('{c!r} is a semicolon'.format(**vars()))
            except:
                rest=v[i:]
                raise inContext('validate at ...{rest!r}'.format(**vars()))
            pass
        return v
    except:
        raise inContext(l1(validateCookiePath.__doc__).format(**vars()))
    pass

