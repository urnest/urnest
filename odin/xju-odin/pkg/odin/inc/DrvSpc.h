!!#ifndef TPS_DRVSPC
#define TPS_DRVSPC

typedef struct _tps_DrvSpc {
   tp_FilPrm FilPrm;
   tp_FKind FKind;
   tp_FilTyp FilTyp;
   tp_Key Key;
   tp_FilHdr FilHdr;
   tp_DrvSpc Next;
   bool InUse;
   }				tps_DrvSpc;

#endif
