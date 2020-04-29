// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/icmp/Pinger.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/test/Calls.hh>
#include <xju/pipe.hh>
#include <xju/ip/icmp/encodeEcho.hh>
#include <thread>
#include <xju/Thread.hh>
#include <xju/nanoseconds.hh>
#include <xju/now.hh>
#include <xju/stringToDouble.hh>
#include <xju/ip/v4/Header.hh>
#include <xju/ip/icmp/encodeDestinationUnreachable.hh>

namespace xju
{
namespace ip
{
namespace icmp
{

namespace stub
{

class SocketIf : public xju::ip::icmp::SocketIf
{
public:
  explicit SocketIf(std::chrono::nanoseconds leeway) noexcept
      :leeway_(leeway),
       input_(xju::pipe(true,true))
  {
  }
  void send(
    xju::ip::v4::Address const& to,
    Message const& x,
    std::chrono::steady_clock::time_point const& deadline) /*throw(
      xju::SyscallFailed,
      xju::DeadlineReached)*/ override
  {
    std::cout << xju::format::time(xju::now())
              << " send(" << to << ", " << x << ")"<<std::endl;
      
    calls_.enqueue(*this,
                   &xju::ip::icmp::SocketIf::send,
                   to,
                   x,
                   deadline)->awaitReturn();
  }
  std::tuple<xju::ip::v4::Address,Message> receive() /*throw(
      xju::SyscallFailed,
      // invalid message, e.g. incorrect checksum
      xju::Exception)*/ override
  {
    std::cout << xju::format::time(xju::now())
              << " receive()"<<std::endl;
      
    char c(' ');
    input_.first->read(&c,1U,xju::steadyNow());
    xju::assert_equal(c,'x');
    return calls_.enqueue(*this,&xju::ip::icmp::SocketIf::receive)
      ->awaitResult();
  }
  xju::test::Calls calls_;

  void makeReadable() noexcept
  {
    input_.second->write("x",1U,xju::steadyNow());
  }

  void receiveMessage(xju::ip::v4::Address a,
                      xju::ip::icmp::Message m) noexcept
  {
    makeReadable();
    calls_.awaitCall(*this,&xju::ip::icmp::SocketIf::receive,
                     xju::steadyNow()+leeway_)->return_(
                       std::make_tuple(a,m));
  }
  
  void receiveFail(xju::Exception const& e) noexcept
  {
    makeReadable();
    calls_.awaitCall(*this,&xju::ip::icmp::SocketIf::receive,
                     xju::steadyNow()+leeway_)->raise(e);
  }
  
  xju::io::Input const& input() const noexcept override
  {
    return *input_.first;
  }
private:
  std::chrono::nanoseconds const leeway_;
  std::pair<std::unique_ptr<xju::io::IStream>,
            std::unique_ptr<xju::io::OStream> > input_;

};

class CollectorIf : public xju::ip::icmp::Pinger::CollectorIf
{
public:
  xju::test::Calls calls_;

  void pinged(xju::ip::v4::Address const& address,
                        unsigned int retry,
                        std::chrono::nanoseconds rtt) noexcept override
  {
    calls_.enqueue(*this,&xju::ip::icmp::Pinger::CollectorIf::pinged,
                   address,
                   retry,
                   rtt)->awaitReturn();
  }
      
  void timeout(xju::ip::v4::Address const& address) noexcept override
  {
    calls_.enqueue(*this,&xju::ip::icmp::Pinger::CollectorIf::timeout,
                   address)->awaitReturn();
  }
  void unreachable(xju::ip::v4::Address const& address,
                   xju::ip::v4::Address const& gatewayOrHost,
                   xju::ip::icmp::Message::Code code) noexcept override
  {
    calls_.enqueue(*this,&xju::ip::icmp::Pinger::CollectorIf::unreachable,
                   address,
                   gatewayOrHost,
                   code)->awaitReturn();
  }
  void sendFailed(xju::Exception const& e) noexcept override
  {
    calls_.enqueue(*this,&xju::ip::icmp::Pinger::CollectorIf::sendFailed,
                   e)->awaitReturn();
  }
  void receiveFailed(xju::Exception const& e) noexcept override
  {
    calls_.enqueue(*this,&xju::ip::icmp::Pinger::CollectorIf::receiveFailed,
                   e)->awaitReturn();
  }
};
  
}

void test1(double rate) {
            
  std::chrono::nanoseconds const step{(long)(1e9/rate)};
  auto leeway=xju::nanoseconds(0.25*step); //allows for non-realtime delays

  stub::SocketIf socket(leeway);
  stub::CollectorIf collector;

  xju::ip::v4::Address const a1{ (1U<<24) | (2U<<16) | (3U<<8) | (4U<<0) };
  xju::ip::v4::Address const a2{ (1U<<24) | (2U<<16) | (3U<<8) | (5U<<0) };
  auto const timeoutPerRetry{xju::nanoseconds(0.4*step)};
  xju::ip::icmp::Echo::Identifier identifier{0xfedc};
  Pinger p{socket,
           collector,
           identifier,
           Echo::Sequence(5),
           {a1,a2},
           rate,
           3,
           timeoutPerRetry};

  auto t1{xju::steadyNow()};
  xju::Thread t{ [&]{ p.run(); }, [&]{ p.stop(); }};

  // round 1 perfect
  {
    auto call=socket.calls_.awaitCall(socket,&SocketIf::send, t1+leeway);
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(5),
                                                        {}))));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    std::this_thread::sleep_for(0.1*step);
    
    socket.receiveMessage(a1,
                          Message(Message::Type::ECHOREPLY,
                                  Message::Code(0),
                                  Checksum(0),
                                  encodeEcho(Echo(identifier,
                                                  Echo::Sequence(5),
                                                  {}))));
    auto call=collector.calls_.awaitCall(
      collector,
      &Pinger::CollectorIf::pinged,
      xju::steadyNow()+xju::nanoseconds(0.1*step));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,0U);
    xju::assert_greater_equal(call->p3_,0.1*step);
    xju::assert_less(call->p3_,0.2*step);
    call->return_();
  }
  {
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(1.0*step+leeway));
    xju::assert_equal(call->p1_,a2);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(5),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),
                              t1+xju::nanoseconds(1.0*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    std::this_thread::sleep_for(0.2*step);
    socket.receiveMessage(a2,
                          Message(Message::Type::ECHOREPLY,
                                  Message::Code(0),
                                  Checksum(0),
                                  encodeEcho(Echo(identifier,
                                                  Echo::Sequence(5),
                                                  {}))));
    auto call=collector.calls_.awaitCall(collector,
                                         &Pinger::CollectorIf::pinged,
                                         t1+xju::nanoseconds(1.3*step));
    xju::assert_equal(call->p1_,a2);
    xju::assert_equal(call->p2_,0U);
    xju::assert_greater_equal(call->p3_,0.2*step);
    xju::assert_less(call->p3_,0.3*step);
    call->return_();
  }

  //round 2 - a1 times out, a2 takes 2 attempts
  {
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(2.0*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(9),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(2.0*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //re-send to a1 after 0.4*step timeout
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(2.4*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(10),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(2.4*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //re-send to a1 after another 0.4*step timeout
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(2.8*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(11),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(2.8*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //a2 ping interleaves
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(3.0*step+leeway));
    xju::assert_equal(call->p1_,a2);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(9),
                                                        {}))));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //re-send to a1 after another 0.4*step timeout
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(3.2*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(12),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(3.2*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //re-send to a2 after 0.4*step timeout
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(3.4*step+leeway));
    xju::assert_equal(call->p1_,a2);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(10),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(3.4*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    //a2 responds
    socket.receiveMessage(a2,
                          Message(Message::Type::ECHOREPLY,
                                  Message::Code(0),
                                  Checksum(0),
                                  encodeEcho(Echo(identifier,
                                                  Echo::Sequence(10),
                                                  {}))));
    auto call=collector.calls_.awaitCall(collector,
                                         &Pinger::CollectorIf::pinged,
                                         t1+xju::nanoseconds(3.4*step+leeway));
    xju::assert_equal(call->p1_,a2);
    xju::assert_equal(call->p2_,1U);
    xju::assert_greater_equal(call->p3_,0.0*step);
    xju::assert_less(call->p3_,0.1*step);
    call->return_();
  }
  {
    //a1 times out = max retries
    auto t1{xju::steadyNow()};
    auto call=collector.calls_.awaitCall(collector,
                                         &Pinger::CollectorIf::timeout,
                                         t1+xju::nanoseconds(3.6*step+leeway));
    xju::assert_equal(call->p1_,a1);
    call->return_();
  }
  return;
}

void test2(double rate) {
            
  std::chrono::nanoseconds const step{(long)(1e9/rate)};
  auto leeway=xju::nanoseconds(0.25*step); //allows for non-realtime delays

  stub::SocketIf socket(leeway);
  stub::CollectorIf collector;

  xju::ip::v4::Address const a1{ (1U<<24) | (2U<<16) | (3U<<8) | (4U<<0) };
  xju::ip::v4::Address const a2{ (1U<<24) | (2U<<16) | (3U<<8) | (5U<<0) };
  xju::ip::v4::Address const a3{ (1U<<24) | (2U<<16) | (3U<<8) | (6U<<0) };
  auto const timeoutPerRetry{xju::nanoseconds(0.4*step)};
  xju::ip::icmp::Echo::Identifier identifier{0xfedc};
  Pinger p{socket,
           collector,
           identifier,
           Echo::Sequence(5),
           {a1},
           rate,
           3,
           timeoutPerRetry};

  auto t1{xju::steadyNow()};
  xju::Thread t{ [&]{ p.run(); }, [&]{ p.stop(); }};

  // a1 unreachable
  {
    auto call=socket.calls_.awaitCall(socket,&SocketIf::send, t1+leeway);
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(5),
                                                        {}))));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    std::this_thread::sleep_for(0.1*step);
    
    socket.receiveMessage(
      a3,
      Message(Message::Type::DEST_UNREACH,
              Message::Code(6),
              Checksum(0),
              encodeDestinationUnreachable(
                128U,
                xju::ip::v4::Header(
                  xju::ip::v4::Header::Version(4),
                  xju::ip::v4::Header::IHL(5),
                  xju::ip::v4::Header::DSCP(0),
                  xju::ip::v4::Header::ECN(0),
                  xju::ip::v4::Header::TotalLength(28),
                  xju::ip::v4::Header::Identification(0),
                  xju::ip::v4::Header::Flags(0),
                  xju::ip::v4::Header::FragmentOffset(0),
                  xju::ip::v4::Header::TTL(0),
                  xju::ip::v4::Header::Protocol(1),
                  xju::ip::v4::Header::HeaderChecksum(0),
                  a3,
                  a1,
                  {}),
                Message(Message::Type::ECHO,
                        Message::Code(0),
                        Checksum(0),
                        encodeEcho(
                          Echo(identifier,
                               Echo::Sequence(5),
                               {}))))));
    auto call=collector.calls_.awaitCall(
      collector,
      &Pinger::CollectorIf::unreachable,
      xju::steadyNow()+xju::nanoseconds(0.1*step));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,a3);
    xju::assert_equal(call->p3_,Message::Code(6));
    call->return_();
  }
  //a1 not tried again until next round
  {
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(1.0*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(9),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(1.0*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
}

void test3(double rate) {
            
  std::chrono::nanoseconds const step{(long)(1e9/rate)};
  auto leeway=xju::nanoseconds(0.25*step); //allows for non-realtime delays

  stub::SocketIf socket(leeway);
  stub::CollectorIf collector;

  xju::ip::v4::Address const a1{ (1U<<24) | (2U<<16) | (3U<<8) | (4U<<0) };
  xju::ip::v4::Address const a2{ (1U<<24) | (2U<<16) | (3U<<8) | (5U<<0) };
  xju::ip::v4::Address const a3{ (1U<<24) | (2U<<16) | (3U<<8) | (6U<<0) };
  auto const timeoutPerRetry{xju::nanoseconds(0.4*step)};
  xju::ip::icmp::Echo::Identifier identifier{0xfedc};
  Pinger p{socket,
           collector,
           identifier,
           Echo::Sequence(5),
           {a1},
           rate,
           3,
           timeoutPerRetry};

  auto t1{xju::steadyNow()};
  xju::Thread t{ [&]{ p.run(); }, [&]{ p.stop(); }};

  // receive fails
  {
    auto call=socket.calls_.awaitCall(socket,&SocketIf::send, t1+leeway);
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(5),
                                                        {}))));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
  {
    std::this_thread::sleep_for(0.1*step);
    xju::Exception e("some error",XJU_TRACED);
    socket.receiveFail(e);
    auto call=collector.calls_.awaitCall(
      collector,
      &Pinger::CollectorIf::receiveFailed,
      xju::steadyNow()+xju::nanoseconds(0.1*step));
    xju::assert_equal(call->p1_,e);
    call->return_();
  }
  {
    std::this_thread::sleep_for(0.1*step);
    
    socket.receiveMessage(a1,
                          Message(Message::Type::ECHOREPLY,
                                  Message::Code(0),
                                  Checksum(0),
                                  encodeEcho(Echo(identifier,
                                                  Echo::Sequence(5),
                                                  {}))));
    auto call=collector.calls_.awaitCall(
      collector,
      &Pinger::CollectorIf::pinged,
      xju::steadyNow()+xju::nanoseconds(0.2*step));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,0U);
    xju::assert_greater_equal(call->p3_,0.2*step);
    xju::assert_less(call->p3_,0.3*step);
    call->return_();
  }
  // send fails
  {
    auto call=socket.calls_.awaitCall(socket,&SocketIf::send,
                                      t1+xju::nanoseconds(1.0*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(9),
                                                        {}))));
    xju::assert_empty(collector.calls_.calls());
    xju::DeadlineReached e(xju::Exception("some error",XJU_TRACED));
    call->raise(e);
    auto call2=collector.calls_.awaitCall(
      collector,
      &Pinger::CollectorIf::sendFailed,
      xju::steadyNow()+xju::nanoseconds(0.1*step));
    xju::assert_equal(call2->p1_,e);
    call2->return_();
  }
  //a1 not tried again until next round
  {
    auto call=socket.calls_.awaitCall(socket,
                                      &SocketIf::send,
                                      t1+xju::nanoseconds(2.0*step+leeway));
    xju::assert_equal(call->p1_,a1);
    xju::assert_equal(call->p2_,Message(Message::Type::ECHO,
                                        Message::Code(0),
                                        Checksum(0),
                                        encodeEcho(Echo(identifier,
                                                        Echo::Sequence(13),
                                                        {}))));
    xju::assert_greater_equal(xju::steadyNow(),t1+xju::nanoseconds(2.0*step));
    xju::assert_empty(collector.calls_.calls());
    call->return_();
  }
}

}
}
}

using namespace xju::ip::icmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(argc>1?xju::stringToDouble(argv[1]):1), ++n;
  test2(argc>1?xju::stringToDouble(argv[1]):1), ++n;
  test3(argc>1?xju::stringToDouble(argv[1]):1), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

