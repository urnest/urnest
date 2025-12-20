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
#include <odin/inc/NodTyp_.h>
#include <odin/inc/TokTyp_.h>
#include <gmc/nod.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Func.hh>
#include <odin/inc/Var.hh>
#include <odin/inc/Client.h>
#include <dg/inc/FKind_.h>
#include <odin/inc/PrmTyp.h>
#include <string.h>


extern int		num_PrmTypS;
extern tp_PrmTyp	PrmTypS;


tp_PTName
PrmTyp_PTName(tp_PrmTyp PrmTyp)
{
   if (PrmTyp == ERROR) return ERROR;
   return PrmTyp->PTName;
   }/*PrmTyp_PTName*/


static tp_PrmTyp
PTName_PrmTyp(tp_PTName PTName)
{
   int i;
   tp_PrmTyp PrmTyp;

   if (PTName == ERROR) {
      return ERROR; }/*if*/;
   for (i=0; i<num_PrmTypS; i++) {
      PrmTyp = &PrmTypS[i];
      if (strcmp(PTName, PrmTyp->PTName) == 0) {
	 return PrmTyp; }/*if*/; }/*for*/;
   return ERROR;
   }/*PTName_PrmTyp*/


tp_FilTyp
PrmTyp_FilTyp(tp_PrmTyp PrmTyp)
{
   if (PrmTyp == ERROR) return ERROR;
   return PrmTyp->FilTyp;
   }/*PrmTyp_FilTyp*/


bool
IsFirst_PrmTyp(tp_PrmTyp PrmTyp)
{
   FORBIDDEN(PrmTyp == ERROR);
   return (PrmTyp->FilTyp == FirstFilTyp);
   }/*IsFirst_PrmTyp*/


int
PrmTyp_I(tp_PrmTyp PrmTyp)
{
   FORBIDDEN(PrmTyp == ERROR);
   return PrmTyp->IPrmTyp;
   }/*PrmTyp_I*/


void
SetPrmTyp_Mark(tp_PrmTyp PrmTyp)
{
   PrmTyp->Mark = true;
   }/*SetPrmTyp_Mark*/


tp_FilHdr
PrmTyp_StrDirFilHdr(tp_PrmTyp PrmTyp)
{
   tp_FilHdr FilHdr;

   if (PrmTyp == ERROR) {
      return ERROR; }/*if*/;
   if (PrmTyp->StrDirLocHdr == NIL) {
      FilHdr = Extend_FilHdr(Copy_FilHdr(StrDirFilHdr), FK_Str,
			     ObjectFilTyp, RootFilPrm, PrmTyp_PTName(PrmTyp));
      PrmTyp->StrDirLocHdr = FilHdr_LocHdr(FilHdr);
      return FilHdr; }/*if*/;
   return LocHdr_FilHdr(PrmTyp->StrDirLocHdr);
   }/*PrmTyp_StrDirFilHdr*/


tp_FilPVal
PrmTyp_RootFilPVal(tp_PrmTyp PrmTyp)
{
   tp_FilPVal FilPVal;
   tp_LocPVal LocPVal;

   if (PrmTyp->RootLocPVal == NIL) {
      FilPVal = New_FilPVal();
      LocPVal = FilPVal_LocPVal(FilPVal);
      Add_RootLocPVal(PrmTyp, LocPVal);
      PrmTyp->RootLocPVal = LocPVal; }/*if*/;
   return LocPVal_FilPVal(PrmTyp->RootLocPVal);
   }/*PrmTyp_RootFilPVal*/


void
SetPrmTyp_RootLocPVal(tp_PrmTyp PrmTyp,tp_LocPVal LocPVal)
{
   FORBIDDEN(PrmTyp->RootLocPVal != NIL || LocPVal == NIL);
   PrmTyp->RootLocPVal = LocPVal;
   }/*SetPrmTyp_RootLocPVal*/


void
SetFilHdr_PrmTypMarks(tp_FilHdr FilHdr)
{
   SetFilHdr_Marks(FilHdr, true);
   }/*SetFilHdr_PrmTypMarks*/


void
Clr_PrmTypMarks()
{
   int i;

   for (i=0; i<num_PrmTypS; i++) {
      PrmTypS[i].Mark = false; }/*for*/;
   Clr_FilTypMarks();
   }/*Clr_PrmTypMarks*/


void
WriteMarkedPrmTyps(tp_FilDsc FilDsc)
{
   int i;
   tp_PrmTyp PrmTyp;
   bool Found;

   Found = false;
   for (i=0; i<num_PrmTypS; i++) {
      PrmTyp = &PrmTypS[i];
      if (PrmTyp->Mark && CurrentClient->HelpLevel >= PrmTyp->HelpLevel) {
	 WriteNameDesc(FilDsc, PrmTyp->PTName, PrmTyp->Desc);
	 Found = true; }/*if*/; }/*for*/;
   if (!Found) {
      Writeln(FilDsc, "(none)"); }/*if*/;
   }/*WriteMarkedPrmTyps*/


tp_PrmTyp
Nod_PrmTyp(tp_Nod DrvTyp_Nod)
{
   tp_Str Str;
   tp_PrmTyp PrmTyp;

   Str = Sym_Str(Nod_Sym(DrvTyp_Nod));
   PrmTyp = PTName_PrmTyp(Str);
   if (PrmTyp == ERROR) SystemError("No parameter type, <%s>.\n", Str);
   return PrmTyp;
   }/*Nod_PrmTyp*/


