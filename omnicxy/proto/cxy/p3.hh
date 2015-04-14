#ifndef _P3_HCP
#define _P3_HCP
// generated from p3.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <xju/Int.hh>
#include <string>
#include <stdint.h>
#include <xju/Tagged.hh>
#include <xju/Float.hh>


namespace p3
{
class MyBoolean_tag {};
typedef ::xju::Int< MyBoolean_tag,bool > MyBoolean;
class MyChar_tag {};
typedef ::xju::Int< MyChar_tag,char > MyChar;
class MyDouble_tag {};
typedef ::xju::Float< double,MyDouble_tag > MyDouble;
class MyFloat_tag {};
typedef ::xju::Float< float,MyFloat_tag > MyFloat;
class MyInt_tag {};
typedef ::xju::Int< MyInt_tag,int32_t > MyInt;
class MyLongLong_tag {};
typedef ::xju::Int< MyLongLong_tag,int64_t > MyLongLong;
class MyOctet_tag {};
typedef ::xju::Int< MyOctet_tag,uint8_t > MyOctet;
class MyShort_tag {};
typedef ::xju::Int< MyShort_tag,int16_t > MyShort;
class MyString_tag {};
typedef ::xju::Tagged<std::string,MyString_tag > MyString;
class MyULong_tag {};
typedef ::xju::Int< MyULong_tag,uint32_t > MyULong;
class MyULongLong_tag {};
typedef ::xju::Int< MyULongLong_tag,uint64_t > MyULongLong;
class MyUShort_tag {};
typedef ::xju::Int< MyUShort_tag,uint16_t > MyUShort;
class F 
{
public:
  virtual ~F() throw();

  virtual ::p3::MyInt f1(
    ::p3::MyInt const& a) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
  virtual ::p3::MyString f2(
    ::p3::MyString const& a) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}
#endif
