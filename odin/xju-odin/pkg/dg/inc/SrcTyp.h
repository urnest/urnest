#ifndef TPS_SRCTYP
#define TPS_SRCTYP
#ifdef BUILDING_DG
typedef struct _tps_SrcTyp {
   tp_Pattern Pattern;
   bool IsPrefix;
   tp_FilTyp FilTyp;
   tp_SrcTyp Link;
   }				tps_SrcTyp;

#endif
#ifdef BUILDING_ODIN

typedef struct _tps_SrcTyp {
   tp_Pattern Pattern;
   bool IsPrefix;
   tp_FilTyp FilTyp;
   }				tps_SrcTyp;

#endif

#endif
