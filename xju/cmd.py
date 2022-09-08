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
import subprocess
from typing import Sequence,Tuple
from xju.xn import in_context,firstLineOf as l1

class CmdFailed(Exception):
    def __init__(self,argv,status,stderr):
        self.argv=argv
        self.status=status
        self.stderr=stderr
        Exception.__init__(self,'non-shell command {argv!r} failed with exit status {status} and stderr {stderr}'.format(**locals()))
        pass
    pass

def doCmd(argv:Sequence[str])->Tuple[bytes,str]:
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
            raise CmdFailed(argv,status,err.decode('utf-8'))
        return out,err.decode('utf-8')
    except:
        raise in_context(l1(doCmd.__doc__).format(**locals())) from None
    pass
