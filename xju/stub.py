# Copyright (c) 2025 Trevor Taylor
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
from asyncio import Future,Queue,get_running_loop
from inspect import BoundArguments,Signature,signature
from typing import Callable,Any
from xju.time import now,Duration,Timestamp
from xju.cmc import Lock,Mutex,Condition
from xju.xn import in_function_context

class Params:
    # note params added dynamically as attributes
    pass

class FunctionCalled:
    timestamp: Timestamp
    params: Params
    guard: Mutex
    changed: Condition
    result: Future

    def __init__(self, args: BoundArguments):
        self.timestamp=now()
        self.params = Params()
        for n, v in args.arguments.items():
            setattr(self.params, n, v)
            pass
        self.guard=Mutex()
        self.changed=Condition(self.guard)
        self.result = Future()
        pass
    def return_(self, result) -> None:
        with Lock(self.guard) as l:
            self.result.set_result(result)
            self.changed.notify_all(l)
        pass
    def raise_(self, e: Exception) -> None:
        with Lock(self.guard) as l:
            self.result.set_exception(e)
            self.changed.notify_all(l)
        pass
    pass

                
class SyncFunctionStub:
    timeout: Duration
    sig: Signature
    guard: Mutex
    changed: Condition
    calls_in_progress: list[FunctionCalled]

    def __init__(self, f: Callable, timeout: Duration):
        """
        stub for {f} that waits for at most {timeout}s for calls to it to be resolved

        i.e. for result or exception to be supplied
        """
        self.timeout=timeout
        self.sig=signature(f)
        self.guard=Mutex()
        self.changed=Condition(self.guard)
        self.calls_in_progress=[]
        pass

    async def called(self, max_time_to_cancel=Duration(1)) -> FunctionCalled:
        """
        wait for {self} to be called

        result.params will contain the params passed (by name
        as attributes)

        complete the call/raise an exception from it via
        result.return_() / result.raise_()

        max_time_to_cancel controls how long this function blocks in a non-asyncio
        cancellable function before awakening to allow cancellation

        wrap this function with asyncio.timeout to provide an overall timeout
        i.e. to actually trigger that cancellation
        """
        try:
            with Lock(self.guard) as l:
                while not self.calls_in_progress:
                    await get_running_loop().run_in_executor(
                        None, self.changed.wait_for, l, max_time_to_cancel)
                    pass
                return self.calls_in_progress.pop(0)
        except Exception:
            raise in_function_context(SyncFunctionStub.called,vars()) from None
        pass

    def __call__(self, *args, **kwargs) -> Any:
        """
        do (fake) call to function with args {args} and kwargs {kwargs}
        """
        try:
            with Lock(self.guard) as l:
                c = FunctionCalled(self.sig.bind(*args, **kwargs))
                self.calls_in_progress.append(c)
                self.changed.notify_all(l)
                pass
            with Lock(c.guard) as l:
                deadline=now()+self.timeout
                while not c.result.done():
                    if (n:=now()) > deadline:
                        raise TimeoutError(
                            f"{self} waited more than {self.timeout} for {c} to be resolved")
                    c.changed.wait_for(l, deadline-n)
                    pass
                return c.result.result()
            pass
        except Exception:
            raise in_function_context(SyncFunctionStub.__call__,vars()) from None
        pass
    pass
