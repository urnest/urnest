#include "inc/test-tc-map-prm-info.h"
#include "inc/assert.h"

#include <iostream>
#include <utility>

namespace tc_map_prm_info
{

FromTyp from(ToTyp const& t)
{
  return FromTyp(t._);
}

void test1()
{
  // a-+p1-->b-+p2+p3->c
  // g-+p4-/          /
  // x-+p5--->y-+p6->/
  //  \<-+p7-/
  //
  // j-+p8->k
  //
  ToTyp a("a");
  ToTyp b("b");
  ToTyp c("c");
  ToTyp g("g");
  ToTyp x("x");
  ToTyp y("y");
  ToTyp j("j");
  ToTyp k("k");
  
  PrmType p1("p1");
  PrmType p2("p2");
  PrmType p3("p3");
  PrmType p4("p4");
  PrmType p5("p5");
  PrmType p6("p6");
  PrmType p7("p7");
  PrmType p8("p8");
  
  DG dg;
  dg.insert(std::make_pair(a, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(b, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(c, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(g, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(x, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(y, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(j, std::map<FromTyp, std::set<PrmTyp> >()));
  dg.insert(std::make_pair(k, std::map<FromTyp, std::set<PrmTyp> >()));
  
  dg[c][from(b)].insert(p3);
  dg[c][from(b)].insert(p2);
  dg[b][from(a)].insert(p1);
  dg[b][from(g)].insert(p4);
  dg[c][from(y)].insert(p6);
  dg[y][from(x)].insert(p5);
  dg[x][from(y)].insert(p7);
  dg[k][from(j)].insert(p8);
  
  std::set<PrmTyp> cp;
  cp.insert(p1);
  cp.insert(p2);
  cp.insert(p3);
  cp.insert(p4);
  cp.insert(p5);
  cp.insert(p6);
  cp.insert(p7);
  
  dg::assert_equal(getRelevantPrms(dg, c), cp);
}

}

int main(int argc, char* argv[])
{
  unsigned int n=0;
  tc_map_prm_info::test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;

  return 0;
}
