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
import xwl
import xwl.wsgi
from xju import pq

from hashlib import sha256
import time
import threading
from io import RawIOBase
from xwl.example import submodule

quit_requested = False

@xwl.public
def public_html():
    return xwl.html('<html><body>public</body></html>')

@xwl.public
def params(params):
    return xwl.plainText('\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]))

@xwl.public
def json():
    return {
        'name':'fred',
        'age':25
    }

@xwl.public
def html():
    return pq.parse('<a href="json">json</a>')

@xwl.public
def redirect():
    return xwl.redirect('html')

@xwl.public
def index_html():
    return xwl.redirect('test.html')

@xwl.public
def post(params):
    return { 'result': '\n'.join(
        ['{name}: {value}'.format(**vars()) for name,value in
         sorted(params.items())]) }

@xwl.public
def post_json(type_: str, name: str,age: int):
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@xwl.public
def post_json_2(type_: str, name: str,age: int):
    return {
        'type':type_,
        'name':name,
        'age':age}

@xwl.public
def post_json_3(type_: str, name: str,age: int):
    return None

@xwl.public
def post_json_params(json_params):
    '''special name "json_params" captures all params as dictionary'''
    return { 'result': json_params }

@xwl.public
def post_json_5s(type_: str, name: str,age: int):
    time.sleep(5)
    return { 'result': {
        'type':type_,
        'name':name,
        'age':age}}

@xwl.public
def client_error(e):
    raise xwl.ClientError(str(e))

@xwl.public
def login():
    return xwl.plainText('OK')+xwl.cookie('some-session','3')

@xwl.public
def logout():
    return xwl.plainText('OK')+xwl.cookie('some-session','')

@xwl.public
def quit():
    '''make server exit'''
    global quit_requested
    quit_requested = True

@xwl.public
def upload_file(file1: xwl.wsgi.FileVar, file2: xwl.wsgi.FileVar):
    sums=[]
    if file1.filename:
        summer=sha256()
        summer.update(file1.content)
        sums.append(f"{file1.filename} type {file1.contentType} sha256 {summer.hexdigest()}")
    if file2.filename:
        summer=sha256()
        summer.update(file2.content)
        sums.append(f"{file2.filename} type {file2.contentType} sha256 {summer.hexdigest()}")
        
    return xwl.plainText('\n'.join(sums))
    
def isLoggedIn(cookies):
    return cookies.get('some-session',None)=='3'

def onlyIfLoggedIn(url,cookies):
    if not isLoggedIn(cookies):
        return pq.parse('<b>you are not logged in</b>')
    pass

@xwl.restricted(onlyIfLoggedIn)
def login_required():
    return xwl.plainText('OK')

@xwl.restricted(onlyIfLoggedIn)
def multipart_form_data(fred: xwl.wsgi.FileVar,
                         jock: str):
    return { 'result': {
        'fred': (fred.filename, fred.contentType, fred.content.decode('utf-8')),
        'jock': jock}}

@xwl.restricted(onlyIfLoggedIn)
def octet_stream(body: tuple[int, RawIOBase]):
    return { 'result': [body[0], [int(b) for b in body[1].read(body[0]) or b'']]}

@xwl.public
def forbidden():
    raise xwl.Forbidden('forbidden')

@xwl.public
def nothing_there():
    raise xwl.NotFound('not found')

lock = threading.Lock()
flag = [ False ]
flag_changed = threading.Condition(lock)

@xwl.public
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

@xwl.public
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
