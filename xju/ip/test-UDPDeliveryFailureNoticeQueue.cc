// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/UDPDeliveryFailureNoticeQueue.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/HostName.hh>
#include <xju/ip/UDPSocket.hh>
#include <xju/steadyNow.hh>
#include <xju/io/poll.hh>

namespace xju
{
namespace ip
{

void test1() {
  // verify can send and receive without errors
  UDPSocket s1;
  auto const p1(s1.port());
  UDPDeliveryFailureNoticeQueue q1(s1);

  xju::assert_equal(
    q1.getFailureNotices(),
    std::vector<UDPDeliveryFailureNoticeQueue::DeliveryFailureNotice>());

  UDPSocket s2;
  UDPDeliveryFailureNoticeQueue q2(s2);

  
  s2.sendTo({xju::ip::v4::Address("127.0.0.1"),p1},
            "fred",5U,xju::steadyNow());
  {
    char buffer[5];
    s1.receive(buffer,sizeof(buffer),xju::steadyNow());
    xju::assert_equal(buffer,std::string("fred"));
  }
}
void test2(xju::HostName const& unreachableHost)
{
  // verify error sets POLLERR and does not try to collect pending
  // inbound messages; verify
}

void test3(xju::ip::v4::Address const& arpFailureHost)
{
  // verify non-existent local network host i.e. arp failure
  // generates local delivery failure
  // verify can send and receive without errors
  UDPSocket s1;
  auto const p1(s1.port());
  UDPDeliveryFailureNoticeQueue q1(s1);

  s1.sendTo({arpFailureHost,xju::ip::Port(22)},
            "fred",4,xju::steadyNow());

  {
    auto ios(xju::io::poll({&s1},
                           {},
                           xju::steadyNow()+std::chrono::seconds(10)));
    xju::assert_equal(ios.second.size(),0U);
    xju::assert_equal(ios.first.size(),1U);
    xju::assert_not_equal(ios.first.find(&s1),ios.first.end());
    xju::assert_equal((*ios.first.find(&s1)).second,
                      xju::io::PollInputState::ERR);
  }

  //REVISIT: we get SyscallFailed(EHOSTUNREACH) rather than
  //         getting the failure notice as successful read?
  xju::assert_equal(q1.getFailureNotices(),
                    {UDPDeliveryFailureNoticeQueue::DeliveryFailureNotice(
                        {arpFailureHost,xju::ip::Port(22)},
                        EHOSTUNREACH,
                        0,
                        std::tuple<
                            xju::ip::v4::Address, // dropped by
                            xju::ip::icmp::Message::Type, // why
                            xju::ip::icmp::Message::Code  //  "
                        >(xju::ip::v4::Address("127.0.0.1"),
                          xju::ip::icmp::Message::Type::DEST_UNREACH,
                          xju::ip::icmp::Message::Code(0)))});
  
  xju::assert_equal(
    xju::io::poll({&s1},
                  {},
                  xju::steadyNow()+std::chrono::seconds(10),
                  {&s1}),
    std::pair<std::map<xju::io::Input const*,xju::io::PollInputState>,
              std::map<xju::io::Output const*,xju::io::PollOutputState> >(
                { {&s1,xju::io::PollInputState::IN} },
                {}));
  {
    char buffer[5];
    s1.receive(buffer,sizeof(buffer),xju::steadyNow());
    xju::assert_equal(buffer,std::string("fred"));
  }
  
}

}
}

using namespace xju::ip;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  if(argc==2 && std::string(argv[1])!=""){
    test2(xju::HostName(argv[1])), ++n;
  }
  if(argc==3 && std::string(argv[2])!=""){
    test3(xju::ip::v4::Address(argv[2])), ++n;
  }
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

