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

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <gmc/System.hh>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


tp_FilDsc	StdInFD;
tp_FilDsc	StdOutFD;
tp_FilDsc	StdErrFD;


extern char *strerror(int);

extern tp_Str		Author;


int			num_Errors = 0;

bool			DumpCore = true;

bool			Is_IPC_Err = false;

static tp_FilDsc        ErrFD;
static tp_FileName	ErrFileName;


void Init_Gmc()
{
   StdInFD = (tp_FilDsc)stdin;
   StdOutFD = (tp_FilDsc)stdout;
   StdErrFD = (tp_FilDsc)stderr;
   ErrFileName = NIL;
   ErrFD = StdErrFD;
   Reset_Err();
   }/*Init_Err*/


void Set_IPC_Err(bool Flag)
{
   Is_IPC_Err = Flag;
   }/*Set_IPC_Err*/


void Set_ErrFile(tp_FileName FileName, bool IsIPC, tp_FilDsc FilDsc)
{
   FORBIDDEN(FileName != NIL && IsIPC);
   if (FileName == ErrFileName && IsIPC == Is_IPC_Err && FilDsc == ErrFD) {
      return; }/*if*/;
   if (ErrFD != NIL && ErrFD != StdOutFD && ErrFD != StdErrFD) Close(ErrFD);
   ErrFileName = FileName;
   Is_IPC_Err = IsIPC;
   ErrFD = FilDsc;
   }/*Set_ErrFile*/


void Save_ErrFile(tp_FileName* FileNamePtr, bool* IsIPC_Ptr, tp_FilDsc* FilDscPtr)
{
   *FileNamePtr = ErrFileName;
   *IsIPC_Ptr = Is_IPC_Err;
   *FilDscPtr = ErrFD;
   }/*Save_ErrFile*/


bool IsErr()
{
   FORBIDDEN(ErrFD == StdOutFD || ErrFD == StdErrFD);
   return (ErrFD != NIL);
   }/*IsErr*/


void Reset_Err()
{
   num_Errors = 0;
   }/*Reset_Err*/


void Increment_Errors()
{
   num_Errors += 1;
   }/*Increment_Errors*/


int Num_Errors()
{
   return num_Errors;
   }/*Num_Errors*/


void FatalError(const char* Message,const char* FileName,int LineNum)
{
   tps_Str Msg;

   (void)sprintf(Msg, "\"%s\", line %d: %s", FileName, LineNum, Message);
   fatal_err(Msg);
   }/*FatalError*/


void SysCallError(tp_FilDsc FilDsc, char* Message)
{
   (void)fprintf((FILE *)FilDsc, "%s: %s.\n", Message, strerror(errno));
   (void)fflush((FILE *)FilDsc);
   }/*SysCallError*/


void SystemError(char * Fmt, ...)
/*VARARGS 1*/
{
   va_list Args;
   tps_Str Message;

   va_start(Args,Fmt);
   Increment_Errors ();
   (void)vsprintf(Message, Fmt, Args);
   /*select*/{
      if (Is_IPC_Err) {
	 ErrMessage(Message);
      }else{
	 Local_ErrMessage(Message); };}/*select*/;
   va_end(Args);
   }/*SystemError*/



void Local_ErrMessage(tp_Str Message)
{
   int status;

   if (ErrFD == NIL) {
      FORBIDDEN(ErrFileName == NIL);
      ErrFD = FileName_WFilDsc(ErrFileName, false);
      if (ErrFD == ERROR) {
	 status = fputs("!! Could not open error file !!", (FILE *)StdErrFD);
	 if (status == EOF) SysCallError(StdOutFD, "fputs(Local_ErrMessage)");
	 ErrFD = StdErrFD; }/*if*/; }/*if*/;
   status = fputs(Message, (FILE *)ErrFD);
   if (status == EOF) SysCallError(StdOutFD, "fputs(Local_ErrMessage)");
   (void)fflush((FILE *)ErrFD);
   }/*Local_ErrMessage*/


void fatal_err(const char* Message)
{
   (void)fprintf(stderr, "%s\n", Message);
   (void)fprintf(stderr, "Anomalous Internal State Detected\n");
   (void)fprintf(stderr, "please mail description to %s\n", Author);
   if (DumpCore) {
      (void)fprintf(stderr,
	"'illegal instruction' issued to generate core for analysis\n");
      abort(); }/*if*/;
   exit(1);
   }/*fatal_err*/


bool GetIsTTY()
{
   return (isatty(0) == 1);
   }/*GetIsTTY*/


tp_FilDsc FileName_CFilDsc(tp_FileName FileName)
{
   int fd;
   FILE * FilDscFILE;

   if (FileName == ERROR) return ERROR;
   fd = open(FileName, O_CREAT|O_WRONLY|O_EXCL, 0644);
   if (fd < 0) {
      return ERROR; }/*if*/;
   FilDscFILE = fdopen(fd, "w");
   if (FilDscFILE == NULL) {
      return ERROR; }/*if*/;
   return (tp_FilDsc)FilDscFILE;
   }/*FileName_CFilDsc*/


tp_FilDsc FileName_FilDsc(tp_FileName FileName, char * Mode, bool RetryFlag)
{
   FILE * FilDscFILE;

   if (FileName == ERROR) return ERROR;
   FilDscFILE = fopen(FileName, Mode);
   if (FilDscFILE == NULL) {
      if (!RetryFlag) {
	 return ERROR; }/*if*/;
      if (Mode[0] == 'w') {
	 if (Exists(FileName)) {
	    Remove(FileName);
	    FilDscFILE = fopen(FileName, Mode);
	    if (FilDscFILE != NULL) {
	       return (tp_FilDsc)FilDscFILE; }/*if*/; }/*if*/; }/*if*/;
      SysCallError(StdOutFD, "Retrying fopen(FileName_FilDsc)");
      while (FilDscFILE == NULL) {
	 sleep(1);
	 FilDscFILE = fopen(FileName, Mode); }/*while*/;
      Writeln(StdOutFD, "fopen(FileName_FilDsc) succeded."); }/*if*/;
   return (tp_FilDsc)FilDscFILE;
   }/*FileName_FilDsc*/


tp_FilDsc FileName_WFilDsc(tp_FileName FileName, bool RetryFlag)
{
   return FileName_FilDsc(FileName, "w", RetryFlag);
   }/*FileName_WFilDsc*/


tp_FilDsc FileName_WBFilDsc(tp_FileName FileName, bool RetryFlag)
{
   return FileName_FilDsc(FileName, "wb", RetryFlag);
   }/*FileName_WBFilDsc*/


tp_FilDsc FileName_AFilDsc(tp_FileName FileName, bool RetryFlag)
{
   return FileName_FilDsc(FileName, "a", RetryFlag);
   }/*FileName_AFilDsc*/


tp_FilDsc FileName_RFilDsc(tp_FileName FileName,bool RetryFlag)
{
   return FileName_FilDsc(FileName, "r", RetryFlag);
   }/*FileName_RFilDsc*/


tp_FilDsc FileName_RWFilDsc(tp_FileName FileName,bool RetryFlag)
{
   return FileName_FilDsc
    (FileName, (Exists(FileName) ? "r+" : "w+"), RetryFlag);
   }/*FileName_RWFilDsc*/


tp_FilDsc FileName_RWBFilDsc(tp_FileName FileName,bool RetryFlag)
{
   return FileName_FilDsc
    (FileName, (Exists(FileName) ? "r+b" : "w+b"), RetryFlag);
   }/*FileName_RWBFilDsc*/


void Flush(tp_FilDsc FilDsc)
{
   int status;

   status = fflush((FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "fflush(Flush)");
   }/*Flush*/


void Rewind(tp_FilDsc FilDsc)
{
   FORBIDDEN(FilDsc == ERROR);
   rewind((FILE *)FilDsc);
   }/*Rewind*/


void Unwind(tp_FilDsc FilDsc)
{
   int status;

   FORBIDDEN(FilDsc == ERROR);
   status = fseek((FILE *)FilDsc, (long)0, 2);
   if (status == -1) SysCallError(StdOutFD, "fseek(Unwind)");
   }/*Unwind*/


void Close(tp_FilDsc FilDsc)
{
   int status;

   FORBIDDEN(FilDsc == ERROR);
   status = fclose((FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "fclose(Close)");
   }/*Close*/


bool EndOfFile(tp_FilDsc FilDsc)
{
   FORBIDDEN(FilDsc == ERROR);
   return feof((FILE *)FilDsc);
   }/*EndOfFile*/


void Write(tp_FilDsc FilDsc,tp_Str Str)
{
   int status;

   FORBIDDEN(FilDsc == ERROR || Str == ERROR);
   status = fputs(Str, (FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "fputs(Write)");
   }/*Write*/


void Writech(tp_FilDsc FilDsc,char ch)
{
   int status;

   FORBIDDEN(FilDsc == ERROR);
   status = putc(ch, (FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "putc(Writech)");
   }/*Writech*/


void WriteInt(tp_FilDsc FilDsc,int Int)
{
   int status;

   FORBIDDEN(FilDsc == ERROR);
   status = fprintf((FILE *)FilDsc, "%d", Int);
   if (status == EOF) SysCallError(StdOutFD, "fprintf(WriteInt)");
   }/*WriteInt*/


void Writeln(tp_FilDsc FilDsc,const char* Str)
{
   int status;

   FORBIDDEN(FilDsc == ERROR || Str == ERROR);
   status = fputs(Str, (FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "fputs(Writeln)");
   status = putc('\n', (FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "putc(Writeln)");
   }/*Writeln*/


void WriteLine(tp_FilDsc FilDsc,tp_Str Str)
{
   int status;
   tp_Str StrPtr;

   FORBIDDEN(FilDsc == ERROR || Str == ERROR);
   for (StrPtr = Str; *StrPtr != 0; StrPtr++) {
      if (*StrPtr == '\n') {
	 status = putc('\\', (FILE *)FilDsc);
	 if (status == EOF) SysCallError(StdOutFD, "putc(WriteLine)"); }/*if*/;
      status = putc(*StrPtr, (FILE *)FilDsc);
      if (status == EOF) SysCallError(StdOutFD, "putc(WriteLine)"); }/*for*/;
   status = putc('\n', (FILE *)FilDsc);
   if (status == EOF) SysCallError(StdOutFD, "putc(WriteLine)");
   }/*WriteLine*/


int Readch(tp_FilDsc FilDsc)
{
   return getc((FILE *)FilDsc);
   }/*Readch*/


tp_Str Readln(tp_Str StrBuf,tp_FilDsc FilDsc)
{
   tp_Str OutStr;
   int Length;

   if (FilDsc == ERROR) return ERROR;
   OutStr = fgets(StrBuf, MAX_Str, (FILE *)FilDsc);
   if (OutStr == NULL) {
      return ERROR; }/*if*/;
   Length = strlen(StrBuf);
   if (Length > 0 && StrBuf[Length-1] == '\n') {
      StrBuf[Length-1] = 0; }/*if*/;
   if (Length == (MAX_Str - 1)) {
      StrBuf[100] = 0;
      SystemError("Line too long, truncated:\n%s\n", StrBuf); }/*if*/;
   return StrBuf;
   }/*Readln*/


tp_Str ReadLine(tp_Str StrBuf,tp_FilDsc FilDsc)
{
   tp_Str OutStr;
   int Length;

   if (FilDsc == ERROR) return ERROR;
   OutStr = fgets(StrBuf, MAX_Str, (FILE *)FilDsc);
   if (OutStr == NULL) {
      return ERROR; }/*if*/;
   Length = strlen(StrBuf);
   while (Length > 1 && Length < (MAX_Str - 1)
	  && StrBuf[Length-2] == '\\' && StrBuf[Length-1] == '\n') {
      StrBuf[Length-2] = '\n';
      OutStr = fgets(&StrBuf[Length-1], MAX_Str-Length, (FILE *)FilDsc);
      if (OutStr == NULL) {
	 return ERROR; }/*if*/;
      Length = strlen(StrBuf); }/*while*/;
   if (Length > 0 && StrBuf[Length-1] == '\n') {
      StrBuf[Length-1] = 0; }/*if*/;
   if (Length == (MAX_Str - 1)) {
      StrBuf[100] = 0;
      SystemError("Line too long, truncated:\n%s\n", StrBuf); }/*if*/;
   return StrBuf;
   }/*ReadLine*/


bool Equal(tp_FilDsc FilDsc1,tp_FilDsc FilDsc2)
{
   int ichar1, ichar2;

   FORBIDDEN(FilDsc1 == ERROR || FilDsc2 == ERROR);
   ichar1 = getc((FILE *)FilDsc1);
   ichar2 = getc((FILE *)FilDsc2);
   while (ichar1 == ichar2 && ichar1 != EOF) {
      ichar1 = getc((FILE *)FilDsc1);
      ichar2 = getc((FILE *)FilDsc2); }/*while*/;
   return (ichar1 == ichar2);
   }/*Equal*/


void FileCopy(tp_FilDsc OutFD,tp_FilDsc InFD)
{
   int ichar, status;

   FORBIDDEN(OutFD == ERROR || InFD == ERROR);
   ichar = getc((FILE *)InFD);
   while (ichar != EOF) {
      status = putc((char)ichar, (FILE *)OutFD);
      if (status == EOF) SysCallError(StdOutFD, "putc(FileCopy)");
      ichar = getc((FILE *)InFD); }/*while*/;
   }/*FileCopy*/

static mode_t	ModeMask = 0777;


void Set_ModeMask(tp_FileName FileName)
{
   int status;
   struct stat buf;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   FORBIDDEN(status != 0);
   ModeMask = buf.st_mode;
   (void)umask(ModeMask ^ 0777);
   }/*Set_ModeMask*/


void MakePlnFile(bool* AbortPtr,tp_FileName FileName)
{
   int fd, status;

   *AbortPtr = true;
   fd = open(FileName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
   if (fd < 0) {
      return; }/*if*/;
   status = fchmod(fd, 0666 & ModeMask);
   if (status != 0) {
      (void)close(fd);
      return; }/*if*/;
   status = close(fd);
   if (status != 0) {
      (void)close(fd);
      return; }/*if*/;
   *AbortPtr = false;
   }/*MakePlnFile*/


void MakeDirFile(bool* AbortPtr,tp_FileName FileName)
{
   struct stat buf;
   int status;
   tps_FileName DirName;
   int i;

   FORBIDDEN(FileName == ERROR);

   *AbortPtr = false;
   status = stat(FileName, &buf);
   if (status == 0) {
      FORBIDDEN((buf.st_mode & S_IFDIR) != S_IFDIR);
      return; }/*if*/;
   status = mkdir(FileName, (0777 & ModeMask));
   if (status != 0) {
      i = strlen(FileName) - 2;
      while (i > 0 && FileName[i] != '/') i -= 1;
      if (i < 1) {
	 SystemError("\"%s\": cannot create.\n", FileName);
	 *AbortPtr = true;
	 return; }/*if*/;
      (void)strcpy(DirName, FileName);
      DirName[i] = 0;
      MakeDirFile(AbortPtr, DirName);
      if (*AbortPtr) {
	 return; }/*if*/;
      status = mkdir(FileName, (0777 & ModeMask));
      if (status != 0) {
	 SystemError("\"%s\": cannot create.\n", FileName);
	 *AbortPtr = true;
	 return; }/*if*/; }/*if*/;
   }/*MakeDirFile*/


char *getcwd();

void GetWorkingDir(bool* AbortPtr,tp_Str DirName)
{
   char *WDstat;

   WDstat = getcwd(DirName, MAX_Str);
   *AbortPtr = (WDstat == 0);
   }/*GetWorkingDir*/


#ifndef HAVE_GETCWD
char *
getcwd(
   GMC_ARG(char*, buf),
   GMC_ARG(int, size)
   )
   GMC_DCL(char*, buf)
   GMC_DCL(int, size)
{
   char *result, *getwd();

   result = getwd(buf);
   FORBIDDEN(result != 0 && strlen(result) >= size);
   return result;
   }/*getcwd*/
#endif


void ChangeDir(bool* AbortPtr,tp_FileName DirName)
{
   int status;

   status = chdir(DirName);
   *AbortPtr = (status != 0);
   }/*ChangeDir*/


bool IsExecutable(tp_FileName FileName)
{
   int status;
   struct stat buf;

   status = stat(FileName, &buf);
   return ((status == 0) && ((buf.st_mode & S_IEXEC) == S_IEXEC));
   }/*IsExecutable*/


void MakeExecutable(tp_FileName FileName)
{
   int status;
   struct stat buf;
   int mode;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   if (status == 0) {
      if ((buf.st_mode & S_IEXEC) != S_IEXEC) {
	 mode = ((buf.st_mode | 0111) & ModeMask);
	 status = chmod(FileName, mode); }/*if*/; }/*if*/;
   if (status != 0) {
      SystemError("\"%s\": cannot make executable.\n", FileName); }/*if*/;
   }/*MakeExecutable*/


void MakeReadOnly(bool* AbortPtr,tp_FileName FileName)
{
   int status;
   struct stat buf;
   mode_t NewMode;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   if (status != 0) {
      *AbortPtr = true;
      return; }/*if*/;
   NewMode = ((buf.st_mode | 0444) & 0555 & ModeMask);
   if (NewMode == buf.st_mode) {
      *AbortPtr = false;
      return; }/*if*/;
   status = chmod(FileName, NewMode);
   *AbortPtr = (status != 0);
   }/*MakeReadOnly*/


void MakeReadWrite(bool* AbortPtr,tp_FileName FileName)
{
   int status;
   struct stat buf;
   mode_t NewMode;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   if (status != 0) {
      *AbortPtr = true;
      return; }/*if*/;
   NewMode = ((buf.st_mode | 0600) & ModeMask);
   if (NewMode == buf.st_mode) {
      *AbortPtr = false;
      return; }/*if*/;
   status = chmod(FileName, NewMode);
   *AbortPtr = (status != 0);
   }/*MakeReadOnly*/


void SymLink(bool* AbortPtr,tp_FileName ToFileName,tp_FileName FromFileName)
{
   int status;

   FORBIDDEN(ToFileName == ERROR || FromFileName == ERROR);
   *AbortPtr = false;
   status = symlink(FromFileName, ToFileName);
   if (status != 0) {
      SysCallError(StdOutFD, "symlink");
      SystemError("\"%s\": Cannot make symbolic link to %s.\n", ToFileName, FromFileName);
      *AbortPtr = true; }/*if*/;
   }/*SymLink*/


void FileName_SymLinkFileName(tp_FileName SymLinkFileName,tp_FileName FileName)
{
   int cc;
   tps_Str buf;
   size_t sz;

   cc = readlink(FileName, buf, MAX_Str - 1);
   if (cc < 0) {
      perror("readlink");
      exit(1); }/*if*/;
   FORBIDDEN(cc == 0);
   buf[cc] = '\0';
   sz = snprintf(SymLinkFileName, MAX_FileName, "%s", buf);
   if (sz >= MAX_FileName) {
      (void)fprintf(stderr, "File name too long (MAX_FileName=%d): %s\n",
                  MAX_FileName, buf);
      exit(1); }/*if*/;
   }/*FileName_SymLinkFileName*/


bool IsDirectory_FileName(tp_FileName FileName)
{
   int status;
   struct stat buf;

   status = stat(FileName, &buf);
   return ((status == 0) && ((buf.st_mode & S_IFDIR) == S_IFDIR));
   }/*IsDirectory_FileName*/


bool Exists(tp_FileName FileName)
{
   int status;
   struct stat buf;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   return (status == 0);
   }/*Exists*/


bool Empty(tp_FileName FileName)
{
   int status;
   struct stat buf;

   FORBIDDEN(FileName == ERROR);
   status = stat(FileName, &buf);
   FORBIDDEN(status != 0);
   return (buf.st_size == 0);
   }/*Empty*/


void FileSize(bool* AbortPtr,int* SizePtr,tp_FileName FileName)
{
   int status;
   struct stat buf;

   FORBIDDEN(FileName == ERROR);
   *AbortPtr = false;
   *SizePtr = 0;
   status = stat(FileName, &buf);
   if (status != 0) {
      *AbortPtr = true;
      return; }/*if*/;
   *SizePtr = buf.st_size;
   }/*FileSize*/


void Remove(tp_FileName FileName)
{
   int status;

   FORBIDDEN(FileName == ERROR);
   status = unlink(FileName);
   if (status != 0) 
     SystemError("\"%s\": rm failed.\n", FileName);
   }/*Remove*/


void RemoveDir(tp_FileName DirName)
{
   int status;
   tps_Str NFS_Hack;

   FORBIDDEN(DirName == ERROR);
   status = rmdir(DirName);
   if (status != 0) {
      (void)sprintf(NFS_Hack, "rm -f %s/.nfs*", DirName);
      status = system(NFS_Hack);
      status = rmdir(DirName);
      if (status != 0) {
	 SystemError("\"%s\": rmdir failed.\n", DirName); }/*if*/; }/*if*/;
   }/*RemoveDir*/


void Rename(bool* AbortPtr,tp_FileName OldFileName,tp_FileName NewFileName)
{
   int status;

   FORBIDDEN(OldFileName == ERROR  || NewFileName == ERROR);
   status = rename(OldFileName, NewFileName);
   if (status != 0) {
      SystemError("\"%s\": bad status from rename to %s.\n", OldFileName, NewFileName);
      if (Exists(OldFileName) || !Exists(NewFileName)) {
	 *AbortPtr = true;
	 return; }/*if*/;
      SystemError("  (but it apparently worked).\n"); }/*if*/;
   *AbortPtr = false;
   }/*Rename*/



int		num_StrS = 0;

#define		MAX_HashedSyms 512
static tp_Sym	HashedSyms [MAX_HashedSyms];

static int	num_Syms = 0;


tp_Str GetEnv(tp_Str VarStr)
{
   tp_Str ValStr;

   ValStr = getenv(VarStr);
   if (ValStr == NULL || strlen(ValStr) == 0) {
      return NIL; }/*if*/;
   return ValStr;
   }/*GetEnv*/


tp_Str Malloc_Str(const char* Str)
{
   num_StrS += 1;
   return strcpy((tp_Str)malloc((unsigned)strlen(Str)+1), Str);
   }/*Malloc_Str*/


bool Is_EmptyStr(tp_Str Str)
{
   return Str[0] == 0;
   }/*Is_EmptyStr*/


int Str_PosInt(tp_Str Str)
{
   int i;

   i = atoi(Str);
   if (i == 0 && strcmp(Str, "0") != 0) i = -1;
   return i;
   }/*Str_PosInt*/


tp_Str Tail(tp_Str Str)
{
   if (Str == ERROR) {
      return ERROR; }/*if*/;
   return &Str[strlen(Str)];
   }/*Tail*/


void StrShift(tp_Str Str, int Count)
{
   int i, Len;
   tp_Str ToStr, FromStr;

   if (Count < 0) {
      FromStr = Str - Count;
      ToStr = Str;
      while (*FromStr != 0) {
	 *ToStr = *FromStr;
	 ToStr += 1; FromStr += 1; }/**/;
      *ToStr = 0;
      return; }/*if*/;
   Len = strlen(Str);
   FromStr = Str + Len;
   ToStr = FromStr + Count;
   *ToStr = 0;
   for (i=0; i<Len; i+=1) {
      ToStr -= 1; FromStr -= 1;
      *ToStr = *FromStr; }/*for*/;
   }/*StrShift*/


static int Str_HashInt(tp_Str Str)
{
   int Length, HashInt;

   Length = strlen(Str);
   if (Length == 0) return 0;
   HashInt = (Str[0]&0xff) + (Str[Length/2]&0xff) + (Str[Length-1]&0xff)
      + (Length << 5);
   FORBIDDEN(HashInt < 0);
   return HashInt;
   }/*Str_HashInt*/


tp_Sym Str_Sym(tp_Str Str)
{
   int HashInt;
   tp_Sym Sym;

   if (Str == ERROR) return ERROR;
   HashInt = Str_HashInt(Str) % MAX_HashedSyms;
   for (Sym = HashedSyms[HashInt]; Sym != NIL; Sym = Sym->Next) {
      if (strcmp(Str, Sym->Str) == 0) {
         return Sym; }/*if*/; }/*for*/;
   Sym = (tp_Sym)malloc(sizeof(tps_Sym));
   num_Syms += 1;
   Sym->Str = Malloc_Str(Str);
   Sym->Att = NIL;
   Sym->Next = HashedSyms[HashInt];
   HashedSyms[HashInt] = Sym;
   return Sym;
   }/*Str_Sym*/


tp_Str Sym_Str(tp_Sym Sym)
{
   if (Sym == ERROR) return ERROR;
   return Sym->Str;
   }/*Sym_Str*/


int Sym_Att(tp_Sym Sym)
{
   FORBIDDEN(Sym == ERROR);
   return Sym->Att;
   }/*Sym_Att*/


void Set_Sym_Att(tp_Sym Sym,int Att)
{
   FORBIDDEN(Sym == ERROR);
   Sym->Att = Att;
   }/*Set_Sym_Att*/


void Write_Syms(tp_FilDsc FilDsc)
{
   int i, Used=0;
   tp_Sym Sym;

   for (i=0; i<MAX_HashedSyms; i++) {
      Sym = HashedSyms[i];
      if (Sym != NIL) {
	 Used += 1;
	 WriteInt(FilDsc, i);
	 while (Sym != NIL) {
	    Write(FilDsc, "\t: "); Writeln(FilDsc, Sym->Str);
	    Sym = Sym->Next; }/*while*/; }/*if*/; }/*for*/;
   if (Used > 0) {
      WriteInt(FilDsc, (int)(num_Syms/Used)); Writeln(FilDsc, ""); }/*if*/;
   }/*Write_Syms*/

