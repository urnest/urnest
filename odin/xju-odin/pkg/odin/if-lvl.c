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
#include "inc/CastEdg.h"
#include "inc/DPType_.h"
#include "inc/DrvEdg.h"
#include "inc/DrvPth.h"
#include "inc/EqvEdg.h"
#include "inc/FKind_.h"
#include "inc/FilTyp.h"
#include "inc/Lvl.h"
#include "inc/MemEdg.h"
#include "inc/Pos.h"
#include "inc/PType_.h"


int			num_PosS = 0;

static tp_Pos		FreePos = NIL;
static int		num_FreePosS = 0;


static void
Init_Lvls()
{
   FORBIDDEN(num_PosS != num_FreePosS);
   }/*Init_Lvls*/


static tp_Pos
New_Pos()
{
   tp_Pos Pos;

   /*select*/{
      if (FreePos == NIL) {
	 Pos = (tp_Pos)malloc(sizeof(tps_Pos));
	 num_PosS += 1;
      }else{
	 Pos = FreePos;
	 FreePos = FreePos->Next;
	 num_FreePosS -= 1; };}/*select*/;
   Pos->Next = NIL;
   Pos->Last = Pos;
   return Pos;
   }/*New_Pos*/


static tp_Lvl
Begin_Lvl()
{
   return NIL;
   }/*Begin_Lvl*/


static bool
IsEmpty_Lvl(tp_Lvl Lvl)
{
   return (Lvl == NIL);
   }/*IsEmpty_Lvl*/


static tp_Pos
First_Pos(tp_Lvl Lvl)
{
   return Lvl;
   }/*First_Pos*/


static tp_Pos
Next_Pos(tp_Pos Pos)
{
   return Pos->Next;
   }/*Next_Pos*/


static tp_FilTyp
Pos_FilTyp(tp_Pos Pos)
{
   return Pos->FilTyp;
   }/*Pos_FilTyp*/


static bool
IsInLvl(tp_FilTyp FilTyp)
{
   return (FilTyp->Pos != NIL);
   }/*IsInLvl*/


static tp_DrvPth
Make_DrvPth(tp_FKind FKind,tp_FilTyp FilTyp)
{
   tp_DrvPth DrvPth;

   DrvPth = FilTyp_Drv_DrvPth(FilTyp, (tp_DrvEdg)NIL);
   DrvPth->FKind = FKind;
   return DrvPth;
   }/*Make_DrvPth*/


static tp_DrvPth
Pos_DrvPth(tp_Pos EndPos)
{
   tp_Pos Pos;
   tp_DrvPth DrvPth, NextDrvPth;

   FORBIDDEN(EndPos == NIL);
   NextDrvPth = NIL;
   for (Pos = EndPos; Pos->PType != PT_Inp; Pos = Pos->Base) {
      switch (Pos->PType) {
	 case PT_Cast: {
	    DrvPth = FilTyp_Cast_DrvPth(Pos->FilTyp);
	    DrvPth->Next = NextDrvPth;
	    NextDrvPth = DrvPth;
	    break;}/*case*/;
	 case PT_Eqv: {
	    DrvPth = FilTyp_Eqv_DrvPth(Pos->FilTyp);
	    DrvPth->Next = NextDrvPth;
	    NextDrvPth = DrvPth;
	    break;}/*case*/;
	 case PT_Drv: {
	    DrvPth = FilTyp_Drv_DrvPth(Pos->FilTyp, Pos->DrvEdg);
	    DrvPth->Next = NextDrvPth;
	    NextDrvPth = DrvPth;
	    break;}/*case*/;
	 default: {
	    FATALERROR("Bad PType"); };}/*switch*/; }/*for*/;
   return NextDrvPth;
   }/*Pos_DrvPth*/


static void
AddInpToLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp FilTyp)
{
   tp_Pos Pos;

   FORBIDDEN(FilTyp->Pos != NIL);
   Pos = New_Pos();
   FilTyp->Pos = Pos;
   Pos->PType = PT_Inp;
   Pos->FilTyp = FilTyp;
   Pos->Base = NIL;
   *PosPtr = Pos;
   if (*LvlPtr == NIL) {
      *LvlPtr = Pos;
      return; }/*if*/;
   (*LvlPtr)->Last->Next = Pos;
   (*LvlPtr)->Last = Pos;
   }/*AddInpToLvl*/


static void
AddCastEdgToLvl(tp_Lvl* LvlPtr,tp_FilTyp FilTyp,tp_Pos BasePos)
{
   tp_Pos Pos;

   FORBIDDEN(FilTyp->Pos != NIL);
   Pos = New_Pos();
   FilTyp->Pos = Pos;
   Pos->PType = PT_Cast;
   Pos->FilTyp = FilTyp;
   Pos->Base = BasePos;
   if (*LvlPtr == NIL) {
      *LvlPtr = Pos;
      return; }/*if*/;
   (*LvlPtr)->Last->Next = Pos;
   (*LvlPtr)->Last = Pos;
   }/*AddCastEdgToLvl*/


static void
AddEqvEdgToLvl(tp_Lvl* LvlPtr,tp_FilTyp FilTyp,tp_Pos BasePos)
{
   tp_Pos Pos;

   FORBIDDEN(FilTyp->Pos != NIL);
   Pos = New_Pos();
   FilTyp->Pos = Pos;
   Pos->PType = PT_Eqv;
   Pos->FilTyp = FilTyp;
   Pos->Base = BasePos;
   if (*LvlPtr == NIL) {
      *LvlPtr = Pos;
      return; }/*if*/;
   (*LvlPtr)->Last->Next = Pos;
   (*LvlPtr)->Last = Pos;
   }/*AddEqvEdgToLvl*/


static void
AddDrvEdgToLvl(tp_Lvl* LvlPtr,tp_FilTyp FilTyp,tp_DrvEdg DrvEdg,tp_Pos BasePos)
{
   tp_Pos Pos;

   FORBIDDEN(FilTyp->Pos != NIL);
   Pos = New_Pos();
   FilTyp->Pos = Pos;
   Pos->PType = PT_Drv;
   Pos->FilTyp = FilTyp;
   Pos->DrvEdg = DrvEdg;
   Pos->Base = BasePos;
   if (*LvlPtr == NIL) {
      *LvlPtr = Pos;
      return; }/*if*/;
   (*LvlPtr)->Last->Next = Pos;
   (*LvlPtr)->Last = Pos;
   }/*AddDrvEdgToLvl*/


static void
AddEqvEdgsToLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_Pos Pos,tp_FilTyp ToFilTyp)
{
   tp_EqvEdg EqvEdg;
   tp_FilTyp EqvFilTyp;
   tp_Pos EqvPos;

   *PosPtr = NIL;
   for (EqvEdg = (Pos_FilTyp(Pos))->EqvEdg;
	EqvEdg != NIL;
	EqvEdg = EqvEdg->Next) {
      EqvFilTyp = EqvEdg->FilTyp;
      if (!IsInLvl(EqvFilTyp)) {
	 AddEqvEdgToLvl(LvlPtr, EqvFilTyp, Pos);
	 EqvPos = EqvFilTyp->Pos;
	 if (EqvFilTyp == ToFilTyp) {
	    *PosPtr = EqvPos;
	    FORBIDDEN(*PosPtr == NIL);
	    return; }/*if*/;
	 AddEqvEdgsToLvl(PosPtr, LvlPtr, EqvPos, ToFilTyp);
	 if (*PosPtr != NIL) {
	    return; }/*if*/; }/*if*/; }/*for*/;
   }/*AddEqvEdgsToLvl*/


static void
AddCastEdgsToLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_Pos Pos,tp_FilTyp ToFilTyp)
{
   tp_CastEdg CastEdg;
   tp_FilTyp CastFilTyp;
   tp_Pos CastPos;

   *PosPtr = NIL;
   for (CastEdg = (Pos_FilTyp(Pos))->CastEdg;
	CastEdg != NIL;
	CastEdg = CastEdg->Next) {
      CastFilTyp = CastEdg->FilTyp;
      if (CastFilTyp != ObjectFilTyp
	  && CastFilTyp != FileFilTyp
	  && CastFilTyp != ListFilTyp) {
	 if (!IsInLvl(CastFilTyp)) {
	    AddCastEdgToLvl(LvlPtr, CastFilTyp, Pos);
	    CastPos = CastFilTyp->Pos;
	    if (CastFilTyp == ToFilTyp) {
	       *PosPtr = CastPos;
	       FORBIDDEN(*PosPtr == NIL);
	       return; }/*if*/;
	    AddCastEdgsToLvl(PosPtr, LvlPtr, CastPos, ToFilTyp);
	    if (*PosPtr != NIL) {
	       return; }/*if*/;
	    AddEqvEdgsToLvl(PosPtr, LvlPtr, CastPos, ToFilTyp);
	    if (*PosPtr != NIL) {
	       return; }/*if*/; }/*if*/; }/*if*/; }/*for*/;
   }/*AddCastEdgsToLvl*/


static void
AddDrvEqvEdgsToLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_DrvEdg DrvEdg,tp_Pos Pos,tp_FilTyp ToFilTyp)
{
   tp_FilTyp DrvFilTyp;
   tp_Pos DrvPos;

   *PosPtr = NIL;
   DrvFilTyp = DrvEdg->FilTyp;
   AddDrvEdgToLvl(LvlPtr, DrvFilTyp, DrvEdg, Pos);
   DrvPos = DrvFilTyp->Pos;
   if (DrvFilTyp == ToFilTyp) {
      *PosPtr = DrvPos;
      FORBIDDEN(*PosPtr == NIL);
      return; }/*if*/;
   AddEqvEdgsToLvl(PosPtr, LvlPtr, DrvPos, ToFilTyp);
   }/*AddDrvEqvEdgsToLvl*/


static void
AddDrvEdgsToLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_Pos Pos,tp_FilTyp ToFilTyp)
{
   tp_DrvEdg DrvEdg, MemDrvEdg;
   tp_FilTyp DrvFilTyp, MemFilTyp;

   *PosPtr = NIL;
   for (DrvEdg = (Pos_FilTyp(Pos))->DrvEdg;
	DrvEdg != NIL;
	DrvEdg = DrvEdg->Next) {
      DrvFilTyp = DrvEdg->FilTyp;
      if (!IsInLvl(DrvFilTyp)) {
	 AddDrvEqvEdgsToLvl(PosPtr, LvlPtr, DrvEdg, Pos, ToFilTyp);
	 if (*PosPtr != NIL) {
	    return; }/*if*/;
	 if (IsStruct_FilTyp(DrvFilTyp)) {
	    for (MemDrvEdg = DrvFilTyp->DrvEdg;
		 MemDrvEdg != NIL;
		 MemDrvEdg = MemDrvEdg->Next) {
	       MemFilTyp = MemDrvEdg->FilTyp;
	       if (IsStructMem_FilTyp(MemFilTyp) && !IsInLvl(MemFilTyp)) {
		  AddDrvEqvEdgsToLvl(PosPtr, LvlPtr,
				     MemDrvEdg, DrvFilTyp->Pos, ToFilTyp);
		  if (*PosPtr != NIL) {
		     return; }/*if*/; }/*if*/; }/*for*/;
	    }/*if*/; }/*if*/; }/*for*/;
   }/*AddDrvEdgsToLvl*/


static void
AppendLvl(tp_Lvl* LvlPtr,tp_Lvl AddLvl)
{
   tp_Lvl Lvl;

   if (AddLvl == NIL) return;
   Lvl = *LvlPtr;
   if (Lvl == NIL) {
      *LvlPtr = AddLvl;
      return; }/*if*/;
   Lvl->Last->Next = AddLvl;
   Lvl->Last = AddLvl->Last;
   }/*AppendLvl*/


static void
Clear_InLvls(tp_Lvl Lvl)
{
   tp_Pos Pos, LastPos;

   if (Lvl == NIL) return;
   for (Pos = Lvl; Pos != NIL; Pos = Pos->Next) {
      num_FreePosS += 1;
      Pos->FilTyp->Pos = NIL;
      LastPos = Pos; }/*for*/;
   LastPos->Next = FreePos;
   FreePos = Lvl;
   }/*Clear_InLvls*/


static void
AddDrvLvls(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp ToFilTyp)
{
   tp_Lvl OldLvls, Lvl, NewLvl;
   tp_Pos Pos;

   OldLvls = Begin_Lvl();
   Lvl = *LvlPtr; NewLvl = Begin_Lvl();
   while (!IsEmpty_Lvl(Lvl)) {
      for (Pos = First_Pos(Lvl); Pos != NIL; Pos = Next_Pos(Pos)) {
	 AddDrvEdgsToLvl(PosPtr, &NewLvl, Pos, ToFilTyp);
	 if (*PosPtr != NIL) {
	    goto done; }/*if*/; }/*for*/;
      AppendLvl(&OldLvls, Lvl);
      Lvl = NewLvl; NewLvl = Begin_Lvl(); }/*while*/;
   *PosPtr = NIL;
done:
   AppendLvl(&OldLvls, Lvl);
   AppendLvl(&OldLvls, NewLvl);
   *LvlPtr = OldLvls;
   }/*AddDrvLvls*/


static void
AddCastLvls1(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_Pos Pos,tp_FilTyp ToFilTyp)
{
   tp_EqvEdg EqvEdg;
   tp_Pos EqvPos;

   AddCastEdgsToLvl(PosPtr, LvlPtr, Pos, ToFilTyp);
   if (*PosPtr != NIL) {
      return; }/*if*/;
   for (EqvEdg = (Pos_FilTyp(Pos))->EqvEdg;
	EqvEdg != NIL;
	EqvEdg = EqvEdg->Next) {
      EqvPos = EqvEdg->FilTyp->Pos;
      FORBIDDEN(EqvPos == NIL);
      AddCastLvls1(PosPtr, LvlPtr, EqvPos, ToFilTyp);
      if (*PosPtr != NIL) {
	 return; }/*if*/; }/*for*/;
   }/*AddCastLvls1*/


static void
AddCastLvls(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_Pos InpPos,tp_FilTyp ToFilTyp)
{
   tp_Lvl CastLvl;

   CastLvl = Begin_Lvl();
   AddCastLvls1(PosPtr, &CastLvl, InpPos, ToFilTyp);
   if (*PosPtr != NIL) {
      goto done; }/*if*/;
   AddDrvLvls(PosPtr, &CastLvl, ToFilTyp);

done:;
   AppendLvl(LvlPtr, CastLvl);
   }/*AddCastLvls*/


static void
Find_Pos(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   tp_Pos InpPos;

   AddInpToLvl(&InpPos, LvlPtr, FrmFilTyp);
   AddEqvEdgsToLvl(PosPtr, LvlPtr, InpPos, ToFilTyp);
   if (*PosPtr != NIL) {
      return; }/*if*/;
   AddDrvLvls(PosPtr, LvlPtr, ToFilTyp);
   if (*PosPtr != NIL) {
      return; }/*if*/;
   AddCastLvls(PosPtr, LvlPtr, InpPos, ToFilTyp);
   if (*PosPtr != NIL) {
      return; }/*if*/;
   }/*Find_Pos*/


static void
ExtendLvl(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   tp_Lvl NewLvl;

   NewLvl = Begin_Lvl();
   Find_Pos(PosPtr, &NewLvl, FrmFilTyp, ToFilTyp);
   AppendLvl(LvlPtr, NewLvl);
   }/*ExtendLvl*/


static void
Do_BuiltInSearch(tp_DrvPth* DrvPthPtr,tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   *DrvPthPtr = NIL;
   if (FrmFilTyp->Pos == NIL) {
      if (ToFilTyp == FrmFilTyp) {
	 *DrvPthPtr = FilTyp_Eqv_DrvPth(ToFilTyp);
	 return; }/*if*/;
      ExtendLvl(PosPtr, LvlPtr, FrmFilTyp, ToFilTyp);
      if (*PosPtr != NIL) {
	 *DrvPthPtr = Pos_DrvPth(*PosPtr); }/*if*/; }/*if*/;
   }/*Do_BuiltInSearch*/


static void
Find_SubTypePos(tp_Pos* PosPtr,tp_Lvl* LvlPtr,tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   tp_Pos InpPos;

   if (FrmFilTyp->Pos != NIL) {
      return; }/*if*/;
   AddInpToLvl(&InpPos, LvlPtr, FrmFilTyp);
   AddEqvEdgsToLvl(PosPtr, LvlPtr, InpPos, ToFilTyp);
   if (*PosPtr != NIL) {
      return; }/*if*/;
   AddCastLvls1(PosPtr, LvlPtr, InpPos, ToFilTyp);
   }/*Find_SubTypePos*/


bool
IsSubType(tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   tp_Lvl Lvl;
   tp_Pos Pos;

   FORBIDDEN(!IsAtmc_FilTyp(ToFilTyp));

   if (ToFilTyp == FrmFilTyp
       || ToFilTyp == FileFilTyp || ToFilTyp == ObjectFilTyp) {
      return true; }/*if*/;

   Init_Lvls();
   Lvl = NIL;
   Find_SubTypePos(&Pos, &Lvl, FrmFilTyp, ToFilTyp);
   if (Pos == NIL) Find_SubTypePos(&Pos, &Lvl, FileFilTyp, ToFilTyp);
   if (Pos == NIL) Find_SubTypePos(&Pos, &Lvl, ObjectFilTyp, ToFilTyp);
   Clear_InLvls(Lvl);
   return (Pos != NIL);
   }/*IsSubType*/


void
Do_Search(tp_DrvPth* DrvPthPtr,bool* IsGenericPtr,tp_FKind FrmFKind,tp_FilTyp FrmFilTyp,tp_FilTyp ToFilTyp)
{
   tp_Lvl Lvl;
   tp_Pos Pos;

   FORBIDDEN(ToFilTyp == FrmFilTyp);
   *DrvPthPtr = NIL;
   *IsGenericPtr = false;

   Init_Lvls();
   Lvl = NIL;
   if (!CanPntrHo_FKind(FrmFKind)) {
      ExtendLvl(&Pos, &Lvl, FrmFilTyp, ToFilTyp);
      /*select*/{
	 if (Pos != NIL) {
	    *DrvPthPtr = Pos_DrvPth(Pos);
	 }else if (IsAtmc_FilTyp(FrmFilTyp)) {
	    Do_BuiltInSearch(DrvPthPtr, &Pos, &Lvl, FileFilTyp, ToFilTyp);
	 }else if (IsList_FilTyp(FrmFilTyp)) {
	    Do_BuiltInSearch(DrvPthPtr, &Pos, &Lvl, ListFilTyp, ToFilTyp);
	    };}/*select*/;
      if (*DrvPthPtr != NIL) {
	 goto done; }/*if*/; }/*if*/;

   Do_BuiltInSearch(DrvPthPtr, &Pos, &Lvl, ObjectFilTyp, ToFilTyp);
   if (*DrvPthPtr != NIL) {
      goto done; }/*if*/;

   /*select*/{
      if (CanPntrHo_FKind(FrmFKind) || IsPntr_FilTyp(FrmFilTyp)) {
	 *DrvPthPtr = Make_DrvPth(FK_PntrHo, ToFilTyp);
      }else if (GenericFilTyp->Pos != NIL) {
	 *IsGenericPtr = true;
	 Pos = GenericFilTyp->Pos;
	 *DrvPthPtr = Pos_DrvPth(Pos); };}/*select*/;

done:;
   Clear_InLvls(Lvl);
   }/*Do_Search*/

