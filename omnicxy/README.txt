omnicxy - a modern CORBA C++ language mapping implemented for omniORB 

Contents

  1. Introduction
  2. Echo example
    x generating C++ code
    x implementing the interface
    x making the implementation accessible to clients
      x object lifetimes
      x omniORB configuration parameters
    x calling the interface
      x object lifetimes
      x omniORB configuration parameters
  3. Mapping of Types
  4. Parameters and Results
  5. Exceptions
      - in params, return type
      - basic types
      - ...
      - exceptions
      - exception mapping
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

  It also puts some thought into build-time scalability, by reducing
  application code dependencies on omniORB header files.

  It is not (yet) a complete mapping, and it does not bother with the
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

  float, double map to float, double  - see REVISIT

  string maps to std::string

  sequence maps to std::vector

  struct maps to struct with same members and generated compare operators. Note
  that the lack of default constructor is deliberate to reduce programming
  errors - see REVISIT. REVISIT: describe mapping to std::pair.

  enum maps to a type-safe class with inner C++ enum - see REVISIT

  union maps to a set of classes with a common base class, allowing
  use of C++ dynamic_cast for discrimination

  exception maps to C++ class derived from the "default" exception class,
  which case be specified when invoking omniidl. More on this in 
  5. Exceptions.

4. Parameters and Results

  "in" params are always passed as const&.
  Returned values are always by value.
  "out" and "inout" param types are not yet implemented

5. Exceptions

