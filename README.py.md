xju python library

Various modules implemented to some broad principles:
  - fine-grained static typing
  - pure context management
  - useful functionality that is hard to use incorrectly

xju/newtype.py[xju.newtype] - static and dynamic distinct int, float and str types

  - unlike typing.NewType the new types are compatible with isinstance, so you
    can actually use them to do real stuff, like implement overloaded methods

xju.cmc - context management

  - unlike python standard library e.g. open(), these are "pure" context managers: resources
    are never acquired until \__enter__

xju.cmc.cmclass - provides context management for class attributes that are context managers

  - managing multiple resource attributes is clumsy with ExitStack, this module implements
    \__enter__ and \__exit__ automatically to ensure correct ordering and cleanup on exceptions

xju.cmc.Dict - dictionary that is a context manager for its (context manager) values

xju.cmc.io - pure context management for e.g. file reading and writing

xju.cmc.tstore - time-based storage, organised as files covering time-ranges

xju.cmc.perflog - time-based json-format record storage built on xju.cmc.tstore

xju.pq - jquery-like html inspection and modification

xju.assert_ - assert functions that capture term values e.g. x and y in Assert(x)==y

xju.cmd - wrapper for subprocess.Popen that captures very common usage without the option-and-flag-warren of subprocess

xju.jsonschema - represents JSON schemas as straight-foward, easy-to-read python data structures,
                 because life's too short for jsonschema

xju.patch - minimal, simple, direct patching(/stub/mock) utility, unlike the mock-warren. Because
            one shouldn't need a degree to read and write a unit test

xju.time - type-safe time and duration classes

xju.xn - Exception wrapping to provide human readable context gathering

For detailed use examples, see the pytest subdirectories at https://github.com/urnest/urnest/tree/master/xju.
