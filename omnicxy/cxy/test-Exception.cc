//     -*- mode: c++ ; c-file-style: "osse" ;  -*-
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
#include <cxy/Exception.hh>

#include <algorithm>
#include <iostream>
#include <string>
#include <xju/assert.hh>
#include <sstream>


using namespace xju;


namespace
{
    std::string getFirst(const std::pair<std::string, cxy::Exception::FileAndLine>& x) throw()
{
  return x.first;
}
}

//
// success
//    construct
//    member access
//    add context
//
void test1()
{
  cxy::Exception e("cause", cxy::Exception::FileAndLine("FA", 100U));
  assert_equal(e.cause_.first, std::string("cause"));
  assert_equal(e.cause_.second.first, std::string("FA"));
  assert_equal(e.cause_.second.second, 100U);
  
  assert_equal(e.context_.size(), 0U);
  
  e.addContext("context", cxy::Exception::FileAndLine("FBB", 8U));
  assert_equal(e.context_.size(), 1U);
  assert_equal(e.context_[0].first, std::string("context"));
  assert_equal(e.context_[0].second.first, std::string("FBB"));
  assert_equal(e.context_[0].second.second, 8U);
  
  e.addContext("c2", std::make_pair(__FILE__, __LINE__));
  assert_equal(e.cause_.first, std::string("cause"));
  assert_equal(e.context_.size(), 2U);
  assert_equal(e.context_[0].first, std::string("context"));
  assert_equal(e.context_[1].first, std::string("c2"));
}

// success - subtyping
class E : public cxy::Exception
{
public:
  E(const std::string& cause) throw():
      Exception(cause, std::make_pair(__FILE__, __LINE__))
  {
  }
  ~E() throw()
  {
  }
};

#define vl(v) (sizeof(v)/sizeof(v[0]))
#define ve(v) (v+vl(v))

std::string expect2[] = 
{
  "context"
};
std::string expect3[] = 
{
  "context",
  "cc"
};


//    catch, add context, rethrow sub-type, catch sub-type
void test2()
{
  try
  {
    try
    {
      E e("cause");
      e.addContext("context", std::make_pair(__FILE__, __LINE__));
      throw e;
    }
    catch(cxy::Exception& e2)
    {
      assert_equal(e2.cause_.first, std::string("cause"));
      
      assert_equal(e2.context_.size(), vl(expect2));
      
      std::vector<std::string> context;
      transform(e2.context_.begin(), 
                e2.context_.end(),
                std::back_inserter(context),
                getFirst);
      
      std::equal(context.begin(),
                 context.end(),
                 expect2,
                 assert_equal<std::string, std::string>);
      
      e2.addContext("cc", std::make_pair(__FILE__, __LINE__));
      throw;
    }
  }
  catch(const E& e3)
  {
    assert_equal(e3.cause_.first, std::string("cause"));
    assert_equal(e3.context_.size(), vl(expect3));
    
    std::vector<std::string> context;
    transform(e3.context_.begin(), 
              e3.context_.end(),
              std::back_inserter(context),
              getFirst);
    
    std::equal(context.begin(), 
               context.end(),
               expect3,
               assert_equal<std::string, std::string>);
  }
}

//
// format
//
void test3()
{
  {
    cxy::Exception e("power failed", std::make_pair(__FILE__, __LINE__));
    e.addContext("watch the football", std::make_pair(__FILE__, __LINE__));
    
    std::ostringstream s;
    s << e;
    assert_equal(s.str(),
                 std::string("Failed to watch the football because\n"
                             "power failed."));
    
  }
  {
    cxy::Exception e("power failed", std::make_pair(__FILE__, __LINE__));
    e.addContext("watch the football", std::make_pair(__FILE__, __LINE__));
    e.addContext("find out who won", std::make_pair(__FILE__, __LINE__));
    std::ostringstream s;
    s << e;
    assert_equal(s.str(),
                 std::string("Failed to find out who won because\n"
                             "failed to watch the football because\n"
                             "power failed."));
  }
}

// ostringstream versions of methods
void test4()
{
  std::ostringstream cause;
  cause << "cause";
  
  cxy::Exception e(cause.str(), cxy::Exception::FileAndLine("FA", 100U));
  assert_equal(e.cause_.first, std::string("cause"));
  assert_equal(e.cause_.second.first, std::string("FA"));
  assert_equal(e.cause_.second.second, 100U);
  
  assert_equal(e.context_.size(), 0U);
  
  std::ostringstream context;
  context << "context";
  
  e.addContext(context.str(), cxy::Exception::FileAndLine("FBB", 8U));
  assert_equal(e.context_.size(), 1U);
  assert_equal(e.context_[0].first, std::string("context"));
  assert_equal(e.context_[0].second.first, std::string("FBB"));
  assert_equal(e.context_[0].second.second, 8U);
}

//
// failure
//    (none)
//


int main(int argc, char* argv[])
{
  unsigned int n = 0;
  test1(); ++n;
  test2(); ++n;
  test3(); ++n;
  test4(); ++n;
  
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
