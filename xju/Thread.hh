// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// Design Notes:
//
// [1] we could do fancy stuff eg avoid copying f and stop where
//     their types allow it, and adding variable parameters to pass
//     to f and stop, but that all just makes it hard to see what
//     types and lifetimes are actually used; it works out
//     better let the caller use lambda functions to easily get the 
//     same effects (much easier to see what's going on).
//
#ifndef XJU_THREAD_H
#define XJU_THREAD_H

#include <thread>
#include "xju/Exception.hh"
#include <memory>

namespace xju
{
class Thread
{
public:
  //
  // creates and starts thread, which will
  // call f() once before returning 0.
  //
  // note destructor joins thread - see ~Thread() below
  //
  template<class F>
  explicit Thread(F f) /*throw(
    std::bad_alloc,
    // other failures, eg thread limit reached
    xju::Exception)*/ try:
      stop_(trigger([](){})),
      impl_(f)
  {
  }
  catch(std::bad_alloc&)
  {
    throw;
  }
  catch(std::exception& e) {
    xju::Exception ee(e.what(),XJU_TRACED);
    ee.addContext("create thread",XJU_TRACED);
  }
  
  //
  // creates and starts thread, which will
  // call f() once before returning 0. See
  // destructor for explanation of stop.
  //
  // note destructor joins thread - see ~Thread() below
  //
  // pre: lifetime(t) > lifetime(this)
  // pre: stop() must not throw
  //
  template<class F, class Stop>
  Thread(F f, Stop stop) /*throw(
    std::bad_alloc,
    // other failures, eg thread limit reached
    xju::Exception)*/ try:
      stop_(trigger(stop)),
      impl_(f)
  {
  }
  catch(std::bad_alloc&)
  {
    throw;
  }
  catch(std::exception& e) {
    xju::Exception ee(e.what(),XJU_TRACED);
    ee.addContext("create thread",XJU_TRACED);
  }

  
  //
  // if stop was specified, call stop(), then...
  //
  // waits for and joins with thread (i.e.
  // waits for f() to return).
  //
  // (Note that client must have somehow
  // arranged for thread to end eventually (i.e.
  // for f to return.)
  //
  ~Thread() throw()
  {
    (*stop_)();
    impl_.join();
  }
  
private:
  struct TriggerIf
  {
    virtual ~TriggerIf() throw() {}
    
    virtual void operator()() throw() = 0;
  };
    
  std::unique_ptr<TriggerIf> stop_;
  std::thread impl_;
  
  template<class T>
  struct Trigger : TriggerIf
  {
    explicit Trigger(T t) throw():
        t_(t)
    {
    }
    virtual void operator()() throw()
    {
      t_();
    }
    T t_;
  };
  template<class T>
  std::unique_ptr< TriggerIf > trigger(T f) /*throw(
    std::bad_alloc)*/
  {
    return std::unique_ptr< TriggerIf >(
      new Trigger<T>(f));
  }
};

}

#endif
