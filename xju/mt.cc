//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/mt.hh>


#include <sstream>
#include <stdlib.h>
#include <xju/assert.hh>

namespace xju
{
namespace mt
{
namespace 
{
Time seqValue(const std::string& stringRepr) throw()
{
  assert_(stringRepr.size(), std::greater<int>(), 6);
  std::string usecS(stringRepr.substr(stringRepr.size()-6,
                                      stringRepr.size()));
  std::string secS(stringRepr.substr(0, stringRepr.size()-6));
  
  // REVISIT: use strtol to check for non-digits.
  return Time(atol(secS.c_str()), atol(usecS.c_str()));
}
}


std::string Seq::Value::stringRepr() const throw()
{
  std::ostringstream s;
  s << _value.getSecs();
  s. width(6);
  s.fill('0');
  s << _value.getUsecs();
  return s.str();
}


Seq::Value::Value(const std::string& stringRepr) throw():
    _value(seqValue(stringRepr))
{
}

Time Seq::next() throw()
{
  Lock l(_guard);
  _last = _last + xju::MicroSeconds(1);
  return _last;
}



Condition::Condition(Mutex& guard) throw():
    _guard(guard)
{
  assert_equal(pthread_cond_init(&_impl, 0),0);
}

void Condition::signal(const Lock& l) throw()
{
  assert_equal(l.holds(_guard), true);
  
  assert_equal(pthread_cond_signal(&_impl),0);
}

void Condition::wait(Lock& l) throw()
{
  assert_equal(l.holds(_guard), true);
  
  assert_equal(pthread_cond_wait(&_impl, &_guard._impl),0);
}

void Condition::wait(Lock& l, const Time& timeout) throw()
{
  assert_equal(l.holds(_guard), true);
  
  struct timespec ts_abs;
  
  ts_abs.tv_sec  = timeout.getSecs();
  ts_abs.tv_nsec = timeout.getUsecs()*1000;
  
  int s = pthread_cond_timedwait(&_impl, &_guard._impl, &ts_abs);
  
  if (!(s==0 || s == ETIMEDOUT || s == EINTR)) {
    assert_abort();
  }
}


Condition::~Condition() throw()
{
  assert_equal(pthread_cond_destroy(&_impl),0);
}
}
}
