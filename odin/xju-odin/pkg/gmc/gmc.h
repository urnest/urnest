#ifndef GMC_GMC_H
#define GMC_GMC_H

#include <stdbool.h>

#ifndef ERROR
#define			ERROR 0
#endif
#if (ERROR != 0)
#define			ERROR 0
#endif

#ifndef NIL
#define			NIL 0
#endif
#if (NIL != 0)
#define			NIL 0
#endif

typedef char *		tp_FileName;
typedef int *		tp_FilDsc;
typedef char *		tp_Str;
#define				MAX_Str 10000
typedef char 			tps_Str [MAX_Str];

#define				MAX_FileName 1024
typedef char 			tps_FileName [MAX_FileName];

extern bool DumpCore;
extern tp_FilDsc	StdErrFD;
extern tp_FilDsc	StdInFD;
extern tp_FilDsc	StdOutFD;

extern void Init_Gmc();
extern void Set_IPC_Err(bool Flag);
extern void Set_ErrFile(tp_FileName FileName, bool IsIPC, tp_FilDsc FilDsc);
extern void Save_ErrFile(tp_FileName* FileNamePtr, bool* IsIPC_Ptr, tp_FilDsc* FilDscPtr);
extern bool IsErr();
extern void Reset_Err();
extern void Increment_Errors();
extern int Num_Errors();
extern void SystemError(char * Fmt, ...);
extern void SysCallError(tp_FilDsc FilDsc, char* Message);
extern void FatalError(const char* Message,const char* FileName,int LineNum);
extern void fatal_err(const char *);
extern void ErrMessage(tp_Str Message);
extern void Local_ErrMessage(tp_Str Message);
extern void fatal_err(const char* Message);

#define FATALERROR(msg) FatalError(msg, __FILE__, __LINE__)

#ifndef NDEBUG
#ifdef __STDC__
#define FORBIDDEN(test) if (test) FatalError(#test, __FILE__, __LINE__)
#else
#define FORBIDDEN(test) if (test) FatalError("test", __FILE__, __LINE__)
#endif
#else
#define FORBIDDEN(test)
#endif

#define FOREACH_CLIENT(Client) for(Client=FirstClient;Client!=NIL;Client=Client_Next(Client)) if (Is_ActiveClient(Client))

extern bool GetIsTTY();
extern tp_FilDsc FileName_CFilDsc(tp_FileName FileName);
extern tp_FilDsc FileName_FilDsc(tp_FileName FileName, char * Mode, bool RetryFlag);
extern tp_FilDsc FileName_WFilDsc(tp_FileName FileName, bool RetryFlag);
extern tp_FilDsc FileName_WBFilDsc(tp_FileName FileName, bool RetryFlag);
extern tp_FilDsc FileName_AFilDsc(tp_FileName FileName, bool RetryFlag);
extern tp_FilDsc FileName_RFilDsc(tp_FileName FileName,bool RetryFlag);
extern tp_FilDsc FileName_RWFilDsc(tp_FileName FileName,bool RetryFlag);
extern void Flush(tp_FilDsc FilDsc);
extern void Rewind(tp_FilDsc FilDsc);
extern void Unwind(tp_FilDsc FilDsc);
extern void Close(tp_FilDsc FilDsc);
extern bool EndOfFile(tp_FilDsc FilDsc);
extern void Write(tp_FilDsc FilDsc,tp_Str Str);
extern void Writech(tp_FilDsc FilDsc,char ch);
extern void WriteInt(tp_FilDsc FilDsc,int Int);
extern void Writeln(tp_FilDsc FilDsc,const char* Str);
extern void WriteLine(tp_FilDsc FilDsc,tp_Str Str);
extern int Readch(tp_FilDsc FilDsc);
extern tp_Str ReadLine(tp_Str StrBuf,tp_FilDsc FilDsc);
extern bool Equal(tp_FilDsc FilDsc1,tp_FilDsc FilDsc2);
extern void FileCopy(tp_FilDsc OutFD,tp_FilDsc InFD);

extern void Set_ModeMask(tp_FileName FileName);
extern void MakePlnFile(bool* AbortPtr,tp_FileName FileName);
extern void MakeDirFile(bool* AbortPtr,tp_FileName FileName);
extern void GetWorkingDir(bool* AbortPtr,tp_Str DirName);
extern void ChangeDir(bool* AbortPtr,tp_FileName DirName);
extern bool IsExecutable(tp_FileName FileName);
extern void MakeExecutable(tp_FileName FileName);
extern void MakeReadOnly(bool* AbortPtr,tp_FileName FileName);
extern void MakeReadWrite(bool* AbortPtr,tp_FileName FileName);
extern void SymLink(bool* AbortPtr,tp_FileName ToFileName,tp_FileName FromFileName);
extern void FileName_SymLinkFileName(tp_FileName SymLinkFileName,tp_FileName FileName);
extern bool IsDirectory_FileName(tp_FileName FileName);
extern bool Exists(tp_FileName FileName);
extern bool Empty(tp_FileName FileName);
extern void FileSize(bool* AbortPtr,int* SizePtr,tp_FileName FileName);
extern void Remove(tp_FileName FileName);
extern void RemoveDir(tp_FileName DirName);
extern void Rename(bool* AbortPtr,tp_FileName OldFileName,tp_FileName NewFileName);

typedef struct _tps_Sym *	tp_Sym;
typedef struct _tps_Sym {
   tp_Str Str;
   int Att;
   tp_Sym Next;
   }				tps_Sym;


extern tp_Str GetEnv(tp_Str VarStr);
extern tp_Str Malloc_Str(const char* Str);
extern bool Is_EmptyStr(tp_Str Str);
extern int Str_PosInt(tp_Str Str);
extern tp_Str Tail(tp_Str Str);
extern void StrShift(tp_Str Str, int Count);
extern tp_Sym Str_Sym(tp_Str Str);
extern tp_Str Sym_Str(tp_Sym Sym);
extern int Sym_Att(tp_Sym Sym);
extern void Set_Sym_Att(tp_Sym Sym,int Att);
extern void Write_Syms(tp_FilDsc FilDsc);

#endif
