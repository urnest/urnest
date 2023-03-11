# Copyright (c) 2023 Trevor Taylor
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
# unix signal
#
from __future__ import annotations

from contextlib import AbstractContextManager
from asyncio import get_event_loop, Future, AbstractEventLoop
from xju import newtype
import signal

class SignumTag: pass
class Signum(newtype.Int[SignumTag]):
    pass

# some common signal types, get other values from signal module or OS
SIGTERM = Signum(signal.SIGTERM)
SIGPIPE = Signum(signal.SIGPIPE)

class AsyncSignalLatch(AbstractContextManager):
    loop: None|AbstractEventLoop
    result: None|Future[Signum]
    signum: Signum

    def __init__(self, signum: Signum):
        '''note must be created in main thread'''
        self.signum=signum
        pass

    def __enter__(self) -> Future[Signum]:
        '''latch next occurrance of signal {self.signum}'''
        self.loop = get_event_loop()
        self.result = self.loop.create_future()
        self.loop.add_signal_handler(self.signum.value(), self._signalled)
        return self.result

    def __exit__(self, t, e, b):
        assert self.result is not None
        assert self.loop is not None
        self.loop.remove_signal_handler(self.signum.value())
        

    def _signalled(self):
        assert self.result is not None
        self.result.set_result(self.signum)
        pass
    pass
