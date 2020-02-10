#!/usr/bin/env python3
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

from xju.wal.rfc2616 import validateToken
from xju.xn import readableRepr
from xju import assert_

if __name__=='__main__':
    validateToken('fred')
    try:
        validateToken('')
    except Exception as e:
        assert_.equal(readableRepr(e),'''\
Failed to validate RFC2616 token '' because
'' is empty.''')
    else:
        assert False
        pass
    try:
        validateToken('fred jones')
    except Exception as e:
        assert_.equal(readableRepr(e),'''\
Failed to validate RFC2616 token 'fred jones' because
failed to validate char at ...' jones' because
' ' is a separator.''')
    else:
        assert False
        pass
    try:
        validateToken('fred\njones')
    except Exception as e:
        assert_.equal(readableRepr(e),'''\
Failed to validate RFC2616 token 'fred\\njones' because
failed to validate char at ...'\\njones' because
'\\n' is a control character.''')
    else:
        assert False
        pass
    pass

