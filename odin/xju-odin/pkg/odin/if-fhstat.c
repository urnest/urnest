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
#include <odin/inc/Func.hh>
#include <odin/inc/Var.hh>
#include <odin/inc/FilHdr.h>
#include <odin/inc/Flag_.h>
#include <odin/inc/LogLevel_.h>
#include <odin/inc/Status_.h>


bool
Is_PendingReadyOrBusy_Status(tp_Status Status)
{
   return (Status == STAT_Pending || Status == STAT_Ready
	   || Status == STAT_Busy);
   }/*Is_PendingReadyOrBusy_Status*/


void
Clr_ErrStatus(tp_FilHdr FilHdr)
{
   tps_FileName FileName;

   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(IsSource(FilHdr));
   if (FilHdr->HdrInf.ErrStatusWord == 0) {
      return; }/*if*/;
   if (FilHdr_HasErrStatus(FilHdr, STAT_Warning)) {
      FilHdr_WarningFileName(FileName, FilHdr);
      Remove(FileName); }/*if*/;
   if (FilHdr_HasErrStatus(FilHdr, STAT_Error)) {
      FilHdr_ErrorFileName(FileName, FilHdr);
      Remove(FileName); }/*if*/;
   FilHdr->HdrInf.ErrStatusWord = 0;
   SetModified(FilHdr);
   }/*Clr_ErrStatus*/


void
Add_ErrStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(IsSource(FilHdr));
   switch (Status) {
      case STAT_Warning: {
	 Do_Log("Warnings generated for", FilHdr, LOGLEVEL_Warnings);
	 break; }/*case*/;
      case STAT_TgtValError: {
	 break; }/*case*/;
      case STAT_Error: {
	 Do_Log("Errors generated for", FilHdr, LOGLEVEL_Errors);
	 break; }/*case*/;
      case STAT_NoFile: {
	 Do_Log("No file generated for", FilHdr, LOGLEVEL_NoFile);
	 break; }/*case*/;
      default: {
	 FATALERROR("bad Status"); };}/*switch*/;
   if ((FilHdr->HdrInf.ErrStatusWord & (1<<Status)) == 0) {
      FilHdr->HdrInf.ErrStatusWord
	 = (FilHdr->HdrInf.ErrStatusWord | (1<<Status));
      SetModified(FilHdr); }/*if*/;
   }/*Add_ErrStatus*/


bool
FilHdr_HasErrStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(FilHdr == ERROR);
   if (IsSource(FilHdr)) {
      return false; }/*if*/;
   return ((FilHdr->HdrInf.ErrStatusWord & (1<<Status)) != 0);
   }/*FilHdr_HasErrStatus*/


tp_Status
FilHdr_MinErrStatus(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr_HasErrStatus(FilHdr, STAT_NoFile)) {
      return STAT_NoFile; }/*if*/;
   if (FilHdr_HasErrStatus(FilHdr, STAT_Error)) {
      return STAT_Error; }/*if*/;
   if (FilHdr_HasErrStatus(FilHdr, STAT_TgtValError)) {
      return STAT_TgtValError; }/*if*/;
   if (FilHdr_HasErrStatus(FilHdr, STAT_Warning)) {
      return STAT_Warning; }/*if*/;
   return STAT_OK;
   }/*FilHdr_MinErrStatus*/


void
Add_StatusFile(tp_FilHdr FilHdr,tp_Status Status,tp_FileName FileName)
{
   tps_FileName ErrFileName;
   bool Abort;

   FORBIDDEN(FilHdr == ERROR || FileName == ERROR);
   Add_ErrStatus(FilHdr, Status);
   switch (Status) {
      case STAT_Warning: {
	 FilHdr_WarningFileName(ErrFileName, FilHdr);
	 break; }/*case*/;
      case STAT_Error: {
	 FilHdr_ErrorFileName(ErrFileName, FilHdr);
	 break; }/*case*/;
      default: {
	 FATALERROR("bad Status"); };}/*switch*/;
   Rename(&Abort, FileName, ErrFileName);
   FORBIDDEN(Abort);
   MakeReadOnly(&Abort, ErrFileName);
   if (Abort) Do_MakeReadOnly(ErrFileName);
   switch (Status) {
      case STAT_Warning: {
	 if (IsIncremental_MsgLevel(Client_WarnLevel(CurrentClient))) {
	    FileErrMessage(ErrFileName); }/*if*/; break;}/*case*/;
      case STAT_Error: {
	 if (IsIncremental_MsgLevel(Client_ErrLevel(CurrentClient))) {
	    FileErrMessage(ErrFileName);
	    if (!Client_KeepGoing(CurrentClient)) {
	       Local_Do_Interrupt(false); }/*if*/; }/*if*/; break;}/*case*/;
      default: {
	 FATALERROR("bad status"); };}/*switch*/;
   }/*Add_StatusFile*/


void
Set_DepStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(Status == ERROR || FilHdr == ERROR);
   FilHdr->DepStatus = Status;
   }/*Set_DepStatus*/


tp_Status
FilHdr_DepStatus(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->DepStatus;
   }/*FilHdr_DepStatus*/


void
Set_DepModDate(tp_FilHdr FilHdr,tp_Date ModDate)
{
   FORBIDDEN(FilHdr == ERROR);
   FilHdr->DepModDate = ModDate;
   }/*Set_DepModDate*/


tp_Date
FilHdr_DepModDate(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->DepModDate;
   }/*FilHdr_DepModDate*/


void
Set_Status(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(Status == ERROR || FilHdr == ERROR);

   if (Status != STAT_Ready && Status != STAT_Busy) {
      FilHdr->DepStatus = STAT_Unknown; }/*if*/;

   if (Status == STAT_Unknown) {
      if (FilHdr->HdrInf.Status != STAT_Unknown) {
	 FilHdr->HdrInf.Status = STAT_Unknown;
	 SetModified(FilHdr);
	 Set_ElmNameStatus(FilHdr, STAT_Unknown); }/*if*/;
      return; }/*if*/;
   if (Status != FilHdr->HdrInf.Status) {
      FilHdr->HdrInf.Status = Status;
      if (Status > STAT_Busy && IsATgt(FilHdr)) {
	 /*select*/{
	    if (Status > STAT_Error) {
	       Install_ActTgt(FilHdr);
	    }else{
	       Uninstall_ActTgt(FilHdr); };}/*select*/; }/*if*/;
      SetModified(FilHdr); }/*if*/;
   if (FilHdr->HdrInf.VerifyDate < CurrentDate) {
      FilHdr->HdrInf.VerifyDate = CurrentDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_Status*/


tp_Status
FilHdr_Status(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.Status;
   }/*FilHdr_Status*/


void
Set_ElmNameStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(Status == ERROR || FilHdr == ERROR);

   if (Status == STAT_Unknown) {
      if (FilHdr->HdrInf.ElmNameStatus != STAT_Unknown) {
	 FilHdr->HdrInf.ElmNameStatus = STAT_Unknown;
	 SetModified(FilHdr);
	 Set_ElmStatus(FilHdr, STAT_Unknown); }/*if*/;
      return; }/*if*/;

   if (Status != FilHdr->HdrInf.ElmNameStatus) {
      FORBIDDEN(!IsRef(FilHdr));
      FORBIDDEN (FilHdr->HdrInf.Status == STAT_Unknown);
      FilHdr->HdrInf.ElmNameStatus = Status;
      SetModified(FilHdr); }/*if*/;
   if (FilHdr->HdrInf.ElmNameVerifyDate < CurrentDate) {
      FilHdr->HdrInf.ElmNameVerifyDate = CurrentDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmNameStatus*/


tp_Status
FilHdr_ElmNameStatus(tp_FilHdr FilHdr)
{
   tp_Status Status;

   if (FilHdr == ERROR) return ERROR;
   Status = FilHdr->HdrInf.Status;
   if (Status <= STAT_Error || !IsRef(FilHdr)) {
      return Status; }/*if*/;
   return FilHdr->HdrInf.ElmNameStatus;
   }/*FilHdr_ElmNameStatus*/


void
Set_ElmStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(Status == ERROR || FilHdr == ERROR);

   FORBIDDEN(IsSource(FilHdr));
   if (Status == STAT_Unknown) {
      if (FilHdr->HdrInf.ElmStatus != STAT_Unknown) {
	 FilHdr->HdrInf.ElmStatus = STAT_Unknown;
	 SetModified(FilHdr); }/*if*/;
      return; }/*if*/;

   if (Status != FilHdr->HdrInf.ElmStatus) {
      FORBIDDEN(!IsRef(FilHdr));
      FORBIDDEN (FilHdr->HdrInf.Status == STAT_Unknown);
      FORBIDDEN (FilHdr->HdrInf.ElmNameStatus == STAT_Unknown);
      FilHdr->HdrInf.ElmStatus = Status;
      SetModified(FilHdr); }/*if*/;
   if (FilHdr->HdrInf.ElmVerifyDate < CurrentDate) {
      FilHdr->HdrInf.ElmVerifyDate = CurrentDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmStatus*/


tp_Status
FilHdr_ElmStatus(tp_FilHdr FilHdr)
{
   tp_Status Status;

   if (FilHdr == ERROR) return ERROR;
   Status = FilHdr->HdrInf.Status;
   if (Status <= STAT_Error || !IsRef(FilHdr)) {
      return Status; }/*if*/;
   return FilHdr->HdrInf.ElmStatus;
   }/*FilHdr_ElmStatus*/


void
Set_TgtValStatus(tp_FilHdr FilHdr,tp_Status Status)
{
   FORBIDDEN(Status == ERROR || FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   if (Status == STAT_Unknown) {
      if (FilHdr->HdrInf.ElmStatus != STAT_Unknown) {
	 FilHdr->HdrInf.ElmStatus = STAT_Unknown;
	 SetModified(FilHdr); }/*if*/;
      return; }/*if*/;

   if (Status != FilHdr->HdrInf.ElmStatus) {
      FilHdr->HdrInf.ElmStatus = Status;
      SetModified(FilHdr); }/*if*/;
   if (FilHdr->HdrInf.ElmVerifyDate < CurrentDate) {
      FilHdr->HdrInf.ElmVerifyDate = CurrentDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_TgtValStatus*/


tp_Status
FilHdr_TgtValStatus(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   if (!IsSource(FilHdr)) {
      return STAT_OK; }/*if*/;
   return FilHdr->HdrInf.ElmStatus;
   }/*FilHdr_TgtValStatus*/


tp_Status
FilHdr_TgtValMinStatus(tp_FilHdr FilHdr)
{
   tp_Status Status, TgtValStatus;

   if (FilHdr == ERROR) return ERROR;
   Status = FilHdr_Status(FilHdr);
   TgtValStatus = FilHdr_TgtValStatus(FilHdr);
   return ((TgtValStatus < Status) ? TgtValStatus : Status);
   }/*FilHdr_TgtValMinStatus*/


tp_Status
FilHdr_MinStatus(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   tp_Status Status;

   if (FilHdr == ERROR) {
      return ERROR; }/*if*/;
   if (!NeedsData(FilHdr, InpKind)) {
      return STAT_OK; }/*if*/;
   Status = FilHdr_TgtValMinStatus(FilHdr);
   /*select*/{
      if (NeedsElmData(FilHdr, InpKind)) {
	 if (FilHdr_ElmStatus(FilHdr) < Status) {
	    Status = FilHdr_ElmStatus(FilHdr); }/*if*/;
      }else if (NeedsElmNameData(FilHdr, InpKind)) {
	 if (FilHdr_ElmNameStatus(FilHdr) < Status) {
	    Status = FilHdr_ElmNameStatus(FilHdr); }/*if*/; };}/*select*/;
   return Status;
   }/*FilHdr_MinStatus*/


void
Set_ModDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   CurrentDate += 1;
   FORBIDDEN(CurrentDate <= FilHdr->HdrInf.ModDate);
   FilHdr->HdrInf.ModDate = CurrentDate;
   FilHdr->HdrInf.OrigModDate = 0;
   SetModified(FilHdr);
   }/*Set_ModDate*/


tp_Date
FilHdr_ModDate(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.ModDate;
   }/*FilHdr_ModDate*/


void
Set_ConfirmDate(tp_FilHdr FilHdr,tp_Date Date)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.ConfirmDate < Date) {
      FilHdr->HdrInf.ConfirmDate = Date;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ConfirmDate*/


void
Clr_ConfirmDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.ConfirmDate != 0) {
      FilHdr->HdrInf.ConfirmDate = 0;
      SetModified(FilHdr); }/*if*/;
   }/*Clr_ConfirmDate*/


tp_Date
FilHdr_ConfirmDate(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.ConfirmDate;
   }/*FilHdr_ConfirmDate*/


void
Set_ElmNameConfirmDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.ElmNameConfirmDate < FilHdr->HdrInf.ElmNameModDate) {
      FilHdr->HdrInf.ElmNameConfirmDate = FilHdr->HdrInf.ElmNameModDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmNameConfirmDate*/


void
Set_ElmConfirmDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.ElmConfirmDate < FilHdr->HdrInf.ElmModDate) {
      FilHdr->HdrInf.ElmConfirmDate = FilHdr->HdrInf.ElmModDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmConfirmDate*/


void
Set_ElmModDate(tp_FilHdr FilHdr,tp_Date ElmModDate)
{
   FORBIDDEN(ElmModDate == ERROR || FilHdr == ERROR);
   if (ElmModDate > FilHdr->HdrInf.ElmModDate) {
      FilHdr->HdrInf.ElmModDate = ElmModDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmModDate*/


tp_Date
FilHdr_ElmModDate(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.ElmModDate;
   }/*FilHdr_ElmModDate*/


void
Set_ElmNameModDate(tp_FilHdr FilHdr,tp_Date ElmNameModDate)
{
   FORBIDDEN(ElmNameModDate == ERROR || FilHdr == ERROR);
   if (ElmNameModDate > FilHdr->HdrInf.ElmNameModDate) {
      FilHdr->HdrInf.ElmNameModDate = ElmNameModDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmNameModDate*/


tp_Date
FilHdr_ElmNameModDate(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.ElmNameModDate;
   }/*FilHdr_ElmNameModDate*/


void
Set_Flag(tp_FilHdr FilHdr,tp_Flag Flag)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN((FilHdr->Flag & 1<<Flag) != 0);
   FilHdr->Flag |= 1<<Flag;
   }/*Set_Flag*/


void
Clr_Flag(tp_FilHdr FilHdr,tp_Flag Flag)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN((FilHdr->Flag & (1<<Flag)) == 0);
   FilHdr->Flag &= ~(1<<Flag);
   }/*Clr_Flag*/


bool
FilHdr_Flag(tp_FilHdr FilHdr,tp_Flag Flag)
{
   if (FilHdr == ERROR) return ERROR;
   return ((FilHdr->Flag & (1<<Flag)) != 0);
   }/*FilHdr_Flag*/


void
Set_AnyOKDepth(tp_FilHdr FilHdr,int Depth)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(FilHdr->AnyOKDepth == Depth);
   FilHdr->AnyOKDepth = Depth;
   }/*Set_AnyOKDepth*/


int
FilHdr_AnyOKDepth(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->AnyOKDepth;
   }/*FilHdr_AnyOKDepth*/


void
Set_ElmDepth(tp_FilHdr FilHdr,int Depth)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(FilHdr->ElmDepth == Depth);
   FilHdr->ElmDepth = Depth;
   }/*Set_ElmDepth*/


int
FilHdr_ElmDepth(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->ElmDepth;
   }/*FilHdr_ElmDepth*/


void
Set_ElmTag(tp_FilHdr FilHdr,int ElmTag)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(FilHdr->ElmTag == ElmTag);
   FilHdr->ElmTag = ElmTag;
   }/*Set_ElmTag*/


int
FilHdr_ElmTag(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->ElmTag;
   }/*FilHdr_ElmTag*/


void
Set_SCC(tp_FilHdr FilHdr,tp_FilHdr SCC)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(FilHdr->SCC == SCC);
   Ret_FilHdr(FilHdr->SCC);
   FilHdr->SCC = Copy_FilHdr(SCC);
   }/*Set_SCC*/


tp_FilHdr
FilHdr_SCC(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(FilHdr->SCC == NIL);
   while (FilHdr->SCC != FilHdr) FilHdr = FilHdr->SCC;
   return Copy_FilHdr(FilHdr);
   }/*FilHdr_SCC*/


/* PndFlag's used to distinguish between dependent on cycle,
 * vs. Dependent on something that has been modified and must be rescanned */

void
Set_ListPndFlag(tp_FilHdr FilHdr,bool PndFlag)
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;

   Set_PndFlag(FilHdr, PndFlag);
   if (IsKeyList(FilHdr)) {
      for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	   FilElm != NIL;
	   FilElm = FilElm_NextFilElm(FilElm)) {
	 ElmFilHdr = FilElm_FilHdr(FilElm);
	 FORBIDDEN(ElmFilHdr == ERROR);
	 FORBIDDEN(!IsKeyListElm(ElmFilHdr));
	 Set_PndFlag(ElmFilHdr, PndFlag);
	 Ret_FilHdr(ElmFilHdr); }/*for*/; }/*if*/;
   }/*Set_ListPndFlag*/


void
Set_PndFlag(tp_FilHdr FilHdr,bool PndFlag)
{
   FORBIDDEN(FilHdr == ERROR);
   Do_Log(PndFlag?"set pending flag of":"clear pending flag of", FilHdr, LOGLEVEL_Process);
   FilHdr->PndFlag = PndFlag;
   }/*Set_PndFlag*/


bool
FilHdr_PndFlag(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->PndFlag;
   }/*FilHdr_PndFlag*/


void
Set_ElmNamePndFlag(tp_FilHdr FilHdr,bool ElmNamePndFlag)
{
   FORBIDDEN(FilHdr == ERROR);
   if (ElmNamePndFlag) Set_ElmPndFlag(FilHdr, true);
   Do_Log(ElmNamePndFlag?"set elem name pending flag of":"clear elem name pending flag of", FilHdr, LOGLEVEL_Process);
   FilHdr->ElmNamePndFlag = ElmNamePndFlag;
   }/*Set_ElmNamePndFlag*/


bool
FilHdr_ElmNamePndFlag(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->ElmNamePndFlag;
   }/*FilHdr_ElmNamePndFlag*/


void
Set_ElmPndFlag(tp_FilHdr FilHdr,bool ElmPndFlag)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(IsSource(FilHdr));
   Do_Log(ElmPndFlag?"set elem pending flag of":"clear elem pending flag of", FilHdr, LOGLEVEL_Process);
   FilHdr->ElmPndFlag = ElmPndFlag;
   }/*Set_ElmPndFlag*/


bool
FilHdr_ElmPndFlag(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (IsSource(FilHdr)) {
      return false; }/*if*/;
   return FilHdr->ElmPndFlag;
   }/*FilHdr_ElmPndFlag*/


void
Set_TgtValPndFlag(tp_FilHdr FilHdr,bool TgtValPndFlag)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   Do_Log(TgtValPndFlag?"set tgt val pending flag of":"clear tgt val pending flag of", FilHdr, LOGLEVEL_Process);
   FilHdr->ElmPndFlag = TgtValPndFlag;
   }/*Set_TgtValPndFlag*/


bool
FilHdr_TgtValPndFlag(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (!IsSource(FilHdr)) {
      return false; }/*if*/;
   return FilHdr->ElmPndFlag;
   }/*FilHdr_TgtValPndFlag*/


tp_LocInp
FilHdr_LocInp(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.LocInp;
   }/*FilHdr_LocInp*/


void
Set_LocElm(tp_FilHdr FilHdr,tp_LocElm LocElm)
{

   FORBIDDEN(FilHdr == ERROR);
   if (LocElm == FilHdr->HdrInf.LocElm) {
      FORBIDDEN(LocElm != NIL);
      return; }/*if*/;

   if (DebugLocElm) Validate_LocElm(FilHdr, LocElm);
   if (FilHdr->HdrInf.LocElm != FilHdr->HdrInf.OldLocElm) {
      DeAlloc_ElmInf(FilHdr->HdrInf.LocElm); }/*if*/;
   FilHdr->HdrInf.LocElm = LocElm;
   SetModified(FilHdr);
   }/*Set_LocElm*/


tp_LocElm
FilHdr_LocElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.LocElm;
   }/*FilHdr_LocElm*/


void
Set_OldLocElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.OldLocElm == FilHdr->HdrInf.LocElm) {
      return; }/*if*/;
   DeAlloc_ElmInf(FilHdr->HdrInf.OldLocElm);
   FilHdr->HdrInf.OldLocElm = FilHdr->HdrInf.LocElm;
   SetModified(FilHdr);
   }/*Set_OldLocElm*/


tp_LocElm
FilHdr_OldLocElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.OldLocElm;
   }/*FilHdr_OldLocElm*/


void
Set_TgtValLocElm(tp_FilHdr FilHdr,tp_LocElm LocElm)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   if (FilHdr->HdrInf.TgtValLocElm == LocElm) {
      return; }/*if*/;
   DeAlloc_ElmInf(FilHdr->HdrInf.TgtValLocElm);
   FilHdr->HdrInf.TgtValLocElm = LocElm;
   SetModified(FilHdr);
   }/*Set_TgtValLocElm*/


void
Set_DfltTgtValLocElm(tp_FilHdr FilHdr)
{
   tp_FilHdr TgtValFilHdr;

   TgtValFilHdr = Do_Deriv(FilHdr_Father(Copy_FilHdr(FilHdr)),
			   RootFilPrm, RootFilPrm, ActTargetsFilTyp);
   FORBIDDEN(TgtValFilHdr == NIL);
   Set_TgtValLocElm(FilHdr, Make_LocElm(TgtValFilHdr, RootFilPrm, FilHdr));
   Set_OrigLocHdr(FilHdr, (tp_LocHdr)NIL);
   Ret_FilHdr(TgtValFilHdr);
   }/*Set_DfltTgtValLocElm*/


tp_LocElm
FilHdr_TgtValLocElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.TgtValLocElm;
   }/*FilHdr_TgtValLocElm*/


bool
FilHdr_ActTgtInstalled(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.OrigLocHdr != 0);
   }/*FilHdr_ActTgtInstalled*/


void
Set_ActTgtInstalled(tp_FilHdr FilHdr,bool Flag)
{
   tp_LocHdr LocHdr;

   FORBIDDEN(FilHdr == ERROR);
   LocHdr = (Flag ? 1 : 0);
   FORBIDDEN(LocHdr == FilHdr->HdrInf.OrigLocHdr);
   FilHdr->HdrInf.OrigLocHdr = LocHdr;
   SetModified(FilHdr);
   }/*Set_ActTgtInstalled*/


void
Set_InpLink(tp_FilHdr FilHdr,tp_LocInp LocInp)
{
   FORBIDDEN(FilHdr == ERROR);
   if (LocInp != FilHdr->HdrInf.InpLink) {
      FilHdr->HdrInf.InpLink = LocInp;
      SetModified(FilHdr); }/*if*/;
   }/*Set_InpLink*/


tp_LocInp
FilHdr_InpLink(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.InpLink;
   }/*FilHdr_InpLink*/


void
Set_ElmLink(tp_FilHdr FilHdr,tp_LocElm LocElm)
{
   FORBIDDEN(FilHdr == ERROR);
   if (LocElm != FilHdr->HdrInf.ElmLink) {
      FilHdr->HdrInf.ElmLink = LocElm;
      SetModified(FilHdr); }/*if*/;
   }/*Set_ElmLink*/


tp_LocElm
FilHdr_ElmLink(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.ElmLink;
   }/*FilHdr_ElmLink*/


int
FilHdr_Size(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.Size == -1) {
      return 0; }/*if*/;
   return FilHdr->HdrInf.Size;
   }/*FilHdr_Size*/


void
Set_Size(tp_FilHdr FilHdr,int Size)
{
   int OldSize;

   FORBIDDEN(FilHdr == ERROR || Size < -1);
   if (Size != FilHdr->HdrInf.Size) {
      OldSize = FilHdr_Size(FilHdr);
      FilHdr->HdrInf.Size = Size;
      SetModified(FilHdr);
      CurSize += FilHdr_Size(FilHdr) - OldSize; }/*if*/;
   }/*Set_Size*/


bool
Data_Exists(tp_FilHdr FilHdr)
{
   return (FilHdr->HdrInf.Size != -1);
   }/*Data_Exists*/


void
Local_Get_CurSize(int* SizePtr)
{
   *SizePtr = CurSize;
   }/*Local_Get_CurSize*/


void
Set_OrigLocHdr(tp_FilHdr FilHdr,tp_LocHdr LocHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   if (FilHdr->HdrInf.OrigLocHdr != LocHdr) {
      FilHdr->HdrInf.OrigLocHdr = LocHdr;
      FilHdr->HdrInf.OrigModDate = 0;
      SetModified(FilHdr); }/*if*/;
   }/*Set_OrigLocHdr*/


tp_LocHdr
FilHdr_OrigLocHdr(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.OrigLocHdr;
   }/*FilHdr_OrigLocHdr*/


void
Set_OrigModDate(tp_FilHdr FilHdr,tp_Date ModDate)
{
   FORBIDDEN(FilHdr == ERROR || ModDate == ERROR);
   if (FilHdr->HdrInf.OrigModDate != ModDate) {
      FilHdr->HdrInf.OrigModDate = ModDate;
      SetModified(FilHdr); }/*if*/;
   }/*Set_OrigModDate*/


tp_Date
FilHdr_OrigModDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->HdrInf.OrigModDate;
   }/*FilHdr_OrigModDate*/



