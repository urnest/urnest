#ifndef TP_STATUS
#define TP_STATUS

enum Status {
  STAT_Unknown=1,
  STAT_Pending=2,

// Ready to perform its update action:
  STAT_Ready=3,

// Its update action is in progress
  STAT_Busy=4,
  STAT_SysAbort=5,
  STAT_NoFile=6,
  STAT_Circular=7,
  STAT_ElmCircular=8,
  STAT_Error=9,
  STAT_TgtValError=10,
  STAT_Warning=11,
  STAT_OK=12
};

#endif
