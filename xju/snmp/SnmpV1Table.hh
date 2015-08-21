// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV1TABLE_H
#define XJU_SNMP_SNMPV1TABLE_H

#include <set>
#include "xju/snmp/Oid.hh"
#include <utility>
#include <memory>
#include "xju/snmp/Value.hh"
#include <vector>
#include <map>

namespace xju
{
namespace snmp
{

class SnmpV1Table
{
public:
  // pre: cols.size()>0
  // post: cols_==cols
  //       !atEnd()
  SnmpV1Table(std::set<Oid> cols) throw();
  
  std::set<Oid> const cols_;

  struct Cell
  {
    explicit Cell(std::pair<Oid, std::shared_ptr<Value const> > v) throw():
        oid_(v.first),
        value_(v.second) {
    }
    Oid oid_;
    std::shared_ptr<Value const> value_;
  };
  
  // pre: col in cols_
  std::vector<Cell> const& operator[](Oid const& col) const throw();
  
  bool atEnd() const throw() { return atEnd_; }

  // post: result.size()==cols_.size()
  std::vector<Oid> nextOids() const throw();
  
  // pre: row.size()==cols_.size()
  void add(
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > const& row)
  throw();

private:
  std::map<Oid, std::vector<Cell> > data_;
  bool atEnd_;
};


}
}

#endif

