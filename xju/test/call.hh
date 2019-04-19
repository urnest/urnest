// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <future>
#include <chrono>
#include <xju/assert.hh>

namespace xju
{
namespace test
{

template<class future>
class Call
{
public:
  Call(future&& f) noexcept:
      f_(std::move(f))
  {
  }
  future f_;
  decltype(f_.get()) getResultBy(
    std::chrono::steady_clock::time_point const& deadline) noexcept
  {
    xju::assert_equal(f_.wait_until(deadline),std::future_status::ready);
    return f_.get();
  }
  template<class E>
  E except(std::chrono::steady_clock::time_point const& deadline) noexcept
  {
    try{
      xju::assert_equal(f_.wait_until(deadline),std::future_status::ready);
      auto const r{f_.get()};
      xju::assert_never_reached();
    }
    catch(E const& e){
      return e;
    }
  }
};

template<class Function,class... Args>
auto call(Function&& f,Args&&... args) noexcept
  -> std::shared_ptr<Call<decltype(std::async(f,args...))>>
{
  return std::shared_ptr<Call<decltype(std::async(f,args...))>>(
    new Call<decltype(std::async(f,args...))>(
      std::async(std::launch::async,f,args...)));
}

}
}
