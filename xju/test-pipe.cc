// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/pipe.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/io/poll.hh>
#include <xju/steadyNow.hh>

namespace xju
{
void assert_a_contains_b(std::string const& a, std::string const& b)
{
  xju::assert_not_equal(std::search(a.begin(),a.end(),b.begin(),b.end()),
                        a.end());
}

void test1() {
  auto p(xju::pipe(true,true));
  xju::assert_not_equal(nullptr,p.first.get());
  xju::assert_not_equal(nullptr,p.second.get());
  assert_a_contains_b(p.first->str(),"readable end of pipe");
  assert_a_contains_b(p.second->str(),"writeable end of pipe");
  {
    auto x(xju::io::poll({p.first.get()},
                         {p.second.get()},
                         xju::steadyNow()));
    xju::assert_equal(x,
                      decltype(x)(
                        {},
                        { {p.second.get(),xju::io::PollOutputState::OUT} }));
  }
  p.second->write("f",1U,xju::steadyNow());
  {
    auto x(xju::io::poll({p.first.get()},
                         {p.second.get()},
                         xju::steadyNow()));
    xju::assert_equal(x,
                      decltype(x)(
                        { {p.first.get(),xju::io::PollInputState::IN} },
                        { {p.second.get(),xju::io::PollOutputState::OUT} }));
  }
  p.second.reset();
  {
    auto x(xju::io::poll({p.first.get()},
                         xju::steadyNow()));
    xju::assert_equal(x,
                      decltype(x)(
                        { {p.first.get(),(xju::io::PollInputState::IN|
                                          xju::io::PollInputState::HUP)} } ));
  }
  {
    char f;
    p.first->read(&f,1,xju::steadyNow());
    xju::assert_equal(f,'f');
    auto x(xju::io::poll({p.first.get()},
                         xju::steadyNow()));
    xju::assert_equal(x,
                      decltype(x)(
                        { {p.first.get(),(xju::io::PollInputState::HUP)} } ));
  }
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

