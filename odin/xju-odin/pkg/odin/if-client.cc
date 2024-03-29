/*
Copyright (C) 1991 Geoffrey M. Clemm

This file is part of the Odin system.

The Odin system is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation (see the file COPYING).

The Odin system is distributed WITHOUT ANY WARRANTY, without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

geoff@boulder.colorado.edu
*/
extern "C"
{
#include "inc/GMC.h"
#include "inc/Client.h"
#include "inc/FileName.h"
#include "inc/Flag_.h"
#include "inc/InpKind_.h"
#include "inc/Job.h"
#include "inc/LogLevel_.h"
#include "inc/Status_.h"
#include "inc/Str.h"
#include "inc/Func.hh"
}

tp_Client	FirstClient = NIL;
tp_Client	CurrentClient = NIL;
tp_Client	LocalClient = NIL;


typedef struct _tps_Pending {
   tp_FilHdr FilHdr;
   tp_InpKind InpKind;
   tp_Date ModDate, ElmModDate, ElmNameModDate;
   tp_Pending Next;
   }		tps_Pending;

static tp_Pending	FreePendingS = NIL;
static int		num_PendingS = 0;

static tp_Pending	FirstPending = NIL;
static tp_Pending	LastPending = NIL;


static int		UseCount = 0;

static tp_Client	FreeClient = NIL;
static int		num_ClientS = 0;


static tp_Job		FreeJob = NIL;
static int		num_JobS = 0;


typedef struct _tps_FHLst {
   tp_FilHdr FilHdr;
   tp_FHLst Next;
   }		tps_FHLst;

static tp_FHLst	FreeFHLstS = NIL;
int		num_FHLstS = 0;


void
Push_Pending(
   GMC_ARG(tp_FilHdr, FilHdr),
   GMC_ARG(tp_InpKind, InpKind)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
   GMC_DCL(tp_InpKind, InpKind)
{
   tp_Pending Pending;

   if (FilHdr_Flag(FilHdr, FLAG_Pending)) {
      Ret_FilHdr(FilHdr);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Pending);
   Do_Log("Pushing pending-flagged to pending: ", FilHdr, LOGLEVEL_Queue);
   /*select*/{
      if (FreePendingS == NIL) {
	 Pending = (tp_Pending)malloc(sizeof(tps_Pending));
	 num_PendingS += 1;
      }else{
	 Pending = FreePendingS;
	 FreePendingS = FreePendingS->Next; };}/*select*/;
   Pending->FilHdr = FilHdr;
   Pending->InpKind = InpKind;
   Pending->ModDate = FilHdr_ModDate(FilHdr);
   Pending->ElmModDate = FilHdr_ElmModDate(FilHdr);
   Pending->ElmNameModDate = FilHdr_ElmNameModDate(FilHdr);
   Pending->Next = NIL;
   /*select*/{
      if (FirstPending == NIL) {
	 FirstPending = Pending;
      }else{
	 LastPending->Next = Pending; };}/*select*/;
   LastPending = Pending;
   }/*Push_Pending*/


static boolean
IsAllPendingUpToDate(GMC_ARG_VOID)
{
   tp_Pending Pending;

   for (Pending = FirstPending; Pending != NIL; Pending = Pending->Next) {
      if (!IsAllUpToDate(Pending->FilHdr, Pending->InpKind)) {
	 return FALSE; }/*if*/; }/*for*/;
   return TRUE;
   }/*IsAllPendingUpToDate*/


// Gather reqs for entries for everything on Pending list, putting
// those reqs on the to-do or pending list.
static void
GetAllPending(GMC_ARG_VOID)
{
   tp_Pending Pending;
   tp_Status Status;

   for (Pending = FirstPending; Pending != NIL; Pending = Pending->Next) {
     /* Set Push_ToDo insertion point to head of ToDo list */
     // REVISIT: why do we do that here? This function does not put anything
     // into ToDo, only int Pending? Should only do this is Clr, which
     // transfers Pending to ToDo?
      CurrentClient->LastToDo = NIL;
      Do_Log("{ Gather all reqs of pending ", Pending->FilHdr, LOGLEVEL_Queue);
      GetAllReqs(Pending->FilHdr, Pending->InpKind);
      Status = FilHdr_MinStatus(Pending->FilHdr, Pending->InpKind);
      /*select*/{
	 if (Pending->ModDate != FilHdr_ModDate(Pending->FilHdr)
	     || Pending->ElmModDate != FilHdr_ElmModDate(Pending->FilHdr)
	     || (Pending->ElmNameModDate
		 != FilHdr_ElmNameModDate(Pending->FilHdr))) {
	    Pending->ModDate = FilHdr_ModDate(Pending->FilHdr);
	    Pending->ElmModDate = FilHdr_ElmModDate(Pending->FilHdr);
	    Pending->ElmNameModDate = FilHdr_ElmNameModDate(Pending->FilHdr);
	    Do_Log("Changed cycle input:", Pending->FilHdr, LOGLEVEL_Circular);
	    Broadcast(Pending->FilHdr, STAT_Unknown);
	 }else if (Status != STAT_Unknown && Status <= STAT_TgtValError
		   && !Is_PendingReadyOrBusy_Status(Status)) {
	    Broadcast(Pending->FilHdr, STAT_TgtValError);
	    };
         }/*select*/;
      Do_Log("} Gather all reqs of pending ", Pending->FilHdr, LOGLEVEL_Queue);
      }/*for*/;
   }/*GetAllPending*/


static boolean
Clr_Pending()
{
   tp_FilHdr FilHdr;
   tp_Pending Pending, LastPending;

   boolean result = TRUE;

   if (FirstPending == NIL) {
      return result; }/*if*/;
   for (Pending = FirstPending; Pending != NIL; Pending = Pending->Next) {
      Do_Log("{ Pop from pending: ", Pending->FilHdr, LOGLEVEL_Queue);
      LastPending = Pending;
      FilHdr = Pending->FilHdr;
      /*select*/{
	 if (!IsAllDone(FilHdr, Pending->InpKind)) {
	    result = FALSE;
	    Broadcast(FilHdr, STAT_Pending);
	 }else if (Is_TgtValErrStatus(FilHdr)) {
	    Broadcast(FilHdr, STAT_TgtValError); };}/*select*/;
      Do_Log("Clearing pnd flag of", FilHdr, LOGLEVEL_Status);
      Clr_Flag(FilHdr, FLAG_Pending);
      Do_Log("} Pop from pending: ", Pending->FilHdr, LOGLEVEL_Queue);
      Ret_FilHdr(FilHdr); }/*for*/;
   LastPending->Next = FreePendingS;
   FreePendingS = FirstPending;
   FirstPending = NIL;
   return result;
   }/*Clr_Pending*/


static tp_FHLst
New_FHLst(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_FHLst NewFHLst;

   FORBIDDEN(FilHdr == ERROR);
   /*select*/{
      if (FreeFHLstS == NIL) {
	 NewFHLst = (tp_FHLst)malloc(sizeof(tps_FHLst));
	 num_FHLstS += 1;
      }else{
	 NewFHLst = FreeFHLstS;
	 FreeFHLstS = FreeFHLstS->Next; };}/*select*/;
   NewFHLst->FilHdr = FilHdr;
   NewFHLst->Next = NIL;
   return NewFHLst;
   }/*New_FHLst*/


static void
Ret_FHLst(
   GMC_ARG(tp_FHLst, FHLst)
   )
   GMC_DCL(tp_FHLst, FHLst)
{
   tp_FHLst TailFHLst;

   if (FHLst == NIL) {
      return; }/*if*/;
   TailFHLst = FHLst;
   while (TRUE) {
      Ret_FilHdr(TailFHLst->FilHdr);
      TailFHLst->FilHdr = NIL;
      if (TailFHLst->Next == NIL) {
	 TailFHLst->Next = FreeFHLstS;
	 FreeFHLstS = FHLst;
	 return; }/*if*/;
      TailFHLst = TailFHLst->Next; }/*while*/;
   }/*Ret_FHLst*/


void
Ret_ToDo(GMC_ARG_VOID)
{
   Ret_FHLst(CurrentClient->ToDo);
   CurrentClient->ToDo = NIL;
   CurrentClient->LastToDo = NIL;
   }/*Ret_ToDo*/


tp_Client
New_Client(
   GMC_ARG(tp_ClientID, ClientID)
   )
   GMC_DCL(tp_ClientID, ClientID)
{
   tp_Client Client;

   ;/*select*/{
      if (FreeClient == NIL) {
	 Client = (tp_Client)malloc(sizeof(tps_Client));
	 num_ClientS += 1;
	 Client->InUse = FALSE;
      }else{
	 Client = FreeClient;
	 FreeClient = FreeClient->Next; };}/*select*/;

   Client->ClientID = ClientID;
   Client->KeepGoing = FALSE;
   Client->ErrLevel = 0;
   Client->WarnLevel = 0;
   Client->LogLevel = 0;
   Client->HelpLevel = 0;
   Client->CWDFilHdrS = NIL;
   Client->FilHdr = NIL;
   Client->ToDo = NIL;
   Client->LastToDo = NIL;
   Client->NumJobs = 0;
   Client->MaxJobs = 0;
   Client->Job = NIL;
   Client->Interrupted = FALSE;
   Client->Next = FirstClient; FirstClient = Client;
   FORBIDDEN(Client->InUse);
   Client->InUse = TRUE;
   return Client;
   }/*New_Client*/


void
Activate_Client(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   tp_Client OldCurrentClient;

   OldCurrentClient = CurrentClient;
   CurrentClient = Client;
   Push_ContextFilHdr(Copy_FilHdr(OdinDirFilHdr));
   CurrentClient = OldCurrentClient;
   UseCount += 1;
   }/*Activate_Client*/


void
Ret_Client(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   FORBIDDEN(!Client->InUse);
   FORBIDDEN(Is_LocalClient(Client) && UseCount > 1);
   Client->InUse = FALSE;
   }/*Ret_Client*/


void
Purge_Clients(GMC_ARG_VOID)
{
   boolean Changed;
   tp_Client PrevClient, Client, NextClient, OldCurrentClient;
   tp_Job Job, NextJob;

   Changed = FALSE;
   PrevClient = NIL;
   Client = FirstClient;
   while (Client != NIL) {
      NextClient = Client->Next;
      /*select*/{
	 if (Client->InUse) {
	    PrevClient = Client;
	 }else{
	    Changed = TRUE;
	    Client->Next = FreeClient;
	    FreeClient = Client;
	    /*select*/{
	       if (PrevClient == NIL) {
		  FORBIDDEN(FirstClient != Client);
		  FirstClient = NextClient;
	       }else{
		  PrevClient->Next = NextClient; };}/*select*/;
	    IPC_Close(Client->ClientID);
	    Client->ClientID = -1;
	    UseCount -= 1;
	    Set_Client_FilHdr(Client, (tp_FilHdr)NIL, FALSE);

	    OldCurrentClient = CurrentClient;
	    CurrentClient = Client;
	    Job = CurrentClient->Job;
	    while (Job != NIL) {
	       NextJob = Job->Next;
	       Local_Job_Done(Job->JobID, TRUE);
	       Job = NextJob; }/*while*/;
	    FORBIDDEN(CurrentClient->Job != NIL);
	    Ret_ToDo();
	    while (CurrentClient->CWDFilHdrS != NIL) {
	       Pop_ContextFilHdr(); }/*while*/;
	    CurrentClient = OldCurrentClient; };}/*select*/;
      Client = NextClient; }/*while*/;
   if (Changed) {
      if (!IsClient && UseCount == 1) {
	 Exit(0); }/*if*/; }/*if*/;
   }/*Purge_Clients*/


boolean
Is_ActiveClient(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return (Client->InUse);
   }/*Is_ActiveClient*/


int
Client_FD(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return (int)Client->ClientID; }/*Client_FD*/


boolean
Client_Interrupted(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->Interrupted; }/*Client_Interrupted*/


void
Set_Client_Interrupted(
   GMC_ARG(tp_Client, Client),
   GMC_ARG(boolean, Flag)
   )
   GMC_DCL(tp_Client, Client)
   GMC_DCL(boolean, Flag)
{
   FORBIDDEN(Client == NIL);
   Client->Interrupted = Flag;
   }/*Set_Client_Interrupted*/


boolean
Client_KeepGoing(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->KeepGoing; }/*Client_KeepGoing*/


int
Client_ErrLevel(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->ErrLevel; }/*Client_ErrLevel*/


int
Client_WarnLevel(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->WarnLevel; }/*Client_WarnLevel*/


tp_LogLevel
Client_LogLevel(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->LogLevel; }/*Client_LogLevel*/


tp_FilHdr
Client_FilHdr(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Copy_FilHdr(Client->FilHdr);
   }/*Client_FilHdr*/


void
Set_Client_FilHdr(
   GMC_ARG(tp_Client, Client),
   GMC_ARG(tp_FilHdr, FilHdr),
   GMC_ARG(boolean, NeedsData)
   )
   GMC_DCL(tp_Client, Client)
   GMC_DCL(tp_FilHdr, FilHdr)
   GMC_DCL(boolean, NeedsData)
{
   if (Client->FilHdr != NIL) {
      Ret_FilHdr(Client->FilHdr); }/*if*/;
   Client->FilHdr = Copy_FilHdr(FilHdr);
   Client->NeedsData = NeedsData;
   }/*Set_Client_FilHdr*/


boolean
Client_NeedsData(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->NeedsData;
   }/*Client_NeedsData*/


// Put entries to update CurrentClient's target onto CurrentClient's ToDo list.
//
// Note this might not be all entries required to update CurrentClient's
// target, since some entries might need to be completed to determine
// further required updates.
//
// Note that "pending" is a temporary staging list used by Push_AllReqs et al
// and is cleared before Push_AllReqs returns.
//
// Push_AllReqs ------------- => GetAllReqs => GetReqs => GetFile => Push_ToDo
//       \=> GetAllPending-/
//       \=> Clr_Pending
// (=> indicates only caller of)
//
void
Push_AllReqs()
{
   Do_Log("{ Push all reqs of", CurrentClient->FilHdr, LOGLEVEL_Process);
  
   CurrentDate += 1;
   PendingDate = CurrentDate;
   while (!(IsAllUpToDate(CurrentClient->FilHdr, IK_Trans)
	    && IsAllPendingUpToDate())) {
      GetAllReqs(CurrentClient->FilHdr, IK_Trans);
      GetAllPending(); }/*while*/;
   Clr_Pending();
   CurrentClient->LastToDo = CurrentClient->ToDo;

   if (Client_LogLevel(CurrentClient) >= LOGLEVEL_Process) {
     tps_Str StrBuf;
     (void)sprintf(StrBuf, "CurrentDate now %d", CurrentDate);
     LogMessage(StrBuf);
   }
   Do_Log("} Push all reqs of", CurrentClient->FilHdr, LOGLEVEL_Process);
}


tp_FHLst
Client_ToDo(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->ToDo; }/*Client_ToDo*/

// Insert FilHdr into CurrentClient->ToDo list at
// CurrentClient->LastToDo (at head if that is NIL) such
// post: CurrentClient->LastToDo points to FilHdr
void
Push_ToDo(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_FHLst FHLst;

   Do_Log("Pushing to ToDo ", FilHdr, LOGLEVEL_Queue);
   FHLst = New_FHLst(FilHdr);
   /*select*/{
      if (CurrentClient->LastToDo == NIL) {
	 FHLst->Next = CurrentClient->ToDo;
	 CurrentClient->ToDo = FHLst;
      }else{
	 FHLst->Next = CurrentClient->LastToDo->Next;
	 CurrentClient->LastToDo->Next = FHLst; };}/*select*/;
   CurrentClient->LastToDo = FHLst;
   }/*Push_ToDo*/


static tp_FHLst	ToBroadcast = NIL;

void
Push_ToBroadcast(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_FHLst FHLst;

   Do_Log("Queuing for broadcast", FilHdr, LOGLEVEL_Queue);
   FHLst = New_FHLst(FilHdr);
   FHLst->Next = ToBroadcast;
   ToBroadcast = FHLst;
   }/*Push_ToBroadcast*/

void
Do_ToBroadcast(GMC_ARG_VOID)
{
   tp_FHLst FHLst;
   tp_FilHdr FilHdr;

   for (FHLst = ToBroadcast; FHLst != NIL; FHLst = FHLst->Next) {
      FilHdr = FHLst->FilHdr;
      Broadcast(FilHdr, STAT_Unknown); }/*for*/;
   Ret_FHLst(ToBroadcast);
   ToBroadcast = NIL;
   }/*Do_ToBroadcast*/


tp_Job
Client_Job(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->Job; }/*Client_Job*/


tp_Client
Client_Next(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return Client->Next; }/*Client_Next*/


boolean
Is_LocalClient(
   GMC_ARG(tp_Client, Client)
   )
   GMC_DCL(tp_Client, Client)
{
   return (Client == LocalClient);
   }/*Is_LocalClient*/


boolean
Is_ServerAction(GMC_ARG_VOID)
{
   return (IsServer && CurrentClient->FilHdr != NIL);
   }/*Is_ServerAction*/


tp_Job
New_Job(GMC_ARG_VOID)
{
   tp_Job Job;
   tps_Str Str;
   boolean Abort;

   /*select*/{
      if (FreeJob == NIL) {
	 Job = (tp_Job)malloc(sizeof(tps_Job));
	 num_JobS += 1;
	 Job->JobID = num_JobS;
	 Job->InUse = FALSE;
	 (void)sprintf(Str, "%s/JOB%d", JobsDirName, Job->JobID);
	 Job->JobDirName = Malloc_Str(Str);
	 MakeDirFile(&Abort, Job->JobDirName);
	 FORBIDDEN(Abort);
	 JobID_LogFileName(Str, Job->JobID);
	 MakePlnFile(&Abort, Str);
	 FORBIDDEN(Abort);
	 (void)sprintf(Str, "%s/WARNINGS", Job->JobDirName);
	 Job->WarningFN = Malloc_Str(Str);
	 (void)sprintf(Str, "%s/ERRORS", Job->JobDirName);
	 Job->ErrorFN = Malloc_Str(Str);
      }else{
	 Job = FreeJob;
	 FreeJob = FreeJob->Next; };}/*select*/;

   Job->Canceled = FALSE;
   Job->Next = NIL;
   FORBIDDEN(Job->InUse);
   Job->InUse = TRUE;
   return Job;
   }/*New_Job*/


tp_Job
Get_Job(
   GMC_ARG(tp_JobID, JobID)
   )
   GMC_DCL(tp_JobID, JobID)
{
   tp_Job Job;

   for (Job = Client_Job(CurrentClient); Job != NIL; Job = Job->Next) {
      if (Job->JobID == JobID) {
	 return Job; }/*if*/; }/*for*/;
   return NIL;
   }/*Get_Job*/


void
Ret_Job(
   GMC_ARG(tp_Job, Job)
   )
   GMC_DCL(tp_Job, Job)
{
   FORBIDDEN(Job == ERROR);
   FORBIDDEN(!Job->InUse);
   Job->InUse = FALSE;
   Job->Next = FreeJob;
   FreeJob = Job;
   }/*Ret_Job*/


tp_FilHdr
Job_FilHdr(
   GMC_ARG(tp_Job, Job)
   )
   GMC_DCL(tp_Job, Job)
{
   return Copy_FilHdr(Job->FilHdr); }/*Job_FilHdr*/


tp_Job
Add_Job(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_Job Job;

   Job = New_Job();
   Job->FilHdr = Copy_FilHdr(FilHdr);
   Job->Next = CurrentClient->Job;
   CurrentClient->Job = Job;
   CurrentClient->NumJobs += 1;
   FORBIDDEN(CurrentClient->NumJobs > CurrentClient->MaxJobs);
   Do_Log("Added Job ", FilHdr, LOGLEVEL_Process);
   return Job;
   }/*Add_Job*/


void
Del_Job(
   GMC_ARG(tp_Job, Job)
   )
   GMC_DCL(tp_Job, Job)
{
   tp_Job PrevJob;

   FORBIDDEN(Job == NIL);
   FORBIDDEN(Is_PendingReadyOrBusy_Status(FilHdr_Status(Job->FilHdr)) &&
             !Job->Canceled);
   Do_Log("Deleting Job ", Job->FilHdr, LOGLEVEL_Process);
   Ret_FilHdr(Job->FilHdr);
   Job->FilHdr = NIL;
   /*select*/{
      if (CurrentClient->Job == Job) {
	 CurrentClient->Job = Job->Next;
      }else{
	 PrevJob = CurrentClient->Job;
	 while (PrevJob->Next != Job) {
	    PrevJob = PrevJob->Next;
	    FORBIDDEN(PrevJob == NIL); }/*while*/;
	 PrevJob->Next = Job->Next; };}/*select*/;
   CurrentClient->NumJobs -= 1;
   Ret_Job(Job);
   }/*Del_Job*/


void
Clr_Status(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_Client OldCurrentClient;
   tp_Job Job;

   if (FilHdr_Status(FilHdr) != STAT_Busy) {
      Do_Log("Clearing status of", FilHdr, LOGLEVEL_Status);
      Set_Status(FilHdr, STAT_Unknown);
      return; }/*if*/;
   OldCurrentClient = CurrentClient;
   FOREACH_CLIENT(CurrentClient) {
      for (Job = Client_Job(CurrentClient); Job != NIL; Job = Job->Next) {
	 if (Job->FilHdr == FilHdr) {
	    if (!Job->Canceled) {
	       Do_Log("Canceling", FilHdr, LOGLEVEL_Cancel);
	       Job->Canceled = TRUE; }/*if*/;
	    CurrentClient = OldCurrentClient;
	    return; }/*if*/; }/*for*/; }/*for*/;
   FATALERROR("Could not find canceled job");
   }/*Clr_Status*/


// Launch jobs off CurrentClient->ToDo from CurrentClient->LastToDo
// until we're interrupted or we've checked the entire to-do list or
// we can't launch a job because we have CurrentClient->MaxJobs
// in progress. Unless we're interrupted, leaves CurrentClinet->LastToDo at
// a ready-to-launch job or at the end.
static void
LaunchSomeToDos()
{
   for (;
	CurrentClient->LastToDo != NIL;
	CurrentClient->LastToDo = CurrentClient->LastToDo->Next)
   {
     tp_FilHdr FilHdr = CurrentClient->LastToDo->FilHdr;
     Do_Log("Processing", FilHdr, LOGLEVEL_Process);
     if (FilHdr_Status(FilHdr) == STAT_Ready) {
       if (CurrentClient->NumJobs == CurrentClient->MaxJobs){
         return;
       }
       Exec(FilHdr);
       if (CurrentClient->Interrupted) {
         return;
       }
     }
   }
}

static boolean
ToDoIsAllDone()
{
  for(tp_FHLst X = CurrentClient->LastToDo; X != NIL; X=X->Next){
    if (!IsAllDone(X->FilHdr, IK_Trans))
    {
      return FALSE;
    }
  }
  return TRUE;
}

boolean
Is_TgtValErrStatus(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   FORBIDDEN(!IsAllDone(FilHdr, IK_Simple));
   return (FilHdr_Status(FilHdr) <= STAT_TgtValError
	   && (FilHdr_Status(FilHdr) > STAT_SysAbort || IsCopy(FilHdr)));
   }/*Is_TgtValErrStatus*/


tp_FilHdr
FilHdr_TgtValFilHdr(
   GMC_ARG(tp_FilHdr, FilHdr)
   )
   GMC_DCL(tp_FilHdr, FilHdr)
{
   tp_FilElm FilElm;
   tp_FilHdr TgtValFilHdr;

   FilElm = LocElm_FilElm(FilHdr_TgtValLocElm(FilHdr));
   FORBIDDEN(FilElm != NIL && FilElm_Next(FilElm) != NIL);
   TgtValFilHdr = FilElm_FilHdr(FilElm);
   Ret_FilHdr(FilHdr); Ret_FilElm(FilElm);
   return TgtValFilHdr;
   }/*FilHdr_TgtValFilHdr*/


// Progress updates for CurrentClient's target.
void
ServerAction(GMC_ARG_VOID)
{
   if (CurrentClient->FilHdr == NIL) {
      return; }/*if*/;
   if (CurrentClient->Interrupted) {
      if (CurrentClient->NumJobs == 0) {
	 Ret_ToDo();
	 End_Get_OdinFile(); }/*if*/;
      return; }/*if*/;

   // If LastToDo is non-NIL there are ready jobs on to-do that
   // LaunchSomeToDos could not launch last time because max-jobs
   // were already launched. So make sure we've launched everything
   // on LastToDo before pushing more reqs.
   if (CurrentClient->LastToDo == NIL){
     Push_AllReqs();
   }
   LaunchSomeToDos();

   if (CurrentClient->Interrupted){
      if (CurrentClient->NumJobs == 0) {
	 Ret_ToDo();
	 End_Get_OdinFile();
      }
      return;
   }
   // Not interrupted, so LaunchSomeToDos will go to end of to-do
   // unless it cannot launch a job as max jobs already launched i.e.:
   if (CurrentClient->NumJobs < CurrentClient->MaxJobs){
     FORBIDDEN(CurrentClient->LastToDo != NIL);
   }
   // ... and so we cannot be all done if not at end of to-do
   if (CurrentClient->LastToDo == NIL)
   {
     // We could be all done, note that LaunchSomeToDos might have
     // skipped over pending stuff, so have to check whole to-do.
     // (You'd think that IsAllDone would be enough, but it's not, because
     // where an input (or target val) is unkown when setting FilHdr status
     // the status is set to OK but the unknown input is put onto to-do -
     // see GetFile STAT_Unknown handling.
     if (CurrentClient->NumJobs == 0 &&
         IsAllDone(CurrentClient->FilHdr, IK_Trans) &&
         ToDoIsAllDone()){
       Ret_ToDo();
       End_Get_OdinFile();
     }
   }
}/*ServerAction*/


void
Local_Do_Interrupt(
   GMC_ARG(boolean, InterruptAll)
   )
   GMC_DCL(boolean, InterruptAll)
{
   tp_FilHdr FilHdr;
   tp_Job Job;

   if (CurrentClient->Interrupted) {
      return; }/*if*/;
   FilHdr = Client_FilHdr(CurrentClient);
   if (FilHdr == NIL) {
      return; }/*if*/;
   IsAny_ReadyServerAction = TRUE;
   CurrentClient->Interrupted = TRUE;
   Ret_FilHdr(FilHdr);
   if (InterruptAll) {
      Job = CurrentClient->Job;
      while (Job != NIL) {
	 Abort_Build(Job->JobID);
	 Job = Job->Next; }/*while*/; }/*if*/;
   }/*Local_Do_Interrupt*/


boolean
IsAny_ServerAction(GMC_ARG_VOID)
{
   tp_Client Client;

   FOREACH_CLIENT(Client) {
      if (Client->FilHdr != NIL || Client->Job != NIL) {
	 return TRUE; }/*if*/; }/*for*/;
   return FALSE;
   }/*IsAny_ServerAction*/


tp_FilHdr
Top_CWDFilHdr(GMC_ARG_VOID)
{
   tp_FilHdr DirFilHdr;

   FORBIDDEN(CurrentClient->CWDFilHdrS == NIL);
   DirFilHdr = FilHdr_SrcFilHdr(Copy_FilHdr(CurrentClient->CWDFilHdrS->FilHdr));
   if (!IsDir(DirFilHdr)) DirFilHdr = FilHdr_DirFilHdr(DirFilHdr);
   return DirFilHdr;
   }/*Top_CWDFilHdr*/


tp_FilHdr
Top_ContextFilHdr(GMC_ARG_VOID)
{
   FORBIDDEN(CurrentClient->CWDFilHdrS == NIL);
   return Copy_FilHdr(CurrentClient->CWDFilHdrS->FilHdr);
   }/*Top_ContextFilHdr*/


void
Push_ContextFilHdr(
   GMC_ARG(tp_FilHdr, CWDFilHdr)
   )
   GMC_DCL(tp_FilHdr, CWDFilHdr)
{
   tp_FHLst NewCWDFilHdrS;

   FORBIDDEN(CWDFilHdr == ERROR);
   NewCWDFilHdrS = New_FHLst(Deref(CWDFilHdr));
   NewCWDFilHdrS->Next = CurrentClient->CWDFilHdrS;
   CurrentClient->CWDFilHdrS = NewCWDFilHdrS;
   }/*Push_ContextFilHdr*/


void
Pop_ContextFilHdr(GMC_ARG_VOID)
{
   tp_FHLst FHLst;

   FHLst = CurrentClient->CWDFilHdrS;
   FORBIDDEN(FHLst == NIL);
   CurrentClient->CWDFilHdrS = FHLst->Next;
   FHLst->Next = NIL;
   Ret_FHLst(FHLst);
   }/*Pop_ContextFilHdr*/


void
Local_Set_CWD(
   GMC_ARG(tp_FileName, FileName)
   )
   GMC_DCL(tp_FileName, FileName)
{
   tp_FilHdr FilHdr;

   FilHdr = HostFN_FilHdr(FileName);
   Pop_ContextFilHdr();
   Push_ContextFilHdr(FilHdr);
   }/*Local_Set_CWD*/


void
Local_Push_Context(
   GMC_ARG(tp_FileName, DirName),
   GMC_ARG(tp_FileName, FileName)
   )
   GMC_DCL(tp_FileName, DirName)
   GMC_DCL(tp_FileName, FileName)
{
   tp_FilHdr FilHdr;

   FilHdr = DataFileName_FilHdr(FileName);
   FORBIDDEN(FilHdr == ERROR);
   Push_ContextFilHdr(FilHdr);
   FilHdr = Top_CWDFilHdr();
   FilHdr_HostFN(DirName, FilHdr, FALSE);
   Ret_FilHdr(FilHdr);
   }/*Local_Push_Context*/


void
Local_Pop_Context(
   GMC_ARG(tp_FileName, DirName)
   )
   GMC_DCL(tp_FileName, DirName)
{
   tp_FilHdr FilHdr;

   Pop_ContextFilHdr();
   FilHdr = Top_CWDFilHdr();
   FilHdr_HostFN(DirName, FilHdr, FALSE);
   Ret_FilHdr(FilHdr);
   }/*Local_Pop_Context*/


void
Local_Set_KeepGoing(
   GMC_ARG(boolean, Flag)
   )
   GMC_DCL(boolean, Flag)
{
   CurrentClient->KeepGoing = Flag;
   }/*Local_Set_KeepGoing*/


void
Local_Set_ErrLevel(
   GMC_ARG(int, ErrLevel)
   )
   GMC_DCL(int, ErrLevel)
{
   CurrentClient->ErrLevel = ErrLevel;
   }/*Local_Set_ErrLevel*/


void
Local_Set_WarnLevel(
   GMC_ARG(int, WarnLevel)
   )
   GMC_DCL(int, WarnLevel)
{
   CurrentClient->WarnLevel = WarnLevel;
   }/*Local_Set_WarnLevel*/


void
Local_Set_LogLevel(
   GMC_ARG(tp_LogLevel, LogLevel)
   )
   GMC_DCL(tp_LogLevel, LogLevel)
{
   CurrentClient->LogLevel = LogLevel;
   }/*Local_Set_LogLevel*/


void
Local_Set_HelpLevel(
   GMC_ARG(int, HelpLevel)
   )
   GMC_DCL(int, HelpLevel)
{
   CurrentClient->HelpLevel = HelpLevel;
   }/*Local_Set_HelpLevel*/


void
Local_Set_MaxJobs(
   GMC_ARG(int, MaxJobs)
   )
   GMC_DCL(int, MaxJobs)
{
   CurrentClient->MaxJobs = MaxJobs;
   }/*Local_Set_MaxJobs*/


int
Local_Get_NumJobs(
   )
{
  return CurrentClient->NumJobs;
   }/*Local_Set_MaxJobs*/


void
Local_Get_UseCount(
   GMC_ARG(int*, CountPtr)
   )
   GMC_DCL(int*, CountPtr)
{
   *CountPtr = (IsClient ? UseCount : (UseCount-1));
   }/*Local_Get_UseCount*/


