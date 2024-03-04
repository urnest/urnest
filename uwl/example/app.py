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
import uwl
import uwl.wsgi
from xju import pq

import time
import threading
from io import RawIOBase
from uwl.example import submodule

quit_requested = False

@uwl.public
def public_html():
    return uwl.html('<html><body>public</body></html>')

@uwl.public
def params(params):
    return uwl.plainText('\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]))

@uwl.public
def json():
    return {
        'name':'fred',
        'age':25
    }

@uwl.public
def html():
    return pq.parse('<a href="json">json</a>')

@uwl.public
def redirect():
    return uwl.redirect('html')

@uwl.public
def index_html():
    return uwl.redirect('test.html')

@uwl.public
def post(params):
    return { 'result': '\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]) }

@uwl.public
def post_json(type_: str, name: str,age: int):
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@uwl.public
def post_json_params(json_params):
    '''special name "json_params" captures all params as dictionary'''
    return { 'result': json_params }

@uwl.public
def post_json_5s(type_: str, name: str,age: int):
    time.sleep(5)
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@uwl.public
def client_error(e):
    raise uwl.ClientError(str(e))

@uwl.public
def login():
    return uwl.plainText('OK')+uwl.cookie('some-session','3')

@uwl.public
def logout():
    return uwl.plainText('OK')+uwl.cookie('some-session','')

@uwl.public
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

@uwl.restricted(onlyIfLoggedIn)
def login_required():
    return uwl.plainText('OK')

@uwl.restricted(onlyIfLoggedIn)
def multipart_form_data(fred: uwl.wsgi.FileVar,
                         jock: str):
    return { 'result': {
        'fred': (fred.filename, fred.contentType, fred.content.decode('utf-8')),
        'jock': jock}}

@uwl.restricted(onlyIfLoggedIn)
def octet_stream(body: tuple[int, RawIOBase]):
    return { 'result': [body[0], [int(b) for b in body[1].read(body[0]) or b'']]}

@uwl.public
def forbidden():
    raise uwl.Forbidden('forbidden')

@uwl.public
def nothing_there():
    raise uwl.NotFound('not found')

lock = threading.Lock()
flag = [ False ]
flag_changed = threading.Condition(lock)

@uwl.public
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

@uwl.public
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
