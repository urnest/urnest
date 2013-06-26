#ifndef TC_MAP_PRM_INFO_H
#define TC_MAP_PRM_INFO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "inc/GMC.h"
#include "inc/CastEdg.h"
#include "inc/DPType_.h"
#include "inc/DrvEdg.h"
#include "inc/EqvEdg.h"
#include "inc/FilTyp.h"

#include "inc/PrmTypLst.h"
#include "inc/PrmTyp.h"
#include "inc/Var.hh"

  struct TcMapPrmInfo_;
  typedef struct TcMapPrmInfo_ TcMapPrmInfo;

  // build info needed for Set_TC_Map_DrvEdg_PrmTypLst below
  const TcMapPrmInfo* Build_TC_Map_PrmTypLst_Info(tp_FilTyp dg);

  // free storage returned by Build_TC_Map_PrmTypLst_Info above
  void Free_TC_Map_PrmTypLst_Info(const TcMapPrmInfo* x);

  // set PrmTypLst, for each DrvEdg in dg that has 
  // FilTyp.Tool->TClass==TC_Map, to the list of only those parameters
  // that are involved in a derivation that can lead to the
  // derivation's FilTyp
  void Set_TC_Map_DrvEdg_PrmTypLsts(const TcMapPrmInfo* x,
                                    tp_FilTyp dg);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

#include <map>
#include <set>
#include <string>
#include "inc/tagged.h"

namespace tc_map_prm_info
{

struct ToTypTag{};
typedef dg::Tagged<std::string,ToTypTag> ToTyp;

struct FromTypTag{};
typedef dg::Tagged<std::string,FromTypTag> FromTyp;

struct PrmTypTag{};
typedef dg::Tagged<std::string,PrmTypTag> PrmTyp;

typedef std::map<ToTyp, std::map<FromTyp, std::set<PrmTyp> > > DG;

// only parameters that can ever be used in deriving to type t
std::set<PrmTyp> getRelevantPrms(DG const& dg,
                                 ToTyp const& t) throw();
  
}

#endif

#endif
