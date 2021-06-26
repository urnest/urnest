// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/I.hh>
#include <iostream>
#include <map>

int main(int argc, char* argv[])
{
  xju::crypt::I const x(std::vector<uint8_t>({0x1f,0xff}));
  std::map<xju::crypt::I, uint64_t> dist;
  for(uint64_t i=0; i != 8191000; ++i){
    xju::crypt::I b(randl(x));
    auto j(dist.insert({b,0}).first);
    ++((*j).second);
  }
  for(auto j: dist)
  {
    auto const k(j.first.value());
    uint32_t kk(k[0]);
    kk<<=8;
    kk+=k[1];
    std::cout << kk << ": " << j.second << "\n";
  }
  return 0;
}
