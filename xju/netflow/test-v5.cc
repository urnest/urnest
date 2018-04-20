// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/netflow/v5.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/Exception.hh"

namespace xju
{
namespace netflow
{
namespace v5
{

void test1() {
  std::vector<uint8_t> const x {
    0,0x05,
      0,0x02, //number of flow records
      0x00,0x00,0x45,0x20,//uptime
      0x01,0x23,0x45,0x67,//unix_secs
      0x02,0xab,0xcd,0xef,//unix_nsecs
      0x00,0x01,0x02,0x03,//seq
      0x00,0x00,0x00,0x00,//reserved

      192,168,1,2,//src
      193,169,19,87,//dest
      188,54,33,1,//nexthop
      0x00,0x03,0x00,0x09,//interface snmp index
      0x00,0x00,0x53,0x24,//pkts
      0x00,0x20,0x32,0xac,//octets
      0x00,0x00,0x20,0x00,//start
      0x00,0x00,0x45,0x20,//end
      0x60,0x00,0x45,0x42,//src port, dest port
      0x00,0x07,17,0x03,//pad1,tcp_flags,prot,tos
      0x00,0x23,0x00,0x77,//src_as, dst_as
      0x00,0x00,0x00,0x00,

      122,43,77,1,//src
      193,169,19,89,//dest
      188,54,33,1,//nexthop
      0x00,0x03,0x00,0x09,//interface snmp index
      0x01,0x02,0x3,0x4,//pkts
      0x00,0x20,0x32,0xac,//octets
      0x00,0x00,0x20,0x00,//start
      0x00,0x00,0x45,0x20,//end
      0x60,0x00,0x45,0x42,//src port, dest port
      0x00,0x07,17,0x03,//pad1,tcp_flags,prot,tos
      0x00,0x23,0x00,0x77,//src_as, dst_as
      0x00,0x00,0x00,0x00
  };
  xju::assert_equal(x.size(),24+48+48);
  
  std::pair<Header,std::vector<Flow> > y(decode(x));
  xju::assert_equal(y.first.routerUpTime_,std::chrono::milliseconds(0x4520));
  xju::assert_equal(y.first.routerTime_,
                    xju::unix_epoch()+std::chrono::seconds(0x01234567)+
                    std::chrono::nanoseconds(0x02abcdef));
  xju::assert_equal(y.first.flowNumberOfFirstFlow_,0x00010203);
  xju::assert_equal(y.second.size(),2U);
  xju::assert_equal(y.second[0].srcAddress_,xju::ip::v4::Address(0xc0a80102));
  xju::assert_equal(y.second[0].destAddress_,xju::ip::v4::Address(0xc1a91357));
  xju::assert_equal(y.second[0].nextHopRouter_,xju::ip::v4::Address(0xbc362101));
  xju::assert_equal(y.second[0].inputInterfaceSnmpIndex_,0x03U);
  xju::assert_equal(y.second[0].outputInterfaceSnmpIndex_,0x09U);
  xju::assert_equal(y.second[0].packetsInFlow_,0x00005324);
  xju::assert_equal(y.second[0].bytesInFlow_,0x002032acU);
  xju::assert_equal(y.second[0].flowStart_,
                    y.first.routerTime_-
                    y.first.routerUpTime_+
                    std::chrono::milliseconds(0x00002000));
  xju::assert_equal(y.second[0].flowEnd_,
                    y.first.routerTime_-
                    y.first.routerUpTime_+
                    std::chrono::milliseconds(0x00004520));
  xju::assert_equal(y.second[0].srcPort_,xju::ip::Port(0x6000U));
  xju::assert_equal(y.second[0].destPort_,xju::ip::Port(0x4542U));
  xju::assert_equal(y.second[0].tcpFlags_,0x07);
  xju::assert_equal(y.second[0].ipProtocol_,17);
  xju::assert_equal(y.second[0].ipTOS_,0x03);
  xju::assert_equal(y.second[1].srcAddress_,xju::ip::v4::Address(0x7a2b4d01));
  xju::assert_equal(y.second[1].destAddress_,xju::ip::v4::Address(0xc1a91359));
  xju::assert_equal(y.second[1].nextHopRouter_,xju::ip::v4::Address(0xbc362101));
  xju::assert_equal(y.second[1].inputInterfaceSnmpIndex_,0x03U);
  xju::assert_equal(y.second[1].outputInterfaceSnmpIndex_,0x09U);
  xju::assert_equal(y.second[1].packetsInFlow_,0x01020304);
  xju::assert_equal(y.second[1].bytesInFlow_,0x002032acU);
  xju::assert_equal(y.second[1].flowStart_,
                    y.first.routerTime_-
                    y.first.routerUpTime_+
                    std::chrono::milliseconds(0x00002000));
  xju::assert_equal(y.second[1].flowEnd_,
                    y.first.routerTime_-
                    y.first.routerUpTime_+
                    std::chrono::milliseconds(0x00004520));
  xju::assert_equal(y.second[1].srcPort_,xju::ip::Port(0x6000U));
  xju::assert_equal(y.second[1].destPort_,xju::ip::Port(0x4542U));
  xju::assert_equal(y.second[1].tcpFlags_,0x07);
  xju::assert_equal(y.second[1].ipProtocol_,17);
  xju::assert_equal(y.second[1].ipTOS_,0x03);
  xju::assert_equal(y.second[1].bgpSrcAs_,0x23U);
  xju::assert_equal(y.second[1].bgpDstAs_,0x77U);

  xju::assert_equal(encode(y),x);
  try {
    std::pair<Header,std::vector<Flow> > y(decode(
                                             std::vector<uint8_t>(
                                               x.begin(),
                                               x.end()-8)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(xju::readableRepr(e),"Failed to decode cisco netflow v5 header and records from 112 bytes of data because\nfailed to decode record after successfully decoding 1 records because\nfailed to get uint16_t field \"bgp dst AS\" because\nuint16_t starting at byte 114 ends beyond end of data.");
  }
  try {
    std::pair<Header,std::vector<Flow> > y(decode(
                                             std::vector<uint8_t>(
                                               x.begin(),
                                               x.begin()+16)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(xju::readableRepr(e),"Failed to decode cisco netflow v5 header and records from 16 bytes of data because\nfailed to get uint32_t field \"flow_sequence\" because\nuint32_t starting at byte 16 ends beyond end of data.");
  }
}
void test2() {
  std::vector<uint8_t> const x {
    0,0x04,0,0,
      0x00,0x00,0x45,0x20,//uptime
      0x01,0x23,0x45,0x67,//unix_secs
      0x02,0xab,0xcd,0xef,//unix_nsecs
      0x00,0x01,0x02,0x03,//seq
      0x00,0x00,0x00,0x00,//reserved

      192,168,1,2,//src
      193,169,19,87,//dest
      188,54,33,1,//nexthop
      0x00,0x00,0x00,0x09,//interface snmp index
      0x00,0x00,0x53,0x24,//pkts
      0x00,0x20,0x32,0xac,//octets
      0x00,0x00,0x20,0x00,//start
      0x00,0x00,0x45,0x20,//end
      0x60,0x00,0x45,0x42,//src port, dest port
      0x00,0x07,17,0x03,//pad1,tcp_flags,prot,tos
      0x00,0x23,0x00,0x77,//src_as, dst_as
      0x00,0x00,0x00,0x00,

      122,43,77,1,//src
      193,169,19,89,//dest
      188,54,33,1,//nexthop
      0x00,0x00,0x00,0x09,//interface snmp index
      0x01,0x02,0x3,0x4,//pkts
      0x00,0x20,0x32,0xac,//octets
      0x00,0x00,0x20,0x00,//start
      0x00,0x00,0x45,0x20,//end
      0x60,0x00,0x45,0x42,//src port, dest port
      0x00,0x07,17,0x03,//pad1,tcp_flags,prot,tos
      0x00,0x23,0x00,0x77,//src_as, dst_as
      0x00,0x00,0x00,0x00
  };
  xju::assert_equal(x.size(),24+48+48);
  try {
    std::pair<Header,std::vector<Flow> > y(decode(
                                             std::vector<uint8_t>(
                                               x.begin(),
                                               x.end()-8)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(xju::readableRepr(e),"Failed to decode cisco netflow v5 header and records from 112 bytes of data because\ndata is netflow version 4, not 5.");
  }
}

}
}
}

using namespace xju::netflow::v5;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

