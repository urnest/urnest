#ifndef TREGRM_FUNC_H
#define TREGRM_FUNC_H

#include <gmc/nod.h>
#include <tregrm/inc/Type.hh>

/* tg.anal.c */
extern void Analyze(tp_Nod, tp_Nod, tp_Nod);
/* tg.att.c */
extern void Reset_Att(tp_AttTyp);
extern tp_Att Get_Att(tp_AttTyp, tp_Sym);
extern void Set_Att(tp_AttTyp, tp_Sym, tp_Att);
/* tg.dummy.c */
extern bool Empty_DummyStack(void);
extern void Push_DummyStack(tp_Nod, int);
extern void Pop_DummyStack(tp_Nod *, int *);
/* tg.gen_grm.c */
extern void Gen_Grammar(tp_FilDsc, tp_Nod, tp_Nod, tp_Nod);
extern void Gen_LHS(tp_FilDsc, tp_Sym, int);
extern void Gen_Rule(tp_FilDsc, tp_Nod, tp_Sym, int *, int *);
extern void Gen_Seq(tp_FilDsc, tp_Nod, tp_Sym, bool, int *, int *);
extern void Gen_Token(tp_FilDsc, tp_Nod, tp_Sym, bool, int *, int *);
extern void Gen_DummySym(tp_FilDsc, tp_Sym, int);
extern void Gen_Act(tp_FilDsc, tp_Nod, int);
extern void Write_Act(tp_FilDsc, int, int);
/* tg.gen_lex.c */
extern void Gen_LexTypes(tp_FilDsc, tp_Nod);
extern bool Is_UserLeafDef(tp_Nod);
/* tg.gen_nod.c */
extern void Gen_NodeTypes(tp_FilDsc, tp_Nod);
/* tg.main.c */
extern int main(void);
/* tg.nod_grm.c */
extern void Write_Node_Grammar(tp_FilDsc, tp_Nod);
extern tp_Nod Create_Node_Grammar(tp_Nod);
extern void Write_Node(tp_FilDsc, tp_Nod);
extern tp_Nod Append_Seq(tp_Nod, tp_Nod);
extern tp_Nod Union_Alternative(tp_Nod, tp_Nod);
extern tp_Nod NUnion(tp_Nod, tp_Nod);
extern tp_Nod NConc(tp_Nod, tp_Nod);
extern tp_Nod Create_StarNod(tp_Nod);
extern tp_Nod Create_PlusNod(tp_Nod);
extern tp_Nod Copy_Nod(tp_Nod);
/* tg.yylex.c */
extern int GetChar(void);
extern void Init_Lex(void);
extern void EndLex(void);
extern int YY_Lex(void);
extern bool IsNameChr(int);
extern void ParseError(tp_Str);
/* tregrm.yacc.c */
extern tp_Nod YY_Parse(void);
extern void yyerror(char *);
extern int yylex(void);
extern int yyparse(void);
/* extern void Get_FileInfo(tp_SKind *, int *, tp_FileName); */
extern void MakePlnFile(bool *, tp_FileName);
extern void MakeDirFile(bool *, tp_FileName);
extern void GetWorkingDir(bool *, tp_FileName);
extern void ChangeDir(bool *, tp_FileName);
extern bool IsExecutable(tp_FileName);
extern void MakeExecutable(tp_FileName);
extern void MakeReadOnly(bool *, tp_FileName);
extern void SymLink(bool *, tp_FileName, tp_FileName);
extern void FileName_SymLinkFileName(tp_FileName, tp_FileName);
extern bool IsDirectory_FileName(tp_FileName);
extern bool Exists(tp_FileName);
extern bool Empty(tp_FileName);
extern void FileSize(bool *, int *, tp_FileName);
extern void Remove(tp_FileName);
extern void RemoveDir(tp_FileName);
extern void Rename(bool *, tp_FileName, tp_FileName);
/* if_nod.c */
extern tp_Nod New_Nod(void);
extern void Ret_Nod(tp_Nod);
extern tp_NodTyp Nod_NodTyp(tp_Nod);
extern void Set_Nod_NodTyp(tp_Nod, tp_NodTyp);
extern tp_Nod Nod_FirstSon(tp_Nod);
extern void Set_Nod_FirstSon(tp_Nod, tp_Nod);
extern tp_Nod Nod_Brother(tp_Nod);
extern void Set_Nod_Brother(tp_Nod, tp_Nod);
extern int Nod_NumSons(tp_Nod);
extern tp_Nod Nod_Son(int, tp_Nod);
extern tp_Sym Nod_Sym(tp_Nod);
extern void Set_Nod_Sym(tp_Nod, tp_Sym);
extern bool Empty_SymStack(void);
extern void Push_SymStack(tp_Sym);
extern void Pop_SymStack(void);
extern void Push_NodStack(tp_Nod);
extern void Pop_NodStack(void);
extern void Push_SonStack(int);
extern void Pop_SonStack(void);
extern void Init_ConstructTree(void);
extern void MakeLeaf(tp_NodTyp, tp_Sym);
extern void MakeEmptyNod(tp_NodTyp);
extern void MakeNod(int);
extern void CollectSons(int);
extern tp_Nod End_ConstructTree(void);
extern void Action(int, int);

#endif
