#ifndef TPS_ENVVAR
#define TPS_ENVVAR
#ifdef BUILDING_DG
typedef struct _tps_EnvVar {
   tp_Str Name;
   tp_Desc Desc;
   int HelpLevel;
   tp_Str Default;
   bool IsFile;
   int Index;
   tp_EnvVar Link;
   }				tps_EnvVar;

#endif
#ifdef BUILDING_ODIN

#include <odin/inc/Type.hh>

typedef struct _tps_EnvVar {
   tp_Str Name;
   tp_Desc Desc;
   int HelpLevel;
   tp_Str Default;
   bool IsFile;
   }				tps_EnvVar;

#endif

#endif
