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
#include <dg/inc/FKind_.h>
#include <odin/inc/Flag_.h>
#include <odin/inc/LogLevel_.h>
#include <odin/inc/NodTyp_.h>
#include <odin/inc/Status_.h>
#include <odin/inc/LookupPlainAbsPath.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void
WriteCat(
   tp_FilDsc FilDsc,
   tp_FilHdr FilHdr
   )
   
   
{
   tp_FilElm FilElm;
   tp_FilDsc InFD;
   tps_FileName FileName;
   tp_FilHdr ElmFilHdr;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      FilHdr_Error("Cycle detected at: %s\n", FilHdr);
      return; }/*if*/;
   if (IsViewSpec(FilHdr)) {
      FilHdr_Error("Illegal view specification argument: %s\n", FilHdr);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);
   /*select*/{
      if (IsStr(FilHdr)) {
	 FilHdr_DataFileName(FileName, FilHdr);
	 Writeln(FilDsc, FileName);
      }else if (!IsRef(FilHdr)) {
	 FilHdr_DataFileName(FileName, FilHdr);
	 InFD = FileName_RFilDsc(FileName, false);
	 if (InFD != NIL) {
	    FileCopy(FilDsc, InFD);
	    Close(InFD); }/*if*/;
      }else{
	 for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	      FilElm != NIL;
	      FilElm = FilElm_NextFilElm(FilElm)) {
	    ElmFilHdr = FilElm_FilHdr(FilElm);
	    FORBIDDEN(ElmFilHdr == ERROR);
	    WriteCat(FilDsc, ElmFilHdr);
	    Ret_FilHdr(ElmFilHdr); }/*for*/; };}/*select*/;
   Clr_Flag(FilHdr, FLAG_Union);
   }/*WriteCat*/


void
WriteFlat(
   tp_FilDsc FilDsc,
   tp_FilHdr FilHdr
   )
   
   
{
   tp_FilElm FilElm;
   tps_FileName FileName;
   tp_FilHdr ElmFilHdr;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      FilHdr_Error("Cycle detected at: %s\n", FilHdr);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);
   /*select*/{
      if (!IsRef(FilHdr)) {
	 FilHdr_DataFileName(FileName, FilHdr);
	 Writeln(FilDsc, FileName);
      }else{
	 for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	      FilElm != NIL;
	      FilElm = FilElm_NextFilElm(FilElm)) {
	    ElmFilHdr = FilElm_FilHdr(FilElm);
	    FORBIDDEN(ElmFilHdr == ERROR);
	    WriteFlat(FilDsc, ElmFilHdr);
	    Ret_FilHdr(ElmFilHdr); }/*for*/; };}/*select*/;
   Clr_Flag(FilHdr, FLAG_Union);
   }/*WriteFlat*/


void
WriteNames(
   tp_FilDsc FilDsc,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm
   )
   
   
   
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      FilHdr_Error("Cycle detected at: %s\n", FilHdr);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);
   /*select*/{
      if (!IsRef(FilHdr)) {
	 WritePrmOdinExpr(FilDsc, FilHdr, FilPrm);
      }else{
	 for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	      FilElm != NIL;
	      FilElm = FilElm_NextFilElm(FilElm)) {
	    ElmFilHdr = FilElm_FilHdr(FilElm);
	    WriteNames(FilDsc, ElmFilHdr,
		       Append_FilPrm(FilElm_FilPrm(FilElm), FilPrm));
	    Ret_FilHdr(ElmFilHdr); }/*for*/; };}/*select*/;
   Clr_Flag(FilHdr, FLAG_Union);
   }/*WriteNames*/


void
WriteLabels(
   tp_FilDsc FilDsc,
   tp_FilHdr FilHdr
   )
   
   
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;
   tps_Str StrBuf;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      FilHdr_Error("Cycle detected at: %s\n", FilHdr);
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);
   /*select*/{
      if (!IsRef(FilHdr)) {
	 Writeln(FilDsc, FilHdr_Label(StrBuf, FilHdr, false));
      }else{
	 for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	      FilElm != NIL;
	      FilElm = FilElm_NextFilElm(FilElm)) {
	    ElmFilHdr = FilElm_FilHdr(FilElm);
	    FORBIDDEN(ElmFilHdr == ERROR);
	    WriteLabels(FilDsc, ElmFilHdr);
	    Ret_FilHdr(ElmFilHdr); }/*for*/; };}/*select*/;
   Clr_Flag(FilHdr, FLAG_Union);
   }/*WriteLabels*/


static void
Get_Union(
   tp_LocElm* FirstLEPtr,
   tp_LocElm* LastLEPtr,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm,
   tp_FilHdr ListFilHdr
   )
   
   
   
   
   
{
   tp_FilHdr ElmFilHdr;
   tp_LocElm LocElm;
   tp_FilElm FilElm;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);

   if (!IsRef(FilHdr)) {
      LocElm = Make_LocElm(FilHdr, FilPrm, ListFilHdr);
      Chain_LocElms(FirstLEPtr, LastLEPtr, LocElm);
      return; }/*if*/;

   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      Get_Union(FirstLEPtr, LastLEPtr, ElmFilHdr,
		Append_FilPrm(FilElm_FilPrm(FilElm), FilPrm), ListFilHdr);
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   }/*Get_Union*/


tp_LocElm
Make_UnionLocElm(
   tp_FilHdr FilHdr,
   tp_FilHdr ListFilHdr
   )
   
   
{
   tp_LocElm FirstLE, LastLE;

   FirstLE = NIL; LastLE = NIL;
   Get_Union(&FirstLE, &LastLE, FilHdr, FilHdr_FilPrm(ListFilHdr), ListFilHdr);
   Clr_UnionFlags(FilHdr);
   return FirstLE;
   }/*Make_UnionLocElm*/


void
Clr_UnionFlags(
   tp_FilHdr FilHdr
   )
   
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;

   if (!FilHdr_Flag(FilHdr, FLAG_Union)) {
      return; }/*if*/;
   Clr_Flag(FilHdr, FLAG_Union);
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      Clr_UnionFlags(ElmFilHdr);
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   }/*Clr_UnionFlags*/


static bool
TagStrCmp(
   tp_Str Str,
   tp_Str Tag
   )
   
   
{
   if (strcmp(Tag, "") == 0) {
      while (*Str == ' ' || *Str == '\t') {
	 Str += 1; }/*while*/; }/*if*/;
   return strcmp(Str, Tag);
   }/*TagStrCmp*/


static void
Get_DefInfo(
   tp_Str* NamePtr,
   bool* CmdFlagPtr,
   bool* ExecFlagPtr,
   tp_Str* TagStrPtr,
   tp_Nod* TgtNodPtr,
   tp_Nod Nod
   )
   
   
   
   
   
   
{
   tp_Nod ValNod;

   *NamePtr = Sym_Str(Nod_Sym(Nod_Son(1, Nod)));
   *TagStrPtr = NIL;
   *TgtNodPtr = NIL;
   ValNod = Nod_Son(2, Nod);
   *CmdFlagPtr = (Nod_NodTyp(ValNod) == EXEDEF);
   if (*CmdFlagPtr) {
      ValNod = Nod_Brother(ValNod); }/*if*/;
   *ExecFlagPtr = (Nod_NodTyp(ValNod) == ETXDEF);
   if (*ExecFlagPtr || Nod_NodTyp(ValNod) == TEXDEF) {
      *TagStrPtr = Sym_Str(Nod_Sym(Nod_Son(1, ValNod)));
      if (*TagStrPtr == NIL) {
	 *TagStrPtr = ""; }/*if*/;
      return; }/*if*/;
   *TgtNodPtr = ValNod;
   }/*Get_DefInfo*/

int IsPlainAbsPath(
    tp_Nod DS_Nod
  )
   
{
  return
    Nod_NodTyp(DS_Nod)==8 && //DRVFIL
    Nod_Son(1,DS_Nod) &&
    Nod_NodTyp(Nod_Son(1,DS_Nod))==23 &&
    Nod_Brother(Nod_Son(1,DS_Nod))==0 &&
    Nod_Son(1,Nod_Son(1,DS_Nod))==0 &&
    Nod_Sym(Nod_Son(1,DS_Nod)) &&
    Sym_Str(Nod_Sym(Nod_Son(1,DS_Nod))) &&
    Sym_Str(Nod_Sym(Nod_Son(1,DS_Nod)))[0]=='/';
}


void
Exec_List(
   tp_FilHdr ListFilHdr,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm,
   bool IsOdinfile
   )
   
   
   
   
{
   tps_FileName FileName;
   tp_LocElm FirstLE, LastLE;
   tp_FilDsc InFD;
   tp_Str Str, Name, TagStr;
   tps_Str StrBuf;
   tp_Nod Nod, DS_Nod, TgtNod;
   tp_NodTyp NodTyp;
   int LineNum;
   tp_LocElm NewLocElm;
   tp_PrmFHdr PrmFHdr;
   tp_FilHdr ElmFilHdr;
   tp_FilPrm ElmFilPrm;
   bool End, CmdFlag, ExecFlag;

   FirstLE = NIL; LastLE = NIL;

   FilHdr_DataFileName(FileName, FilHdr);
   if (IsDirectory_FileName(FileName)) {
      InFD = OpenDir(FileName);
      for (ReadDir(StrBuf, &End, InFD); !End; ReadDir(StrBuf, &End, InFD)) {
	 ElmFilHdr = Do_Key(Copy_FilHdr(FilHdr), StrBuf);
	 NewLocElm = Make_LocElm(ElmFilHdr, RootFilPrm, ListFilHdr);
	 Ret_FilHdr(ElmFilHdr);
	 Chain_LocElms(&FirstLE, &LastLE, NewLocElm); }/*for*/;
      CloseDir(InFD);
      Set_LocElm(ListFilHdr, FirstLE);
      return; }/*if*/;

   InFD = FileName_RFilDsc(FileName, false);
   if (InFD == ERROR) {
      Set_LocElm(ListFilHdr, (tp_LocElm)NIL);
      return; }/*if*/;

   Push_ContextFilHdr(Copy_FilHdr(FilHdr));
   LineNum = 0;
   for (Str = ReadLine(StrBuf, InFD);
	Str != ERROR;
	Str = ReadLine(StrBuf, InFD)) {
      Nod = YY_Parser(Str, FileName, &LineNum);
      DS_Nod = Nod;
      NodTyp = Nod_NodTyp(DS_Nod);
      if (IsOdinfile && (NodTyp == FILDEF || NodTyp == SEGDEF)) {
        Get_DefInfo(&Name, &CmdFlag, &ExecFlag, &TagStr, &TgtNod, Nod);
        if (TagStr != NIL) {
          if (TagStr[0] == '\n') {
            TagStr = &TagStr[1]; }/*if*/;
          for (Str = Readln(StrBuf, InFD), LineNum += 1;
               Str != ERROR && TagStrCmp(Str, TagStr) != 0;
               Str = Readln(StrBuf, InFD), LineNum += 1) {
          }/*for*/;
          if (Str == ERROR && strlen(TagStr) != 0) {
            SystemError("Terminator \"%s\" not found for target \"%s\".\n",
                        TagStr, Name); }/*if*/; }/*if*/;
        DS_Nod = ERROR; }/*if*/;
      if (IsOdinfile && Nod_NodTyp(DS_Nod) == NSTDEF) {
        DS_Nod = Nod_Son(1, DS_Nod); }/*if*/;
      if (Nod_NodTyp(DS_Nod) == DRVFLS) {
        DS_Nod = Nod_Son(1, DS_Nod); }/*if*/;
      if (!IsOdinfile &&
          IsPlainAbsPath(DS_Nod) &&
          FilPrm==RootFilPrm) {
        tps_Str Remaining;
        tp_FilHdr FilHdr;
        FilHdr=LookupPlainAbsPath(Sym_Str(Nod_Sym(Nod_Son(1,DS_Nod))));
        PrmFHdr=New_PrmFHdr(FilHdr, RootFilPrm);
        Use_PrmFHdr(&ElmFilHdr,&ElmFilPrm,PrmFHdr);
        if (ElmFilHdr == ERROR) {
          SystemError("in odin expression at:\n");
          FileError("\n");
        }else{
          NewLocElm = Make_LocElm(ElmFilHdr,
                                  Append_FilPrm(ElmFilPrm, FilPrm),
                                  ListFilHdr);
          Ret_FilHdr(ElmFilHdr);
          Chain_LocElms(&FirstLE, &LastLE, NewLocElm); 
        }
      }
      else {
        while (DS_Nod != NIL) {
          PrmFHdr = Nod_PrmFHdr(DS_Nod);
          Use_PrmFHdr(&ElmFilHdr, &ElmFilPrm, PrmFHdr);
          /*select*/{
	    if (ElmFilHdr == ERROR) {
              SystemError("in odin expression at:\n");
              FileError("\n");
	    }else{
              NewLocElm = Make_LocElm(ElmFilHdr,
                                      Append_FilPrm(ElmFilPrm, FilPrm),
                                      ListFilHdr);
              Ret_FilHdr(ElmFilHdr);
              Chain_LocElms(&FirstLE, &LastLE, NewLocElm); };}/*select*/;
          DS_Nod = Nod_Brother(DS_Nod); }/*while*/;
      }
      Ret_Nod(Nod); }/*for*/;
   if (!EndOfFile(InFD)) {
     FileError("Unexpected EOF\n"); }/*if*/;

   Pop_ContextFilHdr();
   Close(InFD);
   Set_LocElm(ListFilHdr, FirstLE);
   }/*Exec_List*/


void
Exec_TargetsPtr(
   tp_FilHdr FilHdr,
   tp_FilHdr InFilHdr
   )
   
   
{
   tp_LocElm LocElm;
   tp_FilHdr ElmFilHdr;

   /*select*/{
      if (IsDir(InFilHdr)) {
	 ElmFilHdr = Do_Key(Copy_FilHdr(InFilHdr), "Odinfile");
	 ElmFilHdr = Do_Deriv(ElmFilHdr, RootFilPrm, RootFilPrm,
			      TargetsPtrFilTyp);
      }else{
	 ElmFilHdr = Deref(Copy_FilHdr(InFilHdr));
	 if (FilHdr_Status(ElmFilHdr) != STAT_NoFile) {
	    ElmFilHdr = Do_Deriv(ElmFilHdr, RootFilPrm, RootFilPrm,
				 TargetsFilTyp); }/*if*/; };}/*select*/;
   LocElm = ERROR;
   if (ElmFilHdr != ERROR) {
      LocElm = Make_LocElm(ElmFilHdr, RootFilPrm, FilHdr);
      Ret_FilHdr(ElmFilHdr); }/*if*/;
   Set_LocElm(FilHdr, LocElm);
   }/*Exec_TargetsPtr*/


void
Exec_Targets(
   tp_FilDsc OutFD,
   tp_FileName FileName
   )
   
   
{
   tp_FilDsc InFD;
   tp_Str Str, Name, TagStr;
   tps_Str StrBuf;
   tps_Str DataFileName;
   tp_Nod Nod, TgtNod;
   tp_NodTyp NodTyp;
   int LineNum;
   tp_PrmFHdr PrmFHdr;
   tp_FilHdr FilHdr;
   tp_FilPrm FilPrm;
   bool CmdFlag, ExecFlag;

   InFD = FileName_RFilDsc(FileName, false);
   if (InFD == ERROR) {
      return; }/*if*/;

   LineNum = 0;
   for (Str = ReadLine(StrBuf, InFD);
	Str != ERROR;
	Str = ReadLine(StrBuf, InFD)) {
      Nod = YY_Parser(Str, FileName, &LineNum);
      NodTyp = Nod_NodTyp(Nod);
      switch (NodTyp) {
	 case FILDEF: case SEGDEF: {
	    WriteLine(OutFD, Str);
	    Get_DefInfo(&Name, &CmdFlag, &ExecFlag, &TagStr, &TgtNod, Nod);
	    if (TagStr != NIL) {
	       if (TagStr[0] == '\n') {
		  TagStr = &TagStr[1]; }/*if*/;
	       for (Str = Readln(StrBuf, InFD), LineNum += 1;
		    Str != ERROR && TagStrCmp(Str, TagStr) != 0;
		    Str = Readln(StrBuf, InFD), LineNum += 1) {
		  Writeln(OutFD, Str); }/*for*/;
	       if (Str != ERROR) {
		  Writeln(OutFD, Str); }/*if*/; }/*if*/; break;}/*case*/;
	 case NSTDEF: {
	    PrmFHdr = Nod_PrmFHdr(Nod_Son(1, Nod));
	    if (PrmFHdr == ERROR) {
	       SystemError("in odin expression at:\n");
	       FileError("\n"); }/*if*/;
	    Use_PrmFHdr(&FilHdr, &FilPrm, PrmFHdr);
	    Deref_Pntrs(&FilHdr, &FilPrm, FilHdr, true);
	    if (FilHdr != ERROR) {
	       FilHdr_DataFileName(DataFileName, FilHdr);
	       Exec_Targets(OutFD, DataFileName);
	       Ret_FilHdr(FilHdr); }/*if*/; break;}/*case*/;
	 case DRVFLS: {
	    /*select*/{
	       if (Nod_NumSons(Nod) > 0) {
		  FileError("Must be a target.\n");
	       }else{
		  WriteLine(OutFD, Str); };}/*select*/; break;}/*case*/;
	 case NIL: {
	    break;}/*case*/;
	 default: {
	    FileError("Must be a target.\n"); };}/*switch*/;
      Ret_Nod(Nod); }/*for*/;
   if (!EndOfFile(InFD)) {
      FileError("Unexpected EOF\n"); }/*if*/;
   Close(InFD);
   }/*Exec_Targets*/


void
WriteSrcNames(
   tp_FilDsc OutFD,
   tp_FileName FileName,
   bool OpFlag
   )
   
   
   
{
   tp_FilDsc InFD;
   tp_Str Str, TailStr;
   tps_Str StrBuf;
   tp_Nod Nod, DS_Nod, Son;
   int LineNum;
   bool FoundOp;

   InFD = FileName_RFilDsc(FileName, false);
   if (InFD == ERROR) {
      return; }/*if*/;

   LineNum = 0;
   for (Str = ReadLine(StrBuf, InFD);
	Str != ERROR;
	Str = ReadLine(StrBuf, InFD)) {
      Nod = YY_Parser(Str, FileName, &LineNum);
      if (Nod != ERROR) {
	 for (DS_Nod = ((Nod_NodTyp(Nod) == DRVFLS) ? Nod_Son(1, Nod) : Nod);
	      DS_Nod != NIL;
	      DS_Nod = Nod_Brother(DS_Nod)) {
	    /*select*/{
	       if (DS_Nod == ERROR) {
		  SystemError("in odin expression at:\n");
		  FileError("\n");
	       }else{
		  (void)strcpy(Str, "");
		  FoundOp = false;
		  Son = Nod_Son(1, DS_Nod);
		  if (Nod_NodTyp(Son) == SEGOPR && !OpFlag) {
		     (void)strcpy(Str, ".");
		     FoundOp = true; }/*if*/;
		  while (Son != NIL) {
		     TailStr = Tail(Str);
		     switch (Nod_NodTyp(Son)) {
			case PRMOPR: case APLOPR: case DRVOPR:
			case HODOPR: case SEGOPR: {
			   FoundOp = true; };}/*switch*/;
		     if (FoundOp == OpFlag) {
			YY_Unparse(TailStr, Son); }/*if*/;
		     Son = Nod_Brother(Son); }/*while*/;
		  WriteLine(OutFD, Str); };}/*select*/; }/*for*/;
	 Ret_Nod(Nod); }/*if*/; }/*for*/;
   if (!EndOfFile(InFD)) {
      FileError("Unexpected EOF\n"); }/*if*/;
   Close(InFD);
   }/*WriteSrcNames*/


void
Validate_ViewSpec(
   tp_FilHdr FilHdr
   )
   
{
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;
   bool StrFlag;

   StrFlag = true;
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      StrFlag = IsStr(ElmFilHdr);
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   if (!StrFlag) {
      SystemError("Last element of view-specification must be a string.\n");
      }/*if*/;
   }/*Validate_ViewSpec*/


tp_FilElm
FilElm_NextStrFilElm(
   tp_FilElm FilElm
   )
   
{
   tp_FilElm StrFilElm;
   tp_FilHdr StrFilHdr;

   StrFilElm = FilElm;
   StrFilHdr = FilElm_FilHdr(StrFilElm); 
   while (!IsStr(StrFilHdr)) {
      Ret_FilHdr(StrFilHdr);
      FilElm = FilElm_NextFilElm(FilElm);
      StrFilHdr = FilElm_FilHdr(FilElm); }/*while*/;
   Ret_FilHdr(StrFilHdr);
   return FilElm;
   }/*FilElm_NextStrFilElm*/


void
Exec_CmptView(
   bool* ErrPtr,
   tp_FilHdr OutFilHdr,
   tp_FilHdr InpFilHdr
   )
   
   
   
{
   tp_LocElm FirstLE, LastLE;
   tp_FilElm FilElm;
   tp_LocElm NewLocElm;
   tp_FilHdr ElmFilHdr;
   tp_Str Str;

   *ErrPtr = false;
   FirstLE = NIL; LastLE = NIL;
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(InpFilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      /*select*/{
	 if (FilHdr_ElmStatus(ElmFilHdr) == STAT_NoFile) {
	    if (FilHdr_TgtValStatus(ElmFilHdr) != STAT_OK) {
	       *ErrPtr = true; }/*if*/;
	 }else{
	    /*select*/{
	       if (IsStr(ElmFilHdr)) {
		  Str = FilHdr_Ident(ElmFilHdr);
		  if (strcmp(Str, "") != NIL) {
		     SystemError("<%s> not found in view-path.\n", Str);
		     }/*if*/;
	       }else{
		  NewLocElm = Make_LocElm(ElmFilHdr, FilElm_FilPrm(FilElm),
					  OutFilHdr);
		  Chain_LocElms(&FirstLE, &LastLE, NewLocElm);
		  FilElm = FilElm_NextStrFilElm(FilElm);
		  };}/*select*/; };}/*select*/;
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   Set_LocElm(OutFilHdr, FirstLE);
   }/*Exec_CmptView*/


void
Install_ActTgt(
   tp_FilHdr ActTgtFilHdr
   )
   
{
   tp_FilHdr FilHdr;
   tps_Str StrBuf;
   tp_Str Key;

   if (FilHdr_ActTgtInstalled(ActTgtFilHdr)) {
      return; }/*if*/;
   Set_ActTgtInstalled(ActTgtFilHdr, true);
   Key = FilHdr_Key(StrBuf, ActTgtFilHdr);
   FORBIDDEN(Key == NIL);
   FilHdr = Do_Key(FilHdr_SrcFilHdr(Copy_FilHdr(ActTgtFilHdr)), Key);
   switch (FilHdr_FKind(FilHdr)) {
      case FK_SrcReg: {
	 Set_FKind(FilHdr, FK_BoundSrc);
	 break; }/*case*/;
      case FK_SymLinkReg: {
	 Set_FKind(FilHdr, FK_BoundSymLink);
	 break; }/*case*/;
      default: {
	 FORBIDDEN(!IsSource(FilHdr)); };}/*switch*/
   Set_TgtValLocElm(FilHdr, Make_CopyLocElm(ActTgtFilHdr, FilHdr, FilHdr));
   Push_ToBroadcast(Copy_FilHdr(FilHdr));
   Do_Log("Installing target", ActTgtFilHdr, LOGLEVEL_Target);
   Do_Log("   for", FilHdr, LOGLEVEL_Target);
   Ret_FilHdr(FilHdr);
   }/*Install_ActTgt*/


void
Uninstall_ActTgt(
   tp_FilHdr ActTgtFilHdr
   )
   
{
   tp_FilHdr FilHdr;
   tps_Str StrBuf;
   tp_Str Key;

   if (!FilHdr_ActTgtInstalled(ActTgtFilHdr)) {
      return; }/*if*/;
   Set_ActTgtInstalled(ActTgtFilHdr, false);
   Key = FilHdr_Key(StrBuf, ActTgtFilHdr);
   FORBIDDEN(Key == NIL);
   FilHdr = Do_Key(FilHdr_SrcFilHdr(Copy_FilHdr(ActTgtFilHdr)), Key);
   switch (FilHdr_FKind(FilHdr)) {
      case FK_BoundSrc: {
	 Set_FKind(FilHdr, FK_SrcReg);
	 break; }/*case*/;
      case FK_BoundSymLink: {
	 Set_FKind(FilHdr, FK_SymLinkReg);
	 break; }/*case*/;
      default: {
	 FORBIDDEN(!IsSource(FilHdr)); };}/*switch*/
   Set_DfltTgtValLocElm(FilHdr);
   Push_ToBroadcast(Copy_FilHdr(FilHdr));
   Do_Log("Uninstalling odin target", ActTgtFilHdr, LOGLEVEL_Target);
   Do_Log("   for", FilHdr, LOGLEVEL_Target);
   Ret_FilHdr(FilHdr);
   }/*UnInstall_ActTgt*/


void
WriteTextDef(
   tp_FilHdr FilHdr,
   tp_FilDsc OutFD,
   tp_FileName OutFileName,
   tp_FilDsc InFD,
   tp_FileName InFileName
   )
   
   
   
   
   
{
   bool CmdFlag, ExecFlag, Found, NeedLastEOL, NeedEOL;
   tp_Key Key;
   tps_Str StrBuf, KeyBuf;
   tp_Str Str, Name, TagStr;
   tp_Nod Nod, TgtNod;
   int LineNum;

   LineNum = 0;
   Key = FilHdr_Key(KeyBuf, FilHdr);
   FORBIDDEN(Key == NIL);
   for (Str = ReadLine(StrBuf, InFD);
	Str != ERROR;
	Str = ReadLine(StrBuf, InFD)) {
      Nod = YY_Parser(Str, InFileName, &LineNum);
      if (Nod == ERROR) {
	 return; }/*if*/;
      switch (Nod_NodTyp(Nod)) {
	 case FILDEF: case SEGDEF: {
	    Get_DefInfo(&Name, &CmdFlag, &ExecFlag, &TagStr, &TgtNod, Nod);
	    Found = (strcmp(Name, Key) == 0
		     && ((Nod_NodTyp(Nod) == SEGDEF)
			 == IsVTgtText_FKind(FilHdr_FKind(FilHdr))));
	    Ret_Nod(Nod);
	    if (TagStr != NIL) {
	       NeedLastEOL = true;
	       if (TagStr[0] == '\n') {
		  NeedLastEOL = false;
		  TagStr = &TagStr[1]; }/*if*/;
	       NeedEOL = false;
	       for (Str = Readln(StrBuf, InFD), LineNum += 1;
		    Str != ERROR && TagStrCmp(Str, TagStr) != 0;
		    Str = Readln(StrBuf, InFD), LineNum += 1) {
		  if (Found) {
		     if (NeedEOL) Writeln(OutFD, "");
		     Write(OutFD, Str);
		     NeedEOL = true; }/*if*/; }/*for*/;
	       if (Found) {
		  if (NeedLastEOL) Writeln(OutFD, "");
		  if (ExecFlag) MakeExecutable(OutFileName);
		  if (Str == ERROR && strlen(TagStr) != 0) {
		     SystemError
			("Terminator \"%s\" not found for target \"%s\".\n",
			 TagStr, Name); }/*if*/;
		  return; }/*if*/; }/*if*/;
	    Found = false; break;}/*case*/;
	 case DRVFLS: {
	    if (Nod_NumSons(Nod) > 0) {
	       FileError("Must be a target.\n");
	       Ret_Nod(Nod);
	       return; }/*if*/;
	    Ret_Nod(Nod); break;}/*case*/;
	 default: {
	    Ret_Nod(Nod);
	    FileError("Must be a target.\n");
	    return; };}/*switch*/; }/*for*/;
   SystemError("Text target for %s not found.\n", Key);
   }/*WriteTextDef*/


tp_LocElm
Make_TargetsLocElm(
   tp_FilHdr FilHdr,
   tp_FilDsc InFD,
   tp_FileName InFileName,
   tp_Date DepModDate,
   bool VirFlag
   )
   
   
   
   
   
{
   tp_Nod Nod, TgtNod;
   tps_Str StrBuf;
   tp_Str Str, Name, TagStr;
   int LineNum;
   bool CmdFlag, ExecFlag;
   tp_PrmFHdr PrmFHdr;
   tp_FilPrm FilPrm;
   tp_FilHdr ValFilHdr, TgtFilHdr;
   tp_LocElm LocElm, FirstLE, LastLE;
   tp_FKind FKind;

   LineNum = 0;
   FirstLE = NIL; LastLE = NIL;
   for (Str = ReadLine(StrBuf, InFD);
	Str != ERROR;
	Str = ReadLine(StrBuf, InFD)) {
      Nod = YY_Parser(Str, InFileName, &LineNum);
      if (Nod == ERROR) {
	 return FirstLE; }/*if*/;
      switch (Nod_NodTyp(Nod)) {
	 case FILDEF: case SEGDEF: {
	    Get_DefInfo(&Name, &CmdFlag, &ExecFlag, &TagStr, &TgtNod, Nod);
	    if ((Nod_NodTyp(Nod) == SEGDEF) == VirFlag) {
	       FKind = (VirFlag ?(CmdFlag ?FK_VirCmdTgt :FK_VirTgt)
			:(CmdFlag ?FK_ActCmdTgt :FK_ActTgt));
	       TgtFilHdr = Get_KeyDrv(Copy_FilHdr(FilHdr), FKind, Name);
	       Set_FKind(TgtFilHdr, FKind);
	       LocElm = NIL;
	       /*select*/{
		  if (TgtNod != NIL) {
		     PrmFHdr = Nod_PrmFHdr(TgtNod);
		     Use_PrmFHdr(&ValFilHdr, &FilPrm, PrmFHdr);
		  }else{
		     FKind =
			(VirFlag ?(ExecFlag ?FK_VirTgtExText :FK_VirTgtText)
			 :(ExecFlag ?FK_ActTgtExText :FK_ActTgtText));
		     ValFilHdr = Get_KeyDrv(Deref(FilHdr_Father(Copy_FilHdr(FilHdr))),
					    FKind, Name);
		     FilPrm = RootFilPrm; };}/*select*/;
	       if (ValFilHdr != ERROR) {
		  Set_LocElm(TgtFilHdr,
			     Make_LocElm(ValFilHdr, FilPrm, TgtFilHdr));
		  Ret_FilHdr(ValFilHdr);
		  Update_RefFile(TgtFilHdr, STAT_OK, DepModDate);
		  LocElm = Make_LocElm(TgtFilHdr, RootFilPrm, FilHdr);
		  Chain_LocElms(&FirstLE, &LastLE, LocElm); }/*if*/;
	       Ret_FilHdr(TgtFilHdr); }/*if*/;
	    if (TagStr != NIL) {
	       if (TagStr[0] == '\n') {
		  TagStr = &TagStr[1]; }/*if*/;
	       for (Str = Readln(StrBuf, InFD), LineNum += 1;
		    Str != ERROR && TagStrCmp(Str, TagStr) != 0;
		    Str = Readln(StrBuf, InFD), LineNum += 1) {
		  }/*for*/;
	       if (Str == ERROR && strlen(TagStr) != 0) {
		  SystemError
		     ("Terminator \"%s\" not found for target \"%s\".\n",
		      TagStr, Name); }/*if*/; }/*if*/; break;}/*case*/;
	 case DRVFLS: {
	    if (Nod_NumSons(Nod) > 0) {
	       FileError("Must be a target.\n");
	       Ret_Nod(Nod);
	       return FirstLE; }/*if*/;
	    break;}/*case*/;
	 default: {
	    FileError("Must be a target.\n");
	    Ret_Nod(Nod);
	    return FirstLE; };}/*switch*/;
      Ret_Nod(Nod); }/*for*/;
   return FirstLE;
   }/*Make_TargetsLocElm*/


void
Exec_VirDir(
   tp_FilHdr FilHdr,
   tp_FilHdr InpFilHdr
   )
   
   
{
   tps_FileName FileName, ToFileName, ElmFileName;
   tps_Str KeyBuf;
   tp_Str Key;
   tp_FilHdr ElmFilHdr;
   tp_FilElm FilElm;
   bool Abort;
   size_t sz;

   FilHdr_DataFileName(FileName, FilHdr);
   if (!Data_Exists(FilHdr)) {
      MakeDirFile(&Abort, FileName);
      FORBIDDEN(Abort);
      Set_Size(FilHdr, 1); }/*if*/;
   ClearDir(FileName);
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(InpFilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      Key = FilHdr_Key(KeyBuf, ElmFilHdr);
      ElmFilHdr = Deref(ElmFilHdr);
      sz = snprintf(ToFileName, MAX_FileName, "%s/%s", FileName, Key);
      if (sz >= MAX_FileName) {
         (void)fprintf(stderr, "File name too long (MAX_FileName=%d): %s/%s\n",
	             MAX_FileName, FileName, Key);
	 exit(1); }/*if*/;
      FilHdr_DataFileName(ElmFileName, ElmFilHdr);
      SymLink(&Abort, ToFileName, ElmFileName);
      if (Abort) {
	 SystemError("Multiple entries exist with name: %s\n", Key); }/*if*/;
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   }/*Exec_VirDir*/


void
FilPVal_LocElm(
   tp_LocElm* FirstLEPtr,
   tp_LocElm* LastLEPtr,
   tp_FilPVal FilPVal,
   tp_FilHdr FilHdr
   )
   
   
   
   
{
   tp_LocPVal ValLocPVal;
   tp_LocElm LocElm;
   tp_FilHdr ElmFilHdr;

   if (IsRootFilPVal(FilPVal)) {
      return; }/*if*/;

   FilPVal_LocElm(FirstLEPtr, LastLEPtr, FilPVal_Father(FilPVal), FilHdr);
   ValLocPVal = FilPVal_ValLocPVal(FilPVal);
   if (ValLocPVal != NIL) {
      FilPVal_LocElm(FirstLEPtr, LastLEPtr, LocPVal_FilPVal(ValLocPVal), FilHdr);
      return; }/*if*/;
   ElmFilHdr = LocHdr_FilHdr(FilPVal_LocHdr(FilPVal));
   LocElm = Make_LocElm(ElmFilHdr, RootFilPrm, FilHdr);
   Chain_LocElms(FirstLEPtr, LastLEPtr, LocElm);
   Ret_FilHdr(ElmFilHdr);
   }/*FilPVal_LocElm*/


tp_LocElm
Make_PntrHoLocElm(
   tp_FilHdr InFilHdr,
   tp_FilHdr FilHdr
   )
   
   
{
   tp_LocElm LocElm;
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr, NewElmFilHdr;

   FORBIDDEN(!IsPntr(InFilHdr));

   FilElm = LocElm_FilElm(FilHdr_LocElm(InFilHdr));
   FORBIDDEN(FilElm_NextFilElm(FilElm) != NIL);
   ElmFilHdr = FilElm_FilHdr(FilElm);
   FORBIDDEN(ElmFilHdr == ERROR);
   NewElmFilHdr = Do_Deriv
    (Copy_FilHdr(ElmFilHdr), RootFilPrm,
     Append_FilPrm(FilElm_FilPrm(FilElm), FilHdr_FilPrm(FilHdr)),
     FilHdr_FilTyp(FilHdr));
   /*select*/{
      if (NewElmFilHdr == ERROR) {
	 FilHdr_Error(" from: %s\n", ElmFilHdr);
	 LocElm = NIL;
      }else{
	 LocElm = Make_LocElm(NewElmFilHdr, RootFilPrm, FilHdr);
	 Ret_FilHdr(NewElmFilHdr); };}/*select*/;
   Ret_FilHdr(ElmFilHdr);
   return LocElm;
   }/*Make_PntrHoLocElm*/


static tp_FilTyp
FilHdr_ArgFilTyp(
   tp_FilHdr FilHdr
   )
   
{
   return FilTyp_ArgFilTyp(FilHdr_FilTyp(FilHdr));
   }/*FilHdr_ArgFilTyp*/


static void
Get_Recurse(
   tp_LocElm* FirstLEPtr,
   tp_LocElm* LastLEPtr,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm,
   tp_FilTyp FilTyp,
   tp_FilHdr ListFilHdr
   )
   
   
   
   
   
   
{
   tp_FilHdr ElmFilHdr;
   tp_DrvPth DrvPth;
   tp_LocElm LocElm;
   tp_FilElm FilElm;

   if (IsViewSpec(FilHdr)) {
      FilHdr_Error("Illegal view specification argument: %s\n", FilHdr);
      return; }/*if*/;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);

   if (!IsRef(FilHdr)) {
      ElmFilHdr = Copy_FilHdr(FilHdr);
      if (FilHdr_FilTyp(ElmFilHdr) != FilTyp) {
	 DrvPth = Get_DrvPth(ElmFilHdr, FilTyp);
	 if (DrvPth != ERROR) {
	    ElmFilHdr = Do_DrvPth(ElmFilHdr, RootFilPrm, FilPrm, DrvPth);
	    Ret_DrvPth(DrvPth);
	    ElmFilHdr = Do_Deriv(ElmFilHdr, RootFilPrm, FilPrm,
				 FilHdr_FilTyp(ListFilHdr)); }/*if*/; }/*if*/;
      LocElm = Make_LocElm(ElmFilHdr, FilPrm, ListFilHdr);
      Chain_LocElms(FirstLEPtr, LastLEPtr, LocElm);
      Ret_FilHdr(ElmFilHdr);
      return; }/*if*/;

   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      Get_Recurse(FirstLEPtr, LastLEPtr, ElmFilHdr,
		  Append_FilPrm(FilElm_FilPrm(FilElm), FilPrm),
		  FilTyp, ListFilHdr);
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   }/*Get_Recurse*/


tp_LocElm
Make_RecurseLocElm(
   tp_FilHdr FilHdr,
   tp_FilHdr ListFilHdr
   )
   
   
{
   tp_FilTyp FilTyp;
   tp_FilPrm FilPrm;
   tp_LocElm FirstLE, LastLE;

   FilTyp = FilHdr_ArgFilTyp(ListFilHdr);
   FORBIDDEN(FilTyp == ERROR);
   FORBIDDEN(!IsList_FilTyp(FilTyp));

   FilPrm = FilHdr_FilPrm(ListFilHdr);
   FirstLE = NIL; LastLE = NIL;
   Get_Recurse(&FirstLE, &LastLE, FilHdr, FilPrm, FilTyp, ListFilHdr);
   Clr_UnionFlags(FilHdr);
   return FirstLE;
   }/*Make_RecurseLocElm*/


tp_LocElm
Make_DerefPrmValLocElm(
   tp_FilHdr InFilHdr,
   tp_FilHdr FilHdr
   )
   
   
{
   tp_FilTyp FilTyp;
   tp_FilHdr ElmFilHdr;
   tp_LocElm LocElm;

   FilTyp = FilHdr_ArgFilTyp(FilHdr);
   FORBIDDEN(FilTyp == ERROR);

   ElmFilHdr = Do_Deriv(Deref(Copy_FilHdr(InFilHdr)), RootFilPrm,
			FilPrm_DerefPrmVal(FilHdr_FilPrm(FilHdr)), FilTyp);
   if (ElmFilHdr == ERROR) {
      return ERROR; }/*if*/;
   LocElm = Make_LocElm(ElmFilHdr, RootFilPrm, FilHdr);
   Ret_FilHdr(ElmFilHdr);
   return LocElm;
   }/*Make_DerefPrmValLocElm*/


static void
Get_ExDel(
   tp_LocElm* FirstLEPtr,
   tp_LocElm* LastLEPtr,
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm,
   tp_FilTyp FilTyp,
   tp_FilHdr ListFilHdr,
   bool ExFlag
   )
   
   
   
   
   
   
   
{
   tp_LocElm LocElm;
   tp_FilElm FilElm;
   tp_FilHdr ElmFilHdr;
   tp_FilPrm ElmFilPrm;

   if (FilHdr_Flag(FilHdr, FLAG_Union)) {
      return; }/*if*/;
   Set_Flag(FilHdr, FLAG_Union);

   if (!IsRef(FilHdr)) {
      if (ExFlag == IsSubType(FilHdr_FilTyp(FilHdr), FilTyp)) {
	 LocElm = Make_LocElm(FilHdr, FilPrm, ListFilHdr);
	 Chain_LocElms(FirstLEPtr, LastLEPtr, LocElm); }/*if*/;
      return; }/*if*/;
 
   for (FilElm = LocElm_FilElm(FilHdr_LocElm(FilHdr));
	FilElm != NIL;
	FilElm = FilElm_NextFilElm(FilElm)) {
      ElmFilHdr = FilElm_FilHdr(FilElm);
      ElmFilPrm = Append_FilPrm(FilElm_FilPrm(FilElm), FilPrm);
      Get_ExDel(FirstLEPtr, LastLEPtr,
		ElmFilHdr, ElmFilPrm, FilTyp, ListFilHdr, ExFlag);
      Ret_FilHdr(ElmFilHdr); }/*for*/;
   }/*Get_ExDel*/


tp_LocElm
Make_ExDelLocElm(
   tp_FilHdr FilHdr,
   tp_FilHdr ListFilHdr,
   bool ExFlag
   )
   
   
   
{
   tp_LocElm FirstLE, LastLE;
   tp_FilTyp FilTyp;

   if (!(IsList(FilHdr) || IsPntr(FilHdr))) {
      SystemError("Input to :%s must be a list.\n",
		  (ExFlag ? "extract" : "delete"));
      return ERROR; }/*if*/;
   if (IsViewSpec(FilHdr)){
      FilHdr_Error("Illegal view specification argument: %s\n", FilHdr);
      return ERROR; }/*if*/;

   FilTyp = FilHdr_ArgFilTyp(ListFilHdr);
   FORBIDDEN(FilTyp == ERROR);
   if (!IsAtmc_FilTyp(FilTyp)) {
      SystemError("The argument %s to :%s must be an atomic type.\n",
		  FilTyp_FTName(FilTyp), (ExFlag ? "extract" : "delete"));
      return ERROR; }/*if*/;

   FirstLE = NIL; LastLE = NIL;
   Get_ExDel(&FirstLE, &LastLE,
	     FilHdr, RootFilPrm, FilTyp, ListFilHdr, ExFlag);
   Clr_UnionFlags(FilHdr);
   return FirstLE;
   }/*Make_ExDelLocElm*/

