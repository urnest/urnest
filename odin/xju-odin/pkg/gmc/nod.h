#ifndef TPS_NOD
#define TPS_NOD

#include <gmc/gmc.h>

extern void EndLex(); /* supplied by *-yylex.c */

typedef struct _tps_Nod* tp_Nod;

extern tp_Nod New_Nod();
extern void Ret_Nod(tp_Nod Nod);
extern tp_NodTyp Nod_NodTyp(tp_Nod Nod);
extern void Set_Nod_NodTyp(tp_Nod Nod,tp_NodTyp NodTyp);
extern tp_Nod Nod_FirstSon(tp_Nod Nod);
extern void Set_Nod_FirstSon(tp_Nod Nod,tp_Nod FirstSon);
extern tp_Nod Nod_Brother(tp_Nod Nod);
extern void Set_Nod_Brother(tp_Nod Nod,tp_Nod Brother);
extern int Nod_NumSons(tp_Nod Nod);
extern tp_Nod Nod_Son(int I,tp_Nod Nod);
extern tp_Sym Nod_Sym(tp_Nod Nod);
extern void Set_Nod_Sym(tp_Nod Nod,tp_Sym Sym);
#ifdef NODATT
extern tp_Nod Get_NodAtt(tp_Nod Nod);
extern tp_Nod Set_NodAtt(tp_Nod Nod,tp_Nod Att);
extern void Set_All_NodAtts(tp_Nod Nod,tp_Nod Att);
#endif
extern void Init_ConstructTree();
extern tp_Nod End_ConstructTree();
extern void Action(int Typ,int NumSons);

extern void Push_SymStack(tp_Sym);

#define FOREACH_SON(son,root) \
   for (son=Nod_FirstSon(root); son!=NIL; son=Nod_Brother(son))

#define FOREACH_BROTHER(brother, nod) \
   for (brother=Nod_Brother(nod); brother!=NIL; brother=Nod_Brother(brother))

#endif
