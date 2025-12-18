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
#include <gmc/gmc.h>
#include <dg/inc/Func.hh>
#include <dg/inc/Type.hh>
#include <dg/inc/FilTyp.h>
#include <dg/inc/FKind_.h>
#include <dg/inc/FTClass_.h>
#include <dg/inc/SrcTyp.h>
#include <dg/inc/TClass_.h>
#include <dg/inc/Tool.h>


bool
IsPntr_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_PntrHo || FKind == FK_InpPntr
	   || FKind == FK_VirDirElm || FKind == FK_PntrElm
	   || IsVTgt_FKind(FKind) || IsATgt_FKind(FKind));
   }/*IsPntr_FKind*/


bool
CanPntrHo_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return IsPntr_FKind(FKind);
   }/*CanPntrHo_FKind*/


bool
IsATgt_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_ActTgt || FKind == FK_ActCmdTgt);
   }/*IsATgt_FKind*/


bool
IsVTgt_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_VirTgt || FKind == FK_VirCmdTgt);
   }/*IsVTgt_FKind*/


bool
IsATgtText_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_ActTgtText || FKind == FK_ActTgtExText);
   }/*IsATgt_FKind*/


bool
IsVTgtText_FKind(
  tp_FKind  FKind
   )
  
{
   FORBIDDEN(FKind == ERROR);
   return (FKind == FK_VirTgtText || FKind == FK_VirTgtExText);
   }/*IsVTgt_FKind*/


bool
IsExternal_Tool(
  tp_Tool  Tool
   )
  
{
   FORBIDDEN(Tool == ERROR);
   return (Tool->TClass == TC_External);
   }/*IsExternal_Tool*/


bool
IsMap_Tool(
  tp_Tool  Tool
   )
  
{
  return (Tool != ERROR) && (Tool->TClass == TC_Map);
}/*IsExternal_Tool*/


tp_MemEdg
FilTyp_MemEdg(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == NIL);
   return FilTyp->MemEdg;
   }/*FilTyp_MemEdg*/


tp_CastEdg
FilTyp_CastEdg(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return FilTyp->CastEdg;
   }/*FilTyp_CastEdg*/


tp_PrmTypLst
FilTyp_MapPrmTypLst(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return FilTyp->MapPrmTypLst;
   }/*FilTyp_MapPrmTypLst*/


tp_FilTyp
FilTyp_ArgFilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   if (FilTyp == ERROR) return ERROR;
   return FilTyp->ArgFilTyp;
   }/*FilTyp_ArgFilTyp*/


tp_Tool
FilTyp_Tool(
  tp_FilTyp  FilTyp
   )
  
{
   if (FilTyp == ERROR) return ERROR;
   return FilTyp->Tool;
   }/*FilTyp_Tool*/


tp_FTName
FilTyp_ShortFTName(
  tp_FilTyp  FilTyp
   )
  
{
   if (FilTyp == ERROR) return ERROR;
   if (IsSecOrd_FilTyp(FilTyp)) {
      return FilTyp_ShortFTName(FilTyp_ArgFilTyp(FilTyp)); }/*if*/;
   if (IsStruct_FilTyp(FilTyp)) {
      return FilTyp_ShortFTName(MemEdg_FilTyp(FilTyp->MemEdg)); }/*if*/;
   if (FilTyp->Tool == NIL) {
      return FilTyp_FTName(FilTyp); }/*if*/;
   return FilTyp_FTName(EqvEdg_FilTyp(FilTyp->EqvEdg));
   }/*FilTyp_ShortFTName*/


tp_FTName
FilTyp_FTName(
  tp_FilTyp  FilTyp
   )
  
{
   if (FilTyp == ERROR) return ERROR;
   return FilTyp->FTName;
   }/*FilTyp_FTName*/


bool
IsCopy_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return FilTyp->IsCopy;
   }/*IsCopy_FilTyp*/


bool
IsGrouping_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return FilTyp->IsGrouping;
   }/*IsGrouping_FilTyp*/


bool
IsGroupingInput_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return FilTyp->IsGroupingInput;
   }/*IsGroupingInput_FilTyp*/


bool
IsSecOrd_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->ArgFilTyp != NIL);
   }/*IsSecOrd_FilTyp*/


bool
IsRecurse_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (IsSecOrd_FilTyp(FilTyp)
	   && (strcmp(FilTyp->FTName, "recurse") == 0));
   }/*IsRecurse_FilTyp*/


bool
IsExec_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_Exec);
   }/*IsExec_FilTyp*/


bool
IsVoid_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_Void);
   }/*IsVoid_FilTyp*/


bool
IsAtmc_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_Atmc || FilTyp->FTClass == FTC_DrvDir
	   || FilTyp->FTClass == FTC_Generic || FilTyp->FTClass == FTC_Pipe
	   || FilTyp->FTClass == FTC_Exec || FilTyp->FTClass == FTC_Void);
   }/*IsAtmc_FilTyp*/


bool
IsPntr_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_Pntr);
   }/*IsPntr_FilTyp*/


bool
IsList_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   tp_FTClass FTClass;

   FORBIDDEN(FilTyp == ERROR);
   FTClass = FilTyp->FTClass;
   return (FTClass == FTC_List);
   }/*IsList_FilTyp*/


bool
IsDrvDir_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_DrvDir);
   }/*IsDrvDir_FilTyp*/


bool
IsStruct_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->FTClass == FTC_Struct);
   }/*IsStruct_FilTyp*/


bool
IsStructMem_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   FORBIDDEN(FilTyp == ERROR);
   return (FilTyp->Tool != NIL && FilTyp->Tool->TClass == TC_StructMem);
   }/*IsStructMem_FilTyp*/


bool
IsGeneric_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   return FilTyp->FTClass == FTC_Generic;
   }/*IsGeneric_FilTyp*/


bool
IsPipe_FilTyp(
  tp_FilTyp  FilTyp
   )
  
{
   return FilTyp->FTClass == FTC_Pipe;
   }/*IsPipe_FilTyp*/

