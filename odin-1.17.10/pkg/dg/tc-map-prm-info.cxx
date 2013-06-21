#include "inc/tc-map-prm-info.h"

#include <algorithm>
#include <iterator>
#include "inc/assert.h"

namespace tc_map_prm_info
{

namespace
{

void getRelevantPrms_(DG const& dg,
                      ToTyp const& t,
                      std::set<ToTyp>& seen,
                      std::insert_iterator<std::set<PrmTyp> > result)
  throw() {
  dg::assert_equal(seen.find(t), seen.end());
  dg::assert_not_equal(dg.find(t), dg.end());
  
  seen.insert(t);
  
  std::map<FromTyp, std::set<PrmTyp> >::const_iterator i;
  for(i = (*dg.find(t)).second.begin();
      i != (*dg.find(t)).second.end();
      ++i) {
    std::copy((*i).second.begin(), (*i).second.end(), result);
    ToTyp const tt((*i).first._);
    if (seen.find(tt)==seen.end()) {
      getRelevantPrms_(dg, tt, seen, result);
    }
  }
}

}

}

std::set<PrmTyp> getRelevantPrms(DG const& dg,
                                 ToTyp const& t) throw() {
  std::set<ToTyp> seen;
  std::set<PrmTyp> result;
  getRelevantPrms_(dg, t, seen,
                   std::inserter(result, result.begin()));
  return result;
}

}
