#ifndef TREGRM_TYPE_H
#define TREGRM_TYPE_H

#include <gmc/gmc.h>

#define GMC_ARG(x, y) x y
#define GMC_DCL(x, y)
#define GMC_ARG_VOID void
#define GMC_P1(x) x
#define GMC_PN(x) ,x

typedef int			tp_AttTyp;

typedef char *			tp_FileName;
typedef char *			tp_Str;

typedef int *			tp_FilDsc;

typedef union {
   tp_Nod Nod;
   tp_Sym Sym;
   int    Int; }		tp_Att;

#endif
