// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/getIdentifierRefAt.hh>
#include <sstream>

namespace hcp
{
namespace tags
{

namespace
{
bool isIdentifierChar(char c) throw()
{
  return (c>='a'&&c<='z')||
    (c>='A'&&c<='Z')||
    (c>='0'&&c<='9')||
    (c=='_');
}
}

IdentifierRef getIdentifierRefAt(std::string const& text,
                                 std::string::size_type const offset)
  throw(
    // pre: no identifier at offset
    xju::Exception)
{
  try {
    if (offset>text.size()) {
      std::ostringstream s;
      s << "offset " << offset << " is not within text, which has length "
        << text.size();
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    try {
      size_t end(offset);
      while(end<text.size() && isIdentifierChar(text[end])) {
        ++end;
      }
      size_t start(offset);
      while(start>0 && isIdentifierChar(text[start-1])) {
        --start;
      }
      if (start==end) {
        std::ostringstream s;
        s << "character at or just before offset " << offset
          << " is not an identifier char ie not a..z, A..Z, 0..9 or _";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (text[start]>='0'&&text[start]<='9') {
        std::ostringstream s;
        s << "identifier cannot start with digit ('" << text[start] << "' at offset "
          << start << ")";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      return IdentifierRef(std::string(text.begin()+start,text.begin()+end));
    }
    catch(xju::Exception& e) {
      xju::assert_less_equal(offset,text.size());

      auto const at(text.begin()+offset);
      auto const begin(std::min(offset,10UL));
      auto const end(std::min(offset+10,text.size()));
      
      std::string const before(text.begin()+(offset-begin),at);
      std::string const after(at,text.begin()+std::min(offset+10,text.size()));

      std::ostringstream s;
      s << "get identifier ref at or just before \"^^\" in \""
        << before << "^^" << after
        << "\"";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get C++ identifier ref at or just before offset " << offset
      << " in text";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}


}
}

