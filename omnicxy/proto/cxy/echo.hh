#ifndef _ECHO_HH
#define _ECHO_HH
// generated from echo.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

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


#endif
