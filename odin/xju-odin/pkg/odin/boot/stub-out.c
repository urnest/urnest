#include "inc/GMC.h"

extern bool IPC_Do_Return;
extern int *IPC_IArg1, *IPC_IArg2, *IPC_IArg3;
extern tp_Str IPC_SArg1, IPC_SArg2, IPC_SArg3;

#ifndef CLIENT_ONLY
void
Add_BuildArg(
   tp_FileName FileName
   )
   tp_FileName FileName
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Add_BuildArg(FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 2);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
Do_Build(
   tp_JobID JobID,
   tp_FileName JobDirName,
   tp_FileName LogFileName
   )
   tp_JobID JobID
   tp_FileName JobDirName
   tp_FileName LogFileName
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Do_Build(JobID, JobDirName, LogFileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 3);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, JobID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, JobDirName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, LogFileName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
Abort_Build(
   tp_JobID JobID
   )
   tp_JobID JobID
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Abort_Build(JobID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 4);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, JobID);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
Do_MakeReadOnly(
   tp_FileName FileName
   )
   tp_FileName FileName
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Do_MakeReadOnly(FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 5);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
ErrMessage(
   char* Message
   )
   char* Message
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_ErrMessage(Message);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 6);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, Message);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
LogMessage(
   char* Message
   )
   char* Message
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_LogMessage(Message);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 7);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, Message);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
FileErrMessage(
   tp_FileName FileName
   )
   tp_FileName FileName
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_FileErrMessage(FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 8);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef CLIENT_ONLY
void
Next_OdinFile(
   tp_Str OdinExpr,
   int ID
   )
   tp_Str OdinExpr
   int ID
{
   bool IPC_Abort;

#ifndef SERVER_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Next_OdinFile(OdinExpr, ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 9);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef SERVER_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_UseCount(
   int* CountPtr
   )
   int* CountPtr
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_UseCount(CountPtr);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 10);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Int(&IPC_Abort, CountPtr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_CurSize(
   int* SizePtr
   )
   int* SizePtr
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_CurSize(SizePtr);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 11);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Int(&IPC_Abort, SizePtr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
ShutDown()
{
   bool IPC_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_ShutDown();
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 12);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Banner(
   tp_Str Str
   )
   tp_Str Str
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Banner(Str);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 13);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, Str);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Do_Interrupt(
   bool InterruptFlag
   )
   bool InterruptFlag
{
   bool IPC_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Do_Interrupt(InterruptFlag);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 14);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, InterruptFlag);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Do_Alias(
   tp_FileName FileName,
   bool ForceFlag
   )
   tp_FileName FileName
   bool ForceFlag
{
   bool IPC_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Do_Alias(FileName, ForceFlag);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 15);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ForceFlag);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Alias(
   tp_FileName OutFileName,
   tp_FileName FileName
   )
   tp_FileName OutFileName
   tp_FileName, FileName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Alias(OutFileName, FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 16);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, OutFileName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Job_Done(
   tp_JobID JobID,
   bool Abort
   )
   tp_JobID JobID
   bool Abort
{
   bool IPC_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Job_Done(JobID, Abort);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 17);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, JobID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, Abort);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Test(
   tp_FileName FileName
   )
   tp_FileName, FileName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Test(FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 18);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Test_All()
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Test_All();
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 19);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_OdinFile(
   tp_FileName FileName,
   tp_Status* StatusPtr,
   bool* ExecFlagPtr,
   tp_Str OdinExpr,
   bool NeedsData)
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      FORBIDDEN(IPC_Do_Return);
      FORBIDDEN(IPC_SArg1 != NIL);
      IPC_SArg1 = FileName;
      FORBIDDEN(IPC_IArg2 != NIL);
      IPC_IArg2 = StatusPtr;
      FORBIDDEN(IPC_IArg3 != NIL);
      IPC_IArg3 = ExecFlagPtr;
      Local_Get_OdinFile(OdinExpr, NeedsData);
      if (!IPC_Do_Return) {
         IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
         FORBIDDEN(IPC_Cmd_Abort);
         FORBIDDEN(!IPC_Do_Return); }/*if*/;
      IPC_Do_Return = false;
      IPC_SArg1 = NIL;
      IPC_IArg2 = NIL;
      IPC_IArg3 = NIL;
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 20);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, NeedsData);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Read_Int(&IPC_Abort, StatusPtr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Read_Int(&IPC_Abort, ExecFlagPtr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_CWD(
   tp_FileName FileName
   )
   tp_FileName, FileName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_CWD(FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 21);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Push_Context(
   tp_FileName DirName,
   tp_FileName FileName
   )
   tp_FileName DirName
   tp_FileName, FileName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Push_Context(DirName, FileName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 22);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, FileName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, DirName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Pop_Context(
   tp_FileName DirName
   )
   tp_FileName DirName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Pop_Context(DirName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 23);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, DirName);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_KeepGoing(
   int Flag
   )
   int, Flag
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_KeepGoing(Flag);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 24);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, Flag);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_ErrLevel(
   int ErrLevel
   )
   int, ErrLevel
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_ErrLevel(ErrLevel);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 25);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ErrLevel);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_WarnLevel(
   int WarnLevel
   )
   int, WarnLevel
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_WarnLevel(WarnLevel);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 26);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, WarnLevel);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_LogLevel(
   tp_LogLevel LogLevel
   )
   tp_LogLevel, LogLevel
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_LogLevel(LogLevel);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 27);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, LogLevel);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_HelpLevel(
   int HelpLevel
   )
   int, HelpLevel
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_HelpLevel(HelpLevel);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 28);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, HelpLevel);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_Debug(
   tp_Str DebugName
   )
   tp_Str, DebugName
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_Debug(DebugName);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 29);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, DebugName);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Set_MaxJobs(
   int MaxJobs
   )
   int, MaxJobs
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Set_MaxJobs(MaxJobs);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 30);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, MaxJobs);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Redo(
   tp_Str OdinExpr
   )
   tp_Str, OdinExpr
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Redo(OdinExpr);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 31);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
OdinExpr_ID(
   int* IDPtr,
   tp_Str OdinExpr
   )
   int* IDPtr
   tp_Str, OdinExpr
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_OdinExpr_ID(IDPtr, OdinExpr);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 32);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Int(&IPC_Abort, IDPtr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
ID_OdinExpr(
   tp_Str OdinExpr,
   int ID
   )
   tp_Str OdinExpr
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_ID_OdinExpr(OdinExpr, ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 33);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
ID_LongOdinExpr(
   tp_Str OdinExpr,
   int ID
   )
   tp_Str OdinExpr
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_ID_LongOdinExpr(OdinExpr, ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 34);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Status(
   tp_Status* StatusPtr,
   tp_Status* ElmStatusPtr,
   int ID
   )
   tp_Status* StatusPtr
   tp_Status* ElmStatusPtr
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Status(StatusPtr, ElmStatusPtr, ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 35);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
   IPC_Read_Int(&IPC_Abort, StatusPtr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Read_Int(&IPC_Abort, ElmStatusPtr);
   if (IPC_Abort) IPC_Do_Abort();
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Elements(
   int ID
   )
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Elements(ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 36);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_ElementsOf(
   int ID
   )
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_ElementsOf(ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 37);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Inputs(
   int ID
   )
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Inputs(ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 38);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_Outputs(
   int ID
   )
   int, ID
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_Outputs(ID);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 39);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Int(&IPC_Abort, ID);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif

#ifndef SERVER_ONLY
void
Get_DPath(
   tp_Str OdinExpr
   )
   tp_Str, OdinExpr
{
   bool IPC_Abort;

   bool IPC_Cmd_Abort;

#ifndef CLIENT_ONLY
   if (IsServer && Is_LocalClient(CurrentClient)) {
      Local_Get_DPath(OdinExpr);
   }else{
#endif
   IPC_Write_Int(&IPC_Abort, 40);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Write_Str(&IPC_Abort, OdinExpr);
   if (IPC_Abort) IPC_Do_Abort();
   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);
   FORBIDDEN(IPC_Cmd_Abort);
   FORBIDDEN(!IPC_Do_Return);
   IPC_Do_Return = false;
#ifndef CLIENT_ONLY
   };
#endif
   }
#endif
