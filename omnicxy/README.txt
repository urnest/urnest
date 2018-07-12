omnicxy - a modern CORBA C++ language mapping implementation for omniORB/linux

omnicxy, being derived in part from omniORB 4.2.0, is copyright Trevor Taylor,
Apasphere Ltd, AT&T Laboratories Cambridge and others. It is free software.
The programs in omnicxy are distributed under the GNU General Public Licence
as published by the Free Software Foundation. See the file COPYING for
copying permission of these programs. The libraries in omnicxy are
distributed under the GNU Lesser General Public Licence. See the file
COPYING.LIB for copying permission of these libraries.

Contents

  1. Introduction
  2. Echo example
  3. Mapping of Types
  4. Parameters and Results
  5. Exceptions
  6. Building and installing omnicxy
  7. Invoking omniidl
  8. Extras

1. Introduction

  omnicxy is an modern alternative to the OMG CORBA C++ Language Mapping. It
  is designed to be easy to use and hard to misuse (where the OMG C++ mapping
  is hard to use and easy to misuse):

    - uniform parameter passing and member type rules
    - no explicit storage management
    - use of modern C++ types eg std::string, std::vector
    - more readable generated code

  It also aims for good build-time scalability by reducing
  application code dependencies on omniORB header files.

  It is not (yet) a complete mapping, and it does not (yet) bother with the
  plethora of esoteric POA stuff: it does make basic clients and
  servers easy to code, which makes CORBA usable as that small piece of
  IPC in your large system without requiring a disproportionate amount
  of effort to implement and maintain.
  
  It currently exists only as an omniORB "backend", and only compiles on
  linux. It is not a standard and and is not portable to other orbs. It 
  is not a layer on top of the OMG C++ Language Mapping. It should not
  be hard to port to other OSs that omniORB supports.

  TODO.txt has more info on what is/isn't covered.

  The rest of this document assumes some knowledge of CORBA, if it doesn't
  make any sense, read the omniORB manual first.


2. Echo example

  The echo.idl interface:

    interface echo
    {
      string echoString(in string x);
    };


  ... generates echo.hh:

    #include <cxy/Exception.hh>
    #include <string>

    class echo 
    {
    public:
      virtual ~echo() throw();
    
      virtual std::string echoString(
        std::string const& x) throw(
        // ipc failure
        // - note servant may not throw
        cxy::Exception) = 0;
    };

  ... notice that there are no CORBA-isms in this header file; nor are
  there any in any of its included header files. All CORBA-isms are 
  relegated to other generated header files. (Later we will also see that 
  we can use our own exception type, replacing the default "cxy::Exception"
  completely.)

  The server implements this interface, for example:

    class echo_impl : public echo {
      // echo::
      virtual std::string echoString(std::string const& x) throw() {
        return x;
      }
    };

  ... instantiates it, eg:

    echo_impl x;

  ... and makes it available to CORBA clients:

    // advertise echo_impl x under name "X"
    cxy::sref<echo_impl> sx(orb, "X", x);

  The client locates the named object in the server:

    // assume server is at localhost port 33
    cxy::cref<echo_impl> cx(orb, "corbaloc:iiop:localhost:33/X");

  ... cref is essentially an echo*:

    std::cout << cx->echoString("hello") << std::endl;

  Both client and server use a cxy::ORB instance, which is
  created specifying the (omniORB) orb "endpoint", eg:

    cxy::ORB<cxy::Exception> orb("giop::tcp::33");

  Other omniORB parameters can be set via omniORB environment variables (see
  the omniORB manual). The "orb" object must outlive all sref and cref 
  objects that use it. The "orb" is "active" from the outset and it 
  remains active until it is destroyed; there are no "run", "activate",
  "stop" etc methods to (forget to) call.

  Complete echo example code is in proto/cxy/echo-main.cc.


3. Mapping of Types

  short,long etc map to uint16_t, uint32_t etc - see proto/cxy/p2.hh

  float, double map to float, double  - see proto/cxy/p2.hh

  string maps to std::string - see proto/cxy/p2.hh

  sequence maps to std::vector - see proto/cxy/p5.hh
    - bounded sequence of length 1 maps to cxy::optional - see proto/cxy/p17.hh

  struct maps to struct with same members and generated compare 
  operators - see proto/cxy/p4.hh. Note
  that the lack of default constructor is deliberate to reduce programming
  errors. A struct whose name ends in "Pair" and which has two members
  named first and second is mapped to a std::pair - see proto/cxy/p4.hh

  enum maps to a type-safe class with inner C++ enum - see proto/cxy/p10.hh

  union maps to a set of classes with a common base class, allowing
  use of C++ dynamic_cast for discrimination - see proto/cxy/p11.hh,
  proto/cxy/p12.hh, proto/cxy/p13.hh

  exception maps to C++ class derived from the "default" exception class,
  which can be specified when invoking omniidl. More on this in 
  5. Exceptions.

  object reference, ie reference to interface T, maps to a cxy::IOR<T>, which
  can be passed to cxy::cref<T> constructor - see proto/cxy/p15.hh and
  proto/cxy/p16.hh. cxy::IOR<T> can be passed to cxy::cref<U> constructor
  if T is a U (widening), if U is a T (dynamically type-checked narrowing).

  untyped object reference ie idl Object maps to cxy::IOR<void>, which
  can be passed to cxy::cref<T> constructor - see proto/cxy/p18-main.cc.

  cxy::IOR<T> can construct cxy::IOR<U> if T is a U - see proto/cxy/p16-main.cc

  cxy::cref<T> can be narrowed (via .narrow()) to cxy::cref<U> if U is a T
  and the referenced object is a U - see proto/cxy/p16-main.cc

  typedef of basic type maps to a unique C++ type, such that for example
  typedef short X; and typedef short Y result in distinct types X 
  and Y - see proto/cxy/p3.hh

  typedef of struct, sequence, enum, union maps to C++ typedef - see
  proto/cxy/p5.hh

  CORBA::TypeCode maps to ::cxy::TypeCode - see proto/cxy/p24.hh

  CORBA::Any maps to ::cxy::Any<> - see 
  
4. Parameters and Results

  "in" params are always passed as const&.
  Returned values are always by value.
  "out" and "inout" param types are not yet implemented


5. Exceptions

  All generated exception classes derive from the "base exception" type
  specified when invoking omniidl; the default is cxy::Exception.

  The proto/cxy/e directory shows how to use a non-default base exception
  type. Compared to the corresponding examples in proto/cxy, using a
  non-default exception type avoids try/catch blocks in application code.
  
  For class T to be used as the base exception type it must have the
  following members:

    T(std::string const& cause, 
      std::pair<std::string, unsigned int> const& fileAndLine) throw()

    void T::addContext(std::string context, 
                       FileAndLine const& fileAndLine) throw()
    
  ... omnicxy calls these with cause like "connect timed out after 10 seconds"
  (cause) and "connect to host fred port 6253" so that they can be stitched
  together to read "failed to connect to host fred port 6253 because 
  connect timed out after 10 seconds". (cxy::Exception has a readableRepr
  function that produces such a message)


6. Building and installing omnicxy

  Using Odin

    - clone the git repository
    - install its odin/odin-1.17.10x (see the README.txt in that directory),
      usually something like:

         cd odin/odin-1.17.10x &&
         ./configure &&
         ./INSTALL ~/odin-1.17.10x

    - create an odin cache, eg from the directory containing this README.txt
      if omniORB is installed in $HOME/omniORB-4.2.0:

        # ODIN is odin cache location
        export ODIN=~/oc &&

        ODINPATH=$(pwd)/../odin/odin-1.17.10x/xjupkg \
        ODIN_LIB_SP="$HOME/omniORB-4.2.0/lib /lib /usr/lib" \
        ODIN_EXEC_PATH=$HOME/omniORB-4.2.0/bin:/usr/bin:/bin \
        ODIN_OMNICXY_PATH=$HOME/omniORB-4.2.0/bin:/usr/bin:/bin \
        ODIN_OMNICXY_BE_DIR=$(pwd)/../omnicxy/omniidl_be \
        ODIN_CXX_LD_LIBRARY_PATH=$HOME/omniORB-4.2.0/lib \
        ODIN_LD_LIBRARY_PATH=$HOME/omniORB-4.2.0/lib \
        ODIN_CXX_LD_EXTRA_LIBS=-lpthread \
        ODIN_TAR_PATH=/bin \
        ODIN_CXX_I=$HOME/omniORB-4.2.0/include \
          odin -R </dev/null
        
      ... there are other environment variables that matter but they
      are given default values. (To see the full environment that
      will be used for building, see $ODIN/$(hostname)/ENV after cache
      creation; for a brief description of an environment variable, find
      it in one of the $ODIN/$(hostname)/PKGS/*/*.dg files.)

    - build omnicxy, including building all examples and tests and running
      them (from the directory containing this README.txt):
        odin '%all'

    - this full build does some memory leak checking using valgrind, so
      you'll get some errors if that is not around

    - there is nothing to install really

    - if your application code lives in a separate tree, you might like
      to add the omnicxy directory and its parent directory to
      ODIN_CXX_I when creating the cache, eg:
        ODIN_CXX_I="$HOME/xxx/omnicxy $HOME/xxx $HOME/omniORB-4.2.0/include"

  Not using Odin, eg using make

    Your on your own, go for it. Note that hcp 
    (see ../hcp/README.txt) is used for both omnicxy library code and 
    generated code:
      - if you see xxx.hcp, use hcp-split to produce xxx.hh and xxx.cc
      - the omniidl backend scripts eg cxycref.py produce .hcp files

8. Extras

    cxy/UnixTimeMilli.idl defines a time type; cxy/UnixTimeMilli*.hcp
    hand-codes its implementation so that it derives from
    std::chrono::system_clock::time_point, allowing C++ clients and
    servers to work with the C++ standard time representation. Note
    that conversion truncates.
    
