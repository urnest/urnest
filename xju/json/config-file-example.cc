// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

// This example reads a ficticious JSON config file into a
// C++ config class, with validation.

#include <xju/Utf8String.hh>
#include <string>
#include <utility>
#include <chrono>
#include <set>
#include <sstream>
#include <xju/format.hh>
#include <cinttypes>
#include <xju/json/parse.hh>
#include <iostream>

xju::Utf8String const configJSON{std::string(R"--(
{
  "name":"FRED",
  "port":576,
  "targets":[ "JOCK","SALLY","PAULA" ],
  "statConfig":{
    "timeOfDay":1300,
    "secondsBetweenSamples":3.2
  }
}
)--")};

class Config{
public:
  typedef std::pair<std::chrono::minutes,std::chrono::milliseconds> StatConfig;

  xju::Utf8String name_;
  uint16_t port_;
  std::set<xju::Utf8String> targets_;
  StatConfig statConfig_;
  
  Config(xju::Utf8String name,
         int port,
         std::set<xju::Utf8String> targets,
         StatConfig const& statConfig) /*throw(
           //invalid config
           xju::Exception)*/:
      name_(name),
      port_(port),
      targets_(std::move(targets)),
      statConfig_(statConfig) {
    try{
      if (targets_.find(name_)!=targets_.end()){
        std::ostringstream s;
        s << "name (" << xju::format::quote(name_)
          << ") may not be the name of a target";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (port<0 || port>UINT16_MAX){
        std::ostringstream s;
        s << "port, specified as " << port << " must be in range 0.."
          << UINT16_MAX;
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (statConfig_.first>std::chrono::minutes(2359)||
          statConfig_.first<std::chrono::minutes(0)){
        std::ostringstream s;
        s << "statConfig.timeOfDay, specified as " << statConfig_.first.count()
          << ", must be in range 0..2359";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (statConfig_.second<=std::chrono::milliseconds(0)){
        std::ostringstream s;
        s << "statConfig.secondsBetweenSamples must be at least 0.001";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "validate config " << (*this) << std::endl;
    }
  }

  friend std::ostream& operator<<(std::ostream& s, Config const& x) noexcept
  {
    return s << "name " << xju::format::quote(x.name_)
             << ", port " << x.port_
             << ", targets { " << xju::format::join(
               x.targets_.begin(),
               x.targets_.end(),
               [](xju::Utf8String const& t){ return xju::format::quote(t); },
               std::string(", ")) << " }"
             << ", stat config: " << " report time-of-day (minutes): "
             << xju::format::int_(x.statConfig_.first.count(),4)
             << ", seconds between samples: "
             << xju::format::float_(
               1.0*x.statConfig_.second.count()/1000.0,
               std::ios::fixed,3);
  }
};

int main(int argc, char* argv[])
{
  try{
    auto const p{xju::json::parse(configJSON)};

    auto const name{p->getMember("name").asString()};
    auto const port{p->getMember("port").asInt()};
    std::set<xju::Utf8String> targets;
    std::transform(p->getMember("targets").asArray().begin(),
                   p->getMember("targets").asArray().end(),
                   std::inserter(targets,targets.end()),
                   [](std::shared_ptr<xju::json::Element const> x){
                       return x->asString();
                   });
    auto const& statConfig{p->getMember("statConfig")};
    
    Config const c{
      name,port,targets,
      Config::StatConfig(
        std::chrono::minutes(
          statConfig.getMember("timeOfDay").asInt()),
        std::chrono::milliseconds(
          (long)
          (statConfig.getMember("secondsBetweenSamples").asDouble()*1000.0)))};

    std::cout << c << std::endl;
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse config " << xju::format::quote(configJSON);
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}


