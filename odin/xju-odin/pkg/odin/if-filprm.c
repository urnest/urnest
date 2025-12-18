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
#include <odin/inc/FilPrm.h>
#include <dg/inc/InpKind_.h>
#include <odin/inc/NodTyp_.h>
#include <odin/inc/PrmTypLst.h>
#include <stdlib.h>
#include <string.h>


tp_LocPrm	RootLocPrm = 1;
tp_FilPrm	RootFilPrm;

int		num_FilPrmS = 0;


static void
WriteFilPrm(tp_FilPrm FilPrm)
{
   WritePrmInf(&FilPrm->PrmInf, FilPrm->LocPrm);
   }/*WriteFilPrm*/


static tp_LocPrm
Alloc_PrmInf()
{
   return (tp_LocPrm) Alloc(sizeof(tps_PrmInf));
   }/*Alloc_PrmInf*/


static tp_FilPrm
New_FilPrm()
{
   tp_FilPrm FilPrm;

   FilPrm = (tp_FilPrm)malloc(sizeof(tps_FilPrm));
   num_FilPrmS += 1;
   FilPrm->LocPrm = NIL;
   FilPrm->NextHash = NIL;

   FilPrm->PrmInf.Father = NIL;
   FilPrm->PrmInf.IPrmTyp = 0;
   FilPrm->PrmInf.LocPVal = NIL;
   FilPrm->PrmTyp  = NIL;
   FilPrm->FilPVal = NIL;

   FilPrm->Father = NIL;
   FilPrm->Brother = NIL;
   FilPrm->Son = NIL;
   return FilPrm;
   }/*New_FilPrm*/


void
Init_FilPrm()
{
   tps_PrmInf _PrmInf; tp_PrmInf PrmInf = &_PrmInf;
   tp_LocPrm LocPrm;
   tp_PrmTyp PrmTyp;

   for (LocPrm = PValLocPrm; LocPrm != NIL; LocPrm = PrmInf->Father) {
      ReadPrmInf(PrmInf, LocPrm);
      PrmTyp = IPrmTyp_PrmTyp(PrmInf->IPrmTyp);
      SetPrmTyp_RootLocPVal(PrmTyp, PrmInf->LocPVal); }/*for*/;

   RootFilPrm = New_FilPrm();
   RootFilPrm->LocPrm = RootLocPrm;
   }/*Init_FilPrm*/


void
Add_RootLocPVal(tp_PrmTyp PrmTyp,tp_LocPVal LocPVal)
{
   tps_PrmInf _PrmInf; tp_PrmInf PrmInf = &_PrmInf;

   PrmInf->Father = PValLocPrm;
   PrmInf->IPrmTyp = PrmTyp_I(PrmTyp);
   PrmInf->LocPVal = LocPVal;
   PValLocPrm = Alloc_PrmInf();
   WritePrmInf(PrmInf, PValLocPrm);
   }/*Add_RootLocPVal*/


bool
Equal_FilPrm(tp_FilPrm FilPrm1,tp_FilPrm FilPrm2)
{
   FORBIDDEN(FilPrm1 == ERROR || FilPrm2 == ERROR);
   return FilPrm1 == FilPrm2;
   }/*Equal_FilPrm*/


static tp_FilPrm
Add_PrmInf(tp_FilPrm FilPrm,tp_PrmTyp PrmTyp,tp_FilPVal FilPVal)
{
   tp_FilPrm TmpFP;

   FORBIDDEN(FilPrm == ERROR || PrmTyp == ERROR || FilPVal == ERROR);
   for (TmpFP = FilPrm->Son; TmpFP != NIL; TmpFP = TmpFP->Brother) {
      if (PrmTyp == TmpFP->PrmTyp && FilPVal == TmpFP->FilPVal) {
	 return TmpFP; }/*if*/; }/*for*/;
   TmpFP = New_FilPrm();
   TmpFP->PrmInf.IPrmTyp = PrmTyp_I(PrmTyp);
   TmpFP->PrmTyp  = PrmTyp;
   TmpFP->FilPVal = FilPVal;
   TmpFP->Father = FilPrm;
   TmpFP->Brother = FilPrm->Son; FilPrm->Son = TmpFP;
   return TmpFP;
   }/*Add_PrmInf*/


static tp_FilPrm
Add_TailFilPrm(tp_FilPrm FilPrm,tp_FilPrm EndOldFP,tp_FilPrm OldFP)
{
   if (OldFP == EndOldFP) {
      return FilPrm; }/*if*/;
   FORBIDDEN(OldFP == RootFilPrm);
   return Add_PrmInf(Add_TailFilPrm(FilPrm, EndOldFP, OldFP->Father),
		     OldFP->PrmTyp, OldFP->FilPVal);
   }/*Add_TailFilPrm*/


tp_FilPrm
Append_PrmInf(tp_FilPrm FilPrm,tp_PrmTyp PrmTyp,tp_LocHdr LocHdr,tp_LocPVal LocPVal)
{
   tp_FilPrm TmpFP, HeadFP;
   tp_FilPVal NewFilPVal;

   if (FilPrm == ERROR || PrmTyp == ERROR) return ERROR;
   TmpFP = FilPrm;
   while (TmpFP != RootFilPrm && PrmTyp_I(TmpFP->PrmTyp) > PrmTyp_I(PrmTyp)) {
      TmpFP = TmpFP->Father; }/*while*/;
   /*select*/{
      if (PrmTyp == TmpFP->PrmTyp) {
	 if (IsFirst_PrmTyp(PrmTyp)) {
	    return FilPrm; }/*if*/;
	 HeadFP = TmpFP->Father;
	 NewFilPVal = TmpFP->FilPVal;
      }else{
	 HeadFP = TmpFP;
	 NewFilPVal = PrmTyp_RootFilPVal(PrmTyp); };}/*select*/;
   NewFilPVal = Append_PValInf(NewFilPVal, LocHdr, LocPVal);
   return Add_TailFilPrm(Add_PrmInf(HeadFP, PrmTyp, NewFilPVal),
			 TmpFP, FilPrm);
   }/*Append_PrmInf*/


tp_FilPrm
Append_FilPrm(tp_FilPrm FilPrm1,tp_FilPrm FilPrm2)
{
   tp_FilPrm FilPrm, TmpFP, HeadFP;
   tp_PrmTyp PrmTyp;
   tp_FilPVal FilPVal;

   if (FilPrm1 == ERROR || FilPrm2 == ERROR) return ERROR;

   if (FilPrm1 == FilPrm2) {
      return FilPrm1; }/*if*/;
   if (FilPrm1 == RootFilPrm) {
      return FilPrm2; }/*if*/;
   if (FilPrm2 == RootFilPrm) {
      return FilPrm1; }/*if*/;

   FilPrm = Append_FilPrm(FilPrm1, FilPrm2->Father);
   PrmTyp = FilPrm2->PrmTyp;
   FilPVal = FilPrm2->FilPVal;
   TmpFP = FilPrm;
   while (TmpFP != RootFilPrm && PrmTyp_I(TmpFP->PrmTyp) > PrmTyp_I(PrmTyp)) {
      TmpFP = TmpFP->Father; }/*while*/;

   HeadFP = TmpFP;
   if (PrmTyp == TmpFP->PrmTyp) {
      HeadFP = TmpFP->Father;
      if (IsFirst_PrmTyp(PrmTyp)) {
	 return FilPrm; }/*if*/;
      FilPVal = Append_FilPVal(TmpFP->FilPVal, FilPVal);
      if (FilPVal == TmpFP->FilPVal) {
	 return FilPrm; }/*if*/; }/*if*/;

   HeadFP = Add_PrmInf(HeadFP, PrmTyp, FilPVal);
   return Add_TailFilPrm(HeadFP, TmpFP, FilPrm);
   }/*Append_FilPrm*/


tp_LocPrm
FilPrm_LocPrm(tp_FilPrm FilPrm)
{
   FORBIDDEN(FilPrm == ERROR);
   if (FilPrm->LocPrm == NIL) {
      Hash_Item((tp_Item)FilPrm, (tp_Loc)Alloc_PrmInf());
      FilPrm->PrmInf.Father = FilPrm_LocPrm(FilPrm->Father);
      FilPrm->PrmInf.LocPVal = FilPVal_LocPVal(FilPrm->FilPVal);
      WriteFilPrm(FilPrm); }/*if*/;
   return FilPrm->LocPrm;
   }/*FilPrm_LocPrm*/


static tp_FilPrm
Lookup_FilPrm(tp_LocPrm LocPrm)
{
   return (tp_FilPrm)Lookup_Item(LocPrm);
   }/*Lookup_FilPrm*/


tp_FilPrm
LocPrm_FilPrm(tp_LocPrm LocPrm)
{
   tp_FilPrm FilPrm;
   tps_PrmInf _PrmInf; tp_PrmInf PrmInf = &_PrmInf;

   if (LocPrm == ERROR) return ERROR;

   if (LocPrm == RootLocPrm) {
      return RootFilPrm; }/*if*/;

   FilPrm = Lookup_FilPrm(LocPrm);
   if (FilPrm != ERROR) {
      return FilPrm; }/*if*/;

   ReadPrmInf(PrmInf, LocPrm);
   FilPrm = Add_PrmInf(LocPrm_FilPrm(PrmInf->Father),
		       I_PrmTyp(PrmInf->IPrmTyp),
		       LocPVal_FilPVal(PrmInf->LocPVal));
   if (FilPrm->LocPrm == NIL) {
      Hash_Item((tp_Item)FilPrm, (tp_Loc)LocPrm); }/*if*/;
   return FilPrm;
   }/*LocPrm_FilPrm*/


static bool
In_PrmTypLst(tp_PrmTyp PrmTyp,tp_PrmTypLst PrmTypLst)
{
   tp_PrmTypLst OKLElem;

   for (OKLElem = PrmTypLst; OKLElem != NIL; OKLElem = OKLElem->Next) {
      if (PrmTyp == OKLElem->PrmTyp) {
	 return true; }/*if*/; }/*for*/;
   return false;
   }/*In_PrmTypLst*/


tp_FilPrm
Strip_FilPrm(tp_FilPrm FilPrm,tp_PrmTypLst PrmTypLst)
{
   tp_FilPrm FatherFilPrm;

   if (FilPrm == RootFilPrm) {
      return RootFilPrm; }/*if*/;

   FatherFilPrm = Strip_FilPrm(FilPrm->Father, PrmTypLst);
   if (!In_PrmTypLst(FilPrm->PrmTyp, PrmTypLst)) {
      return FatherFilPrm; }/*if*/;
   if (FatherFilPrm == FilPrm->Father) {
      return FilPrm; }/*if*/;
   return Add_PrmInf(FatherFilPrm, FilPrm->PrmTyp, FilPrm->FilPVal);
   }/*Strip_FilPrm*/


tp_FilPrm
StripExcept_FilPrm(tp_FilPrm FilPrm,tp_PrmTyp PrmTyp)
{
   tp_FilPrm FatherFilPrm;

   if (FilPrm == RootFilPrm) {
      return RootFilPrm; }/*if*/;

   FatherFilPrm = StripExcept_FilPrm(FilPrm->Father, PrmTyp);
   if (FilPrm->PrmTyp == PrmTyp) {
      return FatherFilPrm; }/*if*/;
   if (FatherFilPrm == FilPrm->Father) {
      return FilPrm; }/*if*/;
   return Add_PrmInf(FatherFilPrm, FilPrm->PrmTyp, FilPrm->FilPVal);
   }/*StripExcept_FilPrm*/


tp_FilPVal
FilPrm_FilPVal(tp_FilPrm FilPrm)
{
   if (FilPrm == ERROR) {
      return ERROR; }/*if*/;
   return FilPrm->FilPVal;
   }/*FilPrm_FilPVal*/


tp_FilPVal
Get_FilPVal(tp_FilPrm FilPrm,tp_PrmTyp PrmTyp)
{
   tp_FilPrm TmpFP;

   for (TmpFP = FilPrm; TmpFP != RootFilPrm; TmpFP = TmpFP->Father) {
      if (TmpFP->PrmTyp == PrmTyp) {
	 return TmpFP->FilPVal; }/*if*/; }/*for*/;
   return (tp_FilPVal)NIL;
   }/*Get_FilPVal*/


tp_FilHdr
Get_FPVFilHdr(tp_PrmTyp PrmTyp,tp_FilPrm FilPrm)
{
   tp_FilPrm TmpFP;
   tp_FilPVal FilPVal;
   tp_LocHdr LocHdr;
   tp_FilHdr FilHdr;

   for (TmpFP = FilPrm; TmpFP != RootFilPrm; TmpFP = TmpFP->Father) {
      if (PrmTyp == TmpFP->PrmTyp) {
	 FilPVal = TmpFP->FilPVal;
	 if (IsFirst_PrmTyp(PrmTyp)) {
	    if (IsRootFilPVal(FilPVal)) {
	       return Copy_FilHdr(FlagPrmFilHdr); }/*if*/;
	    while (FilPVal_ValLocPVal(FilPVal) != NIL) {
	       FilPVal = LocPVal_FilPVal(FilPVal_ValLocPVal(FilPVal));
	       }/*while*/;
	    LocHdr = FilPVal_LocHdr(FilPVal);
	    FORBIDDEN(LocHdr == NIL);
	    return LocHdr_FilHdr(LocHdr); }/*if*/;
	 LocHdr = FilPVal_DataLocHdr(FilPVal);
	 if (LocHdr != 0) {
	    return LocHdr_FilHdr(LocHdr); }/*if*/;
	 FilHdr = Insert_FilHdr(Copy_FilHdr(PrmDataDirFilHdr),
				FK_User, PrmValuesFilTyp, TmpFP, DfltIdent);
	 FilHdr = Do_Deriv(FilHdr, RootFilPrm, RootFilPrm,
			   PrmTyp_FilTyp(PrmTyp));
	 Set_FilPVal_DataLocHdr(FilPVal, FilHdr_LocHdr(FilHdr));
	 return FilHdr; }/*if*/; }/*for*/;
   return Copy_FilHdr(NilStrFilHdr);
   }/*Get_FPVFilHdr*/


tp_FilPrm
FilPrm_DerefPrmVal(tp_FilPrm FilPrm)
{
   if (FilPrm == RootFilPrm) {
      return FilPrm; }/*if*/;
   return Add_PrmInf
    (FilPrm_DerefPrmVal(FilPrm->Father), FilPrm->PrmTyp,
     FilPVal_DerefPrmVal(FilPrm->FilPVal, FilPrm->PrmTyp));
   }/*FilPrm_DerefPrmVal*/


void
Chain_FilPrm_DerefPrmVal(tp_LocInp* FirstLIPtr,tp_LocInp* LastLIPtr,tp_FilPrm FilPrm,tp_FilHdr OutFilHdr)
{
   tp_FilPrm TmpFP;

   for (TmpFP = FilPrm; TmpFP != RootFilPrm; TmpFP = TmpFP->Father) {
      Chain_FilPVal_DerefPrmVal(FirstLIPtr, LastLIPtr, TmpFP->FilPVal,
				OutFilHdr); }/*for*/;
   }/*Chain_FilPrm_DerefPrmVal*/


void
Print_FilPrm(tp_FilDsc FilDsc,tp_Str Str,tp_FilPrm FilPrm)
{
   FORBIDDEN(((FilDsc == NIL) == (Str == NIL)) || FilPrm == ERROR);

   if (Str != NIL) {
      (void)strcpy(Str, ""); }/*if*/;
   if (FilPrm == RootFilPrm) {
      return; }/*if*/;

   Print_FilPrm(FilDsc, Str, FilPrm->Father);
   Print_FilPVal(FilDsc, Tail(Str), FilPrm->PrmTyp, FilPrm->FilPVal);
   }/*Print_FilPrm*/


void
SetPrmTypLst_Marks(tp_PrmTypLst PrmTypLst)
{
   tp_PrmTypLst TmpPrmTypLst;

   for (TmpPrmTypLst = PrmTypLst;
	TmpPrmTypLst != NIL;
	TmpPrmTypLst = TmpPrmTypLst->Next) {
      SetPrmTyp_Mark(TmpPrmTypLst->PrmTyp); }/*for*/;
   }/*SetPrmTypLst_Marks*/


