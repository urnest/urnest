xju python library
------------------

Various modules implemented to some broad principles:

* fine-grained static typing
* pure context management
* useful functionality that is hard to use incorrectly
* 100% test coverage

`xju.newtype <xju/newtype.py>`_ - static and dynamic distinct int, float and str types

* unlike typing.NewType the new types are compatible with isinstance, so you
  can actually use them to do real stuff, like implement overloaded methods

* see `xju/newtype.py.test <xju/newtype.py.test>`_ for sample code

`xju.cmc <xju/cmc/__init__.py>`_ - context management

* unlike python standard library e.g. open(), these are "pure" context managers: resources
  are never acquired until \__enter__

`xju.cmc.cmclass <xju/cmc/__init__.py>`_  - provides context management for class attributes that are context managers

* managing multiple resource attributes is clumsy with ExitStack, this module implements
  \__enter__ and \__exit__ automatically to ensure correct ordering and cleanup on exceptions

* see `xju/cmc/cmclass.py.test <xju/cmc/cmclass.py.test>`_ for sample code

`xju.cmc.Dict <xju/cmc/__init__.py>`_ - dictionary that is a context manager for its (context manager) values

* see `xju/cmc/Dict.py.test <xju/cmc/Dict.py.test>`_ for sample code

`xju.cmc.io <xju/cmc/io/__init__.py>`_ - pure context management for e.g. file reading and writing

* see unit tests for sample code:
  *  `FileLock.py.test <xju/cmc/io/FileLock.py.test>`_
  *  `FileMode.py.test <xju/cmc/io/FileMode.py.test>`_
  *  `FilePosition.py.test <xju/cmc/io/FilePosition.py.test>`_
  *  `FileReader.py.test <xju/cmc/io/FileReader.py.test>`_
  *  `FileWriter.py.test <xju/cmc/io/FileWriter.py.test>`_

`xju.cmc.tstore <xju/cmc/tstore.py>`_

* time-based storage, organised as files covering time-ranges

* see `xju/cmc/tstore.py.test <xju/cmc/tstore.py.test>`_ for sample code

`xju.cmc.perflog <xju/cmc/perflog.py>`_

* time-based json-format record storage built on xju.cmc.tstore

* see `xju/cmc/perflog.py.test <xju/cmc/perflog.py.test>`_ for sample code

`xju.pq <xju/pq.py>`_

* jquery-like html inspection and modification

* see `xju/pq.py.test <xju/pq.py.test>`_ for sample code

`xju.assert_ <xju/assert_.py>`_

* assert functions that capture term values e.g. x and y in Assert(x)==y

* see `xju/assert_.py.test <xju/assert_.py.test>`_ for sample code

`xju.cmd <xju/cmd.py>`_

* wrapper for subprocess.Popen that captures very common usage without the option-and-flag-warren of subprocess

* see `xju/cmd.py.test <xju/cmd.py.test>`_ for sample code

`xju.json_codec <xju/json_codec.py>`_

* encoding/decoding type-hinted dict/list/int/bool/None/float/str and classes to and from json

* designed to fit well with type checking

* built in support for xju.newtype described above

* generates json schema equivalent schemas for types

* see `xju/json_codec.py.test <xju/json_codec.py.test>`_ for full sample code

`xju.jsonschema <xju/jsonschema.py>`_

* represents JSON schemas as straight-foward, easy-to-read python data structures, because life's too short for jsonschema

* see `xju/jsonschema.py.test <xju/jsonschema.py.test>`_ for sample code

`xju.patch <xju/patch.py>`_

* minimal, simple, direct patching(/stub/mock) utility, unlike mock-warren. Because one shouldn't need a degree to read and write a unit test

* see `xju/patch.py.test <xju/patch.py.test>`_ for sample code

`xju.time <xju/time.py>`_

* type-safe time and duration classes

* see `xju/time.py.test <xju/time.py.test>`_ for sample code

`xju.xn <xju/xn.py>`_

* Exception wrapping to provide human readable context gathering

* see `xju/xn.py.test <xju/xn.py.test>`_ for sample code
