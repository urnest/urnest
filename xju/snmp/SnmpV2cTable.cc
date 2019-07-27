// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV2cTable.hh"
#include <algorithm>
#include <utility>
#include <vector>
#include "xju/assert.hh"

namespace xju
{
namespace snmp
{

SnmpV2cTable::SnmpV2cTable(std::set<Oid> cols) throw():
cols_(cols),
              atEnd_(false)
{
  std::transform(cols.begin(),cols.end(),
                 std::inserter(data_,data_.end()),
                 [](Oid const& x) throw() {
                   return std::make_pair(x, std::vector<SnmpV2cVarResponse>());
                 });
}

std::vector<SnmpV2cVarResponse> const& SnmpV2cTable::operator[](Oid const& col) const throw()
{
  auto const i(data_.find(col));
  xju::assert_not_equal(i,data_.end());
  return (*i).second;
}

std::vector<Oid> SnmpV2cTable::nextOids() const throw()
{
  if ((*data_.begin()).second.size()==0) {
    return std::vector<Oid>(cols_.begin(),cols_.end());
  }
  std::vector<Oid> result;
  for(auto x: data_) {
    result.push_back((*x.second.rbegin()).oid());
  }
  xju::assert_equal(result.size(),cols_.size());
  return result;
}

void SnmpV2cTable::add(
  std::vector<SnmpV2cVarResponse > const& row)
throw()
{
  xju::assert_equal(row.size(),cols_.size());
  auto i = cols_.begin();
  for(auto x: row) {
    if (!(*i++).contains(x.oid())) {
      atEnd_=true;
      return;
    }
    try{
      *x;
    }
    catch(SnmpV2cVarResponse::EndOfMibView const&){
      atEnd_=true;
      return;
    }
    catch(...){
    }
  }
  i = cols_.begin();
  for(auto x: row) {
    data_[*i++].push_back(x);
  }
}


}
}
