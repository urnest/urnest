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
# One common "execute command" usage, navigating some of the subprocess.
#
import subprocess
from typing import Sequence,Tuple
from xju.xn import in_function_context
import os

class CmdFailed(Exception):
    def __init__(self,argv:Sequence[str],status,stderr:str,stdout:bytes):
        self.argv=argv
        self.status=status
        self.stderr=stderr
        self.stdout=stdout
        Exception.__init__(self,f'non-shell command {argv!r} failed with exit status {status} and stderr {stderr}')
        pass
    pass

def do_cmd(argv:Sequence[str])->Tuple[bytes,  # stdout
                                      str]:   # stderr
    '''do non-shell command {argv!r}'''
    '''returns (stdout,stderr) on zero exit status'''
    '''raises CmdFailed on non-zero exit'''
    try:
        try:
            p=subprocess.Popen(args=argv,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
        except OSError as e:
            if e.errno==2:
                path=os.environ['PATH']
                raise Exception('{argv[0]} not found on PATH {path}'.format(**locals())) from None
            raise
        out,err=p.communicate()
        status=p.returncode
        if status!=0:
            raise CmdFailed(argv,status,err.decode('utf-8'),out)
        return out,err.decode('utf-8')
    except:
        raise in_function_context(do_cmd,vars()) from None
    pass
