// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <iostream>
#include <xju/assert.hh>
#include <xju/json/parse.hh>
#include <xju/Utf8String.hh>
#include <example/snmp-json-gateway/snmp_json_gateway.hh>
#include <xju/file/read.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/TimeTicksValue.hh>
#include <xju/snmp/Gauge32Value.hh>
#include <xju/snmp/Counter32Value.hh>
#include <xju/snmp/IPv4AddressValue.hh>
#include <xju/snmp/OpaqueValue.hh>

namespace snmp_json_gateway
{

void test1(std::string const& text) {
  auto const json(xju::json::parse(xju::Utf8String(text)));
  auto const a(json->asArray());
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[0]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(1));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::NO_ERROR);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(0));
    xju::assert_equal(x.values_.size(), 8U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::IntValue(7));
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[2].first,xju::snmp::Oid(".1.4.6.1.27.5"));
    xju::assert_equal(*x.values_[2].second, xju::snmp::StringValue("ann"));
    xju::assert_equal(x.values_[3].first,xju::snmp::Oid(".1.4.6.1.27.6"));
    xju::assert_equal(*x.values_[3].second, xju::snmp::TimeTicksValue(
                        std::chrono::milliseconds(7200)));
    xju::assert_equal(x.values_[4].first,xju::snmp::Oid(".1.4.6.1.27.7"));
    xju::assert_equal(*x.values_[4].second, xju::snmp::Gauge32Value(9987));
    xju::assert_equal(x.values_[5].first,xju::snmp::Oid(".1.4.6.1.27.8"));
    xju::assert_equal(*x.values_[5].second, xju::snmp::Counter32Value(19987));
    xju::assert_equal(x.values_[6].first,xju::snmp::Oid(".1.4.6.1.27.9"));
    xju::assert_equal(*x.values_[6].second, xju::snmp::IPv4AddressValue(
                        xju::ip::v4::Address("188.18.22.11")));
    xju::assert_equal(x.values_[7].first,xju::snmp::Oid(".1.4.6.1.27.10"));
    xju::assert_equal(*x.values_[7].second, xju::snmp::OpaqueValue({18,22,253}));
  }
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[1]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(2));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::TOO_BIG);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(0));
    xju::assert_equal(x.values_.size(), 2U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
  }
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[2]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(3));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::NO_SUCH_NAME);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(1));
    xju::assert_equal(x.values_.size(), 2U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
  }
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[3]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(4));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::BAD_VALUE);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(2));
    xju::assert_equal(x.values_.size(), 2U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
  }
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[4]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(5));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::READ_ONLY);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(2));
    xju::assert_equal(x.values_.size(), 2U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
  }
  {
    auto x(snmp_json_gateway::decodeSnmpV1Response(*a[5]));
    xju::assert_equal(x.responseType_, 3);
    xju::assert_equal(x.community_, xju::snmp::Community("fred"));
    xju::assert_equal(x.id_, xju::snmp::RequestId(6));
    xju::assert_equal(x.error_, xju::snmp::SnmpV1Response::ErrorStatus::GEN_ERR);
    xju::assert_equal(x.errorIndex_, xju::snmp::SnmpV1Response::ErrorIndex(2));
    xju::assert_equal(x.values_.size(), 2U);
    xju::assert_equal(x.values_[0].first,xju::snmp::Oid(".1.4.6.1.27.3"));
    xju::assert_equal(*x.values_[0].second, xju::snmp::NullValue());
    xju::assert_equal(x.values_[1].first,xju::snmp::Oid(".1.4.6.1.27.4"));
    xju::assert_equal(*x.values_[1].second, xju::snmp::NullValue());
  }
}

void test2(){
  {
    std::string const text(R"--({"message_type": "SnmpV2cResponse", "response_type": 3, "community": "fred", "request_id": 1, "error": null, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nmessage type is \"SnmpV2cResponse\" (at line 1 column 18) not \"SnmpV1GetResponse\".");
    }
  }
  {
    std::string const text(R"--({"message_type": "SnmpV1Response", "responsetype": 3, "community": "fred", "request_id": 1, "error": null, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nfailed to get value of object with 6 elements (at line 1 column 1)'s \"response_type\" member because\nobject with 6 elements (at line 1 column 1) has no \"response_type\" member.");
    }
  }
  {
    std::string const text(R"--({"message_type": "SnmpV1Response", "response_type": 18.5, "community": "fred", "request_id": 1, "error": null, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nfailed to get 18.5 (at line 1 column 53) as an int because\nfailed to convert \"18.5\" to an integer because\ncharacter 3 ('.') of \"18.5\" unexpected.");
    }
  }
  {
    std::string const text(R"--({"message_type": "SnmpV1Response", "response_type": 3, "communit": "fred", "request_id": 1, "error": null, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nfailed to get value of object with 6 elements (at line 1 column 1)'s \"community\" member because\nobject with 6 elements (at line 1 column 1) has no \"community\" member.");
    }
  }
  {
    std::string const text(R"--({"message_type": "SnmpV1Response", "response_type": 3, "community": "fred", "request_id": 1, "error": {"error_type": "ReadOnly", "error_index": 5}, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nfailed to decode SnmpV1GetResponse error status and error index from json object with 2 elements (at line 1 column 103) and oid values [\".1.4.6.1.27.3\",{\"value\":7,\"value_type\":\"IntValue\"}], [\".1.4.6.1.27.4\",{\"value_type\":\"NullValue\"}], [\".1.4.6.1.27.5\",{\"value\":\"ann\",\"value_type\":\"StrValue\"}], [\".1.4.6.1.27.6\",{\"value\":720,\"value_type\":\"TimeTicksValue\"}], [\".1.4.6.1.27.7\",{\"value\":9987,\"value_type\":\"GaugeValue\"}], [\".1.4.6.1.27.8\",{\"value\":19987,\"value_type\":\"CounterValue\"}], [\".1.4.6.1.27.9\",{\"value\":\"188.18.22.11\",\"value_type\":\"Ip4AddressValue\"}], [\".1.4.6.1.27.10\",{\"value\":[18,22,253],\"value_type\":\"OpaqueValue\"}] because\nfailed to get value of object with 2 elements (at line 1 column 103)'s \"oid\" member because\nobject with 2 elements (at line 1 column 103) has no \"oid\" member.");
    }
  }
  {
    std::string const text(R"--({"message_type": "SnmpV1Response", "response_type": 3, "community": "fred", "request_id": 1, "error": {"error_type": "ReadOnly", "oid": ".1.4.6.1.27.99"}, "values": [[".1.4.6.1.27.3", {"value_type": "IntValue", "value": 7}], [".1.4.6.1.27.4", {"value_type": "NullValue"}], [".1.4.6.1.27.5", {"value_type": "StrValue", "value": "ann"}], [".1.4.6.1.27.6", {"value_type": "TimeTicksValue", "value": 720}], [".1.4.6.1.27.7", {"value_type": "GaugeValue", "value": 9987}], [".1.4.6.1.27.8", {"value_type": "CounterValue", "value": 19987}], [".1.4.6.1.27.9", {"value_type": "Ip4AddressValue", "value": "188.18.22.11"}], [".1.4.6.1.27.10", {"value_type": "OpaqueValue", "value": [18, 22, 253]}]]})--");
    auto const json(xju::json::parse(xju::Utf8String(text)));
    try{
      auto x(snmp_json_gateway::decodeSnmpV1Response(*json));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e), "Failed to decode object with 6 elements (at line 1 column 1) as an SnmpV1Response because\nfailed to decode SnmpV1GetResponse error status and error index from json object with 2 elements (at line 1 column 103) and oid values [\".1.4.6.1.27.3\",{\"value\":7,\"value_type\":\"IntValue\"}], [\".1.4.6.1.27.4\",{\"value_type\":\"NullValue\"}], [\".1.4.6.1.27.5\",{\"value\":\"ann\",\"value_type\":\"StrValue\"}], [\".1.4.6.1.27.6\",{\"value\":720,\"value_type\":\"TimeTicksValue\"}], [\".1.4.6.1.27.7\",{\"value\":9987,\"value_type\":\"GaugeValue\"}], [\".1.4.6.1.27.8\",{\"value\":19987,\"value_type\":\"CounterValue\"}], [\".1.4.6.1.27.9\",{\"value\":\"188.18.22.11\",\"value_type\":\"Ip4AddressValue\"}], [\".1.4.6.1.27.10\",{\"value\":[18,22,253],\"value_type\":\"OpaqueValue\"}] because\nno such oid .1.4.6.1.27.99 in values.");
    }
  }
}

}

using namespace snmp_json_gateway;

int main(int argc, char* argv[])
{
  if (argc != 2){
    std::cerr << "usage: " << argv[0] << "output-from-test-encode-to-json.py" << std::endl;
    return 1;
  }
  unsigned int n(0);
  test1(xju::file::read(xju::path::split(argv[1]))), ++n;
  test2(); ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
