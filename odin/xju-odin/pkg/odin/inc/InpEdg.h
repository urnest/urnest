#ifndef TPS_SRCEDG
#define TPS_SRCEDG

#include <odin/inc/Type.hh>

typedef struct _tps_InpEdg {
   tp_InpSpc InpSpc;
   tp_InpKind InpKind;
   bool IsUserArg;
   tp_InpEdg Next;
   }				tps_InpEdg;

#endif
