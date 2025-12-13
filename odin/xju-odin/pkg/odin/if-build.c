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

#include <gmc/gmc.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Var.hh>
#include <odin/inc/Func.hh>

#include <odin/inc/Build.h>
#include <odin/inc/LogLevel_.h>
#include <odin/inc/OC_NodTyp_.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static tp_Build	FirstBuild = NIL;
int		MaxBuilds;

static int	ChildWaitPID = 0;


#define		MAX_BUILDARGV 100
static int	Num_BuildArgV = 0;
static char	*BuildArgV[MAX_BUILDARGV];


static tp_Build
BuildID_Build(tp_BuildID BuildID)
{
   tp_Build Build;

   for (Build=FirstBuild; Build!=NIL; Build=Build->Next) {
      if (BuildID == Build->BuildID) {
	 return Build; }/*if*/; }/*for*/;
   return NIL;
   }/*BuildID_Build*/


tp_Build
JobID_Build(tp_JobID JobID)
{
   tp_Build Build;

   for (Build=FirstBuild; Build!=NIL; Build=Build->Next) {
      if (JobID == Build->JobID) {
	 return Build; }/*if*/; }/*for*/;
   return NIL;
   }/*JobID_Build*/


void
Extend_Builds(int NumBuilds)
{
   int i;
   tp_Build PrevBuild, Build;

   PrevBuild = NIL;
   Build = FirstBuild;
   for (i=0; i<NumBuilds; i+=1) {
      if (Build == NIL) {
	 Build = (tp_Build)malloc(sizeof(tps_Build));
	 Build->JobID = 0;
	 Build->BuildID = 0;
	 Build->Host = NIL;
	 Build->Next = NIL;
	 /*select*/{
	    if (PrevBuild == NIL) {
	       FirstBuild = Build;
	    }else{
	       PrevBuild->Next = Build; };}/*select*/; }/*if*/;
      PrevBuild = Build; Build = Build->Next; }/*for*/;
   }/*Extend_Builds*/


void
Set_BuildHosts(bool* AbortPtr, tp_Str Str)
{
   tp_Nod Nod, Son, HostNod;
   tp_Build Build;
   bool IsHelp, IsHandled;

   Nod = OC_Parser(Str, (tp_FileName)NIL, (int *)NIL);
   if (Nod == ERROR) {
      *AbortPtr = true;
      return; }/*if*/;
   Do_Help(AbortPtr, &IsHelp, &IsHandled, Nod);
   if (*AbortPtr) {
     return; }/*if*/;
   if (IsHelp) {
      Writeln(StdOutFD, "?*? A colon separated list of hosts.\n");
      return; }/*if*/;
   for (Son = Nod_Son(1, Nod); Son != NIL; Son = Nod_Brother(Son)) {
      HostNod = (Nod_NodTyp(Son) == DRVOPR ? Nod_Son(1, Son) : Son);
      if (Nod_NodTyp(HostNod) != WORD) {
	 SystemError("BuildHosts must be a colon separated list of hosts.\n");
	 Ret_Nod(Nod); }/*if*/; }/*for*/;
   Extend_Builds(Nod_NumSons(Nod));
   Son = Nod_Son(1, Nod);
   for (Build = FirstBuild; Build != NIL; Build = Build->Next) {
      /*select*/{
	 if (Son == NIL) {
	    Build->Host = NIL;
	 }else{
	    HostNod = (Nod_NodTyp(Son) == DRVOPR ? Nod_Son(1, Son) : Son);
	    Build->Host = Lookup_Host(Sym_Str(Nod_Sym(HostNod)));
	    Son = Nod_Brother(Son); };}/*select*/; }/*for*/;
   Ret_Nod(Nod);
   }/*Set_BuildHosts*/


void
Write_BuildHosts(tp_FilDsc FilDsc)
{
   bool IsFirst;
   tp_Build Build;
   int i;

   IsFirst = true;
   Build = FirstBuild;
   for (i = 0; i < MaxBuilds; i++) {
      FORBIDDEN(Build == NIL);
      if (!IsFirst) Write(FilDsc, " : ");
      IsFirst = false;
      Write(FilDsc, Build->Host == NIL ? "LOCAL" : Host_HostName(Build->Host));
      Build = Build->Next; }/*for*/;
   Writeln(FilDsc, "");
   }/*Write_BuildHosts*/


void
Local_Add_BuildArg(tp_FileName Arg)
{
   FORBIDDEN((Num_BuildArgV+2) > MAX_BUILDARGV);
   if (BuildArgV[Num_BuildArgV] != NIL) free(BuildArgV[Num_BuildArgV]);
   BuildArgV[Num_BuildArgV] = Malloc_Str(Arg);
   Num_BuildArgV += 1;
   }/*Local_Add_BuildArg*/


void
Local_Do_Build(tp_JobID JobID,tp_FileName JobDirName, tp_FileName LogFileName)
{
   tp_Build Build;
   int i;
   tps_Str OldCWD;
   bool Abort;
   tp_FileName FileName;
   struct timeval now;

   if (BuildArgV[Num_BuildArgV] != NIL) free(BuildArgV[Num_BuildArgV]);
   BuildArgV[Num_BuildArgV] = 0;
   Num_BuildArgV = 0;

   i = 0;
   for (Build = FirstBuild; Build->JobID != NIL; Build = Build->Next) {
      i += 1;
      FORBIDDEN(i > MaxBuilds); }/*for*/;
   FORBIDDEN(Build->BuildID != NIL);
   Build->JobID = JobID;
   if (LogLevel >= LOGLEVEL_ExecLine) {
      gettimeofday(&now, 0);
      (void)fprintf((FILE *)StdOutFD, "** %d.%03d {Executing :", 
                    now.tv_sec, now.tv_usec/1000);
      for (i=0; BuildArgV[i] != NIL; i+=1) {
	 (void)fprintf((FILE *)StdOutFD, " '%s'", BuildArgV[i]); }/*for*/;
      (void)fprintf((FILE *)StdOutFD, "\n"); }/*if*/;
   if (Build->Host != NIL) {
      RBS_Do_Build(Build->Host, JobID, JobDirName, LogFileName, BuildArgV);
      return; }/*if*/;
   (void)strcpy(OldCWD, CWDirName);
   ChangeDir(&Abort, JobDirName);
   FORBIDDEN(Abort);
   ClearDir(JobDirName);
   FileName = (MaxBuilds > 1 ? LogFileName : NIL);
   Build->BuildID = SystemExec(BuildArgV[0], BuildArgV, FileName);
   ChangeDir(&Abort, OldCWD);
   FORBIDDEN(Abort);
   if (LogLevel >= LOGLEVEL_ExecLine) {
      gettimeofday(&now, 0);
      (void)fprintf((FILE *)StdOutFD, "** %d.%03d }Executing :", 
                    now.tv_sec, now.tv_usec/1000);
      for (i=0; BuildArgV[i] != NIL; i+=1) {
	 (void)fprintf((FILE *)StdOutFD, " '%s'", BuildArgV[i]); }/*for*/;
      (void)fprintf((FILE *)StdOutFD, "\n"); }/*if*/;
   }/*Local_Do_Build*/


void
Local_Abort_Build(tp_JobID JobID)
{
   tp_Build Build;

   Build = JobID_Build(JobID);
   if (Build == NIL) {
      if (LogLevel >= LOGLEVEL_AbortDone) {
	 Writeln(StdOutFD, "Job to be aborted already completed."); }/*if*/;
      return; }/*if*/;
   if (Build->Host == NIL) {
      SystemInterrupt(Build->BuildID);
      return; }/*if*/;
   RBS_Abort_Build(Build->Host, JobID);
   }/*Local_Abort_Build*/


static bool
Is_ActiveBuild()
{
   tp_Build Build;

   for (Build=FirstBuild; Build!=NIL; Build=Build->Next) {
      if (Build->BuildID != 0) {
	 return true; }/*if*/; }/*for*/;
   return false;
   }/*Is_ActiveBuild*/


void
SystemExecCmdWait(bool* AbortPtr,const char* Cmd,bool Interactive)
{
   FORBIDDEN(ChildWaitPID != 0);
   ChildWaitPID = SystemExecCmd(Cmd, Interactive);
   if (ChildWaitPID <= 0) {
      ChildWaitPID = 0;
      *AbortPtr = true;
      return; }/*if*/;
   IPC_Get_Commands(AbortPtr, (char *)NIL);
   }/*SystemExecCmdWait*/


void
ChildAction(bool* AbortPtr,bool* DonePtr)
{
   tp_BuildID BuildID;
   bool Abort;

   *AbortPtr = false;
   *DonePtr = false;
   while (Is_ActiveBuild() || ChildWaitPID != 0) {
      SystemWait(&BuildID, &Abort);
      if (BuildID == 0) {
	 return; }/*if*/;
      /*select*/{
	 if (BuildID == ChildWaitPID) {
	    ChildWaitPID = 0;
	    *AbortPtr = Abort;
	    *DonePtr = true;
	 }else if (IsServerPId(BuildID)) {
	    DeadServerExit();
	 }else if (PId_Host(BuildID) != NIL) {
	    RBS_Done(PId_Host(BuildID));
	 }else{
	    Build_Done(BuildID_Build(BuildID), Abort);
	    };}/*select*/; }/*while*/;
   }/*ChildAction*/


void
Cancel_Builds(tp_Host Host)
{
   tp_Build Build;

   for (Build=FirstBuild; Build!=NIL; Build=Build->Next) {
      if (Build->Host == Host && Build->JobID != 0) {
	 Build_Done(Build, true); }/*if*/; }/*for*/;
   }/*Cancel_Builds*/


void
Build_Done(tp_Build Build,bool Abort)
{
   tps_Str LogFileName;
   tp_FilDsc LogFD;

   FORBIDDEN(Build == NIL || Build->JobID == 0);
   if (MaxBuilds > 1 || Build->Host != NIL) {
      JobID_LogFileName(LogFileName, Build->JobID);
      LogFD = FileName_RFilDsc(LogFileName, true);
      FileCopy(StdOutFD, LogFD);
      Flush(StdOutFD);
      Close(LogFD); }/*if*/;
   Job_Done(Build->JobID, Abort);
   Build->BuildID = 0;
   Build->JobID = 0;
   }/*Build_Done*/


void
Local_Do_MakeReadOnly(tp_FileName FileName)
{
   bool Abort;

   MakeReadOnly(&Abort, FileName);
   if (Abort) {
      SystemError("\"%s\": could not remove write permission.\n", FileName);
      }/*if*/;
   }/*Local_Do_MakeReadOnly*/


