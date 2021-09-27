#!/bin/env python

# Copyright (c) 2018 Trevor Taylor
#
# Permission to use, copy, modify, distribute and sell this software
# and its documentation for any purpose is hereby granted without fee,
# provided that the above copyright notice appear in all.
# Trevor Taylor makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express or
# implied warranty.
#
import sys

fileTemplate='''\

#include <deque>
#include <memory>
#include <xju/Mutex.hh>
#include <xju/Condition.hh>
#include <xju/test/CallTo.hh>
#include <xju/Lock.hh>
#include <typeinfo>
#include <xju/assert.hh>
#include <stdlib.h>
#include <chrono>
#include <xju/steadyNow.hh>
`includes`

namespace xju
{
namespace test
{

class Calls
{
public:
  Calls() noexcept
    : changed_(guard_)
  {
  }

`methods`

  // Pull out all calls.
  std::deque<std::shared_ptr<CallTo> > calls() noexcept
  {
    xju::Lock l(guard_);
    std::deque<std::shared_ptr<CallTo> > result;
    std::swap(calls_,result);
    return result;
  }
private:
  mutable xju::Mutex guard_;
  xju::Condition changed_;
  std::deque<std::shared_ptr<CallTo> > calls_;
};

}
}
'''


methodN='''\
  template<class T,class U`pClasses`>
  std::shared_ptr<CallToV`nParams`<T,U`paramTypes`> > enqueue(
    std::shared_ptr<CallToV`nParams`<T,U`paramTypes`> > x) noexcept
  {
    xju::Lock l(guard_);
    calls_.push_back(x);
    changed_.signal(l);
    return x;
  }
  template<class T,class U,class R`pClasses`>
  std::shared_ptr<CallToR`nParams`<T,U,R`paramTypes`> > enqueue(
    std::shared_ptr<CallToR`nParams`<T,U,R`paramTypes`> > x) noexcept
  {
    xju::Lock l(guard_);
    calls_.push_back(x);
    changed_.signal(l);
    return x;
  }

  template<class T,class U`pClasses`>
  std::shared_ptr<CallToV`nParams`<T,U`paramTypes`> > awaitCall(
    T& x,
    void (U::*f)(`fParams`),
    std::chrono::steady_clock::time_point deadline) noexcept
  {
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){
      changed_.wait(l,deadline);
    }
    xju::assert_not_equal(calls_.size(),0U);
    std::shared_ptr<CallTo> const call(calls_.front());
    calls_.pop_front();
    auto result{std::dynamic_pointer_cast<CallToV`nParams`<T,U`paramTypes`> >(call)};
    if (!result.get()){
      std::string actualType{typeid(*call).name()};
      ::abort();
    }
    xju::assert_equal(&x,&result->x_);
    xju::assert_equal(f,result->f_);
    return result;
  }

  template<class T,class U,class R`pClasses`>
  std::shared_ptr<CallToR`nParams`<T,U,R`paramTypes`> > awaitCall(
    T& x, R (U::*f)(`fParams`),
    std::chrono::steady_clock::time_point deadline) noexcept
  {
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){
      changed_.wait(l,deadline);
    }
    xju::assert_not_equal(calls_.size(),0U);
    auto call(calls_.front());
    calls_.pop_front();
    auto result{std::dynamic_pointer_cast<CallToR`nParams`<T,U,R`paramTypes`> >(call)};
    if (!result.get()){
      std::string actualType{typeid(*call).name()};
      abort();
    }
    xju::assert_equal(&result->x_,&x);
    xju::assert_equal(result->f_,f);
    return result;
  }

  template<class T,class U`pClasses`>
  std::shared_ptr<CallToVc`nParams`<T,U`paramTypes`> > enqueue(
    std::shared_ptr<CallToVc`nParams`<T,U`paramTypes`> > x) noexcept
  {
    xju::Lock l(guard_);
    calls_.push_back(x);
    changed_.signal(l);
    return x;
  }
  template<class T,class U,class R`pClasses`>
  std::shared_ptr<CallToRc`nParams`<T,U,R`paramTypes`> > enqueue(
    std::shared_ptr<CallToRc`nParams`<T,U,R`paramTypes`> > x) noexcept
  {
    xju::Lock l(guard_);
    calls_.push_back(x);
    changed_.signal(l);
    return x;
  }

  template<class T,class U`pClasses`>
  std::shared_ptr<CallToVc`nParams`<T,U`paramTypes`> > awaitCall(
    T const& x,
    void (U::*f)(`fParams`) const,
    std::chrono::steady_clock::time_point deadline) noexcept
  {
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){
      changed_.wait(l,deadline);
    }
    xju::assert_not_equal(calls_.size(),0U);
    std::shared_ptr<CallTo> const call(calls_.front());
    calls_.pop_front();
    auto result{std::dynamic_pointer_cast<CallToVc`nParams`<T,U`paramTypes`> >(call)};
    if (!result.get()){
      std::string actualType{typeid(*call).name()};
      ::abort();
    }
    xju::assert_equal(&x,&result->x_);
    xju::assert_equal(f,result->f_);
    return result;
  }

  template<class T,class U,class R`pClasses`>
  std::shared_ptr<CallToRc`nParams`<T,U,R`paramTypes`> > awaitCall(
    T const& x, R (U::*f)(`fParams`) const,
    std::chrono::steady_clock::time_point deadline) noexcept
  {
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){
      changed_.wait(l,deadline);
    }
    xju::assert_not_equal(calls_.size(),0U);
    auto call(calls_.front());
    calls_.pop_front();
    auto result{std::dynamic_pointer_cast<CallToRc`nParams`<T,U,R`paramTypes`> >(call)};
    if (!result.get()){
      std::string actualType{typeid(*call).name()};
      abort();
    }
    xju::assert_equal(&result->x_,&x);
    xju::assert_equal(result->f_,f);
    return result;
  }

'''

def expandTemplate(t, vars):
    '''expand `X` in t with vars['X']'''
    rest=t
    result=''
    while '`' in rest:
        before,name,rest=rest.split('`',2)
        result=result+before+str(vars[name])
        pass
    result=result+rest
    return result

maxParams=int(sys.argv[1])

methods_=[]
for nParams in range(0,maxParams+1):
    pClasses=''.join([',class P{n}'.format(**vars())
                      for n in range(1,nParams+1)])
    paramTypes=''.join([',P{n}'.format(**vars())
                        for n in range(1,nParams+1)])
    fParams=','.join(['P{n}'.format(**vars())
                        for n in range(1,nParams+1)])
    methods_.append(expandTemplate(methodN,vars()))
    pass
includes=''.join([f'#include <xju/test/CallTo{nParams}.hh>\n'
                  for nParams in range(0,maxParams+1)])

methods=''.join(methods_)
print(expandTemplate(fileTemplate,vars()))
