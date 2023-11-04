// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2CTABLE_H
#define XJU_SNMP_SNMPV2CTABLE_H

#include <set>
#include <xju/snmp/Oid.hh>
#include <utility>
#include <memory>
#include <xju/snmp/Value.hh>
#include <vector>
#include <map>
#include <xju/snmp/SnmpVar.hh>

namespace xju
{
namespace snmp
{

class SnmpV2cTable
{
public:
  // pre: cols.size()>0
  // post: cols_==cols
  //       !atEnd()
  SnmpV2cTable(std::set<Oid> cols) throw();
  
  std::set<Oid> const cols_;

  // pre: col in cols_
  std::vector<SnmpVar> const& operator[](
    Oid const& col) const throw();
  
  bool atEnd() const throw() { return atEnd_; }

  // post: result.size()==cols_.size()
  std::vector<Oid> nextOids() const throw();
  
  // pre: row.size()==cols_.size()
  void add(std::vector<SnmpVar > const& row) throw();

private:
  std::map<Oid, std::vector<SnmpVar> > data_;
  bool atEnd_;
};


}
}

#endif
