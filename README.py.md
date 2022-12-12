xju python library

Various modules implemented to some broad principles:
  - fine-grained static typing
  - pure context management
  - useful functionality that is hard to use incorrectly

[xju.newtype](xju/newtype.py) - static and dynamic distinct int, float and str types

  - unlike typing.NewType the new types are compatible with isinstance, so you
    can actually use them to do real stuff, like implement overloaded methods

  - see [unit test](xju/newtype.py.test) for sample code

[xju.cmc](xju/cmc/__init__.py) - context management

  - unlike python standard library e.g. open(), these are "pure" context managers: resources
    are never acquired until \__enter__

[xju.cmc.cmclass](xju/cmc/__init__.py) - provides context management for class attributes that are context managers

  - managing multiple resource attributes is clumsy with ExitStack, this module implements
    \__enter__ and \__exit__ automatically to ensure correct ordering and cleanup on exceptions

  - see [unit test](xju/cmc/cmclass.py.test) for sample code

[xju.cmc.Dict](xju/cmc/__init__.py) - dictionary that is a context manager for its (context manager) values

  - see [unit test](xju/cmc/Dict.py.test) for sample code

[xju.cmc.io](xju/cmc/io/__init__.py) - pure context management for e.g. file reading and writing

  - see unit tests for sample code:
    -  [FileLock.py.test](xju/cmc/io/FileLock.py.test)
    -  [FileMode.py.test](xju/cmc/io/FileMode.py.test)
    -  [FilePosition.py.test](xju/cmc/io/FilePosition.py.test)
    -  [FileReader.py.test](xju/cmc/io/FileReader.py.test)
    -  [FileWriter.py.test](xju/cmc/io/FileWriter.py.test)

[xju.cmc.tstore](xju/cmc/tstore.py)

  - time-based storage, organised as files covering time-ranges

  - see [unit test](xju/cmc/tstore.py.test) for sample code

[xju.cmc.perflog](xju/cmc/perflog.py)

  - time-based json-format record storage built on xju.cmc.tstore

  - see [unit test](xju/cmc/perflog.py.test) for sample code

[xju.pq](xju/pq.py)

  - jquery-like html inspection and modification

  - see [unit test](xju/pq.py.test) for sample code

[xju.assert_](xju/assert_.py)

  - assert functions that capture term values e.g. x and y in Assert(x)==y

  - see [unit test](xju/assert_.py.test) for sample code

[xju.cmd](xju/cmd.py)

  - wrapper for subprocess.Popen that captures very common usage without the option-and-flag-warren of subprocess

  - see [unit test](xju/cmd.py.test) for sample code

[xju.jsonschema](xju/jsonschema.py)

  - represents JSON schemas as straight-foward, easy-to-read python data structures, because life's too short for jsonschema

  - see [unit test](xju/jsonschema.py.test) for sample code

[xju.patch](xju/patch.py)

  - minimal, simple, direct patching(/stub/mock) utility, unlike
mock-warren. Because one shouldn't need a degree to read and write a unit test

  - see [unit test](xju/patch.py.test) for sample code

[xju.time](xju/time.py)

  - type-safe time and duration classes

  - see [unit test](xju/time.py.test) for sample code

[xju.xn](xju/xn.py)

  - Exception wrapping to provide human readable context gathering

  - see [unit test](xju/xn.py.test) for sample code

