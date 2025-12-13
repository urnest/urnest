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
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#ifdef BSD_SIGNALS
static int	SigBlockMask;
static int	SigUnblockMask;
#else
static sigset_t SigBlockMaskBuf, *SigBlockMask = &SigBlockMaskBuf;
static sigset_t SigUnblockMaskBuf, *SigUnblockMask = &SigUnblockMaskBuf;
#endif

static bool	SigBlocked = false;

bool		Signalled = false;
bool		Handled = false;
bool		SigChild = false;

static int	sigpipe[2];
static char	sigbuf[4];


static void
Catch_Sig(int sig)
/*ARGSUSED*/
{
   int status;

   Signalled = true;
   status = write(sigpipe[1], sigbuf, 1);
   }/*Catch_Sig*/


static void
Catch_SigChild(int sig)
/*ARGSUSED*/
{
   int status;

   SigChild = true;
   status = write(sigpipe[1], sigbuf, 1);
   }/*Catch_SigChild*/


void
Init_Sigs(bool NoInterrupt)
{
#ifdef BSD_SIGNALS
   int Mask;
#else
   struct sigaction actbuf;
   sigset_t MaskBuf, *Mask = &MaskBuf;
#endif
   int status;

   if (SigBlocked) Unblock_Signals();

   status = pipe(sigpipe);
   FORBIDDEN(status != 0);
   status = fcntl(sigpipe[0], F_SETFL, O_NONBLOCK);
   FORBIDDEN(status != 0);
   status = fcntl(sigpipe[1], F_SETFL, O_NONBLOCK);
   FORBIDDEN(status != 0);

#ifdef BSD_SIGNALS
   (void)signal(SIGINT, Catch_Sig);
   (void)signal(SIGCHLD, Catch_SigChild);
   if (NoInterrupt) {
      (void)signal(SIGQUIT, Catch_Sig);
      (void)signal(SIGTERM, Catch_Sig);
      (void)signal(SIGTSTP, SIG_IGN); }/*if*/;
   Mask = sigmask(SIGHUP) | sigmask(SIGINT) | sigmask(SIGQUIT)
      | sigmask(SIGTERM) | sigmask(SIGCHLD);
   SigUnblockMask = sigblock(Mask);
   SigBlockMask = sigblock(Mask);
#else
   status = sigemptyset(&actbuf.sa_mask);
   FORBIDDEN(status != 0);
   actbuf.sa_flags = 0;

   actbuf.sa_handler = Catch_Sig;
   status = sigaction(SIGINT, &actbuf, (struct sigaction *)0);
   FORBIDDEN(status != 0);
   actbuf.sa_handler = Catch_SigChild;
   status = sigaction(SIGCHLD, &actbuf, (struct sigaction *)0);
   FORBIDDEN(status != 0);
   if (NoInterrupt) {
      actbuf.sa_handler = Catch_Sig;
      status = sigaction(SIGQUIT, &actbuf, (struct sigaction *)0);
      FORBIDDEN(status != 0);
      status = sigaction(SIGTERM, &actbuf, (struct sigaction *)0);
      FORBIDDEN(status != 0);
      actbuf.sa_handler = SIG_IGN;
      status = sigaction(SIGTSTP, &actbuf, (struct sigaction *)0);
      FORBIDDEN(status != 0); }/*if*/;
   status = sigemptyset(Mask);
   FORBIDDEN(status != 0);
   status = sigaddset(Mask, SIGHUP);
   FORBIDDEN(status != 0);
   status = sigaddset(Mask, SIGINT);
   FORBIDDEN(status != 0);
   status = sigaddset(Mask, SIGQUIT);
   FORBIDDEN(status != 0);
   status = sigaddset(Mask, SIGTERM);
   FORBIDDEN(status != 0);
   status = sigaddset(Mask, SIGCHLD);
   FORBIDDEN(status != 0);
   status = sigprocmask(SIG_BLOCK, Mask, SigUnblockMask);
   FORBIDDEN(status != 0);
   status = sigprocmask(SIG_SETMASK, (sigset_t *)0, SigBlockMask);
   FORBIDDEN(status != 0);
#endif
   SigBlocked = true;
   }/*Init_Sigs*/


void
Block_Signals()
{
   int status;

   FORBIDDEN(SigBlocked);
   SigBlocked = true;
#ifdef BSD_SIGNALS
   (void)sigsetmask(SigBlockMask);
#else
   status = sigprocmask(SIG_SETMASK, SigBlockMask, (sigset_t *)0);
   FORBIDDEN(status != 0);
#endif
   }/*Block_Signals*/


void
Unblock_Signals()
{
   int status;

   FORBIDDEN(!SigBlocked);
   SigBlocked = false;
#ifdef BSD_SIGNALS
   (void)sigsetmask(SigUnblockMask);
#else
   status = sigprocmask(SIG_SETMASK, SigUnblockMask, (sigset_t *)0);
   FORBIDDEN(status != 0);
#endif
   }/*Unblock_Signals*/


#ifndef HAVE_UNISTD_H
#include <sys/ioctl.h>
#include <sys/file.h>
pid_t
setsid()
{
   pid_t pgrp;
   int fd;

   pgrp = setpgrp();
   if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
      (void)ioctl(fd, TIOCNOTTY, (char *)0);
      (void)close(fd); }/*if*/;
   return pgrp;
   }/*setsid*/
#endif


void
Lose_ControlTTY()
{
   (void)setsid();
   }/*Lose_ControlTTY*/


int
SystemExec(const char* Tool,char* const * ArgV,const char* LogFileName)
{
   int pid, fd, status;

   pid = fork();
   if (pid == 0) {
      if (LogFileName != NIL) {
	 fd = open("/dev/null", O_RDONLY);
	 status = dup2(fd, 0);
	 FORBIDDEN(status != 0);
	 fd = open(LogFileName, O_WRONLY|O_TRUNC);
	 status = dup2(fd, 1);
	 FORBIDDEN(status != 1);
	 status = dup2(fd, 2);
	 FORBIDDEN(status != 2); }/*if*/;
      if (SigBlocked) Unblock_Signals();
      (void)execv(Tool, ArgV);
      SysCallError(StdOutFD, "execv");
      _exit(1); }/*if*/; /* exit(1) dumps core ... */
   return pid;
   }/*SystemExec*/


int
SystemExecCmd(const char* Cmd,bool Interactive)
{
   int pid;
   tp_Str Shell;

   pid = fork();
   if (pid == 0) {
      Shell = NIL;
      if (Interactive) Shell = GetEnv("SHELL");
      if (Shell == NIL) Shell = "/bin/sh";
      if (SigBlocked) Unblock_Signals();
      (void)execl(Shell, Shell, "-c", Cmd, (char *)0);
      SysCallError(StdOutFD, "execl");
      SystemError("Trying to execute: %s -c %s\n", Shell, Cmd);
      _exit(1); }/*if*/; /* exit(1) dumps core ... */
   return pid;
   }/*SystemExecCmd*/


void
SystemWait(int* pidPtr,bool* AbortPtr)
{
   int status;

   *pidPtr = waitpid(-1, &status, WNOHANG);
   if (*pidPtr < 0) {
      /* probably called system() */
      *pidPtr = 0;
      return; }/*if*/;
   *AbortPtr = (!WIFEXITED(status) || WEXITSTATUS(status) != 0);
   }/*SystemWait*/


void
SystemInterrupt(int pid)
{
   (void)kill(pid, SIGINT);
   }/*SystemInterrupt*/


tp_Str
GetHome(tp_Str Str)
{
   struct passwd *pwd;

   /*select*/{
      if (strlen(Str) == 0) {
	 pwd = getpwuid(getuid());
      }else{
	 pwd = getpwnam(Str); };}/*select*/;
   if (pwd == 0) {
      return NIL; }/*if*/;
   return pwd->pw_dir;
   }/*GetHome*/


int
Await_Event(fd_set* readfds,bool HaveTask)
{
   int nfds;
   int count;

   if (HaveTask) count = write(sigpipe[1], sigbuf, 1);
   FD_SET(sigpipe[0], readfds);

   nfds = select(FD_SETSIZE, readfds, (fd_set *)0, (fd_set *)0, 0);
   if (FD_ISSET(sigpipe[0], readfds)) { 
      count = read(sigpipe[0], sigbuf, sizeof(sigbuf));
   }   
   
   return nfds;
   }/*Await_Event*/
