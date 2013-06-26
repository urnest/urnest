#include "inc/tc-map-prm-info.h"

#include <algorithm>
#include <iterator>
#include "inc/assert.h"


struct TcMapPrmInfo_
{
  tc_map_prm_info::DG dg_;
};

  
const TcMapPrmInfo* Build_TC_Map_PrmTypLst_Info(tp_FilTyp odg)
{
  TcMapPrmInfo* result=new TcMapPrmInfo;
  tc_map_prm_info::DG& dg(result->dg_);

  // build drv info
  for (tp_FilTyp FilTyp = odg; FilTyp != NIL; FilTyp = FilTyp->Link) {
    tc_map_prm_info::FromTyp from(FilTyp->FTName);
    for (tp_DrvEdg DrvEdg = FilTyp->DrvEdg;
         DrvEdg != 0;
         DrvEdg = DrvEdg_Next(DrvEdg)) {
      tc_map_prm_info::ToTyp to(DrvEdg->FilTyp->FTName);
      std::set<tc_map_prm_info::PrmTyp> prms;
      for (tp_PrmTypLst PrmTypElm = DrvEdg->PrmTypLst;
           PrmTypElm != DfltPrmTypLst;
           PrmTypElm = PrmTypElm->Next) {
        prms.insert(tc_map_prm_info::PrmTyp(PrmTypElm->PrmTyp->PTName));
      }
      dg[to][from]=prms;
    }
  }
  
  // build equivalence info

  for (tp_FilTyp FilTyp = odg; FilTyp != NIL; FilTyp = FilTyp->Link) {
    if (FilTyp->EqvEdg != 0) {
      std::set<tc_map_prm_info::ToTyp> x;
      x.insert(tc_map_prm_info::ToTyp(FilTyp->FTName));
      
      for (tp_EqvEdg EqvEdg = FilTyp->EqvEdg;
           EqvEdg != 0;
           EqvEdg = EqvEdg_Next(EqvEdg)) {
        x.insert(tc_map_prm_info::ToTyp(EqvEdg_FilTyp(EqvEdg)->FTName));
      }
      
      for(std::set<tc_map_prm_info::ToTyp>::const_iterator i=x.begin();
          i != x.end();
          ++i) {
        for(std::set<tc_map_prm_info::ToTyp>::const_iterator j=x.begin();
            j != x.end();
            ++j) {
          if (i != j) {
            dg[*i][tc_map_prm_info::FromTyp((*j)._)]=
              std::set<tc_map_prm_info::PrmTyp>();
          }
        }
      }
    }
  }

  // build cast info
  for (tp_FilTyp FilTyp = odg; FilTyp != NIL; FilTyp = FilTyp->Link) {
    for (tp_CastEdg CastEdg = FilTyp->CastEdg;
         CastEdg != 0;
         CastEdg = CastEdg_Next(CastEdg)) {
      dg[tc_map_prm_info::ToTyp(CastEdg_FilTyp(CastEdg)->FTName)][
        tc_map_prm_info::FromTyp(FilTyp->FTName)]=
        std::set<tc_map_prm_info::PrmTyp>();
    }
  }

  return result;
}

void Free_TC_Map_PrmTypLst_Info(const TcMapPrmInfo* x)
{
  delete x;
}


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



std::set<PrmTyp> getRelevantPrms(DG const& dg,
                                 ToTyp const& t) throw() {
  std::set<ToTyp> seen;
  std::set<PrmTyp> result;
  getRelevantPrms_(dg, t, seen,
                   std::inserter(result, result.begin()));
  return result;
}

}
