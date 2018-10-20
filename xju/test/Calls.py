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
{includes}

namespace xju
{{
namespace test
{{

class Calls
{{
public:
  Calls() noexcept
    : changed_(guard_)
  {{
  }}

{methods}

  std::deque<std::shared_ptr<CallTo> > calls() noexcept
  {{
    xju::Lock l(guard_);
    std::deque<std::shared_ptr<CallTo> > result;
    std::swap(calls_,result);
    return result;
  }}
private:
  mutable xju::Mutex guard_;
  xju::Condition changed_;
  std::deque<std::shared_ptr<CallTo> > calls_;
}};

}}
}}
'''


methodN='''\
  template<class T,class U{pClasses}>
  std::shared_ptr<CallToV{nParams}<T,U{paramTypes}> > enqueue(
    T& x, 
    void (U::*f)({fParams}){params}) noexcept
  {{
    xju::Lock l(guard_);
    std::shared_ptr<CallToV{nParams}<T,U{paramTypes}> > result{{
      new CallToV{nParams}<T,U{paramTypes}>(x,f{paramNames})}};
    calls_.push_back(result);
    changed_.signal(l);
    return result;
  }}
  template<class T,class U,class R{pClasses}>
  std::shared_ptr<CallToR{nParams}<T,U,R{paramTypes}> > enqueue(
    T& x, 
    R (U::*f)({fParams}){params}) noexcept
  {{
    xju::Lock l(guard_);
    std::shared_ptr<CallToR{nParams}<T,U,R{paramTypes}> > result{{
      new CallToR{nParams}<T,U,R{paramTypes}>(x,f{paramNames})}};
    calls_.push_back(result);
    changed_.signal(l);
    return result;
  }}

  template<class T,class U{pClasses}>
  std::shared_ptr<CallToV{nParams}<T,U{paramTypes}> > awaitCall(
    T& x,
    void (U::*f)({fParams}),
    std::chrono::steady_clock::time_point deadline) noexcept
  {{
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){{
      changed_.wait(l,deadline);
    }}
    xju::assert_not_equal(calls_.size(),0U);
    std::shared_ptr<CallTo> const call(calls_.front());
    calls_.pop_front();
    auto result{{std::dynamic_pointer_cast<CallToV{nParams}<T,U{paramTypes}> >(call)}};
    if (!result.get()){{
      std::string actualType{{typeid(*call).name()}};
      ::abort();
    }}
    xju::assert_equal(&x,&result->x_);
    xju::assert_equal(f,result->f_);
    return result;
  }}

  template<class T,class U,class R{pClasses}>
  std::shared_ptr<CallToR{nParams}<T,U,R{paramTypes}> > awaitCall(
    T& x, R (U::*f)({fParams}),
    std::chrono::steady_clock::time_point deadline) noexcept
  {{
    xju::Lock l(guard_);
    while(!calls_.size()&&xju::steadyNow()<deadline){{
      changed_.wait(l,deadline);
    }}
    xju::assert_not_equal(calls_.size(),0U);
    auto call(calls_.front());
    calls_.pop_front();
    auto result{{std::dynamic_pointer_cast<CallToR{nParams}<T,U,R{paramTypes}> >(call)}};
    if (!result.get()){{
      std::string actualType{{typeid(*call).name()}};
      abort();
    }}
    xju::assert_equal(&result->x_,&x);
    xju::assert_equal(result->f_,f);
    return result;
  }}

'''

maxParams=int(sys.argv[1])

methods=[]
for nParams in range(0,maxParams+1):
    pClasses=''.join([',class P{n}'.format(**vars())
                      for n in range(1,nParams+1)])
    paramTypes=''.join([',P{n}'.format(**vars())
                        for n in range(1,nParams+1)])
    fParams=','.join(['P{n}'.format(**vars())
                        for n in range(1,nParams+1)])
    params=  ''.join([',\n    P{n} const& p{n}'.format(**vars())
                      for n in range(1,nParams+1)])
    paramNames=''.join([',\n        p{n}'.format(**vars())
                        for n in range(1,nParams+1)])
    methods.append(methodN.format(**vars()))
    pass
includes=''.join(['#include <xju/test/CallTo{nParams}.hh>\n'.format(**vars())
                  for nParams in range(0,maxParams+1)])

methods=''.join(methods)
print fileTemplate.format(**vars())
