#include "getHostAddresses.hh"
#include "xju/assert.hh" //impl
#include <netdb.h> //impl
#include <netinet/ip.h> //impl
#include <sstream> //impl
#include <arpa/inet.h> //impl
#include <set> //impl

namespace xju
{
namespace ip
{
namespace v4
{
std::vector<xju::ip::v4::Address> getHostAddresses(
  xju::HostName const& hostname)
  throw(
    // failure, including unknown host
    xju::Exception)
{
  xju::assert_not_equal(hostname,HostName(""));
  
  try
  {
    struct addrinfo* res=0;
    struct addrinfo hints;
    hints.ai_flags=0;
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_protocol=0;
    
    int const e=::getaddrinfo(hostname.value().c_str(),
                              0,
                              &hints,
                              &res);
    if (e!=0) {
      std::ostringstream s;
      s << "::getaddrinfo failed with error " << e;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    std::set<xju::ip::v4::Address> rs;
    std::vector<xju::ip::v4::Address> result;
    for(struct addrinfo const* x=res; x!=0; x=x->ai_next) {
      struct sockaddr_in const& a=*(struct sockaddr_in const*)x->ai_addr;
      xju::ip::v4::Address address(::ntohl(a.sin_addr.s_addr));
      if (rs.insert(address).second) {
        result.push_back(address);
      }
    }
    ::freeaddrinfo(res);
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get IP v4 addresses of host " << hostname;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}
}
