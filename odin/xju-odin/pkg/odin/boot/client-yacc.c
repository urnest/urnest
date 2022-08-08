/* original parser id follows */
/* ocsccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use OCMAJOR/OCMINOR for ifdefs dependent on parser version) */

#define OCBYACC 1
#define OCMAJOR 1
#define OCMINOR 9
#define OCPATCH 20140715

#define OCEMPTY        (-1)
#define occlearin      (occhar = OCEMPTY)
#define ocerrok        (ocerrflag = 0)
#define OCRECOVERING() (ocerrflag != 0)
#define OCENOMEM       (-2)
#define OCEOF          0
#define OCPREFIX "oc"

#define OCPURE 0

#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod OC_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)ocparse(); Nod = End_ConstructTree();
return Nod;}
void ocerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int oclex() {return OC_Lex();}

#if ! defined(OCSTYPE) && ! defined(OCSTYPE_IS_DECLARED)
/* Default: OCSTYPE is the semantic value type. */
typedef int OCSTYPE;
# define OCSTYPE_IS_DECLARED 1
#endif

/* compatibility with bison */
#ifdef OCPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef OCPARSE_PARAM_TYPE
#  define OCPARSE_DECL() ocparse(OCPARSE_PARAM_TYPE OCPARSE_PARAM)
# else
#  define OCPARSE_DECL() ocparse(void *OCPARSE_PARAM)
# endif
#else
# define OCPARSE_DECL() ocparse(void)
#endif

/* Parameters sent to lex. */
#ifdef OCLEX_PARAM
# define OCLEX_DECL() oclex(void *OCLEX_PARAM)
# define OCLEX oclex(OCLEX_PARAM)
#else
# define OCLEX_DECL() oclex(void)
# define OCLEX oclex()
#endif

/* Parameters sent to ocerror. */
#ifndef OCERROR_DECL
#define OCERROR_DECL() ocerror(const char *s)
#endif
#ifndef OCERROR_CALL
#define OCERROR_CALL(msg) ocerror(msg)
#endif

extern int OCPARSE_DECL();

#define WORDTK 2
#define HOSTWD 3
#define OBJTID 4
#define EXCLPT 5
#define SCOLON 6
#define QUESMK 7
#define PERCNT 8
#define LANGLE 9
#define RANGLE 10
#define COLON 11
#define LPAREN 12
#define RPAREN 13
#define PLUS 14
#define EQUALS 15
#define SLASH 16
#define OCERRCODE 256
typedef short OCINT;
static const OCINT oclhs[] = {                           -1,
    0,    1,    1,    2,    2,    2,    2,    2,    3,    4,
    4,    4,    4,    4,    4,    5,    5,    5,    5,    6,
    6,    6,    7,   10,   10,   10,   11,   11,   11,   11,
   11,   11,   11,   11,   11,   11,   12,   12,   12,   12,
   12,   12,   12,   17,   17,   18,   19,   19,   20,   20,
   14,   16,    9,    9,    8,    8,   15,   21,   13,
};
static const OCINT oclen[] = {                            2,
    1,    3,    1,    0,    1,    1,    1,    1,    1,    2,
    1,    2,    1,    4,    3,    3,    3,    2,    2,    2,
    3,    3,    1,    1,    2,    3,    2,    1,    1,    1,
    2,    2,    2,    2,    2,    2,    3,    4,    2,    5,
    2,    1,    2,    1,    0,    2,    2,    1,    1,    3,
    1,    1,    1,    1,    2,    1,    1,    1,    1,
};
static const OCINT ocdefred[] = {                         0,
   57,   58,   59,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    3,    5,    6,    7,    8,    0,   11,    0,
    0,   24,   29,    0,   56,   55,    0,   33,   53,   35,
   54,   51,   32,   27,   28,    0,   36,   31,    0,    0,
    0,    0,   10,    0,    0,    0,    0,    0,   25,   34,
   15,   26,    2,    0,   17,   16,    0,   21,   22,   43,
    0,    0,   52,    0,   41,   14,    0,    0,    0,   37,
   44,    0,   38,    0,   49,    0,   48,   40,    0,   47,
   50,
};
static const OCINT ocdgoto[] = {                         11,
   12,   13,   14,   15,   16,   17,   18,   19,   35,   21,
   22,   49,   23,   33,   31,   64,   70,   71,   76,   77,
   25,
};
static const OCINT ocsindex[] = {                        56,
    0,    0,    0,   22,    4,   43,   43,  203,   43,   43,
    0,   10,    0,    0,    0,    0,    0,  241,    0,   -2,
   67,    0,    0,   34,    0,    0,   43,    0,    0,    0,
    0,    0,    0,    0,    0,   23,    0,    0,   56,   55,
  218,  218,    0,  188,   43,   43,    7,   43,    0,    0,
    0,    0,    0,   43,    0,    0,   35,    0,    0,    0,
   28,  218,    0,   37,    0,    0,   14,   44,   20,    0,
    0,   43,    0,  218,    0,   20,    0,    0,   52,    0,
    0,
};
static const OCINT ocrindex[] = {                        15,
    0,    0,    0,   18,   79,    0,    0,    0,    0,  113,
    0,   69,    0,    0,    0,    0,    0,   38,    0,  173,
  232,    0,    0,   96,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   15,   91,
  108,  180,    0,  202,    0,    0,    0,  125,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  137,    0,    0,  149,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  161,    0,    0,    0,    0,
    0,
};
static const OCINT ocgindex[] = {                         0,
    0,   41,    0,    0,    0,    0,  -39,  -18,    1,   68,
    0,    0,    0,  -42,   12,    0,    0,    0,    0,   24,
    0,
};
#define OCTABLESIZE 251
static const OCINT octable[] = {                         43,
   20,   55,   56,   61,   58,    1,   30,   32,    1,   37,
   38,   24,   44,   29,    4,   39,   28,   13,   62,   24,
    4,    1,   68,   13,   72,   59,   29,   51,   26,   78,
   45,   74,   27,   46,   79,   52,   47,    9,   48,   20,
   50,   26,   67,    9,    1,   60,   32,   63,   65,   29,
   24,   69,   24,   24,   66,   24,   73,    1,    2,    3,
    4,   26,    5,    6,   81,   54,    7,    8,    1,   75,
    9,   10,   32,   24,   45,   36,   75,   46,   53,   53,
   47,   53,   48,   53,   53,   24,   53,   53,   53,   53,
   12,   53,   53,   53,   53,   54,   12,    0,   54,   80,
   54,   54,    0,   54,   54,   54,   54,   19,   54,   54,
   54,   54,   30,   19,    0,   30,    0,   30,   30,    0,
   30,   30,   30,   30,   42,   30,   30,   42,   30,   42,
   42,    0,   42,   42,   42,   42,   39,   42,   42,   39,
   42,   39,   39,    0,   39,   39,   39,   39,   45,   39,
   39,   45,   39,   45,   45,    0,   45,   45,   45,   45,
   46,   45,   45,   46,   45,   46,   46,    0,   46,   46,
   46,   46,   28,   46,   46,   28,   46,   28,   28,   18,
   28,   28,   28,   28,    0,   18,   28,    0,   28,    1,
    2,    3,   57,    0,    5,    6,    0,    0,    7,    8,
    0,   20,    9,   10,    1,    0,    3,   20,    0,    5,
    6,    0,    0,    7,    8,   34,    0,    9,   10,    1,
    0,    3,    0,    0,    5,    6,    0,    0,    7,    8,
    0,   23,    9,   10,   23,    0,   23,   23,    0,    0,
   23,   23,    0,    2,   23,   40,    0,    0,    0,   41,
   42,
};
static const OCINT occheck[] = {                         18,
    0,   41,   42,   46,   44,    2,    6,    7,    2,    9,
   10,    0,   15,    7,    0,    6,    5,    0,   12,    8,
    6,    2,   62,    6,   11,   44,    7,   27,    7,   72,
    8,   12,   11,   11,   74,   13,   14,    0,   16,   39,
    7,    7,   15,    6,    2,   45,   46,   47,   48,    7,
   39,   15,   41,   42,   54,   44,   13,    2,    3,    4,
    5,    7,    7,    8,   13,   11,   11,   12,    0,   69,
   15,   16,   72,   62,    8,    8,   76,   11,    0,   39,
   14,    3,   16,    5,    6,   74,    8,    9,   10,   11,
    0,   13,   14,   15,   16,    0,    6,   -1,    3,   76,
    5,    6,   -1,    8,    9,   10,   11,    0,   13,   14,
   15,   16,    0,    6,   -1,    3,   -1,    5,    6,   -1,
    8,    9,   10,   11,    0,   13,   14,    3,   16,    5,
    6,   -1,    8,    9,   10,   11,    0,   13,   14,    3,
   16,    5,    6,   -1,    8,    9,   10,   11,    0,   13,
   14,    3,   16,    5,    6,   -1,    8,    9,   10,   11,
    0,   13,   14,    3,   16,    5,    6,   -1,    8,    9,
   10,   11,    0,   13,   14,    3,   16,    5,    6,    0,
    8,    9,   10,   11,   -1,    6,   14,   -1,   16,    2,
    3,    4,    5,   -1,    7,    8,   -1,   -1,   11,   12,
   -1,    0,   15,   16,    2,   -1,    4,    6,   -1,    7,
    8,   -1,   -1,   11,   12,   13,   -1,   15,   16,    2,
   -1,    4,   -1,   -1,    7,    8,   -1,   -1,   11,   12,
   -1,    0,   15,   16,    3,   -1,    5,    6,   -1,   -1,
    9,   10,   -1,    3,   13,    5,   -1,   -1,   -1,    9,
   10,
};
#define OCFINAL 11
#ifndef OCDEBUG
#define OCDEBUG 0
#endif
#define OCMAXTOKEN 256
#define OCUNDFTOKEN 280
#define OCTRANSLATE(a) ((a) > OCMAXTOKEN ? OCUNDFTOKEN : (a))
#if OCDEBUG
static const char *const ocname[] = {

"end-of-file",0,"WORDTK","HOSTWD","OBJTID","EXCLPT","SCOLON","QUESMK","PERCNT",
"LANGLE","RANGLE","COLON","LPAREN","RPAREN","PLUS","EQUALS","SLASH",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const ocrule[] = {
"$accept : xCommand",
"xCommand : xCommand_1",
"xCommand_1 : xCommand_1 SCOLON xSingleCommand",
"xCommand_1 : xSingleCommand",
"xSingleCommand :",
"xSingleCommand : xQuery",
"xSingleCommand : xExecute",
"xSingleCommand : xCopy",
"xSingleCommand : xVariable",
"xQuery : xOdinExpr",
"xExecute : xOdinExpr xEPString",
"xExecute : xEPString",
"xExecute : xOdinExpr EXCLPT",
"xExecute : EXCLPT",
"xExecute : xOdinExpr EXCLPT COLON xWord",
"xExecute : EXCLPT COLON xWord",
"xCopy : xOdinExpr RANGLE xOdinExpr",
"xCopy : xOdinExpr LANGLE xOdinExpr",
"xCopy : xOdinExpr RANGLE",
"xCopy : xOdinExpr LANGLE",
"xVariable : xWord EQUALS",
"xVariable : xWord EQUALS xOdinExpr",
"xVariable : xWord EQUALS xEPString",
"xOdinExpr : xOdinExpr1",
"xOdinExpr1 : xRoot",
"xOdinExpr1 : xOdinExpr1 xOperation",
"xOdinExpr1 : LPAREN xOdinExpr1 RPAREN",
"xRoot : LPAREN RPAREN",
"xRoot : xWord",
"xRoot : OBJTIDleaf",
"xRoot : SLASH",
"xRoot : SLASH xWord",
"xRoot : COLON xFileType",
"xRoot : QUESMK WORDTKleaf",
"xRoot : WORDTKleaf QUESMK",
"xRoot : PERCNT xWord",
"xRoot : EQUALS xWord",
"xOperation : PLUS xPrmType xOperation_1",
"xOperation : PLUS LPAREN xOdinExpr RPAREN",
"xOperation : COLON xFileType",
"xOperation : COLON xFileType EQUALS COLON xFileType",
"xOperation : SLASH xWord",
"xOperation : SLASH",
"xOperation : PERCNT xWord",
"xOperation_1 : xPrmVals",
"xOperation_1 :",
"xPrmVals : EQUALS xPrmVals_1",
"xPrmVals_1 : xPrmVals_1 xPrmVal",
"xPrmVals_1 : xPrmVal",
"xPrmVal : xWord",
"xPrmVal : LPAREN xOdinExpr RPAREN",
"xFileType : xWord",
"xPrmType : xWord",
"xWord : QUESMK",
"xWord : WORDTKleaf",
"xEPString : EXCLPT QUESMK",
"xEPString : HOSTWDleaf",
"WORDTKleaf : WORDTK",
"HOSTWDleaf : HOSTWD",
"OBJTIDleaf : OBJTID",

};
#endif

int      ocdebug;
int      ocnerrs;

int      ocerrflag;
int      occhar;
OCSTYPE  ocval;
OCSTYPE  oclval;

/* define the initial stack-sizes */
#ifdef OCSTACKSIZE
#undef OCMAXDEPTH
#define OCMAXDEPTH  OCSTACKSIZE
#else
#ifdef OCMAXDEPTH
#define OCSTACKSIZE OCMAXDEPTH
#else
#define OCSTACKSIZE 10000
#define OCMAXDEPTH  10000
#endif
#endif

#define OCINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    OCINT    *s_base;
    OCINT    *s_mark;
    OCINT    *s_last;
    OCSTYPE  *l_base;
    OCSTYPE  *l_mark;
} OCSTACKDATA;
/* variables for the parser stack */
static OCSTACKDATA ocstack;

#if OCDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to OCMAXDEPTH */
static int ocgrowstack(OCSTACKDATA *data)
{
    int i;
    unsigned newsize;
    OCINT *newss;
    OCSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = OCINITSTACKSIZE;
    else if (newsize >= OCMAXDEPTH)
        return OCENOMEM;
    else if ((newsize *= 2) > OCMAXDEPTH)
        newsize = OCMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (OCINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return OCENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (OCSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return OCENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if OCPURE || defined(OC_NO_LEAKS)
static void ocfreestack(OCSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define ocfreestack(data) /* nothing */
#endif

#define OCABORT  goto ocabort
#define OCREJECT goto ocabort
#define OCACCEPT goto ocaccept
#define OCERROR  goto ocerrlab

int
OCPARSE_DECL()
{
    int ocm, ocn, ocstate;
#if OCDEBUG
    const char *ocs;

    if ((ocs = getenv("OCDEBUG")) != 0)
    {
        ocn = *ocs;
        if (ocn >= '0' && ocn <= '9')
            ocdebug = ocn - '0';
    }
#endif

    ocnerrs = 0;
    ocerrflag = 0;
    occhar = OCEMPTY;
    ocstate = 0;

#if OCPURE
    memset(&ocstack, 0, sizeof(ocstack));
#endif

    if (ocstack.s_base == NULL && ocgrowstack(&ocstack) == OCENOMEM) goto ocoverflow;
    ocstack.s_mark = ocstack.s_base;
    ocstack.l_mark = ocstack.l_base;
    ocstate = 0;
    *ocstack.s_mark = 0;

ocloop:
    if ((ocn = ocdefred[ocstate]) != 0) goto ocreduce;
    if (occhar < 0)
    {
        if ((occhar = OCLEX) < 0) occhar = OCEOF;
#if OCDEBUG
        if (ocdebug)
        {
            ocs = ocname[OCTRANSLATE(occhar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    OCPREFIX, ocstate, occhar, ocs);
        }
#endif
    }
    if ((ocn = ocsindex[ocstate]) && (ocn += occhar) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == occhar)
    {
#if OCDEBUG
        if (ocdebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    OCPREFIX, ocstate, octable[ocn]);
#endif
        if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM)
        {
            goto ocoverflow;
        }
        ocstate = octable[ocn];
        *++ocstack.s_mark = octable[ocn];
        *++ocstack.l_mark = oclval;
        occhar = OCEMPTY;
        if (ocerrflag > 0)  --ocerrflag;
        goto ocloop;
    }
    if ((ocn = ocrindex[ocstate]) && (ocn += occhar) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == occhar)
    {
        ocn = octable[ocn];
        goto ocreduce;
    }
    if (ocerrflag) goto ocinrecovery;

    OCERROR_CALL("syntax error");

    goto ocerrlab;

ocerrlab:
    ++ocnerrs;

ocinrecovery:
    if (ocerrflag < 3)
    {
        ocerrflag = 3;
        for (;;)
        {
            if ((ocn = ocsindex[*ocstack.s_mark]) && (ocn += OCERRCODE) >= 0 &&
                    ocn <= OCTABLESIZE && occheck[ocn] == OCERRCODE)
            {
#if OCDEBUG
                if (ocdebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", OCPREFIX, *ocstack.s_mark, octable[ocn]);
#endif
                if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM)
                {
                    goto ocoverflow;
                }
                ocstate = octable[ocn];
                *++ocstack.s_mark = octable[ocn];
                *++ocstack.l_mark = oclval;
                goto ocloop;
            }
            else
            {
#if OCDEBUG
                if (ocdebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            OCPREFIX, *ocstack.s_mark);
#endif
                if (ocstack.s_mark <= ocstack.s_base) goto ocabort;
                --ocstack.s_mark;
                --ocstack.l_mark;
            }
        }
    }
    else
    {
        if (occhar == OCEOF) goto ocabort;
#if OCDEBUG
        if (ocdebug)
        {
            ocs = ocname[OCTRANSLATE(occhar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    OCPREFIX, ocstate, occhar, ocs);
        }
#endif
        occhar = OCEMPTY;
        goto ocloop;
    }

ocreduce:
#if OCDEBUG
    if (ocdebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                OCPREFIX, ocstate, ocn, ocrule[ocn]);
#endif
    ocm = oclen[ocn];
    if (ocm)
        ocval = ocstack.l_mark[1-ocm];
    else
        memset(&ocval, 0, sizeof ocval);
    switch (ocn)
    {
case 1:
	{Action(-1,1);}
break;
case 2:
	{Action(0,2);}
break;
case 4:
	{Action(2,0);}
break;
case 10:
	{Action(7,2);}
break;
case 11:
	{Action(7,1);}
break;
case 12:
	{Action(7,1);}
break;
case 13:
	{Action(7,0);}
break;
case 14:
	{Action(8,2);}
break;
case 15:
	{Action(8,1);}
break;
case 16:
	{Action(4,2);}
break;
case 17:
	{Action(6,2);}
break;
case 18:
	{Action(3,1);}
break;
case 19:
	{Action(5,1);}
break;
case 20:
	{Action(9,1);}
break;
case 21:
	{Action(10,2);}
break;
case 22:
	{Action(10,2);}
break;
case 23:
	{Action(15,1);}
break;
case 25:
	{Action(0,2);}
break;
case 27:
	{Action(16,0);}
break;
case 30:
	{Action(17,0);}
break;
case 31:
	{Action(18,1);}
break;
case 32:
	{Action(21,1);}
break;
case 33:
	{Action(14,1);}
break;
case 34:
	{Action(13,1);}
break;
case 35:
	{Action(25,1);}
break;
case 36:
	{Action(29,1);}
break;
case 37:
	{Action(19,2);}
break;
case 38:
	{Action(20,1);}
break;
case 39:
	{Action(21,1);}
break;
case 40:
	{Action(22,2);}
break;
case 41:
	{Action(23,1);}
break;
case 42:
	{Action(24,0);}
break;
case 43:
	{Action(25,1);}
break;
case 45:
	{Action(0,0);}
break;
case 46:
	{Action(27,1);}
break;
case 47:
	{Action(0,2);}
break;
case 50:
	{Action(28,1);}
break;
case 53:
	{Action(11,0);}
break;
case 55:
	{Action(12,0);}
break;
case 57:
	{Action(31,-1);}
break;
case 58:
	{Action(30,-1);}
break;
case 59:
	{Action(32,-1);}
break;
    }
    ocstack.s_mark -= ocm;
    ocstate = *ocstack.s_mark;
    ocstack.l_mark -= ocm;
    ocm = oclhs[ocn];
    if (ocstate == 0 && ocm == 0)
    {
#if OCDEBUG
        if (ocdebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", OCPREFIX, OCFINAL);
#endif
        ocstate = OCFINAL;
        *++ocstack.s_mark = OCFINAL;
        *++ocstack.l_mark = ocval;
        if (occhar < 0)
        {
            if ((occhar = OCLEX) < 0) occhar = OCEOF;
#if OCDEBUG
            if (ocdebug)
            {
                ocs = ocname[OCTRANSLATE(occhar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        OCPREFIX, OCFINAL, occhar, ocs);
            }
#endif
        }
        if (occhar == OCEOF) goto ocaccept;
        goto ocloop;
    }
    if ((ocn = ocgindex[ocm]) && (ocn += ocstate) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == ocstate)
        ocstate = octable[ocn];
    else
        ocstate = ocdgoto[ocm];
#if OCDEBUG
    if (ocdebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", OCPREFIX, *ocstack.s_mark, ocstate);
#endif
    if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM)
    {
        goto ocoverflow;
    }
    *++ocstack.s_mark = (OCINT) ocstate;
    *++ocstack.l_mark = ocval;
    goto ocloop;

ocoverflow:
    OCERROR_CALL("yacc stack overflow");

ocabort:
    ocfreestack(&ocstack);
    return (1);

ocaccept:
    ocfreestack(&ocstack);
    return (0);
}
