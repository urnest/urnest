#ifndef TPS_PRMFHDR
#define TPS_PRMFHDR

#include <odin/inc/Type.hh>

typedef struct _tps_PrmFHdr {
   tp_FilHdr FilHdr;
   tp_FilPrm FilPrm;
   tp_PrmFHdr Next;
   bool InUse;
   }				tps_PrmFHdr;

#endif
