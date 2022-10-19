xju python library

Various modules implemented to some broad principles:
  - fine-grained static typing
  - pure context management philosophy
  - useful functionality that is hard to use incorrectly

xju.newtype - static and dynamic distinct int, float and str types

  - unlike typing.NewType the new types are compatible with isinstance

xju.cmc - context management

  -u nlike python standard library e.g. open(), these are "pure" context managers: resources
  are never acquired until __enter__

xju.cmc.cmclass - provide context management for class attributes that are context managers

  - managing multiple resource attributes is clumbsy with ExitStack, this module implements
  __enter__ and __exit__ automatically to ensure correct ordering and cleanup on exceptions

xju.cmc.Dict - dictionary that is a context manager for its (context manager) values

xju.cmc.io - proper context management for e.g. file reading and writing

xju.cmc.tstore - time-based storage, organised as files covering time-ranges
xju.cmc.perlog - json-format time-based record storage built on tstore

xju.pq - jquery-like html inspection and modification
xju.assert_ - assert functions that capture term values e.g. x and y in Assert(x)==y
xju.cmd - sane wrapper for subprocess.Popen

  - captures very common usage without the option-warren of subprocess.

xju.jsonschema

  - represents JSON schemas as straight-foward, easy-to-read python datastructures, because
    jsonschema is overly complex for many needs

xju.patch

  - minimal, simple, direct patching(/stub/mock) utility, unlike the mock-warren. One shouldn't
    need a degree to read and write a unit test

xju.time

  - type-safe time and duration classes

xju.xn

  - Exception wrapping to provide human readable context gathering
