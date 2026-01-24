#ifndef TPS_CASTEDG
#define TPS_CASTEDG
#ifdef BUILDING_DG
typedef struct _tps_CastEdg {
   tp_FilTyp FrmFilTyp;
   tp_FilTyp FilTyp;
   tp_CastEdg FrmNext;
   tp_CastEdg Next;
   int Index;
   tp_CastEdg Link;
   }				tps_CastEdg;

#endif
#ifdef BUILDING_ODIN

typedef struct _tps_CastEdg {
   tp_FilTyp FilTyp;
   tp_CastEdg Next;
   }				tps_CastEdg;

#endif

#endif
