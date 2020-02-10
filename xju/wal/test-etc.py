#!/usr/bin/env python3

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

from xju.wal.etc import Scope

import io
import sys

from xju.assert_ import Assert
sys.stdout=io.StringIO()
with Scope('do some stuff') as scope:
    pass
Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = None
'''
sys.stdout=io.StringIO()
def f():
    with Scope('do some stuff') as scope:
        return scope.result(77)
    pass

Assert(f())==77
Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = 77
'''
def f():
    with Scope('do some stuff') as scope:
        raise Exception('did not work')
    pass
try:
    f()
except Exception as e:
    Assert(e.readableRepr())=='''\
Failed to do some stuff because
did not work.'''
else:
    assert False,'should not reach'
    pass
