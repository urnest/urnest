#ifndef _XJU_NETFLOW_V5_HH
#define _XJU_NETFLOW_V5_HH
// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <stdint.h>
#include <xju/ip.hh>
#include <utility>
#include <chrono>
#include "xju/Exception.hh"

#include <vector>
#include <xju/format.hh>
#include <xju/unix_epoch.hh>

namespace xju
{
namespace netflow
{
namespace v5
{
  
struct Header
{
  Header(std::chrono::milliseconds routerUpTime,
         std::chrono::system_clock::time_point routerTime,
         uint32_t flowNumberOfFirstFlow) throw();

  std::chrono::milliseconds routerUpTime_;
  std::chrono::system_clock::time_point routerTime_;
  uint32_t flowNumberOfFirstFlow_;
};

struct Flow
{
  Flow(xju::ip::v4::Address srcAddress,
       xju::ip::Port srcPort,
       xju::ip::v4::Address destAddress,
       xju::ip::Port destPort,
       uint32_t interfaceSnmpIndex,
       xju::ip::v4::Address nextHopRouter,
       std::chrono::system_clock::time_point flowStart,
       std::chrono::system_clock::time_point flowEnd,
       uint32_t packetsInFlow,
       uint32_t bytesInFlow,
       uint8_t tcpFlags,
       uint8_t ipProtocol,
       uint8_t ipTOS,
       uint8_t srcNetmaskBits,
       uint8_t dstNetmaskBits,
       uint16_t bgpSrcAs,
       uint16_t bgpDstAs) throw();

  xju::ip::v4::Address srcAddress_;
  xju::ip::Port srcPort_;
  xju::ip::v4::Address destAddress_;
  xju::ip::Port destPort_;
  uint32_t interfaceSnmpIndex_;
  xju::ip::v4::Address nextHopRouter_;
  std::chrono::system_clock::time_point flowStart_;
  std::chrono::system_clock::time_point flowEnd_;
  uint32_t packetsInFlow_;
  uint32_t bytesInFlow_;
  uint8_t tcpFlags_;
  uint8_t ipProtocol_; // eg 6 tdp, 17 udp
  uint8_t ipTOS_;
  uint8_t srcNetmaskBits_; // eg 24
  uint8_t dstNetmaskBits_;
  
  uint16_t bgpSrcAs_;
  uint16_t bgpDstAs_;
};

std::pair<Header,std::vector<Flow> > decode(
  std::vector<uint8_t> const& udpMessageContent) throw(
    xju::Exception);



}
}
}
#endif
