uwl python library
------------------

WSGI dispatcher that maps HTTP requests to python functions of same/similar name.

* function params taken from query string and/or POST content
* function access control via python decorators
* JSON support for parameters and responses
* small javascript library with convenience async postToServer
* and getFromServer functions

see `uwl.example <uwl/example>`_ which implements a HTTP
`server.py <uwl/example/server.py>`_ using
`dispatcher.py <uwl/dispatcher.py>`_ to map HTTP requests to
`app <uwl/example/app.py>`_ and
`app.submodule <uwl/example.submodule.py>`_ functions
