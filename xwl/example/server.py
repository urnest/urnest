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

import sys
import os
from pathlib import Path

d=Path(sys.argv[0]).absolute().parent

sys.path[0:0]=[str(d.parent.parent),] # for xwl, xju
print(sys.path)

import app

from wsgiref.simple_server import make_server

from xwl.dispatcher import Dispatcher

port=int(sys.argv[1])

httpd=make_server('', port, Dispatcher(app,d).main)
print("Serving HTTP on port {port}...".format(**vars()))

# Respond to requests until client requests quit
while not app.quit_requested:
    httpd.handle_request()
