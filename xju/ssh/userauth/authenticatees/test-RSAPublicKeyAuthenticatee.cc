// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/userauth/authenticatees/RSAPublicKeyAuthenticatee.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/test/Calls.hh>
#include <xju/crypt/rsa/PrivateKey.hh>
#include <xju/crypt/rsa/Signer.hh>
#include <xju/crypt/rsa/SigVerifier.hh>
#include <xju/MemOBuf.hh>
#include <xju/net/ostream.hh>
#include <xju/ssh/transport/messages/Ignore.hh>
#include <xju/crypt/rsa/generateKey.hh>
#include <xju/test/call.hh>
#include <xju/ssh/transport/decodeMessage.hh>
#include <xju/ssh/userauth/messages/RSAPublicKeyRequest.hh>
#include <xju/ssh/userauth/MSG.hh>
#include <xju/ssh/encode.hh>
#include <xju/ssh/transport/encodeMessage.hh>
#include <xju/ssh/userauth/messages/Success.hh>
#include <xju/ssh/userauth/AlgorithmName.hh>

namespace xju
{
namespace ssh
{
namespace userauth
{
namespace authenticatees
{

namespace
{
class Receiver : public transport::ReceiveInterface
{
public:
  virtual transport::Message receiveMessage(size_t maxSize) override
  {
    return calls_.enqueue(*this,&transport::ReceiveInterface::receiveMessage,
                          maxSize)->awaitResult();
  }
  xju::test::Calls calls_;
};
class Sender : public transport::SingleThreadedSendInterface
{
public:
  virtual void sendMessage(
    transport::Message const& message,
    std::back_insert_iterator<std::vector<transport::Message> > receivedMessages) override
  {
    calls_.enqueue(*this,&transport::SingleThreadedSendInterface::sendMessage,
                   message,
                   receivedMessages)->awaitReturn();
  }
  xju::test::Calls calls_;
};

}


// good
void test1() {
  Receiver receiver;
  Sender sender;

  std::chrono::seconds leeway(1U);

  xju::UserName const userName("fred");
  xju::crypt::rsa::PrivateKey privateKey(
    xju::crypt::rsa::generateKey(xju::BitCount(3072)));
  auto const publicKey(privateKey.publicKey());
  xju::crypt::rsa::Signer signer(privateKey);
  xju::crypt::rsa::SigVerifier sigVerifier(publicKey);

  RSAPublicKeyAuthenticatee x(userName, publicKey, signer, 2048U);

  xju::ssh::misc::ServiceName const serviceName("service1");
  transport::SessionId const sessionId({1,2,3,4,5,6,7,8});
  
  auto c(
    xju::test::call(
      [&](){
        return x.authenticate(sessionId, sender, receiver, serviceName);
      }));
  
  {
    auto c(sender.calls_.awaitCall(
             sender,&transport::SingleThreadedSendInterface::sendMessage,
             xju::steadyNow()+leeway));
    auto r(
      transport::decodeMessage<messages::RSAPublicKeyRequest>(
        (uint8_t)MSG::REQUEST,
        c->p1_));
    xju::assert_equal(r.userName_,userName);
    xju::assert_equal(r.serviceName_,serviceName);
    xju::assert_equal(r.methodName_,MethodName("publickey"));
    xju::assert_equal(r.publicKey_,publicKey);
    xju::MemOBuf b(1024);
    {
      xju::net::ostream s(b);
      encode(s,sessionId);
      encode(s,(uint8_t)MSG::REQUEST);
      encode(s,userName);
      encode(s,serviceName);
      encode(s,std::string("publickey"));
      encode(s,true);
      encode(s,xju::ssh::userauth::AlgorithmName("ssh-rsa"));
      encode(s,publicKey);
    }
    sigVerifier.verifySignature(&*(b.data().first),
                                b.data().second-b.data().first,
                                r.signature_);
    auto i(c->p2_);
    *i++ = transport::encodeMessage(
      transport::messages::Ignore(std::vector<uint8_t>({5,6,7,8})));
    c->return_();
  }
  {
    auto c(receiver.calls_.awaitCall(
             receiver,&transport::ReceiveInterface::receiveMessage,
             xju::steadyNow()+leeway));
    xju::assert_equal(c->p1_,2048U);
    c->return_(transport::encodeMessage(
                 transport::messages::Ignore(std::vector<uint8_t>({1,2,3,4}))));
    c=receiver.calls_.awaitCall(
      receiver,&transport::ReceiveInterface::receiveMessage,
      xju::steadyNow()+leeway);
    xju::assert_equal(c->p1_,2048U);
    c->return_(transport::encodeMessage(messages::Success()));
  }
  std::vector<transport::Message> const messages(
    c->getResultBy(xju::steadyNow()+leeway));
  xju::assert_equal(
    messages,
    decltype(messages)({
        transport::encodeMessage(transport::messages::Ignore({5,6,7,8})),
        transport::encodeMessage(transport::messages::Ignore({1,2,3,4}))}));
  
  // sign fails
  // send message fails
  // receive fails
  // response is FAILURE, can't continue
  // response is FAILURE, can continue
  // ignore, debug + success
  // unexpected message transport::SERVICE_ACCEPT
}

}
}
}
}

using namespace xju::ssh::userauth::authenticatees;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

