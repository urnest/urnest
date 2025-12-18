#ifndef TP_YYNODTYP
#define TP_YYNODTYP

typedef enum NodTyp {
  NOD_DrvGrf=1,
  NOD_Banner=2,
  NOD_SrcDcl=3,
  NOD_PrmDcl=4,
  NOD_ObjDcl=5,
  NOD_ToolDcl=6,
  NOD_VarDcl=7,
  NOD_Prefix=8,
  NOD_Suffix=9,
  NOD_Hidden=10,
  NOD_BaseSupertype=11,
  NOD_Args=12,
  NOD_Needs=13,
  NOD_NameInput=14,
  NOD_ErrOKInput=15,
  NOD_Results=16,
  NOD_ForeignResult=17,
  NOD_OdinExpr=18,
  NOD_DrvInput=19,
  NOD_PrmInput=20,
  NOD_EmptyFile=21,
  NOD_AbsRoot=22,
  NOD_AbsFile=23,
  NOD_PrmOpr=24,
  NOD_AplOpr=25,
  NOD_PrmVals=26,
  NOD_DrvOpr=27,
  NOD_SecOrdDrvOpr=28,
  NOD_Oprs=29,
  NOD_VirSelOpr=30,
  NOD_SelOpr=31,
  NOD_DirOpr=32,
  NOD_VarWord=33,
  NOD_Word=34
} tp_NodTyp;

#endif
