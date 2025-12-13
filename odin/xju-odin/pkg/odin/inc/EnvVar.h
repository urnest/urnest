#ifndef TPS_ENVVAR
#define TPS_ENVVAR

#include <odin/inc/Type.hh>

typedef struct _tps_EnvVar {
   tp_Str Name;
   tp_Desc Desc;
   int HelpLevel;
   tp_Str Default;
   bool IsFile;
   }				tps_EnvVar;

#endif
