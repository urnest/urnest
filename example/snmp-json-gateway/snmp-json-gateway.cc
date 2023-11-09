// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/Utf8String.hh>
#include <xju/ip/UDPSocket.hh>
#include <xju/json/Object.hh>
#include <xju/json/Number.hh>
#include <iostream>
#include <xju/json/format.hh>
#include <xju/pipe.hh>
#include <vector>
#include <cinttypes>
#include <xju/io/select.hh>
#include <xju/steadyEternity.hh>
#include <xju/Exception.hh>
#include <xju/snmp/decodeSnmpV1GetRequest.hh>
#include <xju/snmp/decodeSnmpV2cGetRequest.hh>
#include <example/snmp-json-gateway/snmp_json_gateway.hh>
#include <xju/snmp/decodeSnmpV1GetNextRequest.hh>
#include <xju/snmp/decodeSnmpV1SetRequest.hh>
#include <xju/format.hh>
#include <thread>
#include <chrono>
#include <xju/readThrough.hh>
#include <xju/assert.hh>
#include <xju/json/parse.hh>
#include <xju/steadyNow.hh>
#include <xju/Thread.hh>
#include <utility>
#include <optional>
#include <xju/ip/Port.hh>
#include <xju/stringToInt.hh>
#include <sstream>
#include <xju/snmp/encode.hh>
#include <xju/snmp/decodeSnmpV2cGetRequest.hh>
#include <xju/snmp/decodeSnmpV2cGetBulkRequest.hh>
#include <xju/snmp/decodeSnmpV2cSetRequest.hh>
#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/decodeSnmpV3ScopedPDU.hh>
#include <xju/snmp/decodeSnmpV3Message.hh>
#include <xju/snmp/SnmpV3Message.hh>
#include <xju/snmp/decodeSnmpV3UsmSecurityParameters.hh>
#include <xju/UserName.hh>
#include <xju/now.hh>
#include <xju/snmp/SnmpV3UsmAuthData.hh>
#include <xju/snmp/SnmpV3UsmPrivData.hh>
#include <xju/snmp/PDU.hh>
#include <xju/snmp/Value.hh>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <xju/snmp/Counter32Value.hh>
#include <xju/snmp/showFirstBytes.hh>

auto const same_line = xju::Utf8String("");

xju::snmp::EngineTime engineTimeNow() {
  return xju::snmp::EngineTime(
    std::chrono::duration_cast<std::chrono::seconds>(xju::now().time_since_epoch()).count());
}

void run(xju::ip::UDPSocket& socket){
  xju::json::Object portMessage({
      {xju::Utf8String("listening_on"),std::make_unique<xju::json::Number>(
          xju::format::str(socket.port().value()))}
    });
  std::cout << xju::json::format(portMessage, same_line) << std::endl;

  auto stop_receiver = xju::pipe(true,true);

  uint64_t usmStatsUnknownEngineIDs=0;
  xju::snmp::ContextEngineID const ourEngineID(std::vector<uint8_t>{0x01,0x02,0x03,0x04});
  uint64_t const ourMaxSize(64000);
  auto from_snmp_to_stdout =
    [&](){
      try{
        std::vector<uint8_t> buffer(UINT16_MAX);
        while(true){
          if (xju::io::select({stop_receiver.first.get(),&socket},{},xju::steadyEternity())
              .first.contains(stop_receiver.first.get())){
            return;
          }
          auto const senderAndSize(socket.receive(buffer.data(), UINT16_MAX, xju::steadyNow()));
          buffer.resize(senderAndSize.second);
          std::vector<xju::Exception> failures;
          try{
            auto const x(xju::snmp::decodeSnmpV1GetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1GetNextRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1SetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetNextRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetBulkRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cSetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV3Message(buffer));
            try {
              if (x.securityModel_ != xju::snmp::SnmpV3Message::SecurityModel(3)) {
                std::ostringstream s;
                s << "snmp v3 security model " << x.securityModel_ << " is not yet implemented";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              if ((x.flags_ & xju::snmp::SnmpV3Message::AUTH) == xju::snmp::SnmpV3Message::AUTH){
                std::ostringstream s;
                s << "snmp v3 auth " << x.securityModel_ << " is not yet implemented";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              auto const sec(xju::snmp::decodeSnmpV3UsmSecurityParameters(x.securityParameters_));
              if ((x.flags_ & xju::snmp::SnmpV3Message::AUTH) == 0 &&
                  (x.flags_ & xju::snmp::SnmpV3Message::REPORTABLE)==
                    xju::snmp::SnmpV3Message::REPORTABLE &&
                  std::get<0>(sec).engineID_==xju::snmp::ContextEngineID({}) &&
                  std::get<0>(sec).userName_==xju::UserName("")){
                // engineid discovery (rfc3414 4. Discovery)
                auto const scopedPDU(xju::snmp::decodeSnmpV3ScopedPDU(x.scopedPduData_));
                auto const m(
                  xju::snmp::encode(
                    xju::snmp::SnmpV3Message(
                      x.id_,
                      ourMaxSize,
                      xju::snmp::SnmpV3Message::Flags(0),
                      xju::snmp::SnmpV3Message::SecurityModel(3),
                      xju::snmp::encode(
                        xju::snmp::SnmpV3UsmSecurityParameters(ourEngineID,
                                                               xju::snmp::EngineBoots(1),
                                                               engineTimeNow(),
                                                               xju::UserName("")),
                        xju::snmp::SnmpV3UsmAuthData({}),
                        xju::snmp::SnmpV3UsmPrivData({})),
                      xju::snmp::encode(
                        xju::snmp::SnmpV3ScopedPDU(
                          ourEngineID,
                          xju::snmp::ContextName({}),
                          xju::snmp::PDU(
                            scopedPDU.pdu_.requestId_,
                            0,
                            0,
                            {xju::snmp::SnmpVar(xju::snmp::Oid(".1.3.6.1.6.3.15.1.1.4"),
                                                std::shared_ptr<xju::snmp::Value const>(
                                                  new xju::snmp::Counter32Value(
                                                    usmStatsUnknownEngineIDs)))},
                            0xa8))))));
                auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
                socket.sendTo(senderAndSize.first.first,senderAndSize.first.second,
                              m.data(),m.size(),deadline);
              }
              else if (std::get<0>(sec).engineID_!=ourEngineID){
                ++usmStatsUnknownEngineIDs;
                std::ostringstream s;
                s << "expected engine id (hex) " << xju::snmp::showFirstBytes(512,ourEngineID._)
                  << ", got " << xju::snmp::showFirstBytes(512,std::get<0>(sec).engineID_._);
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              else {
                try{
                  auto const y(xju::snmp::decodeSnmpV3ScopedPDU(x.scopedPduData_));
                  std::cout << xju::json::format(*snmp_json_gateway::encode(
                                                   senderAndSize.first,
                                                   std::make_tuple(
                                                     x.id_,
                                                     x.maxSize_,
                                                     y)),same_line)
                            << std::endl;
                  continue;
                }
                catch(xju::Exception& e){
                  std::ostringstream s;
                  s << "decode snmp v3 scoped PDU from data "
                    << xju::snmp::showFirstBytes(USHRT_MAX,x.scopedPduData_._);
                  e.addContext(s.str(),XJU_TRACED);
                  failures.push_back(e);
                }
              }
            }
            catch(xju::Exception& e){
              std::ostringstream s;
              s << "handle snmp v3 message " << x;
              e.addContext(s.str(),XJU_TRACED);
              failures.push_back(e);
            }
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          std::cerr << "dropped received udp packet because "
                    << xju::format::join(failures.begin(), failures.end(),
                                         [](auto e){
                                           return xju::readableRepr(e,false,true);
                                         }, " and ") << std::endl;
        };
      }
      catch(xju::Exception& e){
        std::cerr << "will restart receive in 3s because " << xju::readableRepr(e) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
      }
    };
  auto from_stdin_to_snmp =
    [&](){
      try{
        while(true){
          auto const s(xju::readThrough(std::cin, "\n", 10*UINT16_MAX));
          xju::assert_greater_equal(s.size(),1U);
          std::vector<xju::Exception> failures;
          try{
            auto const gm(
              xju::json::parse(xju::Utf8String(std::string(s.begin(), s.end()-1))));
            auto const remoteEndpoint = snmp_json_gateway::decodeEndpoint(
              gm->getMember(xju::Utf8String("remote_endpoint")));
            auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
            try{
              auto const m(xju::snmp::encode(snmp_json_gateway::decodeSnmpV1Response(
                                               gm->getMember(xju::Utf8String("message")))));
              socket.sendTo(remoteEndpoint.first,remoteEndpoint.second,m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              failures.push_back(e);
            }
            try{
              auto const m(xju::snmp::encode(snmp_json_gateway::decodeSnmpV2cResponse(
                                               gm->getMember(xju::Utf8String("message")))));
              socket.sendTo(remoteEndpoint.first,remoteEndpoint.second,m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              failures.push_back(e);
            }
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          std::cerr << "ignoring " << xju::format::cEscapeString(s) << " because "
                    << xju::format::join(failures.begin(), failures.end(),
                                         [](auto e){ return xju::readableRepr(e,false,true);},
                                         " and ")
                    << std::endl;
        }
      }
      catch(xju::Exception& e){
        std::cerr << "stopping because " << xju::readableRepr(e) << std::endl;
      }
    };
  xju::Thread a(from_snmp_to_stdout);
  auto const stopper(std::move(stop_receiver.second));
  xju::Thread b(from_stdin_to_snmp);
}

std::optional<xju::ip::Port> parsePort(std::string const& x){
  try{
    std::optional<xju::ip::Port> result;
    if (x != "auto"){
      auto const p(xju::stringToInt(x));
      if (p<1 || p>UINT16_MAX){
        std::ostringstream s;
        s << "port number is not 1.." << UINT16_MAX;
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      result=xju::ip::Port(p);
    }
    return result;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse " << xju::format::quote(x) << " as an IP port number";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  if (argc != 2){
    std::cerr
      << "usage: " << argv[0] << " port | 'auto'" << std::endl
      << " - with specified udp port (or choose one):" << std::endl
      << "   - write received snmp messages to stdout in json format" << std::endl
      << "   - send stdin json format messages as snmp messages from specified port" << std::endl
      << " - see snmp_json_gateway.py GatewayMessage for message format" << std::endl
      << " - note writes { \"listening_on\": port } line to stdout once port is open" << std::endl;
    return 1;
  }
  try{
    std::optional<xju::ip::Port> const port(parsePort(argv[1]));
    if (port.has_value()){
      xju::ip::UDPSocket s(port.value());
      run(s);
      return 0;
    }
    else{
      xju::ip::UDPSocket s;
      run(s);
      return 0;
    }
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std:: endl;
    return 1;
  }
}
