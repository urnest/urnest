// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef OMNICXY_CXY_ORBIMPLIF_H
#define OMNICXY_CXY_ORBIMPLIF_H

#include <cxy/Exception.hh>
#include <string>
#include <cxy/WrongType.hh>
#include <cxy/NoSuchObject.hh>

namespace PortableServer
{
class POA;
}
namespace cxy
{

class ORBImplIf
{
public:
  virtual ~ORBImplIf() noexcept {}

  virtual void run() throw(cxy::Exception) = 0;
  virtual void stop() throw(cxy::Exception) = 0;
  virtual void* locate(std::string const& uri,
                       std::string const& repoId) throw(
                         cxy::WrongType,
                         cxy::NoSuchObject,
                         cxy::Exception) = 0;
  virtual ::PortableServer::POA& insPOA() throw(
    // eg port in use
    cxy::Exception) = 0;

  virtual std::string orbEndPoint() const throw() = 0;
};

}

#endif

