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
#include <xju/format.hh>


#include <xju/assert.hh>
#include <iostream>
#include <vector>
#include <functional>
#include <set>
#include <math.h>
#include <xju/unix_epoch.hh>

namespace xju
{

// int format
void test1()
{
  {
    std::string result(format::int_(10));
    assert_equal(result, std::string("10"));
  }
  {
    std::string result(format::int_(20, 4));
    assert_equal(result, std::string("0020"));
  }
  {
    std::string result(format::int_(20, 6, ' ', std::ios::left));
    assert_equal(result, std::string("20    "));
  }
}


// quote
void test2()
{
  {
    std::string result(format::quote(std::string("x")));
    assert_equal(result, std::string("\"x\""));
  }
  {
    std::string result(format::quote("'", std::string("x")));
    assert_equal(result, std::string("'x'"));
  }
  {
    std::string result(format::quote("(", ")", std::string("x")));
    assert_equal(result, std::string("(x)"));
  }
}

// join
void test3()
{
  {
    std::vector<int> x;
    std::string result = format::join(x.begin(),
                                      x.end(),
                                      format::Int(),
                                      "-");
    assert_equal(result, std::string(""));
  }
  {
    std::vector<int> x;
    x.push_back(1);
    
    std::string result = format::join(x.begin(),
                                      x.end(),
                                      format::Int(),
                                      "-");
    assert_equal(result, std::string("1"));
  }
  {
    std::vector<int> x;
    x.push_back(1);
    x.push_back(2);
    x.push_back(3);
    
    std::string result = format::join(x.begin(),
                                      x.end(),
                                      format::Int(2),
                                      ", ");
    assert_equal(result, std::string("01, 02, 03"));
  }
}


// set
void test4()
{
  // empty
  {
    std::set<int> x;
    std::string result = format::set(x.begin(),
                                     x.end(),
                                     format::Int());
    assert_equal(result, std::string("{  }"));
  }
  // 1 element
  {
    std::set<int> x;
    x.insert(5);
    
    std::string result = format::set(x.begin(),
                                     x.end(),
                                     format::Int());
    assert_equal(result, std::string("{ 5 }"));
  }
  // > 1 element
  {
    std::set<int> x;
    x.insert(5);
    x.insert(8);
    x.insert(10);
    
    std::string result = format::set(x.begin(),
                                     x.end(),
                                     format::Int());
    assert_equal(result, std::string("{ 5, 8, 10 }"));
  }
}

class Y
{
public:
  Y(const std::string& name) throw():
      _name(name)
  {
  }
  const std::string _name;
};
std::ostream& operator<<(std::ostream& s, const Y& y) throw()
{
  s << "Y called " << y._name;
  return s;
}


// str()
void test5()
{
  assert_equal(format::str('a'), std::string("a"));
  assert_equal(format::str(true), std::string("true"));
  assert_equal(format::str(false), std::string("false"));
  assert_equal(format::str(126), std::string("126"));
  assert_equal(format::str(1268L), std::string("1268"));
  assert_equal(format::str(126U), std::string("126"));
  assert_equal(format::str(1268UL), std::string("1268"));
  assert_equal(format::str(0.5), std::string("0.5"));
  assert_equal(format::str(std::string("abcd")),
               std::string("abcd"));
  assert_equal(format::str(Y("abcd")),
               std::string("Y called abcd"));
}

// float_()
void test6()
{
  {
    assert_equal(format::Float()(12.34), "12.34");
    assert_equal(format::Float(
                   ios_base::fmtflags(0), 3)(12.34), "12.3");
    assert_equal(format::Float(ios_base::scientific)(12.34),
#ifdef __MINGW32__
                 "1.234000e+001"
#else
                 "1.234000e+01"
#endif
      );
    assert_equal(format::Float(ios_base::fixed, 4)(12.34),
                 "12.3400");
  }
  {
    assert_equal(format::Double()(12.34), "12.34");
    assert_equal(format::Double(
                   ios_base::fmtflags(0), 3)(12.34), "12.3");
    assert_equal(
      format::Double(ios_base::scientific)(12.34),
#ifdef __MINGW32__
      "1.234000e+001"
#else
      "1.234000e+01"
#endif
      );
    assert_equal(format::Double(
                   ios_base::fixed, 4)(12.34), "12.3400");
  }
  assert_equal(format::float_(sqrt(-1.0)), "nan");
}

template<int sizeOfShort>
class testHexShort
{
public:
  void operator()()
  {
    std::cerr << "warning: untested for sizeof(short)" 
              << sizeof(short)
              << std::endl;
  };
};

template<>
class testHexShort<2>
{
public:
  void operator()()
  {
    assert_equal(format::hex((short)0x3), "0x0003");
    assert_equal(format::hex((short)0x43), "0x0043");
    assert_equal(format::hex((short)0x143), "0x0143");
    assert_equal(format::hex((short)0x2143), "0x2143");
    assert_equal(format::hex((short)-1), "0xffff");
    
    assert_equal(format::hex((unsigned short)0x3), "0x0003");
    assert_equal(format::hex((unsigned short)0x43), "0x0043");
    assert_equal(format::hex((unsigned short)0x143), "0x0143");
    assert_equal(format::hex((unsigned short)0x2143), "0x2143");
    assert_equal(format::hex((unsigned short)0xffff), "0xffff");
    
    assert_equal(format::octal((short)03), "0000003");
    assert_equal(format::octal((short)043), "0000043");
    assert_equal(format::octal((short)0143), "0000143");
    assert_equal(format::octal((short)02143), "0002143");
    assert_equal(format::octal((short)-1), "0177777");
    
    assert_equal(format::octal((unsigned short)03), "0000003");
    assert_equal(format::octal((unsigned short)043), "0000043");
    assert_equal(format::octal((unsigned short)0143), "0000143");
    assert_equal(format::octal((unsigned short)02143), "0002143");
    assert_equal(format::octal((unsigned short)0177777), "0177777");
    
  };
};

template<>
class testHexShort<4>
{
public:
  void operator()()
  {
    assert_equal(format::hex((short)0x3), "0x00000003");
    assert_equal(format::hex((short)0x43), "0x00000043");
    assert_equal(format::hex((short)0x143), "0x00000143");
    assert_equal(format::hex((short)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((short)-1), "0xffffffff");
    
    assert_equal(format::hex((unsigned short)0x3), "0x00000003");
    assert_equal(format::hex((unsigned short)0x43), "0x00000043");
    assert_equal(format::hex((unsigned short)0x143), "0x00000143");
    assert_equal(format::hex((unsigned short)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((unsigned short)0xffffffff), "0xffffffff");
    
  };
};

template<int sizeOfInt>
class testHexInt
{
public:
  void operator()()
  {
    std::cerr << "warning: untested for sizeof(int)" 
              << sizeof(int)
              << std::endl;
  };
};

template<>
class testHexInt<2>
{
public:
  void operator()()
  {
    assert_equal(format::hex((int)0x3), "0x0003");
    assert_equal(format::hex((int)0x43), "0x0043");
    assert_equal(format::hex((int)0x143), "0x0143");
    assert_equal(format::hex((int)0x2143), "0x2143");
    assert_equal(format::hex((int)-1), "0xffff");
    
    assert_equal(format::hex((unsigned int)0x3), "0x0003");
    assert_equal(format::hex((unsigned int)0x43), "0x0043");
    assert_equal(format::hex((unsigned int)0x143), "0x0143");
    assert_equal(format::hex((unsigned int)0x2143), "0x2143");
    assert_equal(format::hex((unsigned int)0xffff), "0xffff");
    
  };
};

template<>
class testHexInt<4>
{
public:
  void operator()()
  {
    assert_equal(format::hex((int)0x3), "0x00000003");
    assert_equal(format::hex((int)0x43), "0x00000043");
    assert_equal(format::hex((int)0x143), "0x00000143");
    assert_equal(format::hex((int)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((int)-1), "0xffffffff");
    
    assert_equal(format::hex((unsigned int)0x3), "0x00000003");
    assert_equal(format::hex((unsigned int)0x43), "0x00000043");
    assert_equal(format::hex((unsigned int)0x143), "0x00000143");
    assert_equal(format::hex((unsigned int)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((unsigned int)0xffffffff), "0xffffffff");
    
  };
};

template<>
class testHexInt<8>
{
public:
  void operator()()
  {
    assert_equal(format::hex((int)0x3), "0x0000000000000003");
    assert_equal(format::hex((int)0x43), "0x0000000000000043");
    assert_equal(format::hex((int)0x143), "0x0000000000000143");
    assert_equal(format::hex((int)0xaf3933fc2143), "0x0000af3933fc2143");
    assert_equal(format::hex((int)-1), "0xffffffffffffffff");
    
    assert_equal(format::hex((unsigned int)0x3), "0x0000000000000003");
    assert_equal(format::hex((unsigned int)0x43), "0x0000000000000043");
    assert_equal(format::hex((unsigned int)0x143), "0x0000000000000143");
    assert_equal(format::hex((unsigned int)0xaf3933fc2143), "0x0000af3933fc2143");
    assert_equal(format::hex((unsigned int)0xffffffff), "0xffffffffffffffff");
    
  };
};

template<long sizeOfLong>
class testHexLong
{
public:
  void operator()()
  {
    std::cerr << "warning: untested for sizeof(long)" 
              << sizeof(long)
              << std::endl;
  };
};

template<>
class testHexLong<4>
{
public:
  void operator()()
  {
    assert_equal(format::hex((long)0x3), "0x00000003");
    assert_equal(format::hex((long)0x43), "0x00000043");
    assert_equal(format::hex((long)0x143), "0x00000143");
    assert_equal(format::hex((long)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((long)-1), "0xffffffff");
    
    assert_equal(format::hex((unsigned long)0x3), "0x00000003");
    assert_equal(format::hex((unsigned long)0x43), "0x00000043");
    assert_equal(format::hex((unsigned long)0x143), "0x00000143");
    assert_equal(format::hex((unsigned long)0x33fc2143), "0x33fc2143");
    assert_equal(format::hex((unsigned long)0xffffffff), "0xffffffff");
    
  };
};

template<>
class testHexLong<8>
{
public:
  void operator()()
  {
    assert_equal(format::hex((long)0x3), "0x0000000000000003");
    assert_equal(format::hex((long)0x43), "0x0000000000000043");
    assert_equal(format::hex((long)0x143), "0x0000000000000143");
    assert_equal(format::hex((long)0xaf3933fc2143), "0x0000af3933fc2143");
    assert_equal(format::hex((long)-1), "0xffffffffffffffff");
    
    assert_equal(format::hex((unsigned long)0x3), "0x0000000000000003");
    assert_equal(format::hex((unsigned long)0x43), "0x0000000000000043");
    assert_equal(format::hex((unsigned long)0x143), "0x0000000000000143");
    assert_equal(format::hex((unsigned long)0xaf3933fc2143), "0x0000af3933fc2143");
    assert_equal(format::hex((unsigned long)0xffffffffffffffff), "0xffffffffffffffff");
    
  };
};

void test7()
{
  assert_equal(format::hex((char)0x3), "0x03");
  assert_equal(format::hex((char)0x43), "0x43");
  assert_equal(format::hex((char)0xff), "0xff");
  
  assert_equal(format::hex((unsigned char)0x3), "0x03");
  assert_equal(format::hex((unsigned char)0x43), "0x43");
  assert_equal(format::hex((unsigned char)0xff), "0xff");
  
  testHexShort<sizeof(short)>()();
  testHexInt<sizeof(int)>()();
  testHexLong<sizeof(long)>()();
}

// cEscapeChar, cEscapeString
void test8()
{
  assert_equal(format::cEscapeChar('\n'), std::string("\\n"));
  assert_equal(format::cEscapeString("fred\njock"),
               std::string("fred\\njock"));
}

// indent
void test9()
{
  assert_equal(format::indent("", "---"),std::string(""));
  assert_equal(format::indent("abc", "---"),std::string("abc"));
  assert_equal(format::indent("abc\n", "---"),std::string("abc\n---"));
  assert_equal(format::indent("abc\ndef", "---"),std::string("abc\n---def"));
  
}

// time
void test10()
{
  xju::assert_equal(xju::format::time(xju::unix_epoch()+std::chrono::seconds(3745)),"3745.000000");
  xju::assert_equal(xju::format::time(xju::unix_epoch()+std::chrono::microseconds(3745)),"0.003745");
  xju::assert_equal(xju::format::time(xju::unix_epoch()+std::chrono::seconds(85)+std::chrono::microseconds(3745)),"85.003745");
}

}

int main(int argc, char* argv[])
{
  int n = 0;
  xju::test1(); ++n;
  xju::test2(); ++n;
  xju::test3(); ++n;
  xju::test4(); ++n;
  xju::test5(); ++n;
  xju::test6(); ++n;
  xju::test7(); ++n;
  xju::test8(); ++n;
  xju::test9(); ++n;
  xju::test10(); ++n;
  
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
