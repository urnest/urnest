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
#include <string.h>


extern tp_Str		PrevParseStr;
extern tp_Str		ParseStr;
extern tp_FileName	ParseFN;
extern int		*ParseLNPtr;


tp_Nod
YY_Parser(const char* Str,tp_FileName FileName,int* LNPtr)
{
   tps_Str StrBuf;

   if (Str == ERROR) return ERROR;
   (void)strcpy(StrBuf, Str);
   PrevParseStr = StrBuf;
   ParseStr = StrBuf;
   ParseFN = FileName;
   ParseLNPtr = LNPtr;
   return YY_Parse();
   }/*YY_Parser*/


int
YY_Lex()
{
   IsCmdLex = false;
   return Lex();
   }/*YY_Lex*/


void
YY_Unparse(tp_Str Str,tp_Nod Nod)
{
   tp_Nod Son;
   tp_Str SubStr;
   bool First;

   switch (Nod_NodTyp(Nod)) {
      case DRVFIL: {
	 Son = Nod_Son(1, Nod);
	 YY_Unparse(Str, Son);
	 SubStr = Tail(Str);
	 for (Son = Nod_Brother(Son); Son != NIL; Son = Nod_Brother(Son)) {
	    if (Nod_NodTyp(Son) != ELMOPR && Nod_NodTyp(Son) != DIROPR) {
	       (void)strcat(SubStr, " "); }/*if*/;
	    SubStr = Tail(SubStr);
	    YY_Unparse(SubStr, Son); }/*for*/;
	 break; }/*case*/;
      case WORD: {
	 Unlex(Str, Sym_Str(Nod_Sym(Nod)));
	 break; }/*case*/;
      case HOSTWD: {
	 (void)strcpy(Str, "!");
	 Unlex(Tail(Str), Sym_Str(Nod_Sym(Nod)));
	 break; }/*case*/;
      case EMPFIL: {
	 (void)strcpy(Str, "()");
	 break; }/*case*/;
      case ARTFIL: {
	 (void)strcpy(Str, "/");
	 break; }/*case*/;
      case OBJTID: {
	 (void)strcpy(Str, "\0");
	 Unlex(Tail(Str), Sym_Str(Nod_Sym(Nod)));
	 break; }/*case*/;
      case ABSFIL: {
	 (void)strcpy(Str, "/");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 break; }/*case*/;
      case STRING: {
	 (void)strcpy(Str, "=");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 break; }/*case*/;
      case PRMOPR: {
	 (void)strcpy(Str, "+");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 if (Nod_NumSons(Nod) > 1) {
	    (void)strcpy(Tail(Str), "=");
	    YY_Unparse(Tail(Str), Nod_Son(2, Nod)); }/*if*/;
	 break; }/*case*/;
      case APLOPR: {
	 (void)strcpy(Str, "+(");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 (void)strcat(Str, ")");
	 break; }/*case*/;
      case PRMVLS: {
	 (void)strcpy(Str, "");
	 First = true;
	 for (Son = Nod_Son(1, Nod); Son != NIL; Son = Nod_Brother(Son)) {
	    if (!First) (void)strcpy(Tail(Str), " ");
	    if (Nod_NodTyp(Son) == DRVFIL) (void)strcat(Str, "(");
	    YY_Unparse(Tail(Str), Son);
	    if (Nod_NodTyp(Son) == DRVFIL) (void)strcat(Str, ")");
	    First = false; }/*for*/;
	 break; }/*case*/;
      case DRVOPR: {
	 (void)strcpy(Str, ":");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 break; }/*case*/;
      case HODOPR: {
	 (void)strcpy(Str, ":");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 (void)strcpy(Tail(Str), "=:");
	 Son = Nod_Son(2, Nod);
	 YY_Unparse(Tail(Str), Son);
	 break; }/*case*/;
      case ELMOPR: {
	 (void)strcpy(Str, "/");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 break; }/*case*/;
      case SEGOPR: {
	 (void)strcpy(Str, "%");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 break;}/*case*/;
      case DIROPR: {
	 (void)strcpy(Str, "/");
	 break; }/*case*/;
      case OPRTNS: {
	 (void)strcpy(Str, "(");
	 YY_Unparse(Tail(Str), Nod_Son(1, Nod));
	 for (Son = Nod_Son(2, Nod); Son != NIL; Son = Nod_Brother(Son)) {
	    (void)strcpy(Tail(Str), " ");
	    YY_Unparse(Tail(Str), Son); }/*for*/;
	 (void)strcat(Str, ")");
	 break; }/*case*/;
      default: {
	 FATALERROR("Unexpected NodTyp"); };}/*switch*/;
   }/*YY_Unparse*/
