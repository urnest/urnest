xju python library

Various modules implemented to some broad principles:
  - fine-grained static typing
  - pure context management
  - useful functionality that is hard to use incorrectly

[xju.newtype](xju/newtype.py) - static and dynamic distinct int, float and str types

  - unlike typing.NewType the new types are compatible with isinstance, so you
    can actually use them to do real stuff, like implement overloaded methods

[xju.cmc](xju/cmc/__init__.py) - context management

  - unlike python standard library e.g. open(), these are "pure" context managers: resources
    are never acquired until \__enter__

[xju.cmc.cmclass](xju/cmc/__init__.py) - provides context management for class attributes that are context managers

  - managing multiple resource attributes is clumsy with ExitStack, this module implements
    \__enter__ and \__exit__ automatically to ensure correct ordering and cleanup on exceptions

[xju.cmc.Dict](xju/cmc/__init__.py) - dictionary that is a context manager for its (context manager) values

[xju.cmc.io](xju/cmc/io/__init__.py) - pure context management for e.g. file reading and writing

[xju.cmc.tstore](xju/cmc/tstore.py) - time-based storage, organised as files covering time-ranges

[xju.cmc.perflog](xju/cmc/perflog.py) - time-based json-format record storage built on xju.cmc.tstore

[xju.pq](xju/py.py) - jquery-like html inspection and modification

[xju.assert_](xju/assert_.py) - assert functions that capture term values e.g. x and y in Assert(x)==y

[xju.cmd](xju/cmd.py) - wrapper for subprocess.Popen that captures very common usage without the option-and-flag-warren of subprocess

[xju.jsonschema](xju/jsonschema.py) - represents JSON schemas as straight-foward, easy-to-read python data structures, because life's too short for jsonschema

[xju.patch](xju/patch.py) - minimal, simple, direct patching(/stub/mock) utility, unlike
mock-warren. Because one shouldn't need a degree to read and write a unit test

[xju.time](xju/time.py) - type-safe time and duration classes

[xju.xn](xju/xn.py) - Exception wrapping to provide human readable context gathering

For detailed use examples, see the pytest subdirectories at https://github.com/urnest/urnest/tree/master/xju.
