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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmc/gmc.h>
#include <odin/inc/NodTyp_.h>
#include <odin/inc/TokTyp_.h>
#include <gmc/nod.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Func.hh>
#include <odin/inc/Var.hh>
#include <odin/inc/FilHdr.h>
#include <dg/inc/FKind_.h>
#include <odin/inc/Flag_.h>
#include <odin/inc/SKind_.h>
#include <odin/inc/Status_.h>


tp_Date		PendingDate = 0;


bool
IsSource_FKind(tp_FKind FKind)
{
   switch (FKind) {
      case FK_SrcReg: case FK_SrcDir: case FK_SymLinkReg: case FK_SymLinkDir:
      case FK_BoundSrc: case FK_BoundSymLink: {
	 return true; }/*case*/;
      case FK_User: case FK_Instance: case FK_Str:
      case FK_DrvDirElm: case FK_VirDirElm:
      case FK_PntrHo: case FK_InpPntr: case FK_PntrElm:
      case FK_ActTgtText: case FK_VirTgtText:
      case FK_ActTgtExText: case FK_VirTgtExText:
      case FK_VirTgt: case FK_VirCmdTgt: case FK_ActTgt: case FK_ActCmdTgt: {
	 return false; }/*case*/;
      default: {
	 FATALERROR("unknown FKind"); };}/*switch*/;
   /* NOTREACHED */
   return false;
   }/*IsSource_FKind*/


bool
IsSource(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return IsSource_FKind(FilHdr->HdrInf.FKind);
   }/*IsSource*/


bool
IsSymLink(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_SymLinkReg
	   || FilHdr->HdrInf.FKind == FK_SymLinkDir
	   || FilHdr->HdrInf.FKind == FK_BoundSymLink);
   }/*IsSymLink*/


bool
IsDir(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_SrcDir
	   || FilHdr->HdrInf.FKind == FK_SymLinkDir);
   }/*IsDir*/


bool
IsStr(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_Str);
   }/*IsStr*/


bool
IsBound(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_BoundSrc
	   || FilHdr->HdrInf.FKind == FK_BoundSymLink);
   }/*IsBound*/


bool
IsATgt(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return IsATgt_FKind(FilHdr->HdrInf.FKind);
   }/*IsATgt*/


bool
IsVTgt(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return IsVTgt_FKind(FilHdr->HdrInf.FKind);
   }/*IsVTgt*/


bool
IsVTgtText(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return IsVTgtText_FKind(FilHdr->HdrInf.FKind);
   }/*IsVTgtText*/


bool
IsDfltTgtVal(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->FilTyp == ActTargetsFilTyp);
   }/*IsDfltTgtVal*/


bool
IsPntr(tp_FilHdr FilHdr)
{
   tp_FKind FKind;

   FORBIDDEN(FilHdr == ERROR);
   FKind = FilHdr->HdrInf.FKind;
   return (IsPntr_FKind(FKind)
	   || (FKind == FK_User && IsPntr_FilTyp(FilHdr->FilTyp)));
   }/*IsPntr*/


bool
IsGeneric(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_User
	   && IsGeneric_FilTyp(FilHdr->FilTyp));
   }/*IsGeneric*/


bool
IsPipe(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_User
	   && IsPipe_FilTyp(FilHdr->FilTyp));
   }/*IsPipe*/


bool
IsInstance(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_Instance);
   }/*IsInstance*/


bool
IsAtmc(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   switch (FilHdr->HdrInf.FKind) {
      case FK_SrcReg: case FK_SrcDir: case FK_SymLinkReg: case FK_SymLinkDir:
      case FK_BoundSrc: case FK_BoundSymLink:
      case FK_DrvDirElm: case FK_Instance:
      case FK_ActTgtText: case FK_VirTgtText:
      case FK_ActTgtExText: case FK_VirTgtExText: {
	 return true; }/*case*/;
      case FK_User: case FK_Str: {
	 return IsAtmc_FilTyp(FilHdr->FilTyp); }/*case*/;
      case FK_ActTgt: case FK_ActCmdTgt: case FK_VirTgt: case FK_VirCmdTgt:
      case FK_PntrHo: case FK_PntrElm: case FK_InpPntr: case FK_VirDirElm: {
	 return false; }/*case*/;
      default: {
	 FATALERROR("Unexpected FKind"); };}/*switch*/;
   /*NOTREACHED*/
   return false;
   }/*IsAtmc*/


bool
IsList(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_User
	   && IsList_FilTyp(FilHdr->FilTyp));
   }/*IsList*/


bool
IsViewSpec(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_User
	   && FilHdr->FilTyp == ViewSpecFilTyp);
   }/*IsViewSpec*/


bool
IsStruct(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return IsStruct_FilTyp(FilHdr->FilTyp);
   }/*IsStruct*/


bool
IsStructMem(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return IsStructMem_FilTyp(FilHdr->FilTyp);
   }/*IsStructMem*/


bool
IsVoid(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (IsStr(FilHdr) || IsVoid_FilTyp(FilHdr->FilTyp));
   }/*IsVoid*/


bool
IsTargetsPtr(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return (FilHdr->FilTyp == TargetsPtrFilTyp);
   }/*IsTargetsPtr*/


bool
IsTargets(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return (FilHdr->FilTyp == TargetsFilTyp);
   }/*IsTargets*/


bool
IsDrvDir(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return IsDrvDir_FilTyp(FilHdr->FilTyp);
   }/*IsDrvDir*/


bool
IsDrvDirElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_DrvDirElm);
   }/*IsDrvDirElm*/


bool
IsVirDir(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return (FilHdr->FilTyp == VirDirFilTyp);
   }/*IsVirDir*/


bool
IsKeyList(tp_FilHdr FilHdr)
{
   tp_FilTyp FilTyp;

   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   FilTyp = FilHdr->FilTyp;
   return ((IsDrvDir_FilTyp(FilTyp) && FilTyp != VirDirFilTyp)
	   || FilTyp == ActTargetsFilTyp || FilTyp == VirTargetsFilTyp);
   }/*IsKeyList*/


bool
IsKeyListElm(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (FilHdr->HdrInf.FKind == FK_DrvDirElm
	   || FilHdr->HdrInf.FKind == FK_VirDirElm
	   || IsATgt_FKind(FilHdr->HdrInf.FKind)
	   || IsVTgt_FKind(FilHdr->HdrInf.FKind));
   }/*IsKeyListElm*/


bool
IsCopy(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.FKind != FK_User) {
      return false; }/*if*/;
   return IsCopy_FilTyp(FilHdr->FilTyp);
   }/*IsCopy*/


bool
IsAutoExec(tp_FilHdr FilHdr)
{
   tp_FilHdr TgtValFilHdr;
   bool Flag;

   if (IsExec_FilTyp(FilHdr->FilTyp)
       || FilHdr->HdrInf.FKind == FK_ActCmdTgt
       || FilHdr->HdrInf.FKind == FK_VirCmdTgt) {
      return true; }/*if*/;
   if (!IsBound(FilHdr)) {
      return false; }/*if*/;
   TgtValFilHdr = FilHdr_Father(FilHdr_TgtValFilHdr(Copy_FilHdr(FilHdr)));
   FORBIDDEN(TgtValFilHdr == NIL);
   Flag = IsAutoExec(TgtValFilHdr);
   Ret_FilHdr(TgtValFilHdr);
   return Flag;
   }/*IsAutoExec*/


bool
HasKey_FKind(tp_FKind FKind)
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_DrvDirElm || FKind == FK_VirDirElm
	   || FKind == FK_PntrElm || IsSource_FKind(FKind)
	   || IsATgt_FKind(FKind) || IsVTgt_FKind(FKind)
	   || IsATgtText_FKind(FKind) || IsVTgtText_FKind(FKind));
   }/*HasKey_FKind*/


bool
IsRef(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return (IsList(FilHdr) || IsPntr(FilHdr));
   }/*IsRef*/


tp_LocHdr
FilHdr_LocHdr(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->LocHdr;
   }/*FilHdr_LocHdr*/


tp_LocHdr
FilHdr_AliasLocHdr(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.AliasLocHdr;
   }/*FilHdr_AliasLocHdr*/


void
Set_AliasLocHdr(tp_FilHdr FilHdr,tp_LocHdr LocHdr)
{

   FORBIDDEN(FilHdr == ERROR);
   if (LocHdr == FilHdr->HdrInf.AliasLocHdr) {
      return; }/*if*/;
   FilHdr->HdrInf.AliasLocHdr = LocHdr;
   SetModified(FilHdr);
   }/*Set_AliasLocHdr*/


tp_FilHdr
FilHdr_AliasFilHdr(tp_FilHdr FilHdr)
{
   tp_FilHdr AliasFilHdr;

   if (FilHdr->HdrInf.AliasLocHdr == NIL) {
      return FilHdr; }/*if*/;
   AliasFilHdr = LocHdr_FilHdr(FilHdr->HdrInf.AliasLocHdr);
   Ret_FilHdr(FilHdr);
   FORBIDDEN(AliasFilHdr->HdrInf.AliasLocHdr != NIL);
   return AliasFilHdr;
   }/*FilHdr_AliasFilHdr*/


tp_FKind
FilHdr_FKind(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->HdrInf.FKind;
   }/*FilHdr_FKind*/


void
Set_FKind(tp_FilHdr FilHdr,tp_FKind FKind)
{
   FORBIDDEN(FilHdr == ERROR || FKind == ERROR);
   if (FilHdr->HdrInf.FKind != FKind) {
      FilHdr->HdrInf.FKind = FKind;
      SetModified(FilHdr); }/*if*/;
   }/*Set_FKind*/


tp_FilTyp
FilHdr_FilTyp(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   return FilHdr->FilTyp;
   }/*FilHdr_FilTyp*/


tp_FilPrm
FilHdr_FilPrm(tp_FilHdr FilHdr)
{
   if (FilHdr == ERROR) return ERROR;
   FORBIDDEN(FilHdr->FilPrm == NIL);
   return FilHdr->FilPrm;
   }/*FilHdr_FilPrm*/


tp_Ident
FilHdr_Ident(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   return FilHdr->Ident;
   }/*FilHdr_Ident*/


/* Update FilHdr which is a parent directory of an interesting
 * source file.
 *
 * - Note it is not necessary to stat Odinfile in this directory because
 *   odin cannot create i.e. derive source directories. That is why
 *   this is a separate function and not a call to Update_SrcFilHdr.
 * - REVISIT: since we know FilHdr is a directory we can remove code for
 *   other cases. (I don't think a file or directory's parent can be
 *   a soft-link?)
 */
void
Update_SrcParentDirFilHdr(tp_FilHdr FilHdr,bool InitOnly)
{
   tp_Date OldModDate;
   bool Changed;
   tp_SKind SKind;
   tp_FKind FKind;
   tp_HdrInf HdrInf;
   tp_Status Status;
   tps_FileName FileName, SymLinkFileName;
   int SysModTime;
   tp_FilHdr DirFilHdr, SymLinkFH;
   tp_FilElm FilElm;
   tp_LocHdr SymLocHdr;


   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   HdrInf = &(FilHdr->HdrInf);

   if (FilHdr == RootFilHdr) {
      Set_Status(FilHdr, STAT_OK);
      return; }/*if*/;

   if (FilHdr_Flag(FilHdr, FLAG_SymLink)) {
      Set_Status(FilHdr, STAT_Circular);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_SymLink);

   OldModDate = HdrInf->ModDate;
   IsAny_ReadyServerAction = true;

   DirFilHdr = FilHdr_Father(Copy_FilHdr(FilHdr));
   if (!IsSrcUpToDate(DirFilHdr)) {
      Update_SrcParentDirFilHdr(DirFilHdr, InitOnly); }/*if*/;

   Changed = false;
   SymLinkFH = NIL;
   /*select*/{
      if (IsSymLink(DirFilHdr)) {
	 SymLinkFH = Extend_FilHdr
	    (Deref_SymLink(Copy_FilHdr(DirFilHdr)), FK_SrcReg,
	     FilHdr->FilTyp, RootFilPrm, FilHdr->Ident);
      }else{
	 FilHdr_DataFileName(FileName, FilHdr);
	 Get_FileInfo(&SKind, &SysModTime, FileName);
	 if (HdrInf->SysModTime != SysModTime || HdrInf->ModDate == 0) {
	    Changed = true; }/*if*/;
	 switch (SKind) {
	    case SK_NoFile: case SK_Reg: case SK_Exec: case SK_Special: {
	       FKind = (IsBound(FilHdr) ? FK_BoundSrc : FK_SrcReg);
	       Status = ((SKind == SK_NoFile) ? STAT_NoFile : STAT_OK);
	       break;}/*case*/;
	    case SK_Dir: {
	       FKind = FK_SrcDir;
	       Status = STAT_OK; break;}/*case*/;
	    case SK_SymLink: {
	       Push_ContextFilHdr(Copy_FilHdr(DirFilHdr));
	       FileName_SymLinkFileName(SymLinkFileName, FileName);
	       SymLinkFH = HostFN_FilHdr(SymLinkFileName);
	       /*select*/{
		  if (SymLinkFH == ERROR) {
		     FKind = FK_SrcReg;
		     Status = STAT_NoFile;
		  }else if (!IsSource(SymLinkFH)) {
		     SystemError("Symbolic link into cache ignored: %s\n",
				 SymLinkFileName);
		     Ret_FilHdr(SymLinkFH);
		     SymLinkFH = ERROR;
		     FKind = FK_SrcReg;
		     Status = STAT_NoFile; };}/*select*/;
	       Pop_ContextFilHdr(); break;}/*case*/;
	    default: {
	       FATALERROR("Unexpected SKind"); };}/*switch*/; };}/*select*/;

   if (SymLinkFH != NIL) {
      if (!IsSrcUpToDate(SymLinkFH)) {
	 Update_SrcFilHdr(SymLinkFH, InitOnly); }/*if*/;
      SymLinkFH = Deref_SymLink(SymLinkFH);
      if (!IsSrcUpToDate(SymLinkFH)) {
	 Update_SrcFilHdr(SymLinkFH, InitOnly); }/*if*/;
      SymLinkFH = Deref_SymLink(SymLinkFH);
      if (HdrInf->ModDate < SymLinkFH->HdrInf.ModDate) {
	 Changed = true; }/*if*/;

      FilElm = LocElm_FilElm(HdrInf->LocElm);
      SymLocHdr = FilElm_LocHdr(FilElm);
      Ret_FilElm(FilElm);
      if (SymLocHdr != SymLinkFH->LocHdr) {
	 Set_LocElm(FilHdr, Make_LocElm(SymLinkFH, RootFilPrm, FilHdr));
	 Changed = true; }/*if*/;

      if (HdrInf->AliasLocHdr != NIL) {
	 if (SymLinkFH->HdrInf.AliasLocHdr == NIL) {
	    Set_AliasLocHdr(SymLinkFH, HdrInf->AliasLocHdr); }/*if*/;
	 Set_AliasLocHdr(FilHdr, (tp_LocHdr)NIL); }/*if*/;

      FKind = (IsBound(FilHdr) ? FK_BoundSymLink :
	       (IsDir(SymLinkFH) ? FK_SymLinkDir : FK_SymLinkReg));
      Status = SymLinkFH->HdrInf.Status;
      Ret_FilHdr(SymLinkFH); }/*if*/;

   if (HdrInf->FKind == FK_SrcDir && FKind == FK_SymLinkDir) {
      FilHdr_Error("<%s> has changed from a directory to a symbolic link.\n",
		   FilHdr);
      SystemError("The cache should be reset with the -r option.\n");
      FKind = FK_SrcDir; }/*if*/;

   if (Changed || HdrInf->FKind != FKind) {
      Set_ModDate(FilHdr);
      HdrInf->SysModTime = SysModTime;
      HdrInf->FKind = FKind;
      if (!IsSymLink(FilHdr)) {
	 Set_LocElm(FilHdr, (tp_LocElm)NIL); }/*if*/; }/*if*/;

   /*select*/{
      if (IsDir(FilHdr)) {
	 Set_TgtValLocElm(FilHdr, (tp_LocElm)NIL);
      }else if (FilHdr_TgtValLocElm(FilHdr) == NIL) {
	 Set_DfltTgtValLocElm(FilHdr); };}/*select*/;

   if (OldModDate != 0
       && (HdrInf->ModDate != OldModDate
	   || (HdrInf->Status != STAT_Unknown && HdrInf->Status != Status))) {
      /*select*/{
	 if (InitOnly) {
	    Push_ToBroadcast(Copy_FilHdr(FilHdr));
	 }else{
	    Broadcast(FilHdr, STAT_Unknown);};}/*select*/; }/*if*/;

   Ret_FilHdr(DirFilHdr);
   Set_Status(FilHdr, Status);
   Clr_Flag(FilHdr, FLAG_SymLink);
  }/*Update_SrcParentDirFilHdr*/


void
Update_SrcFilHdr(tp_FilHdr FilHdr,bool InitOnly)
{
   tp_Date OldModDate;
   bool Changed;
   tp_SKind SKind;
   tp_FKind FKind;
   tp_HdrInf HdrInf;
   tp_Status Status;
   tps_FileName FileName, SymLinkFileName;
   int SysModTime;
   tp_FilHdr DirFilHdr, SymLinkFH;
   tp_FilElm FilElm;
   tp_LocHdr SymLocHdr;


   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsSource(FilHdr));
   HdrInf = &(FilHdr->HdrInf);

   if (FilHdr == RootFilHdr) {
      Set_Status(FilHdr, STAT_OK);
      return; }/*if*/;

   if (FilHdr_Flag(FilHdr, FLAG_SymLink)) {
      Set_Status(FilHdr, STAT_Circular);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_SymLink);

   OldModDate = HdrInf->ModDate;
   IsAny_ReadyServerAction = true;

   DirFilHdr = FilHdr_Father(Copy_FilHdr(FilHdr));
   if (!IsSrcUpToDate(DirFilHdr)) {
      Update_SrcParentDirFilHdr(DirFilHdr, InitOnly); }/*if*/;

   Changed = false;
   SymLinkFH = NIL;
   /*select*/{
      if (IsSymLink(DirFilHdr)) {
	 SymLinkFH = Extend_FilHdr
	    (Deref_SymLink(Copy_FilHdr(DirFilHdr)), FK_SrcReg,
	     FilHdr->FilTyp, RootFilPrm, FilHdr->Ident);
      }else{
	 FilHdr_DataFileName(FileName, FilHdr);
	 Get_FileInfo(&SKind, &SysModTime, FileName);
	 if (HdrInf->SysModTime != SysModTime || HdrInf->ModDate == 0) {
	    Changed = true; }/*if*/;
	 switch (SKind) {
	    case SK_NoFile: case SK_Reg: case SK_Exec: case SK_Special: {
	       FKind = (IsBound(FilHdr) ? FK_BoundSrc : FK_SrcReg);
	       Status = ((SKind == SK_NoFile) ? STAT_NoFile : STAT_OK);
	       break;}/*case*/;
	    case SK_Dir: {
               tps_FileName OdinfileName;
               tp_FilHdr OdinfileFilHdr;
	       FKind = FK_SrcDir;
	       Status = STAT_OK;
               strncpy(OdinfileName,FileName,sizeof(OdinfileName));
               FORBIDDEN(OdinfileName[sizeof(OdinfileName)-1]!=0);
               OdinfileFilHdr=HostFN_FilHdr(OdinfileName);
               Update_SrcFilHdr(OdinfileFilHdr,InitOnly);
               Ret_FilHdr(OdinfileFilHdr);
               break;}/*case*/;
	    case SK_SymLink: {
	       Push_ContextFilHdr(Copy_FilHdr(DirFilHdr));
	       FileName_SymLinkFileName(SymLinkFileName, FileName);
	       SymLinkFH = HostFN_FilHdr(SymLinkFileName);
	       /*select*/{
		  if (SymLinkFH == ERROR) {
		     FKind = FK_SrcReg;
		     Status = STAT_NoFile;
		  }else if (!IsSource(SymLinkFH)) {
		     SystemError("Symbolic link into cache ignored: %s\n",
				 SymLinkFileName);
		     Ret_FilHdr(SymLinkFH);
		     SymLinkFH = ERROR;
		     FKind = FK_SrcReg;
		     Status = STAT_NoFile; };}/*select*/;
	       Pop_ContextFilHdr(); break;}/*case*/;
	    default: {
	       FATALERROR("Unexpected SKind"); };}/*switch*/; };}/*select*/;

   if (SymLinkFH != NIL) {
      if (!IsSrcUpToDate(SymLinkFH)) {
	 Update_SrcFilHdr(SymLinkFH, InitOnly); }/*if*/;
      SymLinkFH = Deref_SymLink(SymLinkFH);
      if (!IsSrcUpToDate(SymLinkFH)) {
	 Update_SrcFilHdr(SymLinkFH, InitOnly); }/*if*/;
      SymLinkFH = Deref_SymLink(SymLinkFH);
      if (HdrInf->ModDate < SymLinkFH->HdrInf.ModDate) {
	 Changed = true; }/*if*/;

      FilElm = LocElm_FilElm(HdrInf->LocElm);
      SymLocHdr = FilElm_LocHdr(FilElm);
      Ret_FilElm(FilElm);
      if (SymLocHdr != SymLinkFH->LocHdr) {
	 Set_LocElm(FilHdr, Make_LocElm(SymLinkFH, RootFilPrm, FilHdr));
	 Changed = true; }/*if*/;

      if (HdrInf->AliasLocHdr != NIL) {
	 if (SymLinkFH->HdrInf.AliasLocHdr == NIL) {
	    Set_AliasLocHdr(SymLinkFH, HdrInf->AliasLocHdr); }/*if*/;
	 Set_AliasLocHdr(FilHdr, (tp_LocHdr)NIL); }/*if*/;

      FKind = (IsBound(FilHdr) ? FK_BoundSymLink :
	       (IsDir(SymLinkFH) ? FK_SymLinkDir : FK_SymLinkReg));
      Status = SymLinkFH->HdrInf.Status;
      Ret_FilHdr(SymLinkFH); }/*if*/;

   if (HdrInf->FKind == FK_SrcDir && FKind == FK_SymLinkDir) {
      FilHdr_Error("<%s> has changed from a directory to a symbolic link.\n",
		   FilHdr);
      SystemError("The cache should be reset with the -r option.\n");
      FKind = FK_SrcDir; }/*if*/;

   if (Changed || HdrInf->FKind != FKind) {
      Set_ModDate(FilHdr);
      HdrInf->SysModTime = SysModTime;
      HdrInf->FKind = FKind;
      if (!IsSymLink(FilHdr)) {
	 Set_LocElm(FilHdr, (tp_LocElm)NIL); }/*if*/; }/*if*/;

   /*select*/{
      if (IsDir(FilHdr)) {
	 Set_TgtValLocElm(FilHdr, (tp_LocElm)NIL);
      }else if (FilHdr_TgtValLocElm(FilHdr) == NIL) {
	 Set_DfltTgtValLocElm(FilHdr); };}/*select*/;

   if (OldModDate != 0
       && (HdrInf->ModDate != OldModDate
	   || (HdrInf->Status != STAT_Unknown && HdrInf->Status != Status))) {
      /*select*/{
	 if (InitOnly) {
	    Push_ToBroadcast(Copy_FilHdr(FilHdr));
	 }else{
	    Broadcast(FilHdr, STAT_Unknown);};}/*select*/; }/*if*/;

   Ret_FilHdr(DirFilHdr);
   Set_Status(FilHdr, Status);
   Clr_Flag(FilHdr, FLAG_SymLink);
   }/*Update_SrcFilHdr*/


void
FilHdr_Error(
   tp_Str Str,
   tp_FilHdr FilHdr
   )
   
   
{
   tps_Str ObjName;

   SPrint_FilHdr(ObjName, FilHdr);
   SystemError(Str, ObjName);
   }/*FilHdr_Error*/


bool
IsAllDone(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   return (IsAllUpToDate(FilHdr, InpKind) &&
           !Is_PendingReadyOrBusy_Status(FilHdr_MinStatus(FilHdr, InpKind)));
   }/*IsAllDone*/


bool
IsAllUpToDate(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   if (!IsUpToDate(FilHdr)) {
      return false; }/*if*/;
   if (!IsTgtValUpToDate(FilHdr)) {
      return false; }/*if*/;
   if (NeedsElmData(FilHdr, InpKind)) {
      return IsElmUpToDate(FilHdr); }/*if*/;
   if (NeedsElmNameData(FilHdr, InpKind)) {
      return IsElmNameUpToDate(FilHdr); }/*if*/;
   return true;
   }/*IsAllUpToDate*/


void SPrint_VerifyDate(tp_Str To,
                       tp_Str Leader,
                       tp_FilHdr FilHdr,
                       tp_Str Trailer)
{
  sprintf(To, "%s%d%s", Leader, FilHdr->HdrInf.VerifyDate, Trailer);
}

bool
IsSrcUpToDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.VerifyDate < VerifyDate) {
      return false; }/*if*/;
   return (FilHdr->HdrInf.Status > STAT_Unknown);
   }/*IsSrcUpToDate*/


bool
IsUpToDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (FilHdr->HdrInf.VerifyDate < VerifyDate) {
      return false; }/*if*/;
   if (!IsTgtValUpToDate(FilHdr) ||
       ((Is_PendingReadyOrBusy_Status(FilHdr->HdrInf.Status) ||
         FilHdr->PndFlag)
        && FilHdr->HdrInf.VerifyDate < PendingDate)) {
      return false; }/*if*/;
   return (FilHdr->HdrInf.Status > STAT_Unknown);
   }/*IsUpToDate*/


bool
IsElmNameUpToDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsRef(FilHdr));
   if (FilHdr->HdrInf.Status <= STAT_Error) {
      return true; }/*if*/;
   if (FilHdr->HdrInf.ElmNameVerifyDate < VerifyDate) {
      return false; }/*if*/;
   if ((Is_PendingReadyOrBusy_Status(FilHdr->HdrInf.ElmNameStatus) ||
        FilHdr->ElmNamePndFlag)
       && FilHdr->HdrInf.ElmNameVerifyDate < PendingDate) {
      return false; }/*if*/;
   return (FilHdr->HdrInf.ElmNameStatus > STAT_Unknown);
   }/*IsElmNameUpToDate*/


bool
IsElmUpToDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   FORBIDDEN(!IsRef(FilHdr));
   if (FilHdr->HdrInf.Status <= STAT_Error) {
      return true; }/*if*/;
   if (FilHdr->HdrInf.ElmVerifyDate < VerifyDate) {
      return false; }/*if*/;
   if ((Is_PendingReadyOrBusy_Status(FilHdr->HdrInf.ElmStatus) ||
        FilHdr->ElmPndFlag)
       && FilHdr->HdrInf.ElmVerifyDate < PendingDate) {
      return false; }/*if*/;
   return (FilHdr->HdrInf.ElmStatus > STAT_Unknown);
   }/*IsElmUpToDate*/


void SPrint_TgtValVerifyDate(tp_Str To,
                             tp_Str Leader,
                             tp_FilHdr FilHdr,
                             tp_Str Trailer)
{
  sprintf(To, "%s%d%s", Leader, FilHdr->HdrInf.ElmVerifyDate, Trailer);
}

bool
IsTgtValUpToDate(tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);
   if (!IsSource(FilHdr)) {
      return true; }/*if*/;
   if (FilHdr->HdrInf.ElmVerifyDate < VerifyDate) {
      return false; }/*if*/;
   if ((Is_PendingReadyOrBusy_Status(FilHdr->HdrInf.ElmStatus) ||
        FilHdr->ElmPndFlag)
       && FilHdr->HdrInf.ElmVerifyDate < PendingDate) {
      return false; }/*if*/;
   return (FilHdr->HdrInf.ElmStatus > STAT_Unknown);
   }/*IsTgtValUpToDate*/


tp_FilHdr
FilHdr_Father(tp_FilHdr FilHdr)
{
   tp_LocHdr FatherLocHdr;

   FORBIDDEN(FilHdr == ERROR);
   FatherLocHdr = FilHdr->HdrInf.Father;
   Ret_FilHdr(FilHdr);
   return LocHdr_FilHdr(FatherLocHdr);
   }/*FilHdr_Father*/


tp_FilHdr
FilHdr_SrcFilHdr(tp_FilHdr FilHdr)
{
   tp_LocHdr SrcLocHdr;

   while (!IsSource(FilHdr)) {
      SrcLocHdr = FilHdr->HdrInf.Father;
      FORBIDDEN(SrcLocHdr == ERROR);
      Ret_FilHdr(FilHdr);
      FilHdr = LocHdr_FilHdr(SrcLocHdr); }/*while*/;
   return FilHdr;
   }/*FilHdr_SrcFilHdr*/


tp_FilHdr
FilHdr_DirFilHdr(tp_FilHdr FilHdr)
{
   tp_FilHdr DirFilHdr;

   DirFilHdr = FilHdr_SrcFilHdr(FilHdr);
   if (DirFilHdr == RootFilHdr) {
      return DirFilHdr; }/*if*/;
   return FilHdr_Father(DirFilHdr);
   }/*FilHdr_DirFilHdr*/


tp_Str
FilHdr_Key(tp_Str StrBuf,tp_FilHdr FilHdr)
{
   FORBIDDEN(FilHdr == ERROR);

   if (!HasKey_FKind(FilHdr->HdrInf.FKind)) {
      return NIL; }/*if*/;
   return FilHdr_Label(StrBuf, FilHdr, false);
   }/*FilHdr_Key*/


tp_Label
FilHdr_Label(tp_Str StrBuf,tp_FilHdr FilHdr,bool UniqueFlag)
{
   FORBIDDEN(FilHdr == ERROR);

   if (HasKey_FKind(FilHdr_FKind(FilHdr)) && !UniqueFlag) {
      (void)strcpy(StrBuf, FilHdr->Ident);
      return StrBuf; }/*if*/;
   Build_Label(StrBuf, FilHdr->Ident, FilHdr->FilTyp, FilHdr_LocHdr(FilHdr),
	       UniqueFlag);
   return StrBuf;
   }/*FilHdr_Label*/


tp_FilHdr
FilHdr_ElmFilHdr(tp_FilHdr FilHdr)
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;

   FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
   FORBIDDEN(FilElm != NIL && FilElm_Next(FilElm) != NIL);
   ElmFilHdr = FilElm_FilHdr(FilElm);
   Ret_FilHdr(FilHdr); Ret_FilElm(FilElm);
   return ElmFilHdr;
   }/*FilHdr_ElmFilHdr*/

