#ifndef TREGRM_TYPE_H
#define TREGRM_TYPE_H

#include <gmc/gmc.h>

typedef int			tp_AttTyp;

typedef char *			tp_FileName;
typedef char *			tp_Str;

typedef int *			tp_FilDsc;

typedef union {
   tp_Nod Nod;
   tp_Sym Sym;
   int    Int; }		tp_Att;

#endif
