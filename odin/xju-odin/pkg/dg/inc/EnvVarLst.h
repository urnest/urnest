#ifndef TPS_ENVVARLST
#define TPS_ENVVARLST
#ifdef BUILDING_DG
typedef struct _tps_EnvVarLst {
   tp_EnvVar EnvVar;
   tp_EnvVarLst Next;
   tp_EnvVarLst Brother;
   tp_EnvVarLst Son;
   int Index;
   tp_EnvVarLst Link;
   }				tps_EnvVarLst;

#endif
#ifdef BUILDING_ODIN


typedef struct _tps_EnvVarLst {
   tp_EnvVar EnvVar;
   tp_EnvVarLst Next;
   }				tps_EnvVarLst;

#endif

#endif
