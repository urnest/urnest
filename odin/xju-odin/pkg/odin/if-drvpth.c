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

#include <stdlib.h>
#include <gmc/gmc.h>
#include <odin/inc/NodTyp_.h>
#include <odin/inc/TokTyp_.h>
#include <gmc/nod.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Func.hh>
#include <dg/inc/DPType_.h>
#include <dg/inc/DrvPth.h>
#include <dg/inc/FKind_.h>


static tp_DrvPth	FreeDrvPth = NIL;
int		num_DrvPthS = 0;


static tp_DrvPth
New_DrvPth()
{
   tp_DrvPth DrvPth;

   /*select*/{
      if (FreeDrvPth == NIL) {
	 DrvPth = (tp_DrvPth)malloc(sizeof(tps_DrvPth));
	 num_DrvPthS += 1;
	 DrvPth->InUse = false;
      }else{
	 DrvPth = FreeDrvPth;
	 FreeDrvPth = FreeDrvPth->Next; };}/*select*/;

   DrvPth->DPType = ERROR;
   DrvPth->FKind = FK_User;
   DrvPth->FilTyp = ERROR;
   DrvPth->DrvEdg = ERROR;
   DrvPth->Next = NIL;
   FORBIDDEN(DrvPth->InUse);
   DrvPth->InUse = true;
   return DrvPth;
   }/*New_DrvPth*/


void
AppendDrvPth(
   tp_DrvPth* DrvPthPtr,
   tp_DrvPth AddDrvPth
   )
   
   
{
   tp_DrvPth DrvPth;

   if (AddDrvPth == NIL) return;
   DrvPth = *DrvPthPtr;
   if (DrvPth == NIL) {
      *DrvPthPtr = AddDrvPth;
      return; }/*if*/;
   while (DrvPth->Next != NIL) {
      DrvPth = DrvPth->Next; }/*while*/;
   DrvPth->Next = AddDrvPth;
   }/*AppendDrvPth*/


tp_DrvPth
FilTyp_Cast_DrvPth(
   tp_FilTyp FilTyp
   )
   
{
   tp_DrvPth DrvPth;

   FORBIDDEN(FilTyp == ERROR);
   DrvPth = New_DrvPth();
   DrvPth->DPType = DPT_Cast;
   DrvPth->FilTyp = FilTyp;
   return DrvPth;
   }/*FilTyp_Cast_DrvPth*/


tp_DrvPth
FilTyp_Eqv_DrvPth(
   tp_FilTyp FilTyp
   )
   
{
   tp_DrvPth DrvPth;

   FORBIDDEN(FilTyp == ERROR);
   DrvPth = New_DrvPth();
   DrvPth->DPType = DPT_Eqv;
   DrvPth->FilTyp = FilTyp;
   return DrvPth;
   }/*FilTyp_Eqv_DrvPth*/


tp_DrvPth
FilTyp_Drv_DrvPth(
   tp_FilTyp FilTyp,
   tp_DrvEdg DrvEdg
   )
   
   
{
   tp_DrvPth DrvPth;

   FORBIDDEN(FilTyp == ERROR);
   DrvPth = New_DrvPth();
   DrvPth->DPType = DPT_Drv;
   DrvPth->FilTyp = FilTyp;
   DrvPth->DrvEdg = DrvEdg;
   return DrvPth;
   }/*FilTyp_Drv_DrvPth*/


void
Ret_DrvPth(
   tp_DrvPth DrvPth
   )
   
{
   tp_DrvPth TmpDrvPth;

   FORBIDDEN(DrvPth == ERROR);
   for (TmpDrvPth = DrvPth;
	TmpDrvPth->Next != NIL;
	TmpDrvPth = TmpDrvPth->Next) {
      FORBIDDEN(!TmpDrvPth->InUse);
      TmpDrvPth->InUse = false; }/*for*/;
   FORBIDDEN(!TmpDrvPth->InUse);
   TmpDrvPth->InUse = false;
   TmpDrvPth->Next = FreeDrvPth;
   FreeDrvPth = DrvPth;
   }/*Ret_DrvPth*/


tp_DPType
DrvPth_DPType(
   tp_DrvPth DrvPth
   )
   
{
   return DrvPth->DPType;
   }/*DrvPth_DPType*/


tp_FKind
DrvPth_FKind(
   tp_DrvPth DrvPth
   )
   
{
   return DrvPth->FKind;
   }/*DrvPth_FKind*/


tp_FilTyp
DrvPth_FilTyp(
   tp_DrvPth DrvPth
   )
   
{
   return DrvPth->FilTyp;
   }/*DrvPth_FilTyp*/


tp_DrvEdg
DrvPth_DrvEdg(
   tp_DrvPth DrvPth
   )
   
{
   return DrvPth->DrvEdg;
   }/*DrvPth_DrvEdg*/


tp_DrvPth
DrvPth_Next(
   tp_DrvPth DrvPth
   )
   
{
   return DrvPth->Next;
   }/*DrvPth_Next*/



