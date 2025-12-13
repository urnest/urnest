#ifndef TPS_PRMTYP
#define TPS_PRMTYP

#include <odin/inc/Type.hh>

typedef struct _tps_PrmTyp {
   tp_PTName PTName;
   tp_Desc Desc;
   int HelpLevel;
   tp_FilTyp FilTyp;
   tp_LocPVal RootLocPVal;
   tp_LocHdr StrDirLocHdr;
   bool Mark;
   int IPrmTyp;
   }				tps_PrmTyp;

#endif
