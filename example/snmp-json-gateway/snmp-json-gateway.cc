// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/Utf8String.hh>
#include <xju/ip/UDPSocket.hh>
#include <xju/json/Object.hh>
#include <xju/json/Number.hh>
#include <iostream>
#include <xju/json/format.hh>
#include <xju/pipe.hh>
#include <vector>
#include <cinttypes>
#include <xju/io/select.hh>
#include <xju/steadyEternity.hh>
#include <xju/Exception.hh>
#include <xju/snmp/decodeSnmpV1GetRequest.hh>
#include <xju/snmp/decodeSnmpV2cGetRequest.hh>
#include <example/snmp-json-gateway/snmp_json_gateway.hh>
#include <xju/snmp/decodeSnmpV1GetNextRequest.hh>
#include <xju/snmp/decodeSnmpV1SetRequest.hh>
#include <xju/format.hh>
#include <thread>
#include <chrono>
#include <xju/readThrough.hh>
#include <xju/assert.hh>
#include <xju/json/parse.hh>
#include <xju/steadyNow.hh>
#include <xju/Thread.hh>
#include <utility>
#include <optional>
#include <xju/ip/Port.hh>
#include <xju/stringToInt.hh>
#include <sstream>
#include <xju/snmp/encode.hh>
#include <xju/snmp/decodeSnmpV2cGetRequest.hh>
#include <xju/snmp/decodeSnmpV2cGetBulkRequest.hh>
#include <xju/snmp/decodeSnmpV2cSetRequest.hh>
#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/decodeSnmpV3ScopedPDU.hh>
#include <xju/snmp/decodeSnmpV3Message.hh>
#include <xju/snmp/SnmpV3Message.hh>
#include <xju/snmp/decodeSnmpV3UsmSecurityParameters.hh>
#include <xju/UserName.hh>
#include <xju/now.hh>
#include <xju/snmp/SnmpV3UsmPrivData.hh>
#include <xju/snmp/PDU.hh>
#include <xju/snmp/Value.hh>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <xju/snmp/Counter32Value.hh>
#include <xju/snmp/showFirstBytes.hh>
#include <deque>
#include <xju/startsWith.hh>
#include <xju/split.hh>
#include <hcp/parser.hh>
#include <hcp/ast.hh>
#include <xju/stringToUInt.hh>
#include <xju/snmp/SnmpV3UsmMessage.hh>
#include <xju/snmp/encodeSnmpV3UsmMessage.hh>
#include <xju/snmp/decodeSnmpV3UsmMessage.hh>
#include <xju/snmp/SnmpV3UsmAuthKey.hh>
#include <xju/snmp/localiseSnmpV3UsmPassword.hh>
#include <xju/crypt/macs/hmacsha512.hh>
#include <xju/crypt/hashers/SHA512.hh>
#include <xju/crypt/MacCalculator.hh>
#include <xju/crypt/macs/hmacsha1.hh>
#include <xju/crypt/hashers/SHA1.hh>
#include <xju/snmp/hexStr.hh>
#include <xju/crypt/hashers/SHA256.hh>
#include <xju/crypt/macs/hmacsha256.hh>
#include <xju/crypt/macs/hmacsha384.hh>
#include <xju/crypt/hashers/SHA384.hh>
#include <xju/snmp/SnmpV3UsmEncrypter.hh>
#include <xju/snmp/SnmpV3UsmDecrypter.hh>
#include <xju/snmp/NoPrivSnmpV3UsmDecrypter.hh>
#include <xju/snmp/NoPrivSnmpV3UsmEncrypter.hh>
#include <xju/snmp/SnmpV3UsmPrivKey.hh>
#include <xju/snmp/AES128cfbSnmpV3UsmEncrypter.hh>
#include <xju/snmp/AES128cfbSnmpV3UsmDecrypter.hh>

auto const same_line = xju::Utf8String("");

xju::snmp::EngineTime engineTimeNow() {
  return xju::snmp::EngineTime(
    std::chrono::duration_cast<std::chrono::seconds>(xju::now().time_since_epoch()).count());
}

struct AuthAlgNameTag{};
typedef xju::Tagged<std::string,AuthAlgNameTag> AuthAlgName;

struct AuthPasswordTag{};
typedef xju::Tagged<std::string,AuthPasswordTag> AuthPassword;

struct PrivAlgNameTag{};
typedef xju::Tagged<std::string,PrivAlgNameTag> PrivAlgName;

struct PrivPasswordTag{};
typedef xju::Tagged<std::string,PrivPasswordTag> PrivPassword;

class SnmpV3UsmCodec
{
public:
  virtual ~SnmpV3UsmCodec(){}
  virtual xju::snmp::ContextEngineID engineId() = 0;
  virtual xju::snmp::SnmpV3UsmPrivKey localiseSnmpV3UsmPrivPassword(
    PrivPassword const& privPassword,
    size_t const privKeySize) const=0;
  virtual xju::snmp::SnmpV3UsmMessage decodeSnmpV3UsmMessage(
    std::vector<uint8_t> const& data,
    xju::snmp::SnmpV3UsmDecrypter& decrypter) = 0;
  virtual std::vector<uint8_t> encodeSnmpV3UsmMessage(
    xju::snmp::SnmpV3UsmMessage x,
    xju::snmp::SnmpV3UsmEncrypter& encrypter) = 0;
  virtual xju::snmp::SnmpV3Message::Flags authFlag() noexcept = 0;
};

class NoSnmpV3UsmCodecImpl:public SnmpV3UsmCodec
{
public:
  virtual xju::snmp::ContextEngineID engineId() {
    throw xju::Exception("snmp-json-gateway not configured for snmp v3",XJU_TRACED);
  }
  virtual xju::snmp::SnmpV3UsmPrivKey localiseSnmpV3UsmPrivPassword(
    PrivPassword const& privPassword,
    size_t const privKeySize) const override
  {
    try{
      throw xju::Exception("snmp-json-gateway not configured for snmp v3",XJU_TRACED);
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "localise " << privPassword._.size() << " snmp v3 usm priv password";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }

  virtual xju::snmp::SnmpV3UsmMessage decodeSnmpV3UsmMessage(
    std::vector<uint8_t> const& data,
    xju::snmp::SnmpV3UsmDecrypter& decrypter)
  {
    throw xju::Exception("snmp-json-gateway not configured for snmp v3",XJU_TRACED);
  }
      
  virtual std::vector<uint8_t> encodeSnmpV3UsmMessage(
    xju::snmp::SnmpV3UsmMessage x,
    xju::snmp::SnmpV3UsmEncrypter& encrypter)
  {
    throw xju::Exception("snmp-json-gateway not configured for snmp v3",XJU_TRACED);
  }
  virtual xju::snmp::SnmpV3Message::Flags authFlag() noexcept
  {
    return xju::snmp::SnmpV3Message::Flags::NO_AUTH;
  }
  
};


template<class MacCalculator, unsigned int TruncateMacTo, class Hasher>
class SnmpV3UsmCodecImpl:public SnmpV3UsmCodec
{
public:
  explicit SnmpV3UsmCodecImpl(xju::snmp::ContextEngineID const& engineId,
                              AuthPassword const& authPassword) noexcept:
      engineId_(engineId),
      authKey_(xju::snmp::localiseSnmpV3UsmPassword<Hasher>(engineId, xju::Password(authPassword._)))
  {
  }

  virtual xju::snmp::ContextEngineID engineId() {
    return engineId_;
  }
  virtual xju::snmp::SnmpV3UsmPrivKey localiseSnmpV3UsmPrivPassword(
    PrivPassword const& privPassword,
    size_t const privKeySize) const override
  {
    try{
      std::vector<uint8_t> k(
        xju::snmp::localiseSnmpV3UsmPassword<Hasher>(engineId_, xju::Password(privPassword._)));
      if (k.size()<privKeySize){
        std::ostringstream s;
        s << "localisation of priv key via "
          << Hasher().name() << " produced only " << k.size() << "-byte key, expected "
          << "at least " << privKeySize << " bytes";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      k.resize(privKeySize);
      return xju::snmp::SnmpV3UsmPrivKey(k);
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "localise " << privPassword._.size() << " snmp v3 usm priv password";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  
  virtual xju::snmp::SnmpV3UsmMessage decodeSnmpV3UsmMessage(
    std::vector<uint8_t> const& data,
    xju::snmp::SnmpV3UsmDecrypter& decrypter)
  {
    try{
      auto const result(xju::snmp::decodeSnmpV3UsmMessage<MacCalculator, TruncateMacTo>(
                          data, authKey_, decrypter));
      if(result.flags_ && xju::snmp::SnmpV3Message::Flags::AUTH !=
         xju::snmp::SnmpV3Message::Flags::AUTH
      ){
        std::ostringstream s;
        s << "decoded message " << result << " did not specify auth flag";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      return result;
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "decode snmp v3 USM message using "
        << MacCalculator(xju::crypt::MacKey(authKey_._)).name()
        << " MAC algorithm truncating output to " << TruncateMacTo << " bytes and "
        << authKey_._.size()
        << "-byte key from data " << xju::snmp::hexStr(data);
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
      
  virtual std::vector<uint8_t> encodeSnmpV3UsmMessage(
    xju::snmp::SnmpV3UsmMessage x,
    xju::snmp::SnmpV3UsmEncrypter& encrypter)
  {
    xju::assert_equal(x.flags_ && xju::snmp::SnmpV3Message::Flags::AUTH,
                      xju::snmp::SnmpV3Message::Flags::AUTH);
    return xju::snmp::encodeSnmpV3UsmMessage<MacCalculator, TruncateMacTo>(x, authKey_, encrypter);
  }
  virtual xju::snmp::SnmpV3Message::Flags authFlag() noexcept
  {
    return xju::snmp::SnmpV3Message::Flags::AUTH;
  }

private:
  xju::snmp::ContextEngineID engineId_;
  xju::snmp::SnmpV3UsmAuthKey authKey_;  
};

class NoAuthSnmpV3UsmCodecImpl:public SnmpV3UsmCodec
{
public:
  explicit NoAuthSnmpV3UsmCodecImpl(
    xju::snmp::ContextEngineID const& engineId):
      engineId_(engineId)
  {
  }
  virtual xju::snmp::ContextEngineID engineId() {
    return engineId_;
  }

  virtual xju::snmp::SnmpV3UsmPrivKey localiseSnmpV3UsmPrivPassword(
    PrivPassword const&,
    size_t) const override
  {
    throw xju::Exception(
      "snmp-json-gateway not configured for snmp v3 usm message authentication",XJU_TRACED);
  }
  
  virtual xju::snmp::SnmpV3UsmMessage decodeSnmpV3UsmMessage(
    std::vector<uint8_t> const& data,
    xju::snmp::SnmpV3UsmDecrypter& decrypter)
  {
    try{
      return xju::snmp::decodeSnmpV3UsmMessage<xju::snmp::NoAuthMacCalculator, 0U>(
        data, xju::snmp::SnmpV3UsmAuthKey({}), decrypter);
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "decode but do not authenticate snmp v3 USM message from data " << xju::snmp::hexStr(data);
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
      
  virtual std::vector<uint8_t> encodeSnmpV3UsmMessage(
    xju::snmp::SnmpV3UsmMessage x,
    xju::snmp::SnmpV3UsmEncrypter& encrypter)
  {
    return xju::snmp::encodeSnmpV3UsmMessage<xju::snmp::NoAuthMacCalculator, 0U>(
      x, xju::snmp::SnmpV3UsmAuthKey({}), encrypter);
  }
  virtual xju::snmp::SnmpV3Message::Flags authFlag() noexcept
  {
    return xju::snmp::SnmpV3Message::Flags::NO_AUTH;
  }

private:
  xju::snmp::ContextEngineID engineId_;
};


// MACs truncated to lengths per rfc3414, rfc7860
typedef SnmpV3UsmCodecImpl<
  xju::crypt::macs::hmacsha1::Calculator, 12U, xju::crypt::hashers::SHA1> Sha96SnmpV3UsmCodecImpl;
typedef SnmpV3UsmCodecImpl<
  xju::crypt::macs::hmacsha256::Calculator, 24U, xju::crypt::hashers::SHA256> Sha256SnmpV3UsmCodecImpl;
typedef SnmpV3UsmCodecImpl<
  xju::crypt::macs::hmacsha384::Calculator, 32U, xju::crypt::hashers::SHA384> Sha384SnmpV3UsmCodecImpl;
typedef SnmpV3UsmCodecImpl<
  xju::crypt::macs::hmacsha512::Calculator, 48U, xju::crypt::hashers::SHA512> Sha512SnmpV3UsmCodecImpl;


class SnmpV3UsmPriv
{
public:
  virtual ~SnmpV3UsmPriv() {}

  virtual xju::snmp::SnmpV3UsmEncrypter& encrypter() noexcept = 0;
  virtual xju::snmp::SnmpV3UsmDecrypter& decrypter() noexcept = 0;
};

class NoSnmpV3UsmPriv: public SnmpV3UsmPriv
{
public:
  xju::snmp::NoPrivSnmpV3UsmEncrypter encrypter_;
  xju::snmp::NoPrivSnmpV3UsmDecrypter decrypter_;

  virtual xju::snmp::SnmpV3UsmEncrypter& encrypter() noexcept override { return encrypter_; }
  virtual xju::snmp::SnmpV3UsmDecrypter& decrypter() noexcept override { return decrypter_; }
};

class AES128cfbPriv: public SnmpV3UsmPriv
{
public:
  xju::snmp::SnmpV3UsmPrivKey privKey_;
  xju::snmp::AES128cfbSnmpV3UsmEncrypter encrypter_;
  xju::snmp::AES128cfbSnmpV3UsmDecrypter decrypter_;
  
  AES128cfbPriv(SnmpV3UsmCodec& auth,
                PrivPassword const& privPassword):
      privKey_(auth.localiseSnmpV3UsmPrivPassword(privPassword, 16U)),
      encrypter_(privKey_),
      decrypter_(privKey_)
  {
  }
  virtual xju::snmp::SnmpV3UsmEncrypter& encrypter() noexcept override { return encrypter_; }
  virtual xju::snmp::SnmpV3UsmDecrypter& decrypter() noexcept override { return decrypter_; }
};

void run(xju::ip::UDPSocket& socket,
         SnmpV3UsmCodec& snmpV3UsmCodec,
         xju::snmp::SnmpV3UsmEncrypter& encrypter,
         xju::snmp::SnmpV3UsmDecrypter& decrypter
)
{
  xju::json::Object portMessage({
      {xju::Utf8String("listening_on"),std::make_unique<xju::json::Number>(
          xju::format::str(socket.port().value()))}
    });
  std::cout << xju::json::format(portMessage, same_line) << std::endl;

  auto stop_receiver = xju::pipe(true,true);

  uint64_t usmStatsUnknownEngineIDs=0;
  uint64_t usmStatsNotInTimeWindows=0;
  uint64_t const ourMaxSize(64000);
  auto from_snmp_to_stdout =
    [&](){
      try{
        std::vector<uint8_t> buffer(UINT16_MAX);
        while(true){
          buffer.resize(UINT16_MAX);
          if (xju::io::select({stop_receiver.first.get(),&socket},{},xju::steadyEternity())
              .first.contains(stop_receiver.first.get())){
            return;
          }
          auto const senderAndSize(socket.receive(buffer.data(), UINT16_MAX, xju::steadyNow()));
          buffer.resize(senderAndSize.second);
          std::vector<xju::Exception> failures;
          try{
            auto const x(xju::snmp::decodeSnmpV1GetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1GetNextRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV1SetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetNextRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cGetBulkRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            auto const x(xju::snmp::decodeSnmpV2cSetRequest(buffer));
            std::cout << xju::json::format(*snmp_json_gateway::encode(senderAndSize.first,x),same_line)
                      << std::endl;
            continue;
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            // engineid discovery (rfc3414 4. Discovery)
            auto const x(xju::snmp::decodeSnmpV3Message(buffer));
            try {
              if (x.first.securityModel_ != xju::snmp::SnmpV3Message::SecurityModel(3)) {
                std::ostringstream s;
                s << "snmp v3 security model " << x.first.securityModel_ << " is not 3(USM)";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              if ((x.first.flags_ & xju::snmp::SnmpV3Message::AUTH) == xju::snmp::SnmpV3Message::AUTH){
                std::ostringstream s;
                s << "snmp v3 auth " << x.first.securityModel_
                  << " requested - should not be for engine id discovery";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              if ((x.first.flags_ & xju::snmp::SnmpV3Message::REPORTABLE) != xju::snmp::SnmpV3Message::REPORTABLE){
                std::ostringstream s;
                s << "reportable flag is not set";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              auto const sec(xju::snmp::decodeSnmpV3UsmSecurityParameters(x.first.securityParameters_));
              if (std::get<0>(sec).engineID_!=xju::snmp::ContextEngineID({})){
                std::ostringstream s;
                s << "request engine id (0x"
                  << xju::format::join(std::get<0>(sec).engineID_._.begin(),
                                       std::get<0>(sec).engineID_._.end(),
                                       xju::format::Hex(""),
                                       "")
                  << ") is not empty";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              if (std::get<0>(sec).userName_!=xju::UserName("")){
                std::ostringstream s;
                s << "request user name (" << std::get<0>(sec).userName_ << ") is not empty";
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              auto const scopedPDU(xju::snmp::decodeSnmpV3ScopedPDU(x.first.scopedPduData_));
              auto const m(
                xju::snmp::encode(
                  xju::snmp::SnmpV3Message(
                    x.first.id_,
                    ourMaxSize,
                    xju::snmp::SnmpV3Message::Flags(0),
                    xju::snmp::SnmpV3Message::SecurityModel(3),
                    xju::snmp::encode(
                      xju::snmp::SnmpV3UsmSecurityParameters(snmpV3UsmCodec.engineId(),
                                                             xju::snmp::EngineBoots(1),
                                                             engineTimeNow(),
                                                             xju::UserName("")),
                      xju::crypt::Mac({}),
                      xju::snmp::SnmpV3UsmPrivData({})),
                    xju::snmp::encode(
                      xju::snmp::SnmpV3ScopedPDU(
                        snmpV3UsmCodec.engineId(),
                        xju::snmp::ContextName({}),
                        xju::snmp::PDU(
                          scopedPDU.pdu_.requestId_,
                          0,
                          0,
                          {xju::snmp::SnmpVar(xju::snmp::Oid(".1.3.6.1.6.3.15.1.1.4"),
                                              std::shared_ptr<xju::snmp::Value const>(
                                                new xju::snmp::Counter32Value(
                                                  usmStatsUnknownEngineIDs)))},
                          0xa8))))));
              auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
              socket.sendTo(senderAndSize.first.first,senderAndSize.first.second,
                            m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              std::ostringstream s;
              s << "proces snmp v3 message " << x.first
                << " as snmp v3 engine id discovery request";
              e.addContext(s.str(),XJU_TRACED);
              failures.push_back(e);
            }
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          try{
            // snmp v3 USM time sync request
            xju::snmp::NoPrivSnmpV3UsmDecrypter decrypter;
            xju::snmp::SnmpV3UsmMessage x(snmpV3UsmCodec.decodeSnmpV3UsmMessage(buffer, decrypter));
            if (x.securityParameters_.engineID_!=snmpV3UsmCodec.engineId()){
              ++usmStatsUnknownEngineIDs;
              std::ostringstream s;
              s << "expected engine id (hex) "
                << xju::snmp::showFirstBytes(512,snmpV3UsmCodec.engineId()._)
                << ", got " << xju::snmp::showFirstBytes(512,x.securityParameters_.engineID_._);
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            if (x.securityParameters_.engineBoots_ != xju::snmp::EngineBoots(0)){
              std::ostringstream s;
              s << "request contains non-zero engine boots ("
                << x.securityParameters_.engineBoots_ << ")";
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            if (x.securityParameters_.engineTime_ != xju::snmp::EngineTime(0)){
              std::ostringstream s;
              s << "request contains non-zero engine time ("
                << x.securityParameters_.engineTime_ << ")";
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            ++usmStatsNotInTimeWindows;
            xju::snmp::NoPrivSnmpV3UsmEncrypter encrypter;
            
            auto const m(snmpV3UsmCodec.encodeSnmpV3UsmMessage(
                           xju::snmp::SnmpV3UsmMessage(
                             x.id_,
                             ourMaxSize,
                             snmpV3UsmCodec.authFlag(),
                             xju::snmp::SnmpV3UsmSecurityParameters(snmpV3UsmCodec.engineId(),
                                                                    xju::snmp::EngineBoots(1),
                                                                    engineTimeNow(),
                                                                    x.securityParameters_.userName_),
                             xju::snmp::SnmpV3ScopedPDU(
                               snmpV3UsmCodec.engineId(),
                               xju::snmp::ContextName({}),
                               xju::snmp::PDU(
                                 x.scopedPDU_.pdu_.requestId_,
                                 0,
                                 0,
                                 {xju::snmp::SnmpVar(xju::snmp::Oid(".1.3.6.1.6.3.15.1.1.2"),
                                                     std::shared_ptr<xju::snmp::Value const>(
                                                       new xju::snmp::Counter32Value(
                                                         usmStatsNotInTimeWindows)))},
                                 0xa8))),
                           encrypter));
              auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
              socket.sendTo(senderAndSize.first.first,senderAndSize.first.second,
                            m.data(),m.size(),deadline);
            continue;
          }
          catch(xju::Exception& e){
            e.addContext(
              "process snmp v3 USM (rfc3414) message assuming it is a Time Synchronization request",
              XJU_TRACED);
            failures.push_back(e);
          }
          try{
            // snmp v3 USM message
            xju::snmp::SnmpV3UsmMessage x(snmpV3UsmCodec.decodeSnmpV3UsmMessage(buffer, decrypter));
            if (x.securityParameters_.engineID_!=snmpV3UsmCodec.engineId()){
              ++usmStatsUnknownEngineIDs;
              std::ostringstream s;
              s << "expected engine id (hex) "
                << xju::snmp::showFirstBytes(512,snmpV3UsmCodec.engineId()._)
                << ", got " << xju::snmp::showFirstBytes(512,x.securityParameters_.engineID_._);
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            else {
              std::cout << xju::json::format(*snmp_json_gateway::encode(
                                               senderAndSize.first,
                                               std::make_tuple(
                                                 x.id_,
                                                 x.maxSize_,
                                                 x.securityParameters_.userName_,
                                                 x.scopedPDU_)),same_line)
                        << std::endl;
              continue;
            }
          }
          catch(xju::Exception& e){
            e.addContext("process snmp v3 USM message",XJU_TRACED);
            failures.push_back(e);
          }
          std::cerr << "dropped received udp packet because "
                    << xju::format::join(failures.begin(), failures.end(),
                                         [](auto e){
                                           return xju::readableRepr(e,false,true);
                                         }, " and ") << std::endl;
        };
      }
      catch(xju::Exception& e){
        std::cerr << "will restart receive in 3s because " << xju::readableRepr(e) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
      }
    };
  auto from_stdin_to_snmp =
    [&](){
      try{
        while(true){
          auto const s(xju::readThrough(std::cin, "\n", 10*UINT16_MAX));
          xju::assert_greater_equal(s.size(),1U);
          std::vector<xju::Exception> failures;
          try{
            auto const gm(
              xju::json::parse(xju::Utf8String(std::string(s.begin(), s.end()-1))));
            auto const remoteEndpoint = snmp_json_gateway::decodeEndpoint(
              gm->getMember(xju::Utf8String("remote_endpoint")));
            auto const deadline(xju::steadyNow()+std::chrono::seconds(5));
            try{
              auto const m(xju::snmp::encode(snmp_json_gateway::decodeSnmpV1Response(
                                               gm->getMember(xju::Utf8String("message")))));
              socket.sendTo(remoteEndpoint.first,remoteEndpoint.second,m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              failures.push_back(e);
            }
            try{
              auto const m(xju::snmp::encode(snmp_json_gateway::decodeSnmpV2cResponse(
                                               gm->getMember(xju::Utf8String("message")))));
              socket.sendTo(remoteEndpoint.first,remoteEndpoint.second,m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              failures.push_back(e);
            }
            try{
              auto const x(snmp_json_gateway::decodeSnmpV3Response(
                             gm->getMember(xju::Utf8String("message"))));
              xju::snmp::SnmpV3Message::ID const messageId(std::get<0>(x));
              uint32_t const max_size(std::get<1>(x));
              xju::UserName const userName(std::get<2>(x));
              xju::snmp::SnmpV3ScopedPDU const scopedPDU(std::get<3>(x));
              
              auto const m(snmpV3UsmCodec.encodeSnmpV3UsmMessage(
                             xju::snmp::SnmpV3UsmMessage(
                               messageId,
                               max_size,
                               snmpV3UsmCodec.authFlag(),
                               xju::snmp::SnmpV3UsmSecurityParameters(snmpV3UsmCodec.engineId(),
                                                                      xju::snmp::EngineBoots(1),
                                                                      engineTimeNow(),
                                                                      userName),
                               scopedPDU),
                             encrypter));
              socket.sendTo(remoteEndpoint.first,remoteEndpoint.second,m.data(),m.size(),deadline);
              continue;
            }
            catch(xju::Exception& e){
              failures.push_back(e);
            }
          }
          catch(xju::Exception& e){
            failures.push_back(e);
          }
          std::cerr << "ignoring " << xju::format::cEscapeString(s) << " because "
                    << xju::format::join(failures.begin(), failures.end(),
                                         [](auto e){ return xju::readableRepr(e,false,true);},
                                         " and ")
                    << std::endl;
        }
      }
      catch(xju::Exception& e){
        std::cerr << "stopping because " << xju::readableRepr(e) << std::endl;
      }
    };
  xju::Thread a(from_snmp_to_stdout);
  auto const stopper(std::move(stop_receiver.second));
  xju::Thread b(from_stdin_to_snmp);
}

std::optional<xju::ip::Port> parsePort(std::string const& x){
  try{
    std::optional<xju::ip::Port> result;
    if (x != "auto"){
      auto const p(xju::stringToInt(x));
      if (p<1 || p>UINT16_MAX){
        std::ostringstream s;
        s << "port number is not 1.." << UINT16_MAX;
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      result=xju::ip::Port(p);
    }
    return result;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse " << xju::format::quote(x) << " as an IP port number or \"auto\"";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

struct Options
{
  std::optional<xju::snmp::ContextEngineID> v3usmEngineId_;
  std::optional<std::pair<AuthAlgName,AuthPassword>> v3usmAuth_;
  std::optional<std::pair<PrivAlgName,PrivPassword>> v3usmPriv_;
};
template<class Alg, class Pass>
std::pair<Alg, Pass> parseV3UsmAlgAndPassword(std::string const& x)
{
  try{
    auto const v(xju::split(x, ':'));
    if (v.size()==1){
      throw xju::Exception("missing ':'",XJU_TRACED);
    }
    return {Alg(v[0]), Pass(xju::format::join(v.begin()+1, v.end(), std::string(":")))};
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse " << xju::format::quote(x) << " like alg:password";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

xju::snmp::ContextEngineID parseEngineId(std::string const& x)
{
  try{
    struct HexDigitPairTag{};
    typedef hcp_ast::TaggedItem<HexDigitPairTag> HexDigitPair;
    auto const ast(
      hcp_parser::parseString(x.begin(), x.end(),
                              hcp_parser::zeroOrMore() * hcp_parser::named<HexDigitPair>(
                                "hex digit pair",
                                hcp_parser::hexDigit()+hcp_parser::hexDigit())));
    std::vector<uint8_t> result;
    for(auto const hexDigitPair: hcp_ast::findChildrenOfType<HexDigitPair>(ast)){
      result.push_back(xju::stringToUInt(reconstruct(hexDigitPair.get()), 16));
    }
    return xju::snmp::ContextEngineID(result);
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse " << xju::format::quote(x) << " assuming it is a hex-string like \"AA75F2\'";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}
// parse options from start of args (argv[1..])
// - returns remaining args
std::pair<Options, std::vector<std::string> > parseOptions(std::vector<std::string> const& args)
{
  try{
    Options options;
    std::deque<std::string> remaining(args.begin(), args.end());
    auto const next = [&](std::string const& context){
      if (!remaining.size())
      {
        xju::Exception e("ran out of arguments",XJU_TRACED);
        e.addContext(context, XJU_TRACED);
        throw e;
      }
      auto const result(remaining.front());
      remaining.pop_front();
      return result;
    };
    while(remaining.size() && xju::startsWith(remaining.front(), std::string("--")))
    {
      auto const a(next("get option"));
      if (a=="--v3usm"){
        options.v3usmEngineId_=parseEngineId(next("get --v3usm option value"));
      }
      else if (a=="--auth"){
        options.v3usmAuth_=parseV3UsmAlgAndPassword<AuthAlgName,AuthPassword>(
          next("parse --auth option value"));
      }
      else if (a=="--priv"){
        options.v3usmPriv_=parseV3UsmAlgAndPassword<PrivAlgName,PrivPassword>(
          next("parse --priv option value"));
      }
      else {
        throw xju::Exception("unnkown option " + xju::format::quote(a), XJU_TRACED);
      }
    }
    return std::make_pair(options, std::vector<std::string>(remaining.begin(), remaining.end()));
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "parse options from start of " << xju::format::join(args.begin(), args.end(), std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  if (argc == 2 && std::string(argv[1])=="--help"){
    std::cerr
      << "usage: " << argv[0]
      << R"--( [--v3usm engineid [ --auth authalg:auth-password [ --priv privalg:priv-password ]]] port | "auto"
  - with specified udp port (or choose one):
    - write received snmp messages to stdout in json format
    - send stdin json format messages as snmp messages from specified port
  - see snmp_json_gateway.py GatewayMessage for message format
  - note writes { \"listening_on\": port } line to stdout once port is open
  - snmp v3 USM support:
     engineid; hex string e.g. AA83BF
     authalg: "sha256" "sha384" "sha512" "sha96"
     auth-password: password verbatim (not hext string) e.g. fred
     privalg: "aes128cfb"
     priv-password; password verbatim (not hext string) e.g. jock
)--";
    return 1;
  }
  try{
    auto const r(parseOptions(std::vector<std::string>(argv+1, argv+argc)));
    Options const options(r.first);
    std::deque<std::string> remaining(r.second.begin(), r.second.end());
    auto const next = [&](std::string const& context){
      if (!remaining.size())
      {
        xju::Exception e("ran out of arguments",XJU_TRACED);
        e.addContext(context, XJU_TRACED);
        throw e;
      }
      auto const result(remaining.front());
      remaining.pop_front();
      return result;
    };

    std::unique_ptr<SnmpV3UsmCodec> snmpV3UsmCodec(new NoSnmpV3UsmCodecImpl);
    std::unique_ptr<SnmpV3UsmPriv> snmpV3UsmPriv(new NoSnmpV3UsmPriv);
    if (options.v3usmEngineId_.has_value()){
      snmpV3UsmCodec = std::make_unique<NoAuthSnmpV3UsmCodecImpl>(options.v3usmEngineId_.value());
      if (options.v3usmAuth_.has_value()){
        auto const authAlgName(options.v3usmAuth_.value().first);
        auto const authPassword(options.v3usmAuth_.value().second);
        if (authAlgName == AuthAlgName("sha256")){
          snmpV3UsmCodec=std::make_unique<Sha256SnmpV3UsmCodecImpl>(
            options.v3usmEngineId_.value(), authPassword);
        }
        else if (authAlgName == AuthAlgName("sha384")){
          snmpV3UsmCodec=std::make_unique<Sha384SnmpV3UsmCodecImpl>(
            options.v3usmEngineId_.value(), authPassword);
        }
        else if (authAlgName == AuthAlgName("sha512")){
          snmpV3UsmCodec=std::make_unique<Sha512SnmpV3UsmCodecImpl>(
            options.v3usmEngineId_.value(), authPassword);
        }
        else if (authAlgName == AuthAlgName("sha96")){
          snmpV3UsmCodec=std::make_unique<Sha96SnmpV3UsmCodecImpl>(
            options.v3usmEngineId_.value(), authPassword);
        }
        else{
          std::ostringstream s;
          s << "unknown auth alogirithm " << xju::format::quote(xju::format::str(authAlgName))
            << " (only know \"sha256\" \"sha384\" \"sha512\" \"sha96\")";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        if (options.v3usmPriv_.has_value()){
          auto const privAlgName(options.v3usmPriv_.value().first);
          auto const privPassword(options.v3usmPriv_.value().second);
          if (privAlgName == PrivAlgName("aes128cfb")){
            snmpV3UsmPriv=std::make_unique<AES128cfbPriv>(
              *snmpV3UsmCodec, privPassword);
          }
          else{
            std::ostringstream s;
            s << "unknown priv alogirithm " << xju::format::quote(xju::format::str(privAlgName))
              << " (only know \"aes128cfb\")";
            throw xju::Exception(s.str(),XJU_TRACED);
          }
        }
        else if (options.v3usmPriv_.has_value()){
          std::ostringstream s;
          s << "priv alogirithm "
            << xju::format::quote(xju::format::str(options.v3usmPriv_.value().first))
            << " requested but snmpv3 USM privacy is not yet implemented";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
      }
      else if (options.v3usmPriv_.has_value()){
        std::ostringstream s;
        s << "priv alogirithm "
          << xju::format::quote(xju::format::str(options.v3usmPriv_.value().first))
          << " requested no auth algorithm specified";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
    }
    else {
      if (options.v3usmAuth_.has_value()){
        std::ostringstream s;
        s << "auth alogirithm "
          << xju::format::quote(xju::format::str(options.v3usmAuth_.value().first))
          << " requested no engine id specified";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (options.v3usmPriv_.has_value()){
        std::ostringstream s;
        s << "priv alogirithm "
          << xju::format::quote(xju::format::str(options.v3usmPriv_.value().first))
          << " requested no engine id specified";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
    }
    
    std::optional<xju::ip::Port> const port(parsePort(next("parse port arg")));
    if (port.has_value()){
      xju::ip::UDPSocket s(port.value());
      run(s, *snmpV3UsmCodec, snmpV3UsmPriv->encrypter(), snmpV3UsmPriv->decrypter());
      return 0;
    }
    else{
      xju::ip::UDPSocket s;
      run(s, *snmpV3UsmCodec, snmpV3UsmPriv->encrypter(), snmpV3UsmPriv->decrypter());
      return 0;
    }
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std:: endl;
    return 1;
  }
}
