// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/pipe.hh>
#include <xju/async.hh>
#include <xju/steadyEternity.hh>
#include <xju/steadyNow.hh>
#include <xju/DeadlineReached.hh>
#include <xju/io/Output.hh>
#include <signal.h>

namespace xju
{
namespace ssh
{
using namespace std::chrono_literals;

void test1() {
  // sendIdentificationString
  {
    auto p(xju::pipe(true,true));
    xju::async([&](){
                 return transport::sendIdentificationString(
                   *p.second,
                   "fred's-ssh-2.6",
                   "better than all the rest",
                   xju::steadyEternity());
               });
    std::vector<char> buffer;
    buffer.resize(1024);
    xju::io::select({p.first.get()},xju::steadyNow()+1s);
    buffer.resize(p.first->read(&buffer[0],buffer.size(),xju::steadyNow()+50ms));
    xju::assert_equal(std::string(buffer.begin(),buffer.end()),
                      "SSH-2.0-fred's-ssh-2.6 better than all the rest\r\n");
  }
  {
    auto p(xju::pipe(true,true));
    xju::async([&](){
                 return transport::sendIdentificationString(
                   *p.second,
                   "fred's-ssh-2.6",
                   std::string(),
                   xju::steadyEternity());
               });
    std::vector<char> buffer;
    buffer.resize(1024);
    xju::io::select({p.first.get()},xju::steadyNow()+1s);
    buffer.resize(p.first->read(&buffer[0],buffer.size(),xju::steadyNow()+50ms));
    xju::assert_equal(std::string(buffer.begin(),buffer.end()),
                      "SSH-2.0-fred's-ssh-2.6\r\n");
  }
  {
    auto p(xju::pipe(true,true));
    while(xju::io::select({p.second.get()}, xju::steadyNow()).size()){
      std::string x("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
      p.second->write(x.c_str(),x.size(),xju::steadyNow());
    }
    try{
      transport::sendIdentificationString(
        *p.second,
        "fred's-ssh-2.6",
        std::string(),
        xju::steadyNow()+50ms);
    }
    catch(xju::DeadlineReached const& e){
      xju::assert_startswith(readableRepr(e),
                             std::string("Failed to write SSH identification string \"SSH-2.0-fred's-ssh-2.6\\r\\n\" because\nfailed to write 24 more bytes, having written 0, to writeable end of pipe (file descriptor"));
      xju::assert_endswith(readableRepr(e),
                           std::string(") by deadline because\ndeadline reached."));
    }
  }
  {
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss,SIGPIPE);
    sigprocmask(SIG_BLOCK,&ss,0);
  }
  {
    auto p(xju::pipe(true,true));
    while(xju::io::select({p.second.get()}, xju::steadyNow()).size()){
      std::string x("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
      p.second->write(x.c_str(),x.size(),xju::steadyNow());
    }
    p.first.reset();
    try{
      transport::sendIdentificationString(
        *p.second,
        "fred's-ssh-2.6",
        std::string(),
        xju::steadyNow()+50ms);
    }
    catch(xju::io::Output::Closed const& e){
      xju::assert_startswith(readableRepr(e),
                             std::string("Failed to write SSH identification string \"SSH-2.0-fred's-ssh-2.6\\r\\n\" because\nfailed to write 24 more bytes, having written 0, to writeable end of pipe (file descriptor"));
      xju::assert_endswith(readableRepr(e),
                           std::string(") closed."));
    }
  }
}

}
}

using namespace xju::ssh;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

