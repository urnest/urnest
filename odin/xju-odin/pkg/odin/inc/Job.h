#ifndef TPS_JOB
#define TPS_JOB

#include <odin/inc/Type.hh>

typedef struct _tps_Job {
   tp_JobID JobID;
   tp_FilHdr FilHdr;
   tp_FileName JobDirName;
   tp_FileName WarningFN;
   tp_FileName ErrorFN;
   bool Canceled;
   tp_Job Next;
   bool InUse;
} tps_Job;

#endif
