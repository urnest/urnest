# Copyright (c) 2018 Trevor Taylor
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
import wal
import wal.wsgi
from xju import pq

import time
import threading
from io import RawIOBase
from wal.example import submodule

quit_requested = False

@wal.public
def public_html():
    return wal.html('<html><body>public</body></html>')

@wal.public
def params(params):
    return wal.plainText('\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]))

@wal.public
def json():
    return {
        'name':'fred',
        'age':25
    }

@wal.public
def html():
    return pq.parse('<a href="json">json</a>')

@wal.public
def redirect():
    return wal.redirect('html')

@wal.public
def index_html():
    return wal.redirect('test.html')

@wal.public
def post(params):
    return { 'result': '\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]) }

@wal.public
def post_json(type_: str, name: str,age: int):
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@wal.public
def post_json_params(json_params):
    '''special name "json_params" captures all params as dictionary'''
    return { 'result': json_params }

@wal.public
def post_json_5s(type_: str, name: str,age: int):
    time.sleep(5)
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@wal.public
def client_error(e):
    raise wal.ClientError(str(e))

@wal.public
def login():
    return wal.plainText('OK')+wal.cookie('some-session','3')

@wal.public
def logout():
    return wal.plainText('OK')+wal.cookie('some-session','')

@wal.public
def quit():
    '''make server exit'''
    global quit_requested
    quit_requested = True

def isLoggedIn(cookies):
    return cookies.get('some-session',None)=='3'

def onlyIfLoggedIn(url,cookies):
    if not isLoggedIn(cookies):
        return pq.parse('<b>you are not logged in</b>')
    pass

@wal.restricted(onlyIfLoggedIn)
def login_required():
    return wal.plainText('OK')

@wal.restricted(onlyIfLoggedIn)
def multipart_form_data(fred: wal.wsgi.FileVar,
                         jock: str):
    return { 'result': {
        'fred': (fred.filename, fred.contentType, fred.content.decode('utf-8')),
        'jock': jock}}

@wal.restricted(onlyIfLoggedIn)
def octet_stream(body: tuple[int, RawIOBase]):
    return { 'result': [body[0], [int(b) for b in body[1].read(body[0]) or b'']]}

@wal.public
def forbidden():
    raise wal.Forbidden('forbidden')

@wal.public
def nothing_there():
    raise wal.NotFound('not found')

lock = threading.Lock()
flag = [ False ]
flag_changed = threading.Condition(lock)

@wal.public
def await_flag(lock = lock, flag = flag, flag_changed = flag_changed):
    print('+ await-flag')
    with lock:
        flag[0] = False
        while not flag[0]:
            flag_changed.wait()
            pass
        pass
    print('- await-flag')
    return True

@wal.public
def set_flag(lock = lock, flag = flag, flag_changed = flag_changed):
    print('+ set-flag')
    with lock:
        flag[0] = True
        flag_changed.notify_all()
        pass
    print('- set-flag')
    return True

def not_public():
    pass

not_callable=0
