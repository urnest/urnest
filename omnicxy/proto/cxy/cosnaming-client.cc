// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <omnicxy/proto/CosNaming.cref.hh>
#include <cinttypes>
#include <string>
#include <sstream>
#include <cxy/ORB.hh>
#include <vector>
#include <xju/format.hh>
#include <xju/next.hh>
#include <xju/split.hh>
#include <algorithm>
#include <iostream>

namespace
{
std::string format(CosNaming::NameComponent const& x) noexcept
{
  std::ostringstream s;
  s << x.id;
  if (x.kind!=CosNaming::Istring("")){
    s << ":" << x.kind;
  }
  return s.str();
}
std::string format(CosNaming::Name const& x) noexcept
{
  return xju::format::join(x.begin(),
                           x.end(),
                           [](CosNaming::NameComponent const& x){
                             return format(x);
                           },
                           "/");
}

std::string show(cxy::ORB<cxy::Exception>& orb,
                 cxy::cref<CosNaming::NamingContext> ref,
                 std::vector<std::string> const& scope,
                 std::vector<CosNaming::NameComponent> const& rest) throw(
                   cxy::Exception)
{
  try{
    if (rest.size()==0) {
      auto const l{std::get<0>(ref->list(UINT32_MAX))};
      std::string const prefix{
        scope.size()?
          xju::format::join(scope.begin(),scope.end(),std::string("/"))+"/":
          std::string()};
      return xju::format::join(
        l.begin(),
        l.end(),
        [&](CosNaming::Binding const& x){
          std::ostringstream s;
          s << prefix << format(x.binding_name[0]);
          return s.str();
        },
        "\n");
    }
    auto const name{*rest.begin()};
    auto const value{ref->resolve({name})};
    try{
      std::vector<std::string> nextScope{scope};
      nextScope.push_back(format(name));
      return show(
        orb,
        cxy::cref<CosNaming::NamingContext>(orb,value),
        nextScope,
        std::vector<CosNaming::NameComponent>(xju::next(rest.begin()),
                                              rest.end()));
    }
    catch(cxy::WrongType const&){
    }
    if(rest.size()>1){
      std::ostringstream s;
      s << format(name) << " in scope "
        << xju::format::join(scope.begin(),scope.end(),"/")
        << " is not a CosNaming::NamingContext";
      throw cxy::Exception(s.str(),{__FILE__,__LINE__});
    }
    std::ostringstream s;
    s << xju::format::join(scope.begin(),scope.end(),"/") << "/"
      << format(name) << "="
      << value;
    return s.str();
  }
  catch(cxy::Exception& e){
    std::ostringstream s;
    s << "show name " << format(rest) << " in NamingContext "
      << xju::format::join(scope.begin(),scope.end(),"/");
    e.addContext(s.str(),{__FILE__,__LINE__});
    throw;
  }
}

}

int main(int argc, char* argv[])
{
  if (argc<3){
    std::cout << "usage: " << argv[0] << "name-service-IOR a/b/c...\n";
    std::cout << "a/b/c is name, e.g. AUS/CITIES/BRISBANE" << std::endl;
    std::cout << "(components can include kind e.g. CITIES:BIG" << std::endl;
    return 1;
  }
  try{
    std::string const cmd{argv[2]};
    
    cxy::ORB<cxy::Exception> orb("giop:tcp::");
    cxy::cref<CosNaming::NamingContext> ref(orb, argv[1]);
    std::vector<std::string> name{
      xju::split(argc==3?std::string(argv[2]):std::string(""),
                 '/')};
    std::vector<CosNaming::NameComponent> ncs;
    std::transform(name.begin(),name.end(),
                   std::back_inserter(ncs),
                   [](std::string const& x){
                     auto const result(xju::split(x,':'));
                     if (result.size()==1){
                       return CosNaming::NameComponent(
                         CosNaming::Istring(result[0]),
                         CosNaming::Istring(""));
                     }
                     else if (result.size()==2){
                       return CosNaming::NameComponent{
                         CosNaming::Istring(result[0]),
                           CosNaming::Istring(result[1])};
                     }
                     std::ostringstream s;
                     s << xju::format::quote(x) << "contains multiple colons";
                     throw cxy::Exception(s.str(),{__FILE__,__LINE__});
                   });
    std::cout << show(orb,ref,{},ncs) << std::endl;
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
  catch(cxy::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
