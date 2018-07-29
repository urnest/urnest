// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


// deduce at compile time the POD type to use to correctly align
// type T with minimal padding
//

#include <cinttypes>
#include <cstddef>

namespace xju
{

template<size_t Size>
struct MinAlignType
{
private:
  typedef void T; // specialisations below
};

template<>
struct MinAlignType<1U>
{
  // must specialise
  typedef uint8_t T;
};
template<>
struct MinAlignType<2U>
{
  // must specialise
  typedef uint16_t T;
};
template<>
struct MinAlignType<4U>
{
  // must specialise
  typedef uint32_t T;
};
template<>
struct MinAlignType<8U>
{
  // must specialise
  typedef uint64_t T;
};

template<class T_>
struct MinAlign
{
  struct Offset
  {
    std::max_align_t a;
    uint8_t b;
    T_ c;
  };
  typedef typename MinAlignType< offsetof(Offset,c)-offsetof(Offset,b) >::T T;
};

}
