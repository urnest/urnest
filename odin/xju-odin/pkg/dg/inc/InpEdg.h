#ifndef TPS_INPEDG
#define TPS_INPEDG
#ifdef BUILDING_DG
typedef struct _tps_InpEdg {
   tp_FilTyp FilTyp;
   tp_InpSpc InpSpc;
   tp_InpKind InpKind;
   bool IsUserArg;
   tp_InpEdg Next;
   tp_Tool Tool;
   tp_InpEdg InpLink;
   bool Done;
   int Index;
   tp_InpEdg Link;
   }				tps_InpEdg;

#endif
#ifdef BUILDING_ODIN

#include <odin/inc/Type.hh>

typedef struct _tps_InpEdg {
   tp_InpSpc InpSpc;
   tp_InpKind InpKind;
   bool IsUserArg;
   tp_InpEdg Next;
   }				tps_InpEdg;

#endif

#endif
