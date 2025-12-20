/*
Copyright (C) 1991 Geoffrey M. Clemm

This file is part of the Odin system.

The Odin system is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation (see the file COPYING).

The Odin system is distributed WITHOUT ANY WARRANTY, without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

geoff@boulder.colorado.edu
*/

#include <gmc/gmc.h>
#include <odin/inc/NodTyp_.h>
#include <odin/inc/TokTyp_.h>
#include <gmc/nod.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Func.hh>
#include <odin/inc/Var.hh>
#include <odin/inc/FilElm.h>
#include <odin/inc/FilHdr.h>
#include <odin/inc/FilInp.h>
#include <dg/inc/FKind_.h>
#include <odin/inc/HdrInf.h>
#include <odin/inc/LogLevel_.h>
#include <odin/inc/ModKind_.h>
#include <odin/inc/Outputs.h>
#include <odin/inc/Status_.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>


extern int	num_FHLstS;
extern int	num_DrvPthS;
extern int	num_DrvSpcS;
extern int	num_StrS;
extern int	num_FilHdrS;
extern int	num_FilElmS;
extern int	num_FilInpS;
extern int	num_FilPrmS;
extern int	num_FilPValS;
extern int	num_PosS;
extern int	num_NodS;
extern int	num_StackElmS;
extern int	num_PrmFHdrS;

bool			DebugLocElm = false;

static tp_FilDsc	DebugFD = NIL;


static void
Local_Redo1(
   tp_FilHdr FilHdr
   )
   
{
   tp_FilHdr TmpFilHdr;

   if (FilHdr == ERROR) {
      return; }/*if*/;

   TmpFilHdr = Copy_FilHdr(FilHdr);
   if (IsInstance(TmpFilHdr)) {
      TmpFilHdr = FilHdr_Father(TmpFilHdr); }/*if*/;
   if (IsKeyListElm(TmpFilHdr)) {
      TmpFilHdr = FilHdr_Father(TmpFilHdr); }/*if*/;
   if (IsStructMem(TmpFilHdr)) {
      TmpFilHdr = FilHdr_Father(TmpFilHdr); }/*if*/;
   Clr_Status(TmpFilHdr);
   Clr_ConfirmDate(TmpFilHdr);
   Broadcast(TmpFilHdr, STAT_Unknown);
   if (IsPntr(TmpFilHdr)) {
      TmpFilHdr = FilHdr_ElmFilHdr(TmpFilHdr);
      if (FilHdr_Status(TmpFilHdr) != STAT_Unknown
	  || FilHdr_ConfirmDate(TmpFilHdr) != 0) {
	 Local_Redo1(TmpFilHdr); }/*if*/; }/*if*/;
   Ret_FilHdr(TmpFilHdr);
   }/*Local_Redo1*/


void
Local_Redo(
   tp_Str OdinExpr
   )
   
{
   tp_FilHdr FilHdr;

   FilHdr = OdinExpr_FilHdr(OdinExpr);
   Local_Redo1(FilHdr);
   Ret_FilHdr(FilHdr);
   }/*Local_Redo*/


void
Local_OdinExpr_ID(
   int* IDPtr,
   tp_Str OdinExpr
   )
   
   
{
   tp_FilHdr FilHdr;

   FilHdr = OdinExpr_FilHdr(OdinExpr);
   if (FilHdr == ERROR) {
      *IDPtr = 0;
      return; }/*if*/;
   *IDPtr = (int)FilHdr_LocHdr(FilHdr);
   Ret_FilHdr(FilHdr);
   }/*Local_OdinExpr_ID*/


void
Local_ID_OdinExpr(
   tp_Str OdinExpr,
   int ID
   )
   
   
{
   tp_FilHdr FilHdr;

   FilHdr = LocHdr_FilHdr((tp_LocHdr)ID);
   if (FilHdr == ERROR) {
      (void)strcpy(OdinExpr, "");
      return; }/*if*/;
   SPrint_FilHdr(OdinExpr, FilHdr);
   Ret_FilHdr(FilHdr);
   }/*Local_ID_OdinExpr*/


void
Local_ID_LongOdinExpr(
   tp_Str OdinExpr,
   int ID
   )
   
   
{
   tp_FilHdr FilHdr;

   FilHdr = LocHdr_FilHdr((tp_LocHdr)ID);
   if (FilHdr == ERROR) {
      (void)strcpy(OdinExpr, "");
      return; }/*if*/;
   VerboseSPrint_FilHdr(OdinExpr, FilHdr);
   Ret_FilHdr(FilHdr);
   }/*Local_ID_LongOdinExpr*/

void SPrint_Status(tp_Str To, tp_Str Leader, tp_Status Status, tp_Str Trailer){
  switch(Status){
  case STAT_Unknown:
    sprintf(To, "%sUnknown%s", Leader, Trailer); break;
  case STAT_Pending:
    sprintf(To, "%sPending%s", Leader, Trailer); break;
  case STAT_Ready:
    sprintf(To, "%sReady%s", Leader, Trailer); break;
  case STAT_Busy:
    sprintf(To, "%sBusy%s", Leader, Trailer); break;
  case STAT_SysAbort:
    sprintf(To, "%sSysAbort%s", Leader, Trailer); break;
  case STAT_NoFile:
    sprintf(To, "%sNoFile%s", Leader, Trailer); break;
  case STAT_Circular:
    sprintf(To, "%sCircular%s", Leader, Trailer); break;
  case STAT_ElmCircular:
    sprintf(To, "%sElmCircular%s", Leader, Trailer); break;
  case STAT_Error:
    sprintf(To, "%sError%s", Leader, Trailer); break;
  case STAT_TgtValError:
    sprintf(To, "%sTgtValError%s", Leader, Trailer); break;
  case STAT_Warning:
    sprintf(To, "%sWarning%s", Leader, Trailer); break;
  case STAT_OK:
    sprintf(To, "%sOK%s", Leader, Trailer); break;
  default:
    sprintf(To, "%s?%d?%s", Leader, Status, Trailer);
  }
}

void
Do_Log(
   const char* Message,
   tp_FilHdr FilHdr,
   tp_LogLevel LogLevel
   )
{
   tps_Str StrBuf;
   struct timeval now;

   if (Client_LogLevel(CurrentClient) < LogLevel) {
      return; }/*if*/;
   gettimeofday(&now, 0);
   (void)sprintf(StrBuf, "** %d.%03d %s ", now.tv_sec, now.tv_usec/1000, Message);
   SPrint_FilHdr(Tail(StrBuf), FilHdr);
   SPrint_Status(Tail(StrBuf), " [",FilHdr_Status(FilHdr), "");
   SPrint_VerifyDate(Tail(StrBuf), "@", FilHdr, "");
   SPrint_Status(Tail(StrBuf), "/",FilHdr_TgtValStatus(FilHdr), "");
   SPrint_TgtValVerifyDate(Tail(StrBuf), "@", FilHdr, "]");
   
   LogMessage(StrBuf);
   }/*Do_Log*/


tp_FilHdr
OdinExpr_FilHdr(
   const char* OdinExpr
   )
   
{
   tp_Nod DS_Nod;
   tp_PrmFHdr PrmFHdr;
   tp_FilHdr FilHdr;
   tp_FilPrm FilPrm;

   DS_Nod = YY_Parser(OdinExpr, (tp_FileName)NIL, (int *)NIL);
   PrmFHdr = Nod_PrmFHdr(DS_Nod);
   Ret_Nod(DS_Nod);
   Use_PrmFHdr(&FilHdr, &FilPrm, PrmFHdr);
   return FilHdr;
   }/*OdinExpr_FilHdr*/


void
WritePrmOdinExpr(
   tp_FilDsc FilDsc,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm
   )
   
   
   
{
   Print_FilHdr(FilDsc, (tp_Str)NIL, FilHdr);
   Print_FilPrm(FilDsc, (tp_Str)NIL, FilPrm);
   Writeln(FilDsc, "");
   }/*WritePrmOdinExpr*/


void
Local_Set_Debug(
   tp_Str DebugName
   )
   
{
   tps_FileName DebugFileName;

   /*select*/{
      if (strcasecmp(DebugName, "sizes") == 0) {
	 SystemError("num_CWDFilHdrS = %d\n", num_FHLstS);
	 SystemError("num_DrvPthS = %d\n", num_DrvPthS);
	 SystemError("num_DrvSpcS = %d\n", num_DrvSpcS);
	 SystemError("num_StrS = %d\n", num_StrS);
	 SystemError("num_FilHdrS = %d\n", num_FilHdrS);
	 SystemError("num_FilElmS = %d\n", num_FilElmS);
	 SystemError("num_FilInpS = %d\n", num_FilInpS);
	 SystemError("num_FilPrmS = %d\n", num_FilPrmS);
	 SystemError("num_FilPValS = %d\n", num_FilPValS);
	 SystemError("num_PosS = %d\n", num_PosS);
	 SystemError("num_NodS = %d\n", num_NodS);
	 SystemError("num_StackElmS = %d\n", num_StackElmS);
	 SystemError("num_PrmFHdrS = %d\n", num_PrmFHdrS);
      }else if (strcasecmp(DebugName, "symbols") == 0) {
	 Write_Syms(StdOutFD);
      }else if (strcasecmp(DebugName, "locelm") == 0) {
	 if (DebugFD == NIL) {
	    Get_DebugFileName(DebugFileName);
	    DebugFD = FileName_AFilDsc(DebugFileName, false); }/*if*/;
	 DebugLocElm = true;
      }else{
	 SystemError("** Unknown debug request.\n"); };}/*select*/;
   }/*Local_Set_Debug*/


void
Local_Get_Status(
   tp_Status* StatusPtr,
   tp_Status* ElmStatusPtr,
   int ID
   )
   
   
   
{
   tp_FilHdr FilHdr;

   FilHdr = LocHdr_FilHdr((tp_LocHdr)ID);
   if (FilHdr == ERROR) {
      *StatusPtr = STAT_Error;
      *ElmStatusPtr = STAT_Error;
      return; }/*if*/;
   *StatusPtr = FilHdr_TgtValMinStatus(FilHdr);
   *ElmStatusPtr = FilHdr_ElmStatus(FilHdr);
   Ret_FilHdr(FilHdr);
   }/*Local_Get_Status*/


static void
Print_ElmLink(
   tp_LocHdr LocHdr
   )
   
{
   tp_FilHdr FilHdr;
   tp_LocElm LocElmLink, FirstElmLink, NxtLocElmLink;
   tp_FilElm FilElmLink;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      return; }/*if*/;
   LocElmLink = FilHdr_ElmLink(FilHdr);
   FirstElmLink = LocElmLink;
   NxtLocElmLink = NIL;
   while (NxtLocElmLink != FirstElmLink) {
      FilElmLink = LocElm_FilElm(LocElmLink);
      Print_OdinExpr(FilElm_ListLocHdr(FilElmLink), RootFilPrm);
      FORBIDDEN(FilElmLink->ElmInf.LocHdr != LocHdr);
      NxtLocElmLink = FilElm_Link(FilElmLink);
      Ret_FilElm(FilElmLink);
      LocElmLink = NxtLocElmLink; }/*while*/;
   Ret_FilHdr(FilHdr);
   }/*Print_ElmLink*/


static void
Print_LocElm(
   tp_LocHdr LocHdr
   )
   
{
   tp_FilHdr FilHdr, TgtValFilHdr;
   tp_FilElm FilElm;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      return; }/*if*/;
   TgtValFilHdr = FilHdr_TgtValFilHdr(Copy_FilHdr(FilHdr));
   if (TgtValFilHdr != NIL) {
      FORBIDDEN(!IsSource(FilHdr));
      Print_OdinExpr(TgtValFilHdr->LocHdr, RootFilPrm);
      Ret_FilHdr(TgtValFilHdr); }/*if*/;
   if (!(IsRef(FilHdr) || IsSymLink(FilHdr))) {
      SystemError("** Not a list or pointer file.\n");
      Ret_FilHdr(FilHdr);
      return; }/*if*/;
   if (!IsUpToDate(FilHdr)) {
      SystemError("** Warning: element list is not up-to-date.\n"); }/*if*/;
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      FORBIDDEN(FilElm_ListLocHdr(FilElm) != LocHdr);
      Print_OdinExpr(FilElm->ElmInf.LocHdr, FilElm_FilPrm(FilElm)); }/*for*/;
   Ret_FilHdr(FilHdr);
   }/*Print_LocElm*/


static void
Print_InpLink(
   tp_LocHdr LocHdr
   )
   
{
   tp_FilHdr FilHdr;
   tp_LocInp LocInpLink, FirstInpLink, NxtLocInpLink;
   tp_FilInp FilInpLink;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      return; }/*if*/;
   LocInpLink = FilHdr_InpLink(FilHdr);
   FirstInpLink = LocInpLink;
   NxtLocInpLink = NIL;
   while (NxtLocInpLink != FirstInpLink) {
      FilInpLink = LocInp_FilInp(LocInpLink);
      Print_OdinExpr(FilInp_OutLocHdr(FilInpLink), RootFilPrm);
      FORBIDDEN(FilInpLink->InpInf.LocHdr != LocHdr);
      NxtLocInpLink = FilInp_Link(FilInpLink);
      Ret_FilInp(FilInpLink);
      LocInpLink = NxtLocInpLink; }/*while*/;
   Ret_FilHdr(FilHdr);
   }/*Print_InpLink*/


static void
Print_LocInp(
   tp_LocHdr LocHdr
   )
   
{
   tp_FilHdr FilHdr;
   tp_FilInp FilInp;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      return; }/*if*/;
   for (FilInp = LocInp_FilInp(FilHdr_LocInp(FilHdr));
	FilInp != NIL;
	FilInp = FilInp_NextFilInp(FilInp)) {
      Print_OdinExpr(FilInp->InpInf.LocHdr, RootFilPrm);
      FORBIDDEN(FilInp_OutLocHdr(FilInp) != LocHdr); }/*for*/;
   Ret_FilHdr(FilHdr);
   }/*Print_LocInp*/


void
Local_Get_Elements(
   int ID
   )
   
{
   if (ID <= 0) {
      SystemError("Bad object reference.\n");
      return; }/*if*/;
   Print_LocElm((tp_LocHdr)ID);
   }/*Local_Get_Elements*/


void
Local_Get_ElementsOf(
   int ID
   )
   
{
   if (ID <= 0) {
      SystemError("Bad object reference.\n");
      return; }/*if*/;
   Print_ElmLink((tp_LocHdr)ID);
   }/*Local_Get_ElementsOf*/


void
Local_Get_Inputs(
   int ID
   )
   
{
   if (ID <= 0) {
      SystemError("Bad object reference.\n");
      return; }/*if*/;
   Print_LocInp((tp_LocHdr)ID);
   }/*Local_Get_Inputs*/


void
Local_Get_Outputs(
   int ID
   )
   
{
   if (ID <= 0) {
      SystemError("Bad object reference.\n");
      return; }/*if*/;
   Print_InpLink((tp_LocHdr)ID);
   }/*Local_Get_Outputs*/


void
Debug_Alloc_ElmInf(
   tp_LocElm LocElm,
   tp_LocElm FreeLocElm
   )
   
   
{
   Unwind(DebugFD);
   WriteInt(DebugFD, (int)LocElm);
   Write(DebugFD, " re-allocated, new Free = ");
   WriteInt(DebugFD, (int)FreeLocElm);
   Writeln(DebugFD, "");
   Flush(DebugFD);
   }/*Debug_Alloc_ElmInf*/


void
Debug_Ret_ElmInf(
   tp_LocElm LocElm
   )
   
{
   Unwind(DebugFD);
   WriteInt(DebugFD, (int)LocElm);
   Write(DebugFD, " de-allocated");
   Writeln(DebugFD, "");
   Flush(DebugFD);
   }/*Debug_Ret_ElmInf*/


void
Validate_LocElm(
   tp_FilHdr FilHdr,
   tp_LocElm LocElm
   )
   
   
{
   tp_LocHdr LocHdr;
   tp_FilElm FilElm;

   LocHdr = FilHdr_LocHdr(FilHdr);
   for (FilElm = LocElm_FilElm(LocElm);
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      FORBIDDEN(FilElm_ListLocHdr(FilElm) != LocHdr); }/*for*/;
   }/*Validate_LocElm*/


void
Print_OdinExpr(
   tp_LocHdr LocHdr,
   tp_FilPrm FilPrm
   )
   
   
{
   tp_FilHdr FilHdr;
   tps_Str OdinExpr;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      (void)strcpy(OdinExpr, "");
      return; }/*if*/;
   SPrint_FilHdr(OdinExpr, FilHdr);
   if (FilPrm != RootFilPrm)  {
      Print_FilPrm((tp_FilDsc)NIL, Tail(OdinExpr), FilPrm); }/*if*/;
   Next_OdinFile(OdinExpr, (int)LocHdr);
   Ret_FilHdr(FilHdr);
   }/*Print_OdinExpr*/


/*for debugging*/
void
printte(
   tp_LocHdr LocHdr
   )
   
{
   tp_FilHdr FilHdr;

   FilHdr = LocHdr_FilHdr(LocHdr);
   if (FilHdr == ERROR) {
      return; }/*if*/;
   WriteInt(StdOutFD, (int)LocHdr);
   Write(StdOutFD, "\t- ");
   Print_FilHdr(StdOutFD, (tp_Str)NIL, FilHdr);
   Writeln(StdOutFD, "");
   Ret_FilHdr(FilHdr);
   }/*printte*/


