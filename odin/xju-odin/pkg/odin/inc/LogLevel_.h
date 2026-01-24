#ifndef TP_LOGLEVEL
#define TP_LOGLEVEL

enum LogLevel {
  LOGLEVEL_Init=0,
  LOGLEVEL_Abort=1,
  LOGLEVEL_Verbose=1,
  LOGLEVEL_OdinCommand=2,
  LOGLEVEL_Warnings=3,
  LOGLEVEL_Errors=3,
  LOGLEVEL_Generate=4,
  LOGLEVEL_Cancel=4,
  LOGLEVEL_AbortDone=4,
  LOGLEVEL_ExecLine=5,
  LOGLEVEL_IntGenerate=5,
  LOGLEVEL_NoFile=5,
  LOGLEVEL_Target=5,
  LOGLEVEL_Circular=6,
  LOGLEVEL_Status=7,
  LOGLEVEL_IgnorePrm=7,
  LOGLEVEL_Queue=7,
  LOGLEVEL_Process=7,
  LOGLEVEL_Create=7,
  LOGLEVEL_MAX=7
};

#endif
