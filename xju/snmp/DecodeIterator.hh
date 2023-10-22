// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODEITERATOR_H
#define XJU_SNMP_DECODEITERATOR_H

#include <vector>
#include <sstream>
#include "xju/Exception.hh"
#include "xju/assert.hh"

namespace xju
{
namespace snmp
{

class DecodeIterator
{
public:
  explicit DecodeIterator(std::vector<uint8_t> const& data) throw():
      data_(&data),
      at_(data.begin()) {
  }
  uint8_t const& operator*() const /*throw(xju::Exception)*/
  {
    if (atEnd()) {
      std::ostringstream s;
      s << "end of data at offset " << data_->size();
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    return *at_;
  }
  DecodeIterator operator++(int) /*throw(xju::Exception)*/
  {
    DecodeIterator result(*this);
    operator++();
    return result;
  }
  DecodeIterator& operator++() /*throw(xju::Exception)*/
  {
    if (atEnd()) {
      std::ostringstream s;
      s << "end of data at offset " << data_->size();
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    ++at_;
    return *this;
  }
  size_t remaining() const throw()
  {
    return data_->end() - at_;
  }
  bool atEnd() const throw()
  {
    return at_==data_->end();
  }
  std::vector<uint8_t> const* data_;
  std::vector<uint8_t>::const_iterator at_;

  friend std::ostream& operator<<(std::ostream& s, 
                                  DecodeIterator const& i) throw()
  {
    return s << "offset " << (i.at_-i.data_->begin());
  }
  friend DecodeIterator operator+(DecodeIterator const& i,
                                  int n) throw() {
    xju::assert_less_equal(n,i.data_->end()-i.at_);
    DecodeIterator result(i);
    result.at_+=n;
    return result;
  }
  friend bool operator<(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_<j.at_;
  }
  friend bool operator<=(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_<=j.at_;
  }
  friend bool operator==(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_==j.at_;
  }
  friend bool operator>=(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_>=j.at_;
  }
  friend bool operator>(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_>j.at_;
  }
  friend bool operator!=(DecodeIterator const& i, DecodeIterator const& j) 
    throw()
  {
    return i.at_!=j.at_;
  }
  
};


}
}

#endif

