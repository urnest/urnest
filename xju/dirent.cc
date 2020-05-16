// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/dirent.hh>

namespace xju
{
const SyscallF1<DIR*, const char*> opendir={
  "opendir",
  ::opendir};
const SyscallF1<int, DIR*> closedir={
  "closedir",
  ::closedir};

dirent* readdir(DIR* dir,xju::Traced const& t)
// throw SyscallFailed
{
  errno = 0;
  auto const result(::readdir(dir));
  if (result==0 && errno != 0){
    throw SyscallFailed("readdir",errno, t);
  }
  return result;
}
      
}

