// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <example/snmp-json-gateway/snmp_json_gateway.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/json/format.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>
#include <xju/snmp/SnmpV2cGetNextRequest.hh>
#include <xju/snmp/SnmpV2cGetRequest.hh>
#include <xju/snmp/SnmpV2cSetRequest.hh>

namespace snmp_json_gateway
{

void test1() {
  auto const x(xju::json::Array({
        snmp_json_gateway::encode(
          std::make_pair(
            xju::snmp::SnmpV1GetRequest(
              xju::snmp::Community("fred"),
              xju::snmp::RequestId(27),
              {xju::snmp::Oid(".1.4.6.1.27.3"),
               xju::snmp::Oid(".1.4.6.1.19")}),
            std::vector<xju::snmp::Oid>(
              {xju::snmp::Oid(".1.4.6.1.27.3"),
               xju::snmp::Oid(".1.4.6.1.19")}))),
        snmp_json_gateway::encode(
          xju::snmp::SnmpV1GetNextRequest(
            xju::snmp::Community("ann"),
            xju::snmp::RequestId(2),
            {xju::snmp::Oid(".1.4.6.12.27.3"),
             xju::snmp::Oid(".1.4.6.12.19")})),
        snmp_json_gateway::encode(
          std::make_pair(
            xju::snmp::SnmpV1SetRequest(
              xju::snmp::Community("ann"),
              xju::snmp::RequestId(2),
              { {xju::snmp::Oid(".1.4.6.12.27.3"),
                 std::shared_ptr<xju::snmp::Value const>(new xju::snmp::IntValue(3))},
                {xju::snmp::Oid(".1.4.6.12.19"),
                 std::shared_ptr<xju::snmp::Value const>(new xju::snmp::StringValue("fred"))}
              }),
            std::vector<xju::snmp::Oid>{xju::snmp::Oid(".1.4.6.12.27.3"),
                xju::snmp::Oid(".1.4.6.12.19")})),

        snmp_json_gateway::encode(
          std::make_pair(
            xju::snmp::SnmpV2cGetRequest(
              xju::snmp::Community("fred"),
              xju::snmp::RequestId(27),
              {xju::snmp::Oid(".1.4.6.1.27.3"),
               xju::snmp::Oid(".1.4.6.1.19")}),
            std::vector<xju::snmp::Oid>(
              {xju::snmp::Oid(".1.4.6.1.27.3"),
               xju::snmp::Oid(".1.4.6.1.19")}))),
        snmp_json_gateway::encode(
          xju::snmp::SnmpV2cGetNextRequest(
            xju::snmp::Community("ann"),
            xju::snmp::RequestId(2),
            {xju::snmp::Oid(".1.4.6.12.27.3"),
             xju::snmp::Oid(".1.4.6.12.19")})),
        snmp_json_gateway::encode(
          xju::snmp::SnmpV2cGetBulkRequest(
            xju::snmp::Community("ann"),
            xju::snmp::RequestId(2),
            {xju::snmp::Oid(".1.4.6.12.27.3"),
             xju::snmp::Oid(".1.4.6.12.19")},
            {10, {xju::snmp::Oid(".1.4.6.12.27.5")}})),
        snmp_json_gateway::encode(
          std::make_pair(
            xju::snmp::SnmpV2cSetRequest(
              xju::snmp::Community("ann"),
              xju::snmp::RequestId(2),
              { {xju::snmp::Oid(".1.4.6.12.27.3"),
                 std::shared_ptr<xju::snmp::Value const>(new xju::snmp::IntValue(3))},
                {xju::snmp::Oid(".1.4.6.12.19"),
                 std::shared_ptr<xju::snmp::Value const>(new xju::snmp::StringValue("fred"))}
              }),
            std::vector<xju::snmp::Oid>{xju::snmp::Oid(".1.4.6.12.27.3"),
                xju::snmp::Oid(".1.4.6.12.19")}))
      }));
  std::cout << xju::json::format(x, xju::Utf8String("")) << "\n";
}

}

using namespace snmp_json_gateway;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  return 0;
}
