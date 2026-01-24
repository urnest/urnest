#ifndef TPS_FILTYP
#define TPS_FILTYP
#ifdef BUILDING_DG
typedef struct _tps_FilTyp {
   tp_FTClass FTClass;
   tp_FTName FTName;
   tp_Desc Desc;
   int HelpLevel;
   tp_FilTyp ArgFilTyp;
   tp_Tool Tool;
   tp_MemEdg MemEdg;
   tp_InpEdg InpLink;
   tp_EqvEdg EqvEdg;
   tp_EqvEdg FrmEqvEdg;
   tp_CastEdg CastEdg;
   tp_CastEdg FrmCastEdg;
   tp_DrvEdg DrvEdg;
   tp_DrvEdg FrmDrvEdg;
   tp_PrmTypLst MapPrmTypLst;
   bool IsCopy, IsGrouping, IsGroupingInput;
   tp_Pos Pos;
   bool Flag;
   bool Done;
   bool Active;
   bool Reach;
   bool NewReach;
   tp_FilTyp NextOrder, NextBackOrder;
   int Index;
   tp_FilTyp Link, BackLink;
   }				tps_FilTyp;

#endif
#ifdef BUILDING_ODIN

typedef struct _tps_FilTyp {
   tp_FTClass FTClass;
   tp_FTName FTName;
   tp_Desc Desc;
   int HelpLevel;
   tp_FilTyp ArgFilTyp;
   tp_Tool Tool;
   tp_MemEdg MemEdg;
   tp_EqvEdg EqvEdg;
   tp_CastEdg CastEdg;
   tp_DrvEdg DrvEdg;
   tp_PrmTypLst MapPrmTypLst;
   bool IsCopy, IsGrouping, IsGroupingInput;
   bool Reach;
   bool Mark;
   tp_Pos Pos;
   int IFilTyp;
   }				tps_FilTyp;

#endif

#endif
