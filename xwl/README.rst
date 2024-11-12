xwl python library
------------------

WSGI dispatcher that maps HTTP requests to python functions of same/similar name.

* function params taken from query string and/or POST content
* function access control via python decorators
* JSON support for parameters and responses
* small javascript library with convenience async postToServer
* and getFromServer functions

(see the bottom of this readme for release history)

see `xwl.example <xwl/example>`_ which implements a HTTP
`server.py <xwl/example/server.py>`_ using
`dispatcher.py <xwl/dispatcher.py>`_ to map HTTP requests to
`app <xwl/example/app.py>`_ and
`app.submodule <xwl/example.submodule.py>`_ functions

Release History

- 1.0.2 fix None return type handling
- 1.0.1 add missing files to whl
