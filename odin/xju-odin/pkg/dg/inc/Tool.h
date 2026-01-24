#ifndef TPS_TOOL
#define TPS_TOOL
#ifdef BUILDING_DG
typedef struct _tps_Tool {
   tp_TClass TClass;
   tp_FilTyp FilTyp;
   tp_InpEdg InpEdg;
   tp_InpEdg HomInpEdg;
   tp_PrmTypLst BasePrmTypLst, PrmTypLst;
   tp_EnvVarLst EnvVarLst;
   tp_Package Package;
   bool Flag;
   int Defer;
   int Index;
   tp_Tool Link;
   }				tps_Tool;

#endif
#ifdef BUILDING_ODIN

typedef struct _tps_Tool {
   tp_TClass TClass;
   tp_InpEdg InpEdg;
   tp_PrmTypLst PrmTypLst;
   tp_EnvVarLst EnvVarLst;
   tp_Package Package;
   }				tps_Tool;

#endif

#endif
