// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "hcp/translateException.hh"
#include <sstream>

namespace hcp
{

xju::Exception translateException(hcp_parser::Exception const& e) noexcept
{
  std::ostringstream s;
  s << e.at_ << ": " << e.cause_->str();
  
  std::vector<std::pair<std::string, xju::Traced> > context;
  xju::Exception ee(s.str(), XJU_TRACED);
  typedef std::pair<std::pair<hcp_parser::Parser const*, hcp_parser::I>, xju::Traced> C;
  std::vector<C>::const_iterator i;
  
  for(i=e.context_.begin(); i!=e.context_.end(); ++i) {
    // to get a less verbose but hopefully detailed enough
    // error message, we only add context from NamedParsers
    // and from the root-cause parser (whether it is a NamedParser
    // or not)
    if (i==e.context_.begin() ||
        dynamic_cast<hcp_parser::NamedParser_ const*>((*i).first.first)) {
      std::ostringstream s;
      s << "parse " << (*i).first.first->target() 
        << " at " << (*i).first.second;
      ee.addContext(s.str(), (*i).second);
    }
  }
  return ee;
}


}

