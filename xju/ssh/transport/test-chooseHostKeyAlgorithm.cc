// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/chooseHostKeyAlgorithm.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Exception.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

namespace
{
class HK : public HostKeyAlgorithm
{
public:
  HK(bool isEncryptionCapable, bool isSignatureCapable) noexcept:
      x_(isEncryptionCapable),
      y_(isSignatureCapable)
  {
  }
  virtual bool isEncryptionCapable() const noexcept override
  {
    return x_;
  }
  virtual bool isSignatureCapable() const noexcept override
  {
    return y_;
  }
private:
  bool x_;
  bool y_;
};

}

void test1(){
  HostKeyAlgorithmName HK1("HK1");
  HK hk1(true,true);
  HostKeyAlgorithmName HK2("HK2");
  HK hk2(false,false);
  HostKeyAlgorithmName HK3("HK3");
  HK hk3(true,true);

  {
    auto const& x(chooseHostKeyAlgorithm(
                    {HK1,HK2,HK3},
                    {HK2,HK3},
                    true,
                    true,
                    { {HK1,std::ref(hk1)},
                      {HK2,std::ref(hk2)},
                      {HK3,std::ref(hk3)} }));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(chooseHostKeyAlgorithm(
                    {HK1,HK2,HK3},
                    {HK2,HK3},
                    false,
                    true,
                    { {HK1,std::ref(hk1)},
                      {HK2,std::ref(hk2)},
                      {HK3,std::ref(hk3)} }));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(chooseHostKeyAlgorithm(
                    {HK1,HK2,HK3},
                    {HK2,HK3},
                    true,
                    false,
                    { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} }));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(chooseHostKeyAlgorithm(
                    {HK1,HK2,HK3},
                    {HK2,HK3},
                    false,
                    false,
                    { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} }));
    xju::assert_equal(&x,&hk2);
  }
  try
  {
    auto const& x(chooseHostKeyAlgorithm(
                    {HK1,HK2,HK3},
                    {HK2},
                    true,
                    false,
                    { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} }));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose host key algorithm given client requested HK1; HK2; HK3, server supports HK2 noting encryption capable algorithm is required and signature capable algorithm is not required because\nclient and server have no common host key algorithm that meets the encryption and signature requirements.");
  }
}


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

