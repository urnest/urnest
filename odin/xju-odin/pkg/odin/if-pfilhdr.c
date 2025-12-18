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
#include <odin/inc/PrmFHdr.h>
#include <stdlib.h>

int			num_PrmFHdrS = 0;

static int		num_PrmFHdrs = 0;
static tp_PrmFHdr	FreePrmFHdr = NIL;

/* PrmFHdr's are absorbed when used as an argument to a function
 * that returns a PrmFHdr.
 */

tp_PrmFHdr
New_PrmFHdr(
   tp_FilHdr FilHdr,
   tp_FilPrm FilPrm
   )
   
   
{
   tp_PrmFHdr PrmFHdr;

   FORBIDDEN(FilHdr == ERROR || FilPrm == ERROR);

   /*select*/{
      if (FreePrmFHdr == NIL) {
	 PrmFHdr = (tp_PrmFHdr)malloc(sizeof(tps_PrmFHdr));
	 num_PrmFHdrS += 1;
	 PrmFHdr->InUse = false;
      }else{
	 PrmFHdr = FreePrmFHdr;
	 FreePrmFHdr = FreePrmFHdr->Next; };}/*select*/;

   FORBIDDEN(PrmFHdr->InUse);
   num_PrmFHdrs += 1;
   PrmFHdr->InUse = true;
   PrmFHdr->FilHdr = FilHdr;
   PrmFHdr->FilPrm = FilPrm;
   PrmFHdr->Next = NIL;
   return PrmFHdr;
   }/*New_PrmFHdr*/


static void
Rls_PrmFHdr(
   tp_PrmFHdr PrmFHdr
   )
   
{
   FORBIDDEN(PrmFHdr == ERROR);
   PrmFHdr->Next = FreePrmFHdr;
   FORBIDDEN(!PrmFHdr->InUse);
   FORBIDDEN(PrmFHdr->FilHdr != NIL);
   PrmFHdr->InUse = false;
   num_PrmFHdrs -= 1;
   FreePrmFHdr = PrmFHdr;
   }/*Rls_PrmFHdr*/


void
Use_PrmFHdr(
   tp_FilHdr* FilHdrPtr,
   tp_FilPrm* FilPrmPtr,
   tp_PrmFHdr PrmFHdr
   )
   
   
   
{
   if (PrmFHdr == ERROR) {
      *FilHdrPtr = ERROR;
      *FilPrmPtr = ERROR;
      return; }/*if*/;
   *FilHdrPtr = PrmFHdr->FilHdr;
   PrmFHdr->FilHdr = NIL;
   *FilPrmPtr = PrmFHdr->FilPrm;
   Rls_PrmFHdr(PrmFHdr);
   }/*Use_PrmFHdr*/


bool
PrmFHdrs_InUse()
{
   return (num_PrmFHdrs != 0);
   }/*PrmFHdrs_InUse*/

