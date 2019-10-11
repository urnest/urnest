// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef HCP_CHARS_HH
#define HCP_CHARS_HH

#include <set>
#include <string>
#include <bitset>
#include <iosfwd>

namespace hcp
{

class Chars
{
public:
  // a-f in pattern means abcdef
  // otherwise pattern is individual chars
  // (note f-a means the three chars -fa not fedcba)
  explicit Chars(std::string const& pattern) noexcept;
  
  std::string const& pattern() const noexcept { return pattern_; }
  std::bitset<256> const& bits() const noexcept { return bits_; }
  std::set<char> const& chars() const noexcept { return chars_; }

  // note that result.pattern_ might not accurately describe
  // the resulting character set, e.g. Chars("a-")+Chars("b")
  // will have patter()=="a-b" but cover only the three characters
  // 'a', '-' and 'b'
  Chars& operator+=(Chars const& b) noexcept;
private:
  std::string pattern_;
  std::bitset<256> bits_;
  std::set<char> chars_;

  friend bool operator<(Chars const& a, Chars const& b) noexcept {
    return a.chars_ < b.chars_;
  }
  friend std::ostream& operator<<(std::ostream& s, Chars const& x);

  friend Chars operator+(Chars const& a, Chars const& b) noexcept
  {
    Chars result(a);
    result+=b;
    return result;
  }
};

}

#endif
