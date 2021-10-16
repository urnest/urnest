// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <example/hierarchy.hh>

#include <iostream>
#include <xju/assert.hh>

namespace example
{
struct tag{
};
template<>
struct hierarchy_config<std::string,tag>{
  typedef std::list<std::string> children_type;
};
typedef hierarchy<std::string, tag> H;

void test1() {
  H page_template_({ { "html",
                      { "div class=q",
                        { "p class=q", { "cdata", { "2 + 2 =", {} } } },
                        { "p class=a", { "cdata", { "4" } } } },
                      { "div class=q",
                        { "p class=q", { "cdata", { "6 + 1 =", {} } } },
                        { "p class=a", { "cdata", { "7" } } } } } });
  
  auto real_test = { {"8 * 8", "64"},
                     {"7 * 2", "14"},
                     {"9 - 3", "6"} };

  H::node qa_template = template_.find("div class=q").first();

  auto page = page_template_;
  // remove all template questions
  page.apply(H::node_f([](H::path p)
                       {
                         if((*p).second.value_ == "div class=q"){
                           p.back().second=
                             (p.back().first->erase(p.back().second));
                         }
                         return p;
                       }));
                           
  // put in real test questions with no answers
  for(auto q: test){
    auto q = qa_template;
    q.find_value("p class=a").find_value("cdata") = "?";
    page.apply(H::node_f([](H::path p)
                         {
                           if ((*p.back().second)=="html"){
                             p.back().first.push_back(q);
                             return H::path(); // end now
                           }
                           return p;
                         }));
  }

  H const expect({
    { "html",
      { "div class=q",
        { "p class=q", { "cdata", { "6 * 8 =", {} } } },
        { "p class=a", { "cdata", { "?" } } } },
      { "div class=q",
        { "p class=q", { "cdata", { "7 * 2 =", {} } } },
        { "p class=a", { "cdata", { "?" } } } },
      { "div class=q",
        { "p class=q", { "cdata", { "9 - 3 =", {} } } },
        { "p class=a", { "cdata", { "?" } } } } } });

  xju::assert_equal(page, expect);
}

}

using namespace example;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

