//     -*- mode: c++ ; c-file-style: "osse" ; -*-
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

#include <xju/Time.hh>
#include <xju/assert.hh>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "xju/MicroSeconds.hh"


namespace t=xju::mt;


void test1(); // Lock
void test2(); // atomicity of conversion of TryLock to Lock
void test3(); // basic condition wait/signal
void test5(); // does thread really run concurrently?
void test6(); // threads reused?
void test7(); // Seq multi-thread uniqueness


class T1
{
public:
    T1() throw():
      _duration(5000000),
      _targetIterations(100000),
      _value(1, 1)
    {
    }
	 
	 void run1() throw()
	 {
		  const xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  
		  while(xju::Time::now() < until)
		  {
				t::Lock l(_guard);
				xju::assert_equal(_value.first, _value.second);
				_value = std::make_pair(1, 1);
				++n;
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T1 thread 2 only did " << n
                  << "of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds" << std::endl;
		  }
	 }
	 
	 void run2() throw()
	 {
		  const xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  
		  while(xju::Time::now() < until)
		  {
				t::Lock l(_guard);
				xju::assert_equal(_value.first, _value.second);
				_value = std::make_pair(2, 2);
				++n;
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T1 thread 2 only did " << n
                  << "of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds" << std::endl;
		  }
	 }
	 
private:
	 const xju::MicroSeconds _duration;
	 const unsigned int _targetIterations;
	 
	 t::Mutex _guard;
	 std::pair<int, int> _value;
};


// Lock
void test1()
{
	 T1 t5;
	 t::Thread<T1> t1(t5, &T1::run1);
	 t::Thread<T1> t2(t5, &T1::run2);
}

class T2
{
public:
	 T2() throw():
		  _duration(xju::MicroSeconds(5000000)),
		  _targetIterations(100000),
		  _value(1, 1)
	 {
	 }
	 
	 void run1() throw()
	 {
		  const xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  
		  while(xju::Time::now() < until)
		  {
				t::TryLock t(_guard);
				if (t)
				{
					 xju::assert_equal(_value.first, _value.second);
					 _value = std::make_pair(1, 1);
					 
					 t::Lock l(t);
					 xju::assert_equal(_value, std::make_pair(1, 1));
					 ++n;
				}
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T2 thread 2 only did " << n
                  << "of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds" << std::endl;
		  }
	 }
	 
	 void run2() throw()
	 {
		  const xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  
		  while(xju::Time::now() < until)
		  {
				t::TryLock t(_guard);
				if (t)
				{
					 xju::assert_equal(_value.first, _value.second);
					 _value = std::make_pair(2, 2);
					 
					 t::Lock l(t);
					 xju::assert_equal(_value, std::make_pair(2, 2));
					 ++n;
				}
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T5 thread 2 only did " << n
                  << "of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds" << std::endl;
		  }
	 }
	 
private:
	 const xju::MicroSeconds _duration;
	 const unsigned int _targetIterations;
	 
	 t::Mutex _guard;
	 std::pair<int, int> _value;
};


// atomicity of conversion of TryLock to Lock
void test2()
{
	 T2 t;
	 t::Thread<T2> t1(t, &T2::run1);
	 t::Thread<T2> t2(t, &T2::run2);
}


class T3
{
public:
	 T3() throw():
		  _c(_guard),
		  _b(false)
	 {
	 }
	 void wait() throw()
	 {
		  t::Lock l(_guard);
		  while(!_b)
		  {
				_c.wait(l);
		  }
	 }
	 void signal() throw()
	 {
		  sleep(1);
		  t::Lock l(_guard);
		  _b = true;
		  _c.signal(l);
	 }
private:
	 t::Mutex _guard;
	 t::Condition _c;
	 bool _b;
};


// basic condition wait/signal
void test3()
{
	 const xju::Time begin(xju::Time::now());
	 T3 t4;
	 t::Thread<T3> t(t4, &T3::signal);
	 t4.wait();
	 const xju::Time end(xju::Time::now());
	 
	 const xju::MicroSeconds diff(end - begin);
	 
	 xju::assert_(diff, std::greater_equal<xju::MicroSeconds>(), xju::MicroSeconds(1000000));
	 xju::assert_(diff, std::less<xju::MicroSeconds>(), xju::MicroSeconds(1500000));
}


class T4
{
public:
	 T4(t::Mutex& m, t::Condition& c, bool& flag, int secs):
		  _m(m),
		  _c(c),
		  _flag(flag),
		  _secs(secs)
	 {
	 }
	 void main() throw()
	 {
		  sleep(_secs);
		  t::Lock l(_m);
		  _flag = true;
		  _c.signal(l);
	 }
	 t::Mutex& _m;
	 t::Condition& _c;
	 bool& _flag;
	 int _secs;
};


// condition variable - timed wait
void test4()
{
	 t::Mutex m;
	 t::Condition c(m);
	 bool flag(false);
	 
	 const xju::Time n(xju::Time::now());
	 
	 T4 t(m, c, flag, 5);
	 
	 t::Thread<T4> thread(t, &T4::main);
	 
	 t::Lock l(m);
	 
	 while(xju::Time::now() < n+xju::MicroSeconds(2000000))
	 {
		  c.wait(l, n+xju::MicroSeconds(2000000));
	 }
	 xju::assert_equal(flag, false);
	 
	 while(!flag && xju::Time::now() < n+xju::MicroSeconds(20000000))
	 {
		  c.wait(l, n+xju::MicroSeconds(20000000));
	 }
	 xju::assert_equal(flag, true);
	 xju::assert_(xju::Time::now() - n, std::less<xju::MicroSeconds>(), xju::MicroSeconds(6000000));
}


class T5
{
public:
	 T5() throw():
		  _c(_guard),
		  _b(false)
	 {
	 }
	 
	 // does not return until signal() is called
	 void run() throw()
	 {
		  t::Lock l(_guard);
		  while(!_b)
		  {
				_c.wait(l);
		  }
	 }
	 
	 void signal() throw()
	 {
		  t::Lock l(_guard);
		  _b = true;
		  _c.signal(l);
	 }
	 
private:
	 t::Mutex _guard;
	 t::Condition _c;
	 bool _b;
};


// does thread really run concurrently?
void test5()
{
	 T5 x;
	 t::Thread<T5> t(x, &T5::run);
	 x.signal();
}

class T6
{
public:
	 void run() throw()
	 {
	 }
};


// threads reused?
void test6()
{
	 const unsigned int duration(10);
	 
	 const xju::Time until(xju::Time::now()+xju::MicroSeconds(duration*1000000));
	 const unsigned int targetIterations(10000);
	 
	 unsigned int n(0);
	 T6 t3;
	 while((xju::Time::now() < until) && (n < targetIterations))
	 {
		  t::Thread<T6> t(t3, &T6::run);
		  ++n;
	 }
	 if (n != targetIterations)
	 {
		  std::cout << "Warning: test6 only did " << n << " of " << targetIterations
						<< " iterations in " << duration
						<< " seconds" << std::endl;
	 }
}


class T7
{
public:
	 T7() throw():
		  _duration(5000000),
		  _targetIterations(25000)
	 {
	 }
	 
	 void run1() throw()
	 {
		  xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  while(xju::Time::now() < until)
		  {
				t::Seq::Value v(_seq);
				_values1.push_back(v.stringRepr());
				++n;
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T7::run1() only did " << n
                  << " of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds"
                  << std::endl;
		  }
	 }
	 void run2() throw()
	 {
		  xju::Time until(xju::Time::now()+_duration);
		  unsigned int n(0);
		  while(xju::Time::now() < until)
		  {
				t::Seq::Value v(_seq);
				_values2.push_back(v.stringRepr());
				++n;
		  }
		  if (n < _targetIterations)
		  {
				std::cout << "Warning: T7::run2() only did " << n
                  << " of " << _targetIterations
                  << " iterations in " << _duration.value()/1000000
                  << " seconds"
                  << std::endl;
		  }
	 }
	 
	 ~T7() throw()
	 {
		  std::set<std::string> unique;
		  copy(_values1.begin(), _values1.end(), inserter(unique, unique.end()));
		  copy(_values2.begin(), _values2.end(), inserter(unique, unique.end()));
		  xju::assert_equal(unique.size(), _values1.size()+_values2.size());
	 }
	 
private:
	 const xju::MicroSeconds _duration;
	 const unsigned int _targetIterations;
	 
	 t::Seq _seq;
	 
	 std::vector<std::string> _values1;
	 std::vector<std::string> _values2;
};


//
// Seq multi-thread uniqueness
//
void test7()
{
	 T7 t;
	 t::Thread<T7> t1(t, &T7::run1);
	 t::Thread<T7> t2(t, &T7::run2);
}


int main(int argc, char* argv[])
{
	 int n = 0;
	 
	 test1(); ++n;
	 test2(); ++n;
	 test3(); ++n;
	 test4(); ++n;
	 test5(); ++n;
	 test6(); ++n;
	 test7(); ++n;
	 
	 
	 std::cout << "PASS - " << n << " steps" << std::endl;
	 return 0;
}
