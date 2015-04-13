omnicxy - a modern CORBA C++ language mapping implementation for omniORB 

Contents

  1. Introduction
  2. Echo example
  3. Mapping of Types
  4. Parameters and Results
  5. Exceptions
  6. Building and installing omnicxy
  7. Invoking omniidl


1. Introduction

  omnicxy is an alternative to the OMG CORBA C++ Language Mapping. It
  is designed to be hard to misuse (where the OMG C++ mapping is both
  hard to use and easy to misuse):

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
  
  It currently exists only as an omniORB "backend". It is not a standard
  and and is not portable to other orbs. It is not a layer on top of
  the OMG C++ Language Mapping.


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
  relegated to other generated header files. (Later we will see that 
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

  ... cref is essentially a echo*:

    std::cout << cx->echoString("hello") << std::endl;

  Both client and server use a cxy::ORB instance, which is
  created specifying the (omniORB) orb "endpoint", eg:

    cxy::ORB<cxy::Exception> orb("giop::tcp::33");

  Other omniORB parameters can be set via environment variables. The "orb" 
  object must outlive all sref and cref objects that use it. The "orb"
  is "active" from the outset and it remains active until it is destroyed;
  there are no "run", "activate", "stop" etc methods to (forget to) call.

  Complete echo example code is in proto/cxy/echo*.{idl,hh,cc}.


3. Mapping of Types

  short,long etc map to uint16_t, uint32_t etc - see proto/cxy/p2.hh

  float, double map to float, double  - see proto/cxy/p2.hh

  string maps to std::string - see proto/cxy/p2.hh

  sequence maps to std::vector - see proto/cxy/p5.hh

  struct maps to struct with same members and generated compare 
  operators - see proto/cxy/p4.hh. Note
  that the lack of default constructor is deliberate to reduce programming
  errors. A struct whose name ends in "Pair" and which has two members
  named first and second is mapped to a std::pair - see proto/cxy/p4.hh

  enum maps to a type-safe class with inner C++ enum - see proto/cxy/p10.hh

  union maps to a set of classes with a common base class, allowing
  use of C++ dynamic_cast for discrimination - see proto/cxy/p11.hh

  exception maps to C++ class derived from the "default" exception class,
  which case be specified when invoking omniidl. More on this in 
  5. Exceptions.

  object reference, ie reference to interface T, maps to a cxy::IOR<T>, which
  can be passed to cxy::cref<T> constructor - see proto/cxy/p15.hh and
  proto/cxy/p16.hh

  typedef of basic type maps to a unique C++ type, such that for example
  typedef short X; and typedef short Y result in distinct types X 
  and Y - see proto/cxy/p3.hh

  typedef of struct, sequence, enum, union maps to C++ typedef - see
  proto/cxy/p5.hh


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
    - install its odin-1.17.10x (see the README.txt in that directory)
    - create an odin cache, eg:
        REVISIT
      ... note the environment variables have defaults, see them using
        REVISIT
    - build omnicxy, including building all examples and running them
      (from the directory containing this README.txt):
        odin '%all'

  Not using Odin

    Your on your own, go for it. Note that hcp 
    (see ../hcp/README.txt) is used for both omnicxy library code and 
    generated code:
      - if you see xxx.hcp, use hcp-split to produce xxx.hh and xxx.cc
      - the omniidl backend scripts eg cxycref.py produce .hcp files
