// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/Utf8String.hh>

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
  int port_;
  std::set<xju::Utf8String> targets_;
  StatConfig statConfig_;
  
  Config(xju::Utf8String name,
         int port,
         std::set<xju::Utf8String> targets,
         StatConfig const& statConfig) throw(
           //invalid config
           xju::Exception):
      name_(name),
      port_(port),
      targets_(std::move(targets)),
      statConfig_(statConfig) {
    try{
      if (targets_.find(name_)!=targets_end()){
        std::ostringstream s;
        s << "name (" << xju::format::quote(name_)
          << ") may not be the name of a target";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (statConfig_.first>std::chrono::minutes(2359)||
          statConfig_.first<std::chrono::minutes(0)){
        std::ostringstream s;
        s << "statConfig.timeOfDay, specified as " << statConfig_.first.count()
          << ", must be in range 0..2359";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (statConfig_.second<=0){
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
             << xju::format::int(x.statConfig_.first.count(),4)
             << ", seconds between samples: "
             << xju::format::float_(
               1.0*x.statConfig_.second.count()/1000.0,
               ios::fixed,3);
  }
};

int main(int argc, char* argv[])
{
  try{
    auto const p{xju::json::parse(configJSON)};

    auto const name{p->getMember(xju::Utf8String("name")).asString()};
    auto const port{p->getMember(xju::Utf8String("port")).asInt()};
    std::set<xju::Utf8String> targets;
    std::transform(p->getMember(xju::Utf8String("targets")).asArray().begin(),
                   p->getMember(xju::Utf8String("targets")).asArray().end(),
                   [](std::shared_ptr<xju::json::Element const> x){
                       return x.asString();
                   },
                   std::inserter(targets,targets.end()));
    auto const statConfig{p->getMember(xju::Utf8String("statConfig"))};
    
    Config const c{
      name,port,targets,
      Config::StatConfig(
        statConfig.getMember(xju::Utf8String("timeOfDay")).asInt(),
        std::chrono::milliseconds(
          p->getMember(xju::Utf8String("statConfig")).getMember(
            xju::Utf8String("secondsBetweenSamples")).asFloat()*1000.0))};

    std::cout << c << std::endl;
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse config " << xju::format::quote(configJSON);
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readable(e) << std::endl;
    return 1;
  }
}


