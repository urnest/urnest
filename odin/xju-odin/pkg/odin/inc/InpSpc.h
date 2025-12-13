#ifndef TPS_INPSPC
#define TPS_INPSPC

#include <odin/inc/Type.hh>

typedef struct _tps_InpSpc {
   tp_ISKind ISKind;
   tp_FilTyp FilTyp;
   tp_PrmTyp PrmTyp;
   tp_Str Str;
   bool IsEnvVar;
   tp_InpSpc InpSpc;
   tp_InpSpc Next;
   }				tps_InpSpc;

#endif
