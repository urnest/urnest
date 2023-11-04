// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/SnmpV2cTable.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/NullValue.hh>

namespace xju
{
namespace snmp
{

void test1() throw()
{
  // SnmpV2cTable
  Oid const col1(".1.3.5.20");
  Oid const col2(".1.3.5.21");
  SnmpV2cTable t({col1,col2});
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
  xju::assert_equal(t.nextOids(),std::vector<Oid>({col1,col2}));
  typedef SnmpVar V;
  t.add(
    {V(Oid(".1.3.5.20.1"),std::shared_ptr<Value const>(new IntValue(3))),
     V(Oid(".1.3.5.21.1"),std::shared_ptr<Value const>(new IntValue(10)))});
  
  xju::assert_equal(t[col1].size(),1U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col2].size(),1U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
    
  xju::assert_equal(t.nextOids()[0],Oid(".1.3.5.20.1"));
  xju::assert_equal(t.nextOids()[1],Oid(".1.3.5.21.1"));

  t.add(
    {V(Oid(".1.3.5.20.2"),std::shared_ptr<Value const>(new IntValue(4))),
     V(Oid(".1.3.5.21.2"),std::shared_ptr<Value const>(new IntValue(11)))});
  
  xju::assert_equal(t[col1].size(),2U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col1][1].oid(),Oid(".1.3.5.20.2"));
  xju::assert_equal(t[col1][1]->intValue(),4);
  xju::assert_equal(t[col2].size(),2U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  xju::assert_equal(t[col2][1].oid(),Oid(".1.3.5.21.2"));
  xju::assert_equal(t[col2][1]->intValue(),11);
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
    
  xju::assert_equal(t.nextOids()[0],Oid(".1.3.5.20.2"));
  xju::assert_equal(t.nextOids()[1],Oid(".1.3.5.21.2"));

  t.add(
    {V(Oid(".1.3"),std::shared_ptr<Value const>(new NullValue)),
     V(Oid(".1.3"),std::shared_ptr<Value const>(new NullValue))});

  xju::assert_equal(t[col1].size(),2U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col1][1].oid(),Oid(".1.3.5.20.2"));
  xju::assert_equal(t[col1][1]->intValue(),4);
  xju::assert_equal(t[col2].size(),2U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  xju::assert_equal(t[col2][1].oid(),Oid(".1.3.5.21.2"));
  xju::assert_equal(t[col2][1]->intValue(),11);
  
  xju::assert_equal(t.atEnd(),true);
}

void test2() throw()
{
  // SnmpV2cTable
  Oid const col1(".1.3.5.20");
  Oid const col2(".1.3.5.21");
  SnmpV2cTable t({col1,col2});
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
  xju::assert_equal(t.nextOids(),std::vector<Oid>({col1,col2}));
  typedef SnmpVar V;
  t.add(
    {V(Oid(".1.3.5.20.1"),std::shared_ptr<Value const>(new IntValue(3))),
     V(Oid(".1.3.5.21.1"),std::shared_ptr<Value const>(new IntValue(10)))});
  
  xju::assert_equal(t[col1].size(),1U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col2].size(),1U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
    
  xju::assert_equal(t.nextOids()[0],Oid(".1.3.5.20.1"));
  xju::assert_equal(t.nextOids()[1],Oid(".1.3.5.21.1"));

  t.add(
    {V(Oid(".1.3.5.20.2"),
       SnmpVar::NoSuchObject(Oid(".1.3.5.20.2"),XJU_TRACED)),
     V(Oid(".1.3.5.21.2"),std::shared_ptr<Value const>(new IntValue(11)))});
  
  xju::assert_equal(t[col1].size(),2U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col1][1].oid(),Oid(".1.3.5.20.2"));
  try{
    xju::assert_never_returns(t[col1][1]->intValue());
  }
  catch(SnmpVar::NoSuchObject const& e){
  }
  
  xju::assert_equal(t[col2].size(),2U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  xju::assert_equal(t[col2][1].oid(),Oid(".1.3.5.21.2"));
  xju::assert_equal(t[col2][1]->intValue(),11);
  
  xju::assert_equal(t.atEnd(),false);
  xju::assert_equal(t.nextOids().size(),2U);
    
  xju::assert_equal(t.nextOids()[0],Oid(".1.3.5.20.2"));
  xju::assert_equal(t.nextOids()[1],Oid(".1.3.5.21.2"));

  t.add(
    {V(Oid(".1.3.5.20.2"),
       SnmpVar::EndOfMibView(Oid(".1.3.5.20.2"),XJU_TRACED)),
     V(Oid(".1.3.5.21.2"),
       SnmpVar::EndOfMibView(Oid(".1.3.5.21.2"),XJU_TRACED))});

  xju::assert_equal(t[col1].size(),2U);
  xju::assert_equal(t[col1][0].oid(),Oid(".1.3.5.20.1"));
  xju::assert_equal(t[col1][0]->intValue(),3);
  xju::assert_equal(t[col1][1].oid(),Oid(".1.3.5.20.2"));
  try{
    xju::assert_never_returns(t[col1][1]->intValue());
  }
  catch(SnmpVar::NoSuchObject const& e){
  }
  xju::assert_equal(t[col2].size(),2U);
  xju::assert_equal(t[col2][0].oid(),Oid(".1.3.5.21.1"));
  xju::assert_equal(t[col2][0]->intValue(),10);
  xju::assert_equal(t[col2][1].oid(),Oid(".1.3.5.21.2"));
  xju::assert_equal(t[col2][1]->intValue(),11);
  
  xju::assert_equal(t.atEnd(),true);
}


}
}

using namespace xju::snmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

