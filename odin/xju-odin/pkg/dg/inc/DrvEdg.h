#ifndef TPS_DRVEDG
#define TPS_DRVEDG
#ifdef BUILDING_DG
typedef struct _tps_DrvEdg {
   tp_FilTyp FrmFilTyp;
   tp_FilTyp FilTyp;
   tp_PrmTypLst PrmTypLst;
   tp_DrvEdg FrmNext;
   tp_DrvEdg Next;
   int Index;
   tp_DrvEdg Link;
   bool Flag;
   }				tps_DrvEdg;

#endif
#ifdef BUILDING_ODIN


typedef struct _tps_DrvEdg {
   tp_FilTyp FilTyp;
   tp_PrmTypLst PrmTypLst;
   tp_DrvEdg Next;
   }				tps_DrvEdg;

#endif

#endif
