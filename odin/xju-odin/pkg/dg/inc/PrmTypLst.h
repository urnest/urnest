#ifndef TPS_PRMTYPLST
#define TPS_PRMTYPLST
#ifdef BUILDING_DG
typedef struct _tps_PrmTypLst {
   tp_PrmTyp PrmTyp;
   tp_PrmTypLst Next;
   tp_PrmTypLst Brother;
   tp_PrmTypLst Son;
   int Index;
   tp_PrmTypLst Link;
   }				tps_PrmTypLst;

#endif
#ifdef BUILDING_ODIN


typedef struct _tps_PrmTypLst {
   tp_PrmTyp PrmTyp;
   tp_PrmTypLst Next;
   }				tps_PrmTypLst;

#endif

#endif
