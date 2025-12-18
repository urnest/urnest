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
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmc/gmc.h>
#include <odin/inc/Type.hh>
#include <odin/inc/Func.hh>
#include <dirent.h>

#include <odin/inc/SKind_.h>

tp_FilDsc
OpenDir(tp_FileName FileName)
{
   tp_FilDsc FilDsc;

   FilDsc = (tp_FilDsc)opendir(FileName);
   if (FilDsc == 0) {
      return ERROR; }/*if*/;
   return FilDsc;
   }/*OpenDir*/


void
CloseDir(tp_FilDsc FilDsc)
{
   int status;

   FORBIDDEN(FilDsc == NIL);
   status = closedir((DIR *)FilDsc);
   if (status == -1) SysCallError(StdOutFD, "closedir(CloseDir)");
   }/*CloseDir*/


void
ReadDir(tp_FileName FileName,bool* EndPtr,tp_FilDsc FilDsc)
{
   struct dirent *dp;

   tps_Str Str;
   size_t sz;

   FORBIDDEN(FilDsc == NIL);
   *EndPtr = false;
   dp = readdir((DIR *)FilDsc);
   if (dp == NULL) {
      *EndPtr = true;
      return; }/*if*/;
   (void)strcpy(Str, dp->d_name);
   sz = snprintf(FileName, MAX_FileName, "%s", Str);
   if (sz >= MAX_FileName) {
      (void)fprintf(stderr, "File name too long (MAX_FileName=%d): %s\n",
                  MAX_FileName, Str);
      exit(1); }/*if*/;
   if (strcmp(FileName, ".") == 0 || strcmp(FileName, "..") == 0) {
      ReadDir(FileName, EndPtr, FilDsc); }/*if*/;
   }/*ReadDir*/


void
ClearDir(tp_FileName DirName)
{
   tp_FilDsc FilDsc;
   tps_Str Str;
   bool End;
   tps_FileName FileName;
   size_t sz;
   tp_SKind k;
   int modTime;

   FORBIDDEN(DirName == NIL);
   FilDsc = OpenDir(DirName);
   if (FilDsc == ERROR) {
      return; }/*if*/;
   for (ReadDir(Str, &End, FilDsc); !End; ReadDir(Str, &End, FilDsc)) {
      sz = snprintf(FileName, MAX_FileName, "%s/%s", DirName, Str);
      if (sz >= MAX_FileName) {
         (void)fprintf(stderr, "File name too long (MAX_FileName=%d): %s/%s\n",
	             MAX_FileName, DirName, Str);
	 exit(1); }/*if*/;
      Get_FileInfo(&k, &modTime, FileName);
      if (k==SK_Dir){
        bool abort;
        MakeReadWrite(&abort, FileName);
        if (abort) {
          (void)fprintf(stderr, "Warning: Could not make %s writable\n", FileName);
        }
        ClearDir(FileName);
        RemoveDir(FileName);
      }
      else{
        Remove(FileName); 
      } 
   }/*for*/;
   CloseDir(FilDsc);
   }/*ClearDir*/


