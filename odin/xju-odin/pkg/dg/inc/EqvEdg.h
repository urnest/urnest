#ifndef TPS_EQVEDG
#define TPS_EQVEDG
#ifdef BUILDING_DG
typedef struct _tps_EqvEdg {
   tp_FilTyp FrmFilTyp;
   tp_FilTyp FilTyp;
   tp_EqvEdg FrmNext;
   tp_EqvEdg Next;
   int Index;
   tp_EqvEdg Link;
   }				tps_EqvEdg;

#endif
#ifdef BUILDING_ODIN


typedef struct _tps_EqvEdg {
   tp_FilTyp FilTyp;
   tp_EqvEdg Next;
   }				tps_EqvEdg;

#endif

#endif
