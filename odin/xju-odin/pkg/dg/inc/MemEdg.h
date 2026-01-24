#ifndef TPS_MEMEDG
#define TPS_MEMEDG
#ifdef BUILDING_DG
typedef struct _tps_MemEdg {
   tp_FilTyp FilTyp;
   tp_MemEdg Next;
   int Index;
   tp_MemEdg Link;
   }				tps_MemEdg;

#endif
#ifdef BUILDING_ODIN


typedef struct _tps_MemEdg {
   tp_FilTyp FilTyp;
   tp_MemEdg Next;
   }				tps_MemEdg;

#endif

#endif
