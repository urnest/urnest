// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_MUTEX_HH
#define XJU_MUTEX_HH

#include <pthread.h>
#include "assert.hh"

namespace xju
{

class Mutex
{
public:
  // post: not isHeld()
  Mutex() throw();
  
  // pre: not isHeld()
  ~Mutex() throw();
  
  //
  // current holder, if any
  //
  // post: lifetime(result) = lifetime(this)
  //
  bool isHeld() const throw();
  
private:
  pthread_mutex_t _impl;
  void* _holder;				  // could be Lock or TryLock, for example
  
  friend class Lock;
  friend class Condition;
  
  // not implemented
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);
};

inline Mutex::Mutex() throw():
    _holder(0)
{
  pthread_mutex_init(&_impl, 0);
}

inline Mutex::~Mutex() throw()
{
  assert_equal(_holder, (void*)0);
  
  pthread_mutex_destroy(&_impl);
}

inline bool Mutex::isHeld() const throw()
{
  return _holder != 0;
}

}


#endif
