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
#include <odin/inc/CastEdg.h>
#include <odin/inc/DrvEdg.h>
#include <odin/inc/EqvEdg.h>
#include <odin/inc/MemEdg.h>
#include <odin/inc/InpEdg.h>
#include <dg/inc/InpKind_.h>


tp_PrmTypLst
DrvEdg_PrmTypLst(tp_DrvEdg DrvEdg)
{
   FORBIDDEN(DrvEdg == ERROR);
   return DrvEdg->PrmTypLst;
   }/*DrvEdg_PrmTypLst*/


tp_InpSpc
InpEdg_InpSpc(tp_InpEdg InpEdg)
{
   FORBIDDEN(InpEdg == ERROR);
   return InpEdg->InpSpc;
   }/*InpEdg_InpSpc*/


tp_InpKind
InpEdg_InpKind(tp_InpEdg InpEdg)
{
   FORBIDDEN(InpEdg == ERROR);
   return InpEdg->InpKind;
   }/*InpEdg_InpKind*/


bool
InpEdg_IsUserArg(tp_InpEdg InpEdg)
{
   FORBIDDEN(InpEdg == ERROR);
   return InpEdg->IsUserArg;
   }/*InpEdg_IsUserArg*/


tp_InpEdg
InpEdg_Next(tp_InpEdg InpEdg)
{
   FORBIDDEN(InpEdg == ERROR);
   return InpEdg->Next;
   }/*InpEdg_Next*/


tp_FilTyp
EqvEdg_FilTyp(tp_EqvEdg EqvEdg)
{
   return EqvEdg->FilTyp;
   }/*EqvEdg_FilTyp*/


tp_FilTyp
MemEdg_FilTyp(tp_MemEdg MemEdg)
{
   return MemEdg->FilTyp;
   }/*MemEdg_FilTyp*/


static bool
InpKind_IsName(tp_InpKind InpKind)
{
   return (InpKind == IK_Name);
   }/*InpKind_IsName*/


static bool
InpKind_IsTransName(tp_InpKind InpKind)
{
   return (InpKind == IK_TransName);
   }/*InpKind_IsTransName*/


static bool
InpKind_IsTrans(tp_InpKind InpKind)
{
   return (InpKind == IK_Trans || InpKind == IK_AnyOK);
   }/*InpKind_IsTrans*/


bool
InpKind_IsAnyOK(tp_InpKind InpKind)
{
   return (InpKind == IK_AnyOK);
   }/*InpKind_IsAnyOK*/


bool
NeedsData(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   return !(InpKind_IsName(InpKind)
	    || (InpKind_IsTransName(InpKind) && !IsRef(FilHdr)));
   }/*NeedsData*/


bool
NeedsElmData(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   return ((InpKind_IsTrans(InpKind) && IsList(FilHdr))
	   || (InpKind != IK_Pntr && InpKind != IK_TransName
	       && IsPntr(FilHdr)));
   }/*NeedsElmData*/


bool
NeedsElmNameData(tp_FilHdr FilHdr,tp_InpKind InpKind)
{
   return (InpKind_IsTransName(InpKind) && IsRef(FilHdr));
   }/*NeedsElmNameData*/


int
NumInputs(tp_FilTyp FilTyp)
{
   int i;
   tp_InpEdg InpEdg;
   bool IsUserArg;

   IsUserArg = true;
   i = 0;
   for (InpEdg = Tool_InpEdg(FilTyp_Tool(FilTyp));
	InpEdg != NIL;
	InpEdg = InpEdg->Next) {
      /*select*/{
	 if (InpEdg->IsUserArg) {
	    FORBIDDEN(!IsUserArg);
	    i++;
	 }else{
	    IsUserArg = false; };}/*select*/; }/*while*/;
   return i;
   }/*NumInputs*/


void
GetOutTyps(tp_FilTyp FilTyp,tp_OutTyps OutTyps,int* NumOutputsPtr)
{
   int i;
   tp_MemEdg MemEdg;

   if (!IsStruct_FilTyp(FilTyp)) {
      OutTyps[0] = FilTyp;
      *NumOutputsPtr = 1;
      return; }/*if*/;

   for (i = 0, MemEdg = FilTyp_MemEdg(FilTyp);
	MemEdg != NIL;
	i += 1, MemEdg = MemEdg->Next) {
      OutTyps[i] = MemEdg->FilTyp; }/*for*/;
   *NumOutputsPtr = i;
   }/*GetOutTyps*/


void
SetEqvEdg_Marks(tp_EqvEdg EqvEdg,bool CastFlag,bool PrmTypFlag)
{
   tp_EqvEdg TmpEE;

   for (TmpEE = EqvEdg; TmpEE != NIL; TmpEE = TmpEE->Next) {
      if (!PrmTypFlag) SetFilTyp_Mark(TmpEE->FilTyp);
      SetFilTyp_Marks(TmpEE->FilTyp, CastFlag, PrmTypFlag); }/*for*/;
   }/*SetEqvEdg_Marks*/


void
SetCastEdg_Marks(tp_CastEdg CastEdg,bool PrmTypFlag)
{
   tp_CastEdg TmpCE;

   for (TmpCE = CastEdg; TmpCE != NIL; TmpCE = TmpCE->Next) {
      if (!PrmTypFlag) SetFilTyp_Mark(TmpCE->FilTyp);
      SetFilTyp_Marks(TmpCE->FilTyp, true, PrmTypFlag); }/*for*/;
   }/*SetCastEdg_Marks*/


void
SetDrvEdg_Marks(tp_DrvEdg DrvEdg,bool PrmTypFlag)
{
   tp_DrvEdg TmpDE;

   for (TmpDE = DrvEdg; TmpDE != NIL; TmpDE = TmpDE->Next) {
      ;/*select*/{
	 if (PrmTypFlag) {
	    SetPrmTypLst_Marks(TmpDE->PrmTypLst);
	 }else{
	    SetFilTyp_Mark(TmpDE->FilTyp); };}/*select*/;
      SetFilTyp_Marks(TmpDE->FilTyp, false, PrmTypFlag); }/*for*/;
   }/*SetDrvEdg_Marks*/



