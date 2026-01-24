#ifndef TP_FKIND
#define TP_FKIND

enum FKind {
  FK_SrcReg=       1, /* host file */
  FK_SrcDir=       2, /* host directory */
  FK_SymLinkReg=   3, /* link to host file */
  FK_SymLinkDir=   4, /* link to host directory */
  FK_BoundSrc=     5, /* bound host file */
  FK_BoundSymLink= 6, /* bound link to host file */
  FK_Str=          7, /* a dummy file that is just a string */
  FK_User=         8, /* produced by user tool */
  FK_DrvDirElm=    9, /* element of a generated directory object */
  FK_VirDirElm=   10, /* element of a virtual directory object */
  FK_ActTgt=      11, /* an actual target */
  FK_ActCmdTgt=   12, /* an actual command target */
  FK_VirTgt=      13, /* a virtual target */
  FK_VirCmdTgt=   14, /* a virtual command target */
  FK_ActTgtText=  15, /* an actual target text definition */
  FK_VirTgtText=  16, /* a virtual target text definition */
  FK_ActTgtExText=17, /* an actual target executable-text definition */
  FK_VirTgtExText=18, /* a virtual target executable-text definition */
  FK_PntrHo=      19, /* pointer homomorphism */
  FK_PntrElm=     20, /* element of a pointer */
  FK_InpPntr=     21, /* input pointer (for trailing parameters) */
  FK_Instance=    22  /* an instantiation of a generic (FK_User) */
 /* 22 used */
};

#endif
