// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/disk-space/statFileSystem.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/steadyNow.hh>
#include <xju/path.hh>
#include <xju/format.hh>
#include <xju/endsWith.hh>
#include <xju/startsWith.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

void test1(std::string const& self) {
  xju::assert_equal(statFileSystemOfPath(
                      xju::path::splitdir("/1"),
                      xju::steadyNow()+std::chrono::seconds(5),
                      self),
                    FileSystemStat(60819811ULL,
                                   36164358ULL,
                                   4096ULL));
  try{
    auto const r{
      statFileSystemOfPath(
        xju::path::splitdir("/2"),
        xju::steadyNow()+std::chrono::seconds(5),
        self)};
    xju::assert_never_reached();
  }
  catch(xju::Exception& e){
    auto const r{readableRepr(e)};
    xju::assert_equal(xju::startsWith(r,std::string("Failed to get size and use info for file system containing path /2 because\nfailed to run "+self+" -f --format=%b %f %s /2 within ")),true);
    xju::assert_equal(xju::endsWith(r,std::string("ms because\ncommand failed with status 256, stdout \"\" and stderr \"unknown file system\\n\".")),true);
  }
}

int stub(std::vector<std::string> const& args) {
  if (args.size()!=3){
    std::cerr << args[0] << " stub expected 3 args not "
              << xju::format::join(args.begin(),args.end(),std::string(", "))
              << "\n";
    return 1;
  }
  if (args[0]!="-f"){
    std::cerr << args[0] << " stub expected \"-f\" as first argument not "
              << args[0] << "\n";
    return 1;
  }
  if (args[1]!="--format=%b %f %s"){
    std::cerr << args[1] << " stub expected --format=%b %f %s as 2nd "
              << "argument not "
              << args[1] << "\n";
    return 1;
  }
  if (args[2]=="/1"){
    std::cout << "60819811 36164358 4096\n";
    return 0;
  }
  else if (args[2]=="/2"){
    std::cerr << "unknown file system\n";
    return 1;
  }
  std::cerr << args[0] << "unexpected 3rd argument " << args[2] << "\n";
  return 1;
}

}
}
}

using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  if (argc!=1){
    return stub(std::vector<std::string>(argv+1,argv+argc));
  }
  unsigned int n(0);
  test1(argv[0]), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

