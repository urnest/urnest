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

#include "inc/GMC.h"
#include "inc/FileName.h"
#include "inc/ElmInf.h"
#include "inc/FilHdr.h"
#include "inc/HdrInf.h"
#include "inc/Item.h"
#include "inc/PrmInf.h"
#include "inc/PValInf.h"
#include "inc/InpInf.h"
#include "inc/Str.h"


static tp_FilDsc	InfoFD;

static tp_Loc	LocDataNum;
static tp_Loc	LocCurrentDate;
static tp_Loc	LocVerifyDate;
static tp_Loc	LocCurSize;
static tp_Loc	LocFreeLocElm;
static tp_Loc	LocPValLocPrm;
static tp_Loc	LocLastLoc;

int		DataNum;
static int	OldDataNum;

tp_Date		CurrentDate;
static tp_Date	OldCurrentDate;
tp_Date		VerifyDate;
static tp_Date	OldVerifyDate;

int		CurSize;
static int	OldCurSize;

tp_LocElm	FreeLocElm;
static tp_LocElm	OldFreeLocElm;

tp_LocPrm	PValLocPrm;
static tp_LocPrm	OldPValLocPrm;

tp_Loc		LastLoc = NIL;
static tp_Loc	OldLastLoc = NIL;

tp_LocHdr	RootLocHdr;

tp_LocStr	LocNilStr = 1;


#define		MAX_HashItemS 4096
#define		HASH_MASK 07777
static tp_Item	HashItemS [MAX_HashItemS];


static int
Loc_HashVal(tp_Loc Loc)
{
   return (Loc & HASH_MASK);
   }/*Loc_HashVal*/


void
Hash_Item(tp_Item Item,tp_Loc Loc)
{
   int HashVal;

   HashVal = Loc_HashVal(Loc);
   Item->Loc = Loc;
   Item->NextHash = HashItemS[HashVal];
   HashItemS[HashVal] = Item;
   }/*Hash_Item*/


void
UnHash_Item(tp_Item Item)
{
   int HashVal;
   tp_Item PrevItem;

   HashVal = Loc_HashVal(Item->Loc);
   Item->Loc = NIL;
   PrevItem = HashItemS[HashVal];
   if (PrevItem == Item) {
      HashItemS[HashVal] = Item->NextHash;
      return; }/*if*/;
   FORBIDDEN(PrevItem == NIL);
   while (PrevItem->NextHash != Item) {
      PrevItem = PrevItem->NextHash;
      FORBIDDEN(PrevItem == NIL); }/*while*/;
   PrevItem->NextHash = Item->NextHash;
   }/*UnHash_Item*/


tp_Item
Lookup_Item(tp_Loc Loc)
{
   int HashVal;
   tp_Item Item;

   HashVal = Loc_HashVal(Loc);
   Item = HashItemS[HashVal];
   while (Item != NIL) {
      if (Item->Loc == Loc) {
	 return Item; }/*if*/;
      Item = Item->NextHash; }/*while*/;
   return ERROR;
   }/*Lookup_Item*/


static void
InfoWrite(tp_Loc Loc,char* Buf,int Size)
{
   int status, count;

   FORBIDDEN(Loc == NIL || Buf == NIL || Size == 0);
   status = fseek((FILE *)InfoFD, Loc, 0);
   if (status == -1) {
      SysCallError(StdOutFD, "Retrying fseek(InfoWrite)");
      while (status == -1) {
	 (void)sleep(1);
	 status = fseek((FILE *)InfoFD, Loc, 0); }/*while*/;
      Writeln(StdOutFD, "fseek(InfoWrite) succeeded."); }/*if*/;
   count = fwrite(Buf, Size, 1, (FILE *)InfoFD);
   if (count != 1) {
      SysCallError(StdOutFD, "Retrying fwrite(InfoWrite)");
      while (count != 1) {
	 (void)sleep(1);
	 count = fwrite(Buf, Size, 1, (FILE *)InfoFD); }/*while*/;
      Writeln(StdOutFD, "fwrite(InfoWrite) succeeded."); }/*if*/;
   }/*InfoWrite*/


static void
InfoRead(tp_Loc Loc,char* Buf,int Size)
{
   int status, count;

   FORBIDDEN(Loc == NIL || Buf == NIL || Size == 0);
   status = fflush((FILE *)InfoFD);
   if (status == EOF) {
      SysCallError(StdOutFD, "Retrying fflush(InfoRead)");
      while (status == EOF) {
	 (void)sleep(1);
	 status = fflush((FILE *)InfoFD); }/*while*/;
      Writeln(StdOutFD, "fflush(InfoRead) succeeded."); }/*if*/;
   status = fseek((FILE *)InfoFD, Loc, 0);
   if (status == -1) {
      SysCallError(StdOutFD, "Retrying fseek(InfoRead)");
      while (status == -1) {
	 (void)sleep(1);
	 status = fseek((FILE *)InfoFD, Loc, 0); }/*while*/;
      Writeln(StdOutFD, "fseek(InfoRead) succeeded."); }/*if*/;
   count = fread(Buf, Size, 1, (FILE *)InfoFD);
   if (count != 1) {
      SysCallError(StdOutFD, "Retrying fread(InfoRead)");
      while (count != 1) {
	 (void)sleep(1);
	 count = fread(Buf, Size, 1, (FILE *)InfoFD); }/*while*/;
      Writeln(StdOutFD, "fread(InfoRead) succeeded."); }/*if*/;
   }/*InfoRead*/


static void
WriteWord(int Word,tp_Loc Loc)
{
   InfoWrite(Loc, (char *)&Word, (sizeof(Word)));
   }/*WriteWord*/


static int
ReadWord(tp_Loc Loc)
{
   int Word;

   InfoRead(Loc, (char *)&Word, sizeof(Word));
   return Word;
   }/*ReadWord*/


static void
WriteLoc(tp_Loc LocVal,tp_Loc Loc)
{
   InfoWrite(Loc, (char *)&LocVal, sizeof(LocVal));
   }/*WriteLoc*/


static tp_Loc
ReadLoc(tp_Loc Loc)
{
   tp_Loc LocVal;

   InfoRead(Loc, (char *)&LocVal, sizeof(LocVal));
   return LocVal;
   }/*ReadLoc*/


static void
WriteDataNum(int Num)
{
   WriteWord(Num, LocDataNum);
   }/*WriteDataNum*/


static int
ReadDataNum()
{
   return ReadWord(LocDataNum);
   }/*ReadDataNum*/


void
Append_DataNum(tp_Str Str,int Num)
{
   int Div, Rem;
   tp_Str TailStr;

   Div = Num / 26;
   Rem = Num - (Div * 26);
   TailStr = Str;
   if (Div > 0) {
      Append_DataNum(Str, Div-1);
      TailStr = Tail(Str); }/*if*/;
   TailStr[0] = '/';
   TailStr[1] = (char)((int)'a' + Rem);
   TailStr[2] = 0;
   }/*Append_DataNum*/


static void
WriteCurrentDate(tp_Date Date)
{
   WriteWord(Date, LocCurrentDate);
   }/*WriteCurrentDate*/


static tp_Date
ReadCurrentDate()
{
   return ReadWord(LocCurrentDate);
   }/*ReadCurrentDate*/


static void
WriteVerifyDate(tp_Date Date)
{
   WriteWord(Date, LocVerifyDate);
   }/*WriteVerifyDate*/


static tp_Date
ReadVerifyDate()
{
   return ReadWord(LocVerifyDate);
   }/*ReadVerifyDate*/


static void
WriteCurSize(int CurSize)
{
   WriteWord(CurSize, LocCurSize);
   }/*WriteCurSize*/


static int
ReadCurSize()
{
   return ReadWord(LocCurSize);
   }/*ReadCurSize*/


static void
WritePValLocPrm(tp_LocElm PValLocPrm)
{
   WriteLoc(PValLocPrm, LocPValLocPrm);
   }/*WritePValLocPrm*/


static tp_LocPrm
ReadPValLocPrm()
{
   return (tp_LocPrm)ReadLoc(LocPValLocPrm);
   }/*ReadPValLocPrm*/


static void
WriteFreeLocElm(tp_LocElm FreeLocElm)
{
   WriteLoc(FreeLocElm, LocFreeLocElm);
   }/*WriteFreeLocElm*/


static tp_LocElm
ReadFreeLocElm()
{
   return (tp_LocElm)ReadLoc(LocFreeLocElm);
   }/*ReadFreeLocElm*/


static void
WriteLastLoc(tp_Loc LastLoc)
{
   WriteLoc(LastLoc, LocLastLoc);
   }/*WriteLastLoc*/


static tp_Loc
ReadLastLoc()
{
   return (tp_Loc)ReadLoc(LocLastLoc);
   }/*ReadLastLoc*/


tp_Loc
Alloc(int Size)
{
   tp_Loc Loc;

   FORBIDDEN(LastLoc == NIL);
   Loc = LastLoc;
   LastLoc += Size;
   return Loc;
   }/*Alloc*/


tp_LocStr
WriteStr(tp_Str Str)
{
   tp_LocStr LocStr;
   int count, Length, Size;

   if (Str == NIL) {
      return LocNilStr; }/*if*/;
   Length = strlen(Str);
   Size = Length * sizeof(char);
   LocStr = (tp_LocStr)Alloc(sizeof(Length) + Size);
   InfoWrite(LocStr, (char *)&Length, sizeof(Length));
   if (Length > 0) {
      count = fwrite(Str, Size, 1, (FILE *)InfoFD);
      if (count != 1) {
	 SysCallError(StdOutFD, "fwrite(WriteStr)");
	 InfoWrite(LocStr+sizeof(Length), Str, Size); }/*if*/; }/*if*/;
   return LocStr;
   }/*WriteStr*/


tp_Str
ReadStr(tp_LocStr LocStr)
{
   int count, Length, Size;
   tps_Str Str;

   if (LocStr == LocNilStr) {
      return NIL; }/*if*/;
   InfoRead(LocStr, (char *)&Length, sizeof(Length));
   FORBIDDEN(Length > MAX_Str);
   if (Length > 0) {
      Size = Length * sizeof(char);
      count = fread(Str, Size, 1, (FILE *)InfoFD);
      if (count != 1) {
	 SysCallError(StdOutFD, "fread(ReadStr)");
	 InfoRead(LocStr + sizeof(Length), Str, Size); }/*if*/; }/*if*/;
   Str[Length] = 0;
   return Sym_Str(Str_Sym(Str));
   }/*ReadStr*/


void
WritePrmInf(tp_PrmInf PrmInf,tp_LocPrm LocPrm)
{
   InfoWrite(LocPrm, (char *)PrmInf, sizeof(*PrmInf));
   }/*WritePrmInf*/


void
ReadPrmInf(tp_PrmInf PrmInf,tp_LocPrm LocPrm)
{
   InfoRead(LocPrm, (char *)PrmInf, sizeof(*PrmInf));
   }/*ReadPrmInf*/


void
WritePValInf(tp_PValInf PValInf,tp_LocPVal LocPVal)
{
   InfoWrite(LocPVal, (char *)PValInf, sizeof(*PValInf));
   }/*WritePValInf*/


void
ReadPValInf(tp_PValInf PValInf,tp_LocPVal LocPVal)
{
   InfoRead(LocPVal, (char *)PValInf, sizeof(*PValInf));
   }/*ReadPValInf*/


void
WriteHdrInf(tp_HdrInf HdrInf,tp_LocHdr LocHdr)
{
   InfoWrite(LocHdr, (char *)HdrInf, sizeof(*HdrInf));
   }/*WriteHdrInf*/


void
ReadHdrInf(tp_HdrInf HdrInf,tp_LocHdr LocHdr)
{
   InfoRead(LocHdr, (char *)HdrInf, sizeof(*HdrInf));
   }/*ReadHdrInf*/


void
WriteInpInf(tp_InpInf InpInf,tp_LocInp LocInp)
{
   InfoWrite(LocInp, (char *)InpInf, sizeof(*InpInf));
   }/*WriteInpInf*/


void
ReadInpInf(tp_InpInf InpInf,tp_LocInp LocInp)
{
   InfoRead(LocInp, (char *)InpInf, sizeof(*InpInf));
   }/*ReadInpInf*/


void
WriteElmInf(tp_ElmInf ElmInf,tp_LocElm LocElm)
{
   InfoWrite(LocElm, (char *)ElmInf, sizeof(*ElmInf));
   }/*WriteElmInf*/


void
ReadElmInf(tp_ElmInf ElmInf,tp_LocElm LocElm)
{
   InfoRead(LocElm, (char *)ElmInf, sizeof(*ElmInf));
   }/*ReadElmInf*/


void
Init_Info(bool* NewFlagPtr)
{
   tps_FileName InfoFileName;
   tp_Loc Initial_LastLoc;
   tps_HdrInf _NewRootHdrInf; tp_HdrInf NewRootHdrInf = &_NewRootHdrInf;

   *NewFlagPtr = false;
   Get_InfoFileName(InfoFileName);
   LocDataNum = 4;
   LocCurrentDate = LocDataNum + sizeof(int);
   LocVerifyDate = LocCurrentDate + sizeof(int);
   LocCurSize = LocVerifyDate + sizeof(int);
   LocFreeLocElm = LocCurSize + sizeof(int);
   LocPValLocPrm = LocFreeLocElm + sizeof(tp_Loc);
   LocLastLoc = LocPValLocPrm + sizeof(tp_Loc);
   RootLocHdr = LocLastLoc + sizeof(tp_Loc);
   Initial_LastLoc = RootLocHdr + sizeof(tps_HdrInf);

   if (!Exists(InfoFileName) || Empty(InfoFileName)) {
      *NewFlagPtr = true;
      InfoFD = FileName_WBFilDsc(InfoFileName, false);
      if (InfoFD == ERROR) {
	 SystemError("Cannot open Odin database file: %s.\n", InfoFileName);
	 IPC_Finish();
	 exit(1); }/*if*/;

      WriteDataNum((int)0);
      WriteCurrentDate((tp_Date)1);
      WriteVerifyDate((tp_Date)1);
      WriteCurSize((int)0);
      WriteFreeLocElm((tp_LocElm)NIL);
      WritePValLocPrm((tp_LocPrm)NIL);
      WriteLastLoc(Initial_LastLoc);

      Init_HdrInf(NewRootHdrInf);
      Make_RootHdrInf(NewRootHdrInf, RootLocHdr);
      WriteHdrInf(NewRootHdrInf, RootLocHdr);

      Close(InfoFD); }/*if*/;

   InfoFD = FileName_RWBFilDsc(InfoFileName, false);
   if (InfoFD == ERROR) {
      SystemError("Cannot open Odin database file: %s.\n", InfoFileName);
      IPC_Finish();
      exit(1); }/*if*/;
   DataNum = OldDataNum = ReadDataNum();
   CurrentDate = OldCurrentDate = ReadCurrentDate();
   VerifyDate = OldVerifyDate = ReadVerifyDate();
   CurSize = OldCurSize = ReadCurSize();
   FreeLocElm = OldFreeLocElm = ReadFreeLocElm();
   PValLocPrm = OldPValLocPrm = ReadPValLocPrm();
   LastLoc = OldLastLoc = ReadLastLoc();
   }/*Init_Info*/


void
Close_Info()
{
   Close(InfoFD);
   }/*Close_Info*/


void
Update_Info()
{
   if (DataNum != OldDataNum) {
      WriteDataNum(DataNum);
      OldDataNum = DataNum; }/*if*/;
   if (CurrentDate != OldCurrentDate) {
      WriteCurrentDate(CurrentDate);
      OldCurrentDate = CurrentDate; }/*if*/;
   if (VerifyDate != OldVerifyDate) {
      WriteVerifyDate(VerifyDate);
      OldVerifyDate = VerifyDate; }/*if*/;
   if (CurSize != OldCurSize) {
      WriteCurSize(CurSize);
      OldCurSize = CurSize; }/*if*/;
   if (FreeLocElm != OldFreeLocElm) {
      WriteFreeLocElm(FreeLocElm);
      OldFreeLocElm = FreeLocElm; }/*if*/;
   if (PValLocPrm != OldPValLocPrm) {
      WritePValLocPrm(PValLocPrm);
      OldPValLocPrm = PValLocPrm; }/*if*/;
   if (LastLoc != OldLastLoc) {
      WriteLastLoc(LastLoc);
      OldLastLoc = LastLoc; }/*if*/;
   if (ModFilHdr) WriteFilHdrs();
   if (ModFilElm) WriteFilElms();
   if (ModFilInp) WriteFilInps();
   }/*Update_Info*/

