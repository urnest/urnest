// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/startService.hh>

#include <iostream>
#include <xju/assert.hh>
#include <future>
#include <chrono>
#include <xju/test/call.hh>
#include <xju/test/Calls.hh>
#include <xju/ssh/decodeMessage.hh>
#include <xju/ssh/encodeMessage.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

namespace
{
class Receiver : public ReceiveInterface
{
public:
  virtual Message receiveMessage(size_t maxSize) override
  {
    return calls_.enqueue(*this,&ReceiveInterface::receiveMessage,
                          maxSize)->awaitResult();
  }
  xju::test::Calls calls_;
};
class Sender : public SingleThreadedSendInterface
{
public:
  virtual void sendMessage(
    Message const& message,
    std::back_insert_iterator<std::vector<Message> > receivedMessages) override
  {
    calls_.enqueue(*this,&SingleThreadedSendInterface::sendMessage,
                   message,
                   receivedMessages)->awaitReturn();
  }
  xju::test::Calls calls_;
};

}

void test1() {
  Receiver receiver;
  Sender sender;

  std::chrono::seconds leeway(1U);

  xju::ssh::misc::ServiceName const userauth("ssh-userauth");
                           
  auto c(xju::test::call([&](){
                           return startService(
                             userauth,
                             sender,
                             receiver);
                         }));
  {
    auto c(sender.calls_.awaitCall(
             sender,&SingleThreadedSendInterface::sendMessage,
             xju::steadyNow()+leeway));
    xju::assert_equal(
      xju::ssh::decodeMessage<messages::ServiceRequest>(
        (uint8_t)MSG::SERVICE_REQUEST,
        c->p1_),
      messages::ServiceRequest(xju::ssh::misc::ServiceName("ssh-userauth")));
    c->return_();
  }
  {
    auto c(receiver.calls_.awaitCall(
             receiver,&ReceiveInterface::receiveMessage,
             xju::steadyNow()+leeway));
    xju::assert_equal(c->p1_,4096U);
    c->return_(encodeMessage(
                 messages::Ignore(std::vector<uint8_t>({1,2,3,4}))));
    c=receiver.calls_.awaitCall(
      receiver,&ReceiveInterface::receiveMessage,
      xju::steadyNow()+leeway);
    xju::assert_equal(c->p1_,4096U);
    c->return_(encodeMessage(messages::ServiceAccept(
                               xju::ssh::misc::ServiceName("ssh-userauth"))));
  }
  std::vector<Message> messages(c->getResultBy(xju::steadyNow()+leeway));
  xju::assert_equal(messages.size(),1U);
  xju::assert_equal(decodeMessage<messages::Ignore>(
                      (uint8_t)MSG::IGNORE,messages[0]),
                    messages::Ignore(std::vector<uint8_t>({1,2,3,4})));
}

// wrong service returned
// send fails
// 2nd receive fails
// disconnect

}
}
}

using namespace xju::ssh::transport;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

