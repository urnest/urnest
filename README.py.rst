xju python library
------------------

Various modules implemented to some broad principles:

* fine-grained static typing
* pure context management
* useful functionality that is hard to use incorrectly
* 100% test coverage

(see the bottom of this readme for release history)

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

`xju.cmc.Opt <xju/cmc/__init__.py>`_ - context manager for its optional (context manager) value

* see `xju/cmc/Opt.py.test <xju/cmc/Opt.py.test>`_ for sample code

`xju.cmc.async_cmclass <xju/cmc/__init__.py>`_  - provides async context management for class attributes that are async / sync context managers

* managing multiple resource attributes is clumsy with AsyncExitStack, this module implements
  \__aenter__ and \__aexit__ automatically to ensure correct ordering and cleanup on exceptions

* see `xju/cmc/async_cmclass.py.test <xju/cmc/async_cmclass.py.test>`_ for sample code


`xju.cmc.AsyncDict <xju/cmc/__init__.py>`_ - dictionary that is a async context manager for its (async context manager) values

* see `xju/cmc/AsyncDict.py.test <xju/cmc/AsyncDict.py.test>`_ for sample code

`xju.cmc.AsyncOpt <xju/cmc/__init__.py>`_ - async context manager for its optional (async context manager) value

* see `xju/cmc/AsyncOpt.py.test <xju/cmc/AsyncOpt.py.test>`_ for sample code


`xju.cmc.io <xju/cmc/io/__init__.py>`_ - pure context management for e.g. file reading and writing, non-blocking io

* see unit tests for sample code:
  *  `FileLock.py.test <xju/cmc/io/FileLock.py.test>`_
  *  `FileMode.py.test <xju/cmc/io/FileMode.py.test>`_
  *  `FilePosition.py.test <xju/cmc/io/FilePosition.py.test>`_
  *  `FileReader.py.test <xju/cmc/io/FileReader.py.test>`_
  *  `FileWriter.py.test <xju/cmc/io/FileWriter.py.test>`_
  *  `UnixStreamListener.py.test <xju/cmc/io/UnixStreamListener.py.test>`_
  *  `UnixStreamSocket.py.test <xju/cmc/io/UnixStreamSocket.py.test>`_
  *  `Pipe.py.test <xju/cmc/io/Pipe.py.test>`_


`xju.cmc.tstore <xju/cmc/tstore.py>`_

* time-based storage, organised as files covering time-ranges

* see `xju/cmc/tstore.py.test <xju/cmc/tstore.py.test>`_ for sample code


`xju.cmc.perflog <xju/cmc/perflog.py>`_

* time-based json-format record storage built on xju.cmc.tstore


`xju.cmc.Process <xju/cmc/__init__.py>`_

* subprocess (Popen wrapper)

* see `xju/cmc/Process.py.test <xju/cmc/ThreadMutexLockCondition.py.test>`_ for sample code


`xju.cmc.Thread/Mutex/Lock/Condition <xju/cmc/__init__.py>`_

* threading primitives that encourage correct design

* see `xju/cmc/ThreadMutexLockCondition.py.test <xju/cmc/ThreadMutexLockCondition.py.test>`_ for sample code


`xju.cmc.AsyncTask/Mutex/Lock/Condition <xju/cmc/__init__.py>`_

* asyncio Task/Mutex/Lock/Condition context managers

* see `xju/cmc/Task.py.test <xju/cmc/AsyncTaskMutexLockCondition.py.test>`_ for sample code


`xju.cmc.AsyncServiceQueue <xju/cmc/__init__.py>`_

* asyncio thread-safe service queue, allows any thread to queue a coroutine on an event loop
  so it is executed by a task in that event loop

* see `xju/cmc/AsyncServiceQueue.py.test <xju/cmc/AsyncServiceQueue.py.test>`_ for sample code


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

* encoding/decoding type-hinted dict/list/int/bool/None/float/str/Enum and classes to and from json

* designed to fit well with type checking

* built in support for xju.newtype described above

* generates json schema equivalent schemas for types

* generates typescript code (types, type-guards and dynamic casts) equivalents

* extensible with custom encodings

* see `xju/json_codec.py.test <xju/json_codec.py.test>`_ for full sample code


`xju.jsonschema <xju/jsonschema.py>`_

* represents JSON schemas as straight-foward, easy-to-read python data structures, because life's too short for jsonschema.org

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

`mypy-plugins <mypy-plugins>`_

* type checking for xju.json_codec - see `json_codec_mypy_plugin.py <mypy-plugins/json_codec_mypy_plugin.py>`_
* usage checking for xju.xn.in_function_context - see `xn_mypy_plugin.py <mypy-plugins/xn_mypy_plugin.py>`_


Release History

- 4.0.1 allow typing.NewTypes as json_codec dict keys
- 4.0.0 rework json_codec custom encoding v typescript
        - now allows non-str dict keys
        - to generate typescript for non-str dict keys,
          any CustomClassCodec implementations must also
          implement either CustomStringKeyClassCodec or
          CustomNonStringKeyClassCodec, see examples
          in json_codec.py.test
- 4.0.0 tighten json_codec_mypy_plugin checking
        - *breaking change* xju.json_codec.CustomClassCodec
          implementations will need to replace callable
          attributes with proper class method functions
- 4.0.0 fix non-str dict key handling in xju.json_codec
- 4.0.0 fix jsonschema for xju.json_codec non-str dict keys
- 3.6.0 better KeyError diagnostics from xju.xn
- 3.6.0 add xn_mypy_plugin
- 3.6.0 xju.json_codec non-str dict key support
- 3.6.0 xju.json_codec now encoded xju.time.Timestamp as float
- 3.6.0 xju.json_codec now handles Final attributes
- 3.6.0 make xju.time.Timestamp hashable
- 3.6.0 fix a few missing docstrings v xju.xn.in_function_context
- 3.6.0 xn_mypy_plugin checks xju.xn.in_function_context docstrings v vars()
- 3.5.0 add some python socket features to xju.cmc
- 3.5.0 rewrite json_codec_mypy_plugin from scratch and add lots of test cases
- 3.5.0 xju.json_codec add literal enum value support
- 3.5.0 refine typescript generation to require less manual additions when generating code
- 3.5.0 fix AsyncOpt pop(x, None)
- 3.4.0 xju.json_codec add attr name mapping feature encode_attr_as()
- 3.4.0 support mypy 1.11
- 3.3.1 xju.cmc reject @cmclass where @async_cmclass needed
  ... where a @cmclass -annotated class has async context manager base classes or
  attributes, @cmclass now fails, suggesting use @async_cmclass instead
- 3.2.1 keep Array as typescript type for json_codec encoding of python set
        - json_codec typescript side has no transforming decode/encode step -
          it assumes the json objects *are* the typescript objects
        - someday perhaps json_codec will generate full typescript codecs,
          which will then allow non-json types on the typescript side
- 3.2.0 add set support to xju.json_codec
- 3.1.0 add validity pattern support to xju.newtype.Str
- 3.1.0 fix xju.json_codec encode of subclass value
- 3.0.1 fix json_codec_mypy_plugin issues 2, 3
- 3.0.0 actually switch xju.xn.in_function_context to use first_para_of not first_line_of
        - see *breaking change* below, this time actually did that change
- 2.0.3 improve xju.json_codec typescript object key generation v xju.NewStr
- 2.0.3 add xju.cmc_mypy_plugin, no need for xju.cmc.AsyncCM/CM when using async_cmclass/cmclass decorator
- 2.0.2 add xju.cmc.delay_cancellation
- 2.0.2 add bytes support to xju.json_codec
- 2.0.2 use class name as xn exception message where exception message is empty
- 2.0.1 add python 3.12 support
- 2.0.0 add xju.time.async_sleep_until()
- 2.0.0 add xju.json_codec_mypy_plugin, avoids type: ignore against json_codec.codec()
- 2.0.0 xn first line -> first para *breaking change*
        - xn.in_function_context now uses first paragraph of docstring
          not just first line (paragraph ends at empty line); paragraph
          lines are stripped and joined by single space
- 2.0.0 add xju.newtype.Bool
- 1.4.1 fix type hints on xju.cmc.async_cmclass and xju.cmc.cmclass
- 1.4.0 add Enum support to xju.json_codec
- 1.3.0 add xju.cmc.AsyncDict, like xju.cmc.Dict but async
- 1.3.0 xju.cmc.AsyncOpt/Opt async context manager that holds an optional async context manager
- 1.3.0 xju.cmc.Opt context manager that holds an optional context manager
- 1.3.0 python xju.cmc add async_cmclass, like xju.cmc.cmclass; handles both async and non-async attrs
- 1.3.0 strip leading whitespace from doc strings, for compatibility with code formatters like black
- 1.2.13 xju.newtype Literals now handle more than one value, e.g. Literal['fred','jock']
- 1.2.13 xju.newtype eq/neq now follows python "you can compare apples to oranges", rely on mypy --strict-equality (which for what it's worth is broken at mypy 1.3.0)
- 1.2.13 now compatible with mypy --strict-equality
- 1.2.13 add xju.cmc.AsyncTask/Mutex/Condition/Lock (thread equivalents for asyncio); note Task deprecated, use AsyncTask
- 1.2.13 add custom encoding facility to xju.json_codec
- 1.2.13 add typescript aliases to json_codec generated code for xju.newtype Str/Int/Float
- 1.2.12 fixes typescript null v object handling
- 1.2.12 adds typescript aliases for NewStr, NewInt, NewFloat
- 1.2.11 adds typescript --strict support and fixes typescript code generation bugs
- 1.2.11 xju.json_codec supports Literal[int] and Literal[bool]
- 1.2.11 xju.json_codec supports generic classes
- 1.2.10 xju.json_codec supports typing.NewType str/int/bool/float
- 1.2.9 xju.json_codec generates typescript equivalents
- 1.2.9 xju.json_codec adds codec() convenience method
- 1.2.9 xju.json_codec uses kw_args to construct classes

- 1.2.8 xju.json_codec supports string type-hints (for foward definitions)
- 1.2.8 xju.json_codec adds typing.Self support (for recursive types)
- 1.2.8 xju.json_codec requires python 3.11, tested with mypy 1.1.1
