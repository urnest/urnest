#ifndef TPS_FILSRC
#define TPS_FILSRC

#include <odin/inc/InpInf.h>

typedef struct _tps_FilInp {
   tp_LocInp LocInp;
   tp_Item NextHash;

   tps_InpInf InpInf;

   int Cnt;
   tp_FilInp PrevFree, NextFree;

   bool Modified;
   tp_FilInp NextMod;
   }				tps_FilInp;

#endif
