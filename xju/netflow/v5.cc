#include <xju/netflow/v5.hh>
#line 1 "/home/xju/urnest/xju/netflow/v5.hcp"
#line 16
#include <sstream> //impl
namespace xju
{
namespace netflow
{
namespace v5
{
#line 30
Header::Header(std::chrono::milliseconds routerUpTime,
         std::chrono::system_clock::time_point routerTime,
         uint32_t flowNumberOfFirstFlow) throw():
      routerUpTime_(routerUpTime),
      routerTime_(routerTime),
      flowNumberOfFirstFlow_(flowNumberOfFirstFlow) {
  }
  
#line 44
Flow::Flow(xju::ip::v4::Address srcAddress,
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
       uint16_t bgpDstAs) throw():
      srcAddress_(srcAddress),
      srcPort_(srcPort),
      destAddress_(destAddress),
      destPort_(destPort),
      interfaceSnmpIndex_(interfaceSnmpIndex),
      nextHopRouter_(nextHopRouter),
      flowStart_(flowStart),
      flowEnd_(flowEnd),
      packetsInFlow_(packetsInFlow),
      bytesInFlow_(bytesInFlow),
      tcpFlags_(tcpFlags),
      ipProtocol_(ipProtocol),
      ipTOS_(ipTOS),
      srcNetmaskBits_(srcNetmaskBits),
      dstNetmaskBits_(dstNetmaskBits),
      bgpSrcAs_(bgpSrcAs),
      bgpDstAs_(bgpDstAs)
  {
  }
  
#line 100
std::pair<Header,std::vector<Flow> > decode(
  std::vector<uint8_t> const& udpMessageContent) throw(
    xju::Exception)
{
  try {
    auto get8=[&udpMessageContent](size_t const i,std::string const& name) throw(
      xju::Exception) {
      try {
        if (i>=udpMessageContent.size()) {
          std::ostringstream s;
          s << "byte " << i << " is beyond end of data";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return udpMessageContent[i];
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "get byte field " << xju::format::quote(name);
        e.addContext(s.str(),XJU_TRACED);
        throw;
      }
    };
    auto get16=[&udpMessageContent](size_t const i,std::string const& name) throw(
      xju::Exception) {
      try {
        if ((i+1)>=udpMessageContent.size()) {
          std::ostringstream s;
          s << "uint16_t starting at byte " << i << " ends beyond end of data";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return ((((uint16_t)udpMessageContent[i+0])<<8)+
                (((uint16_t)udpMessageContent[i+1])<<0));
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "get uint16_t field " << xju::format::quote(name);
        e.addContext(s.str(),XJU_TRACED);
        throw;
      }
    };
    auto get32=[&udpMessageContent](size_t const i,std::string const& name) throw(
      xju::Exception) {
      try {
        if ((i+3)>=udpMessageContent.size()) {
          std::ostringstream s;
          s << "uint32_t starting at byte " << i << " ends beyond end of data";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return ((((uint16_t)udpMessageContent[i+0])<<24)+
                (((uint16_t)udpMessageContent[i+1])<<16)+
                (((uint16_t)udpMessageContent[i+2])<< 8)+
                (((uint16_t)udpMessageContent[i+3])<< 0));
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "get uint32_t field " << xju::format::quote(name);
        e.addContext(s.str(),XJU_TRACED);
        throw;
      }
    };
    if (get8(0,"version")!=5) {
      std::ostringstream s;
      s << "data is netflow version " << xju::format::int_(get8(0,"version")) 
        << ", not 5";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    Header h(
      std::chrono::milliseconds(get32(4,"sysUptime")),
      std::chrono::system_clock::time_point(
        xju::unix_epoch()+
        std::chrono::seconds(get32(8,"unix_secs"))+
        std::chrono::nanoseconds(get32(12,"unix_nsecs"))),
      get32(16,"flow_sequence"));
    std::vector<Flow> records;
    try {
      for(size_t i=24; i!=udpMessageContent.size(); i+=48) {
        Flow r(
          xju::ip::v4::Address(get32(i+0,"srcaddr")),
          xju::ip::Port(get16(i+32,"srcport")),
          xju::ip::v4::Address(get32(i+4,"dstaddr")),
          xju::ip::Port(get16(i+34,"dstport")),
          get32(i+12,"interface"),
          xju::ip::v4::Address(get32(i+8,"nexthop")),
          h.routerTime_-h.routerUpTime_+std::chrono::milliseconds(
                      get32(i+24,"start")),
          h.routerTime_-h.routerUpTime_+std::chrono::milliseconds(
                    get32(i+28,"end")),
          get32(i+16,"packets"),
          get32(i+20,"bytes"),
          get8(i+37,"tcp flags"),
          get8(i+38,"ip protocol"),
          get8(i+39,"ip TOS"),
          get8(i+44,"srcaddr netmask"),
          get8(i+45,"dstaddr netmask"),
          get16(i+40,"bgp src AS"),
          get16(i+42,"bgp dst AS"));
        records.push_back(r);
      }
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "decode record after successfully decoding " << records.size() 
        << " records";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
    return std::make_pair(h,records);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode cisco netflow v5 header and records from "
      << udpMessageContent.size()
      << " bytes of data";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}


}
}
}
