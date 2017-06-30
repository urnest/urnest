#include "UDPSocket.hh"
#include <sstream> //impl
#include <netinet/ip.h> //impl
#include "xju/socket.hh" //impl
#include "xju/io/select.hh" //impl
#include "xju/format.hh" //impl


namespace xju
{
namespace ip
{
namespace
{
// bind to specified port (choose a port if specified as Port(0))
// - returns bound port
xju::ip::Port bindToPort(xju::AutoFd const& socket,
                         xju::ip::Port const& port) throw(
  xju::SyscallFailed)
{
  sockaddr_in a;
  a.sin_family=AF_INET;
  a.sin_port=port.value();
  a.sin_addr.s_addr=INADDR_ANY;
  xju::syscall(xju::bind,XJU_TRACED)(socket.fd(),(sockaddr*)&a,sizeof(a));
  
  socklen_t l(sizeof(a));
  xju::syscall(xju::getsockname,XJU_TRACED)(socket.fd(),(sockaddr*)&a,&l);
  xju::assert_equal(l,sizeof(a));
  return xju::ip::Port(a.sin_port);
}

}

UDPSocket::UDPSocket() throw(
    xju::Exception,
    xju::SyscallFailed) try:
      fd_(xju::syscall(xju::socket,XJU_TRACED)(
            AF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0)),
      port_(bindToPort(fd_,xju::ip::Port(0)))
  {
  }
  catch(xju::SyscallFailed& e)
  {
    std::ostringstream s;
    s << "create UDP socket";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
    
  // udp socket bound to specific port
  
UDPSocket::UDPSocket(xju::ip::Port port) throw(
    xju::ip::PortInUse,
    xju::SyscallFailed) try:
      fd_(xju::syscall(xju::socket,XJU_TRACED)(
          AF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0)),
      port_(bindToPort(fd_,port))
  {
  }
  catch(xju::SyscallFailed& e)
  {
    std::ostringstream s;
    s << "create UDP socket bound to port "+xju::format::str(port);
    e.addContext(s.str(),XJU_TRACED);
    if (e._errno==EADDRINUSE){
      throw xju::ip::PortInUse(e);
    }
    throw;
  }

  // send datagram of specified size towards host:port by deadline
  
void UDPSocket::sendTo(std::pair<xju::ip::v4::Address,xju::ip::Port> const& host_port,
              void const* const datagram,
              size_t const size,
              std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,xju::SyscallFailed)
  {
    return sendTo(host_port.first,
                  host_port.second,
                  datagram,
                  size,
                  deadline);
  }
  
void UDPSocket::sendTo(xju::ip::v4::Address const& host,
              xju::ip::Port const& port,
              void const* const datagram,
              size_t const size,
              std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,xju::SyscallFailed)
  {
    try {
      if (xju::io::select({(xju::io::Output*)this},deadline).size()) {
        sockaddr_in dest_addr;
        dest_addr.sin_family=AF_INET;
        dest_addr.sin_port=port.value();
        dest_addr.sin_addr.s_addr=::htonl(host.value());
        
        auto const bytesSent=xju::syscall(xju::sendto,XJU_TRACED)(
          fileDescriptor(),
          datagram,
          size,
          MSG_NOSIGNAL,
          (sockaddr*)&dest_addr,
          sizeof(dest_addr));
        if (bytesSent<size) {
          std::ostringstream s;
          s << "only sent " << bytesSent << " bytes of " << size;
          throw xju::DeadlineReached(xju::Exception(s.str(),XJU_TRACED));
        }
        return;
      }
      std::ostringstream s;
      s << "deadline reached before socket writable";
      throw xju::DeadlineReached(xju::Exception(s.str(),XJU_TRACED));
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "send " << size << " byte udp datagram to host "
        << host << " port " << port << " from port " << port_
        << " before " << xju::format::time(deadline);
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }

  
std::pair<UDPSocket::Sender,size_t> UDPSocket::receive(
    void* buffer,
    size_t const size,
    std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,
          xju::SyscallFailed,
          // eg truncation due to buffer too small
          xju::Exception)
  {
    try {
      if (xju::io::select({(xju::io::Input*)this},deadline).size()) {
        sockaddr_in sender_addr;
        struct iovec v={buffer,size};
        struct msghdr h={
          &sender_addr,sizeof(sender_addr),
          &v,1,
          0,0,
          0
        };
        ssize_t const bytesRead=xju::syscall(xju::recvmsg,XJU_TRACED)(
          fileDescriptor(),
          &h,
          MSG_NOSIGNAL);
        if(h.msg_flags&MSG_TRUNC) {
          std::ostringstream s;
          s << "buffer too small";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return std::make_pair(
          Sender(xju::ip::v4::Address(::ntohl(sender_addr.sin_addr.s_addr)),
                 xju::ip::Port(sender_addr.sin_port)),
          bytesRead);
      }
      std::ostringstream s;
      s << "deadline reached before socket readable";
      throw xju::DeadlineReached(xju::Exception(s.str(),XJU_TRACED));
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "receive udp datagram (up to " << size << " bytes) on port "
        << port_ << " before " << xju::format::time(deadline);
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }

  
std::string UDPSocket::str() const throw()
  {
    return "UDP socket";
  }

  
xju::ip::Port UDPSocket::port() const throw()
  {
    return port_;
  }
  

int UDPSocket::fileDescriptor() const throw() {
    return fd_.fd();
  }

}
}