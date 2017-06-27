#include "getAddrInfo.hh"
#include "xju/assert.hh" //impl
#include <netdb.h> //impl
#include <netinet/ip.h> //impl
#include <sstream> //impl
#include <utility> //impl
#include <arpa/inet.h> //impl
#include <set> //impl

namespace xju
{
namespace ip
{
namespace v4
{
std::vector<std::pair<xju::ip::v4::Address,xju::ip::Port> > getAddrInfo(
  std::string const& hostname,
  std::string const& service,
  int ai_socktype,
  int ai_protocol)
  throw(
    xju::Exception)
{
  xju::assert_not_equal(ai_socktype,0);
  xju::assert_not_equal(hostname.size(),0);
  xju::assert_not_equal(service.size(),0);
  
  try
  {
    struct addrinfo* res=0;
    struct addrinfo hints;
    hints.ai_flags=0;
    hints.ai_family=AF_INET;
    hints.ai_socktype=ai_socktype;
    hints.ai_protocol=ai_protocol;
    
    int const e=::getaddrinfo(hostname.c_str(),
                              service.c_str(),
                              &hints,
                              &res);
    if (e!=0) {
      std::ostringstream s;
      s << "::getaddrinfo failed with error " << e;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    std::set<std::pair<xju::ip::v4::Address,xju::ip::Port> > rs;
    std::vector<std::pair<xju::ip::v4::Address,xju::ip::Port> > result;
    for(struct addrinfo const* x=res; x!=0; x=x->ai_next) {
      struct sockaddr_in const& a=*(struct sockaddr_in const*)x->ai_addr;
      auto const r{std::make_pair(
          xju::ip::v4::Address(::ntohl(a.sin_addr.s_addr)),
          xju::ip::Port(::ntohs(a.sin_port)))};
      if (rs.insert(r).second) {
        result.push_back(r);
      }
    }
    ::freeaddrinfo(res);
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get ipv4 address and port of host " << hostname
      << " service " << service
      << ", considering socktype " << ai_socktype
      << " and ai_protocol " << ai_protocol;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}
}
