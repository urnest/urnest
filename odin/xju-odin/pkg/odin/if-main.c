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
#include <string.h>


tp_Str		Author = "odin-build-users@lists.sourceforge.net";

bool		IsTTY;

void
InterruptAction()
{
   Do_Interrupt(true);
   }/*InterruptAction*/


void
TopLevelCI(bool* AbortPtr,tp_Str Str)
{
   tp_Nod Root;

   Root = OC_Parser(Str, (tp_FileName)NIL, (int *)NIL);
   if (Root == ERROR) {
      *AbortPtr = true;
      return; }/*if*/;
   if (VerifyLevel >= 2 && IsTTY) {
      Test_All(); }/*if*/;
   CommandInterpreter(AbortPtr, Root, IsTTY);
   Ret_Nod(Root);
   }/*TopLevelCI*/


void
Get_Commands(bool* AbortPtr)
{
   static bool In_Get_Commands = false;

   if (In_Get_Commands) {
      SystemError("Already reading commands.\n");
      *AbortPtr = true;
      return; }/*if*/;
   In_Get_Commands = true;
   if (IsTTY) Print_Banner();
   IPC_Get_Commands(AbortPtr, (IsServer ? "=> " : "-> "));
   if (IsTTY) Writeln(StdOutFD, "");
   In_Get_Commands = false;
   }/*Get_Commands*/


int
c_main(int argc,char** argv)
{
   bool Abort, NewFlag;
   int i;

   Init_IO();
   Init_Err();
   Init_Env();
   Init_Sigs(false);
   IPC_Init();
   IPC_Action = TopLevelCI;

   if (IsServer) {
      Init_Sigs(true);
      Read_DrvGrf();
      Init_Info(&NewFlag);
      Init_FilHdrs();
      Init_FilInps();
      Init_FilElms();
      Init_FilPrm();
      Init_FilHdrTree();
      Activate_Client(LocalClient);
      if (NewFlag) {
	 Write_ENV2(); }/*if*/; }/*if*/;

   if (IsClient) {
      IsTTY = GetIsTTY();
      Init_Vars();
      Init_CWD(); }/*if*/;

   Read_ENV2();

   if (IsClient) {
      if (VerifyLevel >= 1) Test_All();

      if (argc <= 1) {
	 Get_Commands(&Abort);
	 Exit((Abort?1:0)); }/*if*/;

      for (i=1; i<argc; i+=1) {
	 ;/*select*/{
	    if (strlen(argv[i]) == 0) {
	       Get_Commands(&Abort);
	    }else{
	       TopLevelCI(&Abort, argv[i]); };}/*select*/;
	 if (Abort) {
	    Exit(1); }/*if*/; }/*for*/;
      Exit(0); }/*if*/;

   IPC_Get_Commands(&Abort, (tp_Str)NIL);
   Exit((Abort?1:0));
   return 0; /*to make lint happy*/
   }/*main*/


