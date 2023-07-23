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

auto const same_line = xju::Utf8String("");

void run(xju::ip::UDPSocket& socket){
  xju::json::Object portMessage({
      {xju::Utf8String("listening_on"),std::make_unique<xju::json::Number>(socket.port())}
    });
  std::cout << xju::json::format(portMessage, same_line) << std::endl;

  auto stop_receiver = xju::pipe(true,true);
  
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
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x))
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1GetNextRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x))
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1SetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x))
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          std::cerr << "dropped received udp packet because "
                    << xju::format::join(failures.begin(), failures.end(),
                                         [](auto e){
                                           return xju::readableRepr(e);
                                         }, " and ") << std::endl;
        };
      }
      catch(xju::Exception& e){
        std::cerr << "will restart receive in 3s because " << xju::readableRepr(e) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
      }
    };
  auto from_stdout_to_snmp =
    [&](){
      try{
        while(true){
          auto const s(xju::readThrough(std::cin, "\n", 10*UINT16_MAX));
          xju::assert_greater_equal(s.size(),1U);
          try{
            auto const gm(
              xju::json::parse(xju::Utf8String(std::string(s.begin(), s.end()-1))));
            auto const remoteEndpoint = snmp_json_gateway::decodeEndpoint(
              gm->getMember(xju::Utf8String("remote_endpoint")));
            auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
            std::vector<xju::Exception> failures;
            try{
              auto const m(xju::snmp::encode(snmp_json_gateway::decodeSnmpV1Response(
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
                    << xju::format::join(failures.begin(), failures.end(), " and ", xju::readableRepr)
                    << std::endl;
        }
      }
      catch(xju::Exception& e){
        std::cerr << "stopping because " << xju::readableRepr(e) << std::endl;
      }
    };
  xju::Thread a(from_snmp_to_stdout);
  auto const stopper(std::move(stop_receiver.second));
  xju::Thread b(from_stdout_to_snmp);
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
      << " - note writes { "listening_on": port } to stdout once port is open" << std::endl;
    return 1;
  }
  try{
    std::optional<xju::ip::Port> const port(parsePort(argv[1]));
    return run(port.valid()?xju::ip::UDPSocket(port.value()):xju::ip::UDPSocket());
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std:: endl;
    return 1;
  }
}
