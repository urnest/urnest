#ifndef DG_INC_FUNC_H
#define DG_INC_FUNC_H

#include <stdio.h>
#include <gmc/gmc.h>
#include <dg/inc/Type.hh>

/* dg-analyze.c */
extern void Set_FTClasses();
extern bool Is_Output(tp_FilTyp, tp_Tool);
extern void Make_DrvEdgs();
extern void Make_PrmTypLsts();
/* dg-build.c */
extern void Init_InpSpcs();
extern void Build_DerivationGraph(tp_FileName, tp_Package);
extern void Write_ENV();
extern void Write_DerivationGraph();
/* dg-edg-c */
extern void Clear_EdgFlags();
extern tp_FilTyp EqvEdg_FilTyp(tp_EqvEdg);
extern tp_EqvEdg EqvEdg_Next(tp_EqvEdg);
extern tp_FilTyp EqvEdg_FrmFilTyp(tp_EqvEdg);
extern tp_EqvEdg EqvEdg_FrmNext(tp_EqvEdg);
extern tp_FilTyp CastEdg_FilTyp(tp_CastEdg);
extern tp_CastEdg CastEdg_Next(tp_CastEdg);
extern tp_FilTyp DrvEdg_FrmFilTyp(tp_DrvEdg);
extern tp_DrvEdg DrvEdg_FrmNext(tp_DrvEdg);
extern tp_DrvEdg DrvEdg_Next(tp_DrvEdg);
extern tp_FilTyp InpEdg_FilTyp(tp_InpEdg);
extern tp_InpEdg InpEdg_Next(tp_InpEdg);
extern void Add_InpEdg(tp_InpSpc, tp_InpKind, bool, tp_Tool);
extern void Add_HomInpEdg(tp_InpSpc, tp_Tool);
extern bool HasInput(tp_Tool);
extern void Set_SystemTool_InpKinds(tp_Tool, tp_InpKind);
extern void Set_SystemTool_InpKind(tp_Tool, int, tp_InpKind);
extern tp_FilTyp MemEdg_FilTyp(tp_MemEdg);
extern void Add_MemEdg(tp_FilTyp, tp_FilTyp);
extern void Add_EqvEdg(tp_FilTyp, tp_FilTyp);
extern void Add_CastEdg(tp_FilTyp, tp_FilTyp);
extern void Add_DrvEdg(tp_FilTyp, tp_FilTyp);
extern tp_PrmTypLst DrvEdg_PrmTypLst(tp_DrvEdg);
extern void Print_DrvEdg(tp_FilDsc, tp_DrvEdg);
extern void Print_MemEdgs(tp_FilDsc, tp_MemEdg);
extern void Write_Edgs(FILE *, FILE *);
/* dg-envvar.c */
extern void Init_EnvVars();
extern tp_EnvVar Lookup_EnvVar(tp_Str);
extern tp_Desc EnvVar_Desc(tp_EnvVar);
extern void Set_EnvVar_Desc(tp_EnvVar, tp_Desc, bool);
extern void Set_EnvVar_Default(tp_EnvVar, tp_Str, bool);
extern tp_EnvVarLst EnvVarLst_Next(tp_EnvVarLst);
extern tp_EnvVarLst Make_EnvVarLst(tp_EnvVar);
extern tp_EnvVarLst Union_EnvVarLst(tp_EnvVarLst, tp_EnvVarLst);
extern void Print_EnvVarLst(tp_FilDsc, tp_EnvVarLst);
extern void Write_EnvVars(FILE *, FILE *);
extern tp_InpSpc New_InpSpc();
extern void Set_InpSpc_FilTyp(tp_InpSpc, tp_FilTyp);
extern tp_PrmTyp InpSpc_PrmTyp(tp_InpSpc);
extern void Set_InpSpc_PrmTyp(tp_InpSpc, tp_PrmTyp);
extern tp_InpSpc InpSpc_Next(tp_InpSpc);
extern void Write_InpSpcs(FILE *, FILE *);
/* dg-filtyp.c */
extern void Init_FilTyps();
extern void Set_Tool(tp_FilTyp, tp_Tool);
extern tp_Desc FilTyp_Desc(tp_FilTyp);
extern void Set_FilTyp_Desc(tp_FilTyp, tp_Desc, bool);
extern int FilTyp_Index(tp_FilTyp);
extern bool Has_SubTypes(tp_FilTyp);
extern tp_FilTyp Create_StructFilTyp(tp_Package, tp_FTName);
extern tp_FilTyp Create_OutputFilTyp(tp_Package, tp_Package, tp_FTName);
extern tp_FilTyp Lookup_SecOrdFilTyp(tp_FTName, tp_FilTyp);
extern tp_FilTyp Lookup_FilTyp(tp_FTName);
extern tp_SrcTyp Lookup_SrcTyp(tp_Pattern, bool);
extern void Set_SrcTyp_FilTyp(tp_SrcTyp, tp_FilTyp);
extern tp_FilTyp SrcTyp_FilTyp(tp_SrcTyp);
extern tp_FilTyp Key_FilTyp(tp_Key);
extern void Clear_FilTypFlags();
extern void Print_Info(tp_FilDsc);
extern void Print_FilTyp(tp_FilDsc, tp_FilTyp);
extern void Write_FilTyps(FILE *, FILE *);
/* dg-main.c */
extern int main(int, char **);
/* dg-prmtyp.c */
extern void Init_PrmTyps();
extern tp_PTName PrmTyp_PTName(tp_PrmTyp);
extern tp_Desc PrmTyp_Desc(tp_PrmTyp);
extern void Set_PrmTyp_Desc(tp_PrmTyp, tp_Desc, bool);
extern tp_FilTyp PrmTyp_FilTyp(tp_PrmTyp);
extern void Set_PrmTyp_FilTyp(tp_PrmTyp, tp_FilTyp);
extern tp_PrmTyp Lookup_PrmTyp(tp_PTName);
extern void Print_PrmTyp(tp_FilDsc, tp_PrmTyp);
extern tp_PrmTypLst PrmTypLst_Next(tp_PrmTypLst);
extern tp_PrmTypLst Make_PrmTypLst(tp_PrmTyp);
extern tp_PrmTypLst Union_PrmTypLst(tp_PrmTypLst, tp_PrmTypLst);
extern void Print_PrmTypLst(tp_FilDsc, tp_PrmTypLst);
extern void Write_PrmTyps(FILE *, FILE *);
/* dg-tool.c */
extern tp_Tool New_Tool();
extern void Init_Tools();
extern tp_EnvVarLst Tool_EnvVarLst(tp_Tool);
extern void Set_Tool_EnvVarLst(tp_Tool, tp_EnvVarLst);
extern void Clear_ToolFlags();
extern tp_Str Tool_Name(tp_Tool);
extern bool IsDummy_Tool(tp_Tool);
extern void Write_Tools(FILE *, FILE *);
/* dg-valid.c */
extern void Validate_DerivationGraph();
/* dg-yylex.c */
extern void Init_Parse();
extern void ParseError(tp_Str);
extern void Init_Lex();
extern void EndLex();
extern int YY_Lex();
extern void YY_Unparse(tp_Str, tp_Nod);
/* drvgrf.yacc.c */
extern tp_Nod YY_Parse();
extern void yyerror(char *);
extern int yylex();
extern int yyparse();
/* if-drvpth.c */
extern void AppendDrvPth(tp_DrvPth *, tp_DrvPth);
extern tp_DrvPth FilTyp_Cast_DrvPth(tp_FilTyp);
extern tp_DrvPth FilTyp_Eqv_DrvPth(tp_FilTyp);
extern tp_DrvPth FilTyp_Drv_DrvPth(tp_FilTyp, tp_DrvEdg);
extern void Ret_DrvPth(tp_DrvPth);
extern tp_DPType DrvPth_DPType(tp_DrvPth);
extern tp_FKind DrvPth_FKind(tp_DrvPth);
extern tp_FilTyp DrvPth_FilTyp(tp_DrvPth);
extern tp_DrvEdg DrvPth_DrvEdg(tp_DrvPth);
extern tp_DrvPth DrvPth_Next(tp_DrvPth);
/* if-ft.c */
extern bool IsPntr_FKind(tp_FKind);
extern bool CanPntrHo_FKind(tp_FKind);
extern bool IsATgt_FKind(tp_FKind);
extern bool IsVTgt_FKind(tp_FKind);
extern bool IsATgtText_FKind(tp_FKind);
extern bool IsVTgtText_FKind(tp_FKind);
extern bool IsExternal_Tool(tp_Tool);
extern tp_FTName FilTyp_ShortFTName(tp_FilTyp);
extern tp_FTName FilTyp_FTName(tp_FilTyp);
extern tp_MemEdg FilTyp_MemEdg(tp_FilTyp);
extern tp_CastEdg FilTyp_CastEdg(tp_FilTyp);
extern tp_PrmTypLst FilTyp_MapPrmTypLst(tp_FilTyp);
extern tp_Tool FilTyp_Tool(tp_FilTyp);
extern bool IsCopy_FilTyp(tp_FilTyp);
extern bool IsGrouping_FilTyp(tp_FilTyp);
extern bool IsGroupingInput_FilTyp(tp_FilTyp);
extern bool IsSecOrd_FilTyp(tp_FilTyp);
extern bool IsRecurse_FilTyp(tp_FilTyp);
extern bool IsExec_FilTyp(tp_FilTyp);
extern bool IsVoid_FilTyp(tp_FilTyp);
extern bool IsAtmc_FilTyp(tp_FilTyp);
extern bool IsPntr_FilTyp(tp_FilTyp);
extern bool IsList_FilTyp(tp_FilTyp);
extern bool IsDrvDir_FilTyp(tp_FilTyp);
extern bool IsStruct_FilTyp(tp_FilTyp);
extern bool IsStructMem_FilTyp(tp_FilTyp);
extern bool IsGeneric_FilTyp(tp_FilTyp);
extern bool IsPipe_FilTyp(tp_FilTyp);
/* if-lvl.c */
extern bool IsSubType(tp_FilTyp, tp_FilTyp);
extern void Do_Search(tp_DrvPth *, bool *, tp_FKind, tp_FilTyp, tp_FilTyp);
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

#endif
