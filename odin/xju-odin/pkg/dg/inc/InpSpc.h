#ifndef TPS_INPSPC
#define TPS_INPSPC
#ifdef BUILDING_DG
typedef struct _tps_InpSpc {
   tp_ISKind ISKind;
   tp_FilTyp FilTyp;
   tp_PrmTyp PrmTyp;
   tp_Str Str;
   bool IsEnvVar;
   tp_InpSpc InpSpc;
   tp_InpSpc Next;
   int Index;
   tp_InpSpc Link;
   }				tps_InpSpc;

#endif
#ifdef BUILDING_ODIN

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

#endif
