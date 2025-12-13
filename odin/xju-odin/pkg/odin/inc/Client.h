#ifndef TPS_CLIENT
#define TPS_CLIENT

#include <odin/inc/Type.hh>

typedef struct _tps_Client {
   tp_ClientID ClientID;
   tp_FHLst CWDFilHdrS;
   bool KeepGoing;
   int ErrLevel, WarnLevel, LogLevel, HelpLevel;
   tp_FilHdr FilHdr;
   bool NeedsData;
   tp_FHLst ToDo, LastToDo;
   int NumJobs, MaxJobs;
   tp_Job Job;
   bool Interrupted;
   tp_Client Next;
   bool InUse;
   }				tps_Client;

#endif
