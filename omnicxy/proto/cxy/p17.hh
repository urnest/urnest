#ifndef _P17_HCP
#define _P17_HCP
// generated from p17.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <string>
#include <cxy/optional.hh>
#include <xju/Tagged.hh>


class p17 
{
public:
  virtual ~p17() throw();

  class Name_tag {};
  typedef ::xju::Tagged<std::string,Name_tag > Name;
  typedef cxy::optional< ::p17::Name > OptionalName;
  virtual ::p17::OptionalName echo(
    ::p17::OptionalName const& n) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

#endif
