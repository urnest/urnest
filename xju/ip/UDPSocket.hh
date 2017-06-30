#ifndef _UDPSOCKET_HH
#define _UDPSOCKET_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include "xju/io/Input.hh"
#include "xju/io/Output.hh"
#include "xju/Exception.hh"
#include "xju/ip/v4/Address.hh"
#include "xju/ip/Port.hh"
#include "xju/ip/PortInUse.hh"
#include <unistd.h>
#include <chrono>
#include <xju/DeadlineReached.hh>
#include "xju/AutoFd.hh"

#include "xju/syscall.hh"
namespace xju
{
namespace ip
{
class UDPSocket : public virtual xju::io::Input,
                  public virtual xju::io::Output
{
public:
  // udp socket bound to some free local port
  UDPSocket() throw(
    xju::Exception,
    xju::SyscallFailed) ;

    
  // udp socket bound to specific port
  explicit UDPSocket(xju::ip::Port port) throw(
    xju::ip::PortInUse,
    xju::SyscallFailed) ;


  // send datagram of specified size towards host:port by deadline
  void sendTo(std::pair<xju::ip::v4::Address,xju::ip::Port> const& host_port,
              void const* const datagram,
              size_t const size,
              std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,xju::SyscallFailed);

  void sendTo(xju::ip::v4::Address const& host,
              xju::ip::Port const& port,
              void const* const datagram,
              size_t const size,
              std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,xju::SyscallFailed);


  typedef std::pair<xju::ip::v4::Address,xju::ip::Port> Sender;

  // receive datagram by deadline, copying into specified buffer of
  // specified size
  std::pair<UDPSocket::Sender,size_t> receive(
    void* buffer,
    size_t const size,
    std::chrono::system_clock::time_point const& deadline)
    throw(xju::DeadlineReached,
          xju::SyscallFailed,
          // eg truncation due to buffer too small
          xju::Exception);


  std::string str() const throw();


  xju::ip::Port port() const throw();

  
private:
  xju::AutoFd fd_;

  xju::ip::Port port_;

  int fileDescriptor() const throw();

};
  
}
}
#endif
