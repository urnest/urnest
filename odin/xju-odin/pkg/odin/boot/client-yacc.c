/* original parser id follows */
/* ocsccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use OCMAJOR/OCMINOR for ifdefs dependent on parser version) */

#define OCBYACC 1
#define OCMAJOR 2
#define OCMINOR 0
#define OCPATCH 20221106

#define OCEMPTY        (-1)
#define occlearin      (occhar = OCEMPTY)
#define ocerrok        (ocerrflag = 0)
#define OCRECOVERING() (ocerrflag != 0)
#define OCENOMEM       (-2)
#define OCEOF          0
#undef OCBTYACC
#define OCBTYACC 0
#define OCDEBUGSTR OCPREFIX "debug"
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

#if !(defined(oclex) || defined(OCSTATE))
int OCLEX_DECL();
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
typedef int OCINT;
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
#if defined(OCDESTRUCT_CALL) || defined(OCSTYPE_TOSTRING)
static const OCINT ocstos[] = {                           0,
    2,    3,    4,    5,    7,    8,   11,   12,   15,   16,
  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,
  268,  269,  271,  273,  279,    7,   11,  273,    7,  267,
  273,  267,  272,   13,  267,  268,  267,  267,    6,    5,
    9,   10,  266,   15,    8,   11,   14,   16,  270,    7,
  267,   13,  260,   11,  265,  265,    5,  265,  266,  267,
  272,   12,  267,  274,  267,  267,   15,  265,   15,  275,
  276,   11,   13,   12,  267,  277,  278,  272,  265,  278,
   13,
};
#endif /* OCDESTRUCT_CALL || OCSTYPE_TOSTRING */
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
#if OCBTYACC
static const OCINT occindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
#endif
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
#if OCBTYACC
static const OCINT occtable[] = {                        -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
};
#endif
#define OCFINAL 11
#ifndef OCDEBUG
#define OCDEBUG 0
#endif
#define OCMAXTOKEN 256
#define OCUNDFTOKEN 280
#define OCTRANSLATE(a) ((a) > OCMAXTOKEN ? OCUNDFTOKEN : (a))
#if OCDEBUG
static const char *const ocname[] = {

"$end",0,"WORDTK","HOSTWD","OBJTID","EXCLPT","SCOLON","QUESMK","PERCNT",
"LANGLE","RANGLE","COLON","LPAREN","RPAREN","PLUS","EQUALS","SLASH",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error",
"$accept","xCommand","xCommand_1","xSingleCommand","xQuery","xExecute","xCopy",
"xVariable","xOdinExpr","xEPString","xWord","xOdinExpr1","xRoot","xOperation",
"OBJTIDleaf","xFileType","WORDTKleaf","xPrmType","xOperation_1","xPrmVals",
"xPrmVals_1","xPrmVal","HOSTWDleaf","illegal-symbol",
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

#if OCDEBUG
int      ocdebug;
#endif

int      ocerrflag;
int      occhar;
OCSTYPE  ocval;
OCSTYPE  oclval;
int      ocnerrs;

#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
OCLTYPE  ocloc; /* position returned by actions */
OCLTYPE  oclloc; /* position from the lexer */
#endif

#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
#ifndef OCLLOC_DEFAULT
#define OCLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = OCRHSLOC(rhs, 0).last_line; \
        (loc).first_column = OCRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = OCRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = OCRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = OCRHSLOC(rhs, 1).first_line; \
        (loc).first_column = OCRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = OCRHSLOC(rhs, n).last_line; \
        (loc).last_column  = OCRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* OCLLOC_DEFAULT */
#endif /* defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED) */
#if OCBTYACC

#ifndef OCLVQUEUEGROWTH
#define OCLVQUEUEGROWTH 32
#endif
#endif /* OCBTYACC */

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

#ifndef OCINITSTACKSIZE
#define OCINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    OCINT    *s_base;
    OCINT    *s_mark;
    OCINT    *s_last;
    OCSTYPE  *l_base;
    OCSTYPE  *l_mark;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    OCLTYPE  *p_base;
    OCLTYPE  *p_mark;
#endif
} OCSTACKDATA;
#if OCBTYACC

struct OCParseState_s
{
    struct OCParseState_s *save;    /* Previously saved parser state */
    OCSTACKDATA            ocstack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    OCINT                  ctry;    /* saved index in occtable[] for this conflict */
};
typedef struct OCParseState_s OCParseState;
#endif /* OCBTYACC */
/* variables for the parser stack */
static OCSTACKDATA ocstack;
#if OCBTYACC

/* Current parser state */
static OCParseState *ocps = 0;

/* ocpath != NULL: do the full parse, starting at *ocpath parser state. */
static OCParseState *ocpath = 0;

/* Base of the lexical value queue */
static OCSTYPE *oclvals = 0;

/* Current position at lexical value queue */
static OCSTYPE *oclvp = 0;

/* End position of lexical value queue */
static OCSTYPE *oclve = 0;

/* The last allocated position at the lexical value queue */
static OCSTYPE *oclvlim = 0;

#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static OCLTYPE *oclpsns = 0;

/* Current position at lexical position queue */
static OCLTYPE *oclpp = 0;

/* End position of lexical position queue */
static OCLTYPE *oclpe = 0;

/* The last allocated position at the lexical position queue */
static OCLTYPE *oclplim = 0;
#endif

/* Current position at lexical token queue */
static OCINT  *oclexp = 0;

static OCINT  *oclexemes = 0;
#endif /* OCBTYACC */

/* For use in generated program */
#define ocdepth (int)(ocstack.s_mark - ocstack.s_base)
#if OCBTYACC
#define octrial (ocps->save)
#endif /* OCBTYACC */

#if OCDEBUG
#include <stdio.h>	/* needed for printf */
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
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    OCLTYPE *newps;
#endif

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

#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    newps = (OCLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == 0)
        return OCENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if OCDEBUG
    if (ocdebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", OCPREFIX, newsize);
#endif
    return 0;
}

#if OCPURE || defined(OC_NO_LEAKS)
static void ocfreestack(OCSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define ocfreestack(data) /* nothing */
#endif /* OCPURE || defined(OC_NO_LEAKS) */
#if OCBTYACC

static OCParseState *
ocNewState(unsigned size)
{
    OCParseState *p = (OCParseState *) malloc(sizeof(OCParseState));
    if (p == NULL) return NULL;

    p->ocstack.stacksize = size;
    if (size == 0)
    {
        p->ocstack.s_base = NULL;
        p->ocstack.l_base = NULL;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        p->ocstack.p_base = NULL;
#endif
        return p;
    }
    p->ocstack.s_base    = (OCINT *) malloc(size * sizeof(OCINT));
    if (p->ocstack.s_base == NULL) return NULL;
    p->ocstack.l_base    = (OCSTYPE *) malloc(size * sizeof(OCSTYPE));
    if (p->ocstack.l_base == NULL) return NULL;
    memset(p->ocstack.l_base, 0, size * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    p->ocstack.p_base    = (OCLTYPE *) malloc(size * sizeof(OCLTYPE));
    if (p->ocstack.p_base == NULL) return NULL;
    memset(p->ocstack.p_base, 0, size * sizeof(OCLTYPE));
#endif

    return p;
}

static void
ocFreeState(OCParseState *p)
{
    ocfreestack(&p->ocstack);
    free(p);
}
#endif /* OCBTYACC */

#define OCABORT  goto ocabort
#define OCREJECT goto ocabort
#define OCACCEPT goto ocaccept
#define OCERROR  goto ocerrlab
#if OCBTYACC
#define OCVALID        do { if (ocps->save)            goto ocvalid; } while(0)
#define OCVALID_NESTED do { if (ocps->save && \
                                ocps->save->save == 0) goto ocvalid; } while(0)
#endif /* OCBTYACC */

int
OCPARSE_DECL()
{
    int ocm, ocn, ocstate, ocresult;
#if OCBTYACC
    int ocnewerrflag;
    OCParseState *ocerrctx = NULL;
#endif /* OCBTYACC */
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    OCLTYPE  ocerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if OCDEBUG
    const char *ocs;

    if ((ocs = getenv("OCDEBUG")) != 0)
    {
        ocn = *ocs;
        if (ocn >= '0' && ocn <= '9')
            ocdebug = ocn - '0';
    }
    if (ocdebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", OCPREFIX);
#endif
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    memset(ocerror_loc_range, 0, sizeof(ocerror_loc_range));
#endif

#if OCBTYACC
    ocps = ocNewState(0); if (ocps == 0) goto ocenomem;
    ocps->save = 0;
#endif /* OCBTYACC */
    ocm = 0;
    /* ocn is set below */
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
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    ocstack.p_mark = ocstack.p_base;
#endif
    ocstate = 0;
    *ocstack.s_mark = 0;

ocloop:
    if ((ocn = ocdefred[ocstate]) != 0) goto ocreduce;
    if (occhar < 0)
    {
#if OCBTYACC
        do {
        if (oclvp < oclve)
        {
            /* we're currently re-reading tokens */
            oclval = *oclvp++;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            oclloc = *oclpp++;
#endif
            occhar = *oclexp++;
            break;
        }
        if (ocps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (oclvp == oclvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (oclvp - oclvals);
                size_t s = (size_t) (oclvlim - oclvals);

                s += OCLVQUEUEGROWTH;
                if ((oclexemes = (OCINT *)realloc(oclexemes, s * sizeof(OCINT))) == NULL) goto ocenomem;
                if ((oclvals   = (OCSTYPE *)realloc(oclvals, s * sizeof(OCSTYPE))) == NULL) goto ocenomem;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                if ((oclpsns   = (OCLTYPE *)realloc(oclpsns, s * sizeof(OCLTYPE))) == NULL) goto ocenomem;
#endif
                oclvp   = oclve = oclvals + p;
                oclvlim = oclvals + s;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                oclpp   = oclpe = oclpsns + p;
                oclplim = oclpsns + s;
#endif
                oclexp  = oclexemes + p;
            }
            *oclexp = (OCINT) OCLEX;
            *oclvp++ = oclval;
            oclve++;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            *oclpp++ = oclloc;
            oclpe++;
#endif
            occhar = *oclexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* OCBTYACC */
        occhar = OCLEX;
#if OCBTYACC
        } while (0);
#endif /* OCBTYACC */
        if (occhar < 0) occhar = OCEOF;
#if OCDEBUG
        if (ocdebug)
        {
            if ((ocs = ocname[OCTRANSLATE(occhar)]) == NULL) ocs = ocname[OCUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            OCDEBUGSTR, ocdepth, ocstate, occhar, ocs);
#ifdef OCSTYPE_TOSTRING
#if OCBTYACC
            if (!octrial)
#endif /* OCBTYACC */
                fprintf(stderr, " <%s>", OCSTYPE_TOSTRING(occhar, oclval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if OCBTYACC

    /* Do we have a conflict? */
    if (((ocn = occindex[ocstate]) != 0) && (ocn += occhar) >= 0 &&
        ocn <= OCTABLESIZE && occheck[ocn] == (OCINT) occhar)
    {
        OCINT ctry;

        if (ocpath)
        {
            OCParseState *save;
#if OCDEBUG
            if (ocdebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                OCDEBUGSTR, ocdepth, ocstate);
#endif
            /* Switch to the next conflict context */
            save = ocpath;
            ocpath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != ocstate) OCABORT;
            ocFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            OCParseState *save;
#if OCDEBUG
            if (ocdebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", OCDEBUGSTR, ocdepth, ocstate);
                if (ocps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = ocNewState((unsigned)(ocstack.s_mark - ocstack.s_base + 1));
            if (save == NULL) goto ocenomem;
            save->save            = ocps->save;
            save->state           = ocstate;
            save->errflag         = ocerrflag;
            save->ocstack.s_mark  = save->ocstack.s_base + (ocstack.s_mark - ocstack.s_base);
            memcpy (save->ocstack.s_base, ocstack.s_base, (size_t) (ocstack.s_mark - ocstack.s_base + 1) * sizeof(OCINT));
            save->ocstack.l_mark  = save->ocstack.l_base + (ocstack.l_mark - ocstack.l_base);
            memcpy (save->ocstack.l_base, ocstack.l_base, (size_t) (ocstack.l_mark - ocstack.l_base + 1) * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            save->ocstack.p_mark  = save->ocstack.p_base + (ocstack.p_mark - ocstack.p_base);
            memcpy (save->ocstack.p_base, ocstack.p_base, (size_t) (ocstack.p_mark - ocstack.p_base + 1) * sizeof(OCLTYPE));
#endif
            ctry                  = octable[ocn];
            if (occtable[ctry] == -1)
            {
#if OCDEBUG
                if (ocdebug && occhar >= OCEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", OCDEBUGSTR, ocdepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (ocps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!oclexemes)
                {
                    oclexemes = (OCINT *) malloc((OCLVQUEUEGROWTH) * sizeof(OCINT));
                    if (oclexemes == NULL) goto ocenomem;
                    oclvals   = (OCSTYPE *) malloc((OCLVQUEUEGROWTH) * sizeof(OCSTYPE));
                    if (oclvals == NULL) goto ocenomem;
                    oclvlim   = oclvals + OCLVQUEUEGROWTH;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                    oclpsns   = (OCLTYPE *) malloc((OCLVQUEUEGROWTH) * sizeof(OCLTYPE));
                    if (oclpsns == NULL) goto ocenomem;
                    oclplim   = oclpsns + OCLVQUEUEGROWTH;
#endif
                }
                if (oclvp == oclve)
                {
                    oclvp  = oclve = oclvals;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                    oclpp  = oclpe = oclpsns;
#endif
                    oclexp = oclexemes;
                    if (occhar >= OCEOF)
                    {
                        *oclve++ = oclval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                        *oclpe++ = oclloc;
#endif
                        *oclexp  = (OCINT) occhar;
                        occhar   = OCEMPTY;
                    }
                }
            }
            if (occhar >= OCEOF)
            {
                oclvp--;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                oclpp--;
#endif
                oclexp--;
                occhar = OCEMPTY;
            }
            save->lexeme = (int) (oclvp - oclvals);
            ocps->save   = save;
        }
        if (octable[ocn] == ctry)
        {
#if OCDEBUG
            if (ocdebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                OCDEBUGSTR, ocdepth, ocstate, occtable[ctry]);
#endif
            if (occhar < 0)
            {
                oclvp++;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                oclpp++;
#endif
                oclexp++;
            }
            if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM)
                goto ocoverflow;
            ocstate = occtable[ctry];
            *++ocstack.s_mark = (OCINT) ocstate;
            *++ocstack.l_mark = oclval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            *++ocstack.p_mark = oclloc;
#endif
            occhar  = OCEMPTY;
            if (ocerrflag > 0) --ocerrflag;
            goto ocloop;
        }
        else
        {
            ocn = occtable[ctry];
            goto ocreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* OCBTYACC */
    if (((ocn = ocsindex[ocstate]) != 0) && (ocn += occhar) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == (OCINT) occhar)
    {
#if OCDEBUG
        if (ocdebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            OCDEBUGSTR, ocdepth, ocstate, octable[ocn]);
#endif
        if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM) goto ocoverflow;
        ocstate = octable[ocn];
        *++ocstack.s_mark = octable[ocn];
        *++ocstack.l_mark = oclval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        *++ocstack.p_mark = oclloc;
#endif
        occhar = OCEMPTY;
        if (ocerrflag > 0)  --ocerrflag;
        goto ocloop;
    }
    if (((ocn = ocrindex[ocstate]) != 0) && (ocn += occhar) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == (OCINT) occhar)
    {
        ocn = octable[ocn];
        goto ocreduce;
    }
    if (ocerrflag != 0) goto ocinrecovery;
#if OCBTYACC

    ocnewerrflag = 1;
    goto ocerrhandler;
    goto ocerrlab; /* redundant goto avoids 'unused label' warning */

ocerrlab:
    /* explicit OCERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    ocstack.s_mark -= ocm;
    ocstate = *ocstack.s_mark;
    ocstack.l_mark -= ocm;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    ocstack.p_mark -= ocm;
#endif

    ocnewerrflag = 0;
ocerrhandler:
    while (ocps->save)
    {
        int ctry;
        OCParseState *save = ocps->save;
#if OCDEBUG
        if (ocdebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            OCDEBUGSTR, ocdepth, ocstate, ocps->save->state,
                    (int)(oclvp - oclvals - ocps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (ocerrctx == NULL || ocerrctx->lexeme < oclvp - oclvals)
        {
            /* Free old saved error context state */
            if (ocerrctx) ocFreeState(ocerrctx);
            /* Create and fill out new saved error context state */
            ocerrctx                 = ocNewState((unsigned)(ocstack.s_mark - ocstack.s_base + 1));
            if (ocerrctx == NULL) goto ocenomem;
            ocerrctx->save           = ocps->save;
            ocerrctx->state          = ocstate;
            ocerrctx->errflag        = ocerrflag;
            ocerrctx->ocstack.s_mark = ocerrctx->ocstack.s_base + (ocstack.s_mark - ocstack.s_base);
            memcpy (ocerrctx->ocstack.s_base, ocstack.s_base, (size_t) (ocstack.s_mark - ocstack.s_base + 1) * sizeof(OCINT));
            ocerrctx->ocstack.l_mark = ocerrctx->ocstack.l_base + (ocstack.l_mark - ocstack.l_base);
            memcpy (ocerrctx->ocstack.l_base, ocstack.l_base, (size_t) (ocstack.l_mark - ocstack.l_base + 1) * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            ocerrctx->ocstack.p_mark = ocerrctx->ocstack.p_base + (ocstack.p_mark - ocstack.p_base);
            memcpy (ocerrctx->ocstack.p_base, ocstack.p_base, (size_t) (ocstack.p_mark - ocstack.p_base + 1) * sizeof(OCLTYPE));
#endif
            ocerrctx->lexeme         = (int) (oclvp - oclvals);
        }
        oclvp          = oclvals   + save->lexeme;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        oclpp          = oclpsns   + save->lexeme;
#endif
        oclexp         = oclexemes + save->lexeme;
        occhar         = OCEMPTY;
        ocstack.s_mark = ocstack.s_base + (save->ocstack.s_mark - save->ocstack.s_base);
        memcpy (ocstack.s_base, save->ocstack.s_base, (size_t) (ocstack.s_mark - ocstack.s_base + 1) * sizeof(OCINT));
        ocstack.l_mark = ocstack.l_base + (save->ocstack.l_mark - save->ocstack.l_base);
        memcpy (ocstack.l_base, save->ocstack.l_base, (size_t) (ocstack.l_mark - ocstack.l_base + 1) * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        ocstack.p_mark = ocstack.p_base + (save->ocstack.p_mark - save->ocstack.p_base);
        memcpy (ocstack.p_base, save->ocstack.p_base, (size_t) (ocstack.p_mark - ocstack.p_base + 1) * sizeof(OCLTYPE));
#endif
        ctry           = ++save->ctry;
        ocstate        = save->state;
        /* We tried shift, try reduce now */
        if ((ocn = occtable[ctry]) >= 0) goto ocreduce;
        ocps->save     = save->save;
        save->save     = NULL;
        ocFreeState(save);

        /* Nothing left on the stack -- error */
        if (!ocps->save)
        {
#if OCDEBUG
            if (ocdebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                OCPREFIX, ocdepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            oclvp          = oclvals   + ocerrctx->lexeme;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            oclpp          = oclpsns   + ocerrctx->lexeme;
#endif
            oclexp         = oclexemes + ocerrctx->lexeme;
            occhar         = oclexp[-1];
            oclval         = oclvp[-1];
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            oclloc         = oclpp[-1];
#endif
            ocstack.s_mark = ocstack.s_base + (ocerrctx->ocstack.s_mark - ocerrctx->ocstack.s_base);
            memcpy (ocstack.s_base, ocerrctx->ocstack.s_base, (size_t) (ocstack.s_mark - ocstack.s_base + 1) * sizeof(OCINT));
            ocstack.l_mark = ocstack.l_base + (ocerrctx->ocstack.l_mark - ocerrctx->ocstack.l_base);
            memcpy (ocstack.l_base, ocerrctx->ocstack.l_base, (size_t) (ocstack.l_mark - ocstack.l_base + 1) * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            ocstack.p_mark = ocstack.p_base + (ocerrctx->ocstack.p_mark - ocerrctx->ocstack.p_base);
            memcpy (ocstack.p_base, ocerrctx->ocstack.p_base, (size_t) (ocstack.p_mark - ocstack.p_base + 1) * sizeof(OCLTYPE));
#endif
            ocstate        = ocerrctx->state;
            ocFreeState(ocerrctx);
            ocerrctx       = NULL;
        }
        ocnewerrflag = 1;
    }
    if (ocnewerrflag == 0) goto ocinrecovery;
#endif /* OCBTYACC */

    OCERROR_CALL("syntax error");
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    ocerror_loc_range[1] = oclloc; /* lookahead position is error start position */
#endif

#if !OCBTYACC
    goto ocerrlab; /* redundant goto avoids 'unused label' warning */
ocerrlab:
#endif
    ++ocnerrs;

ocinrecovery:
    if (ocerrflag < 3)
    {
        ocerrflag = 3;
        for (;;)
        {
            if (((ocn = ocsindex[*ocstack.s_mark]) != 0) && (ocn += OCERRCODE) >= 0 &&
                    ocn <= OCTABLESIZE && occheck[ocn] == (OCINT) OCERRCODE)
            {
#if OCDEBUG
                if (ocdebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    OCDEBUGSTR, ocdepth, *ocstack.s_mark, octable[ocn]);
#endif
                if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM) goto ocoverflow;
                ocstate = octable[ocn];
                *++ocstack.s_mark = octable[ocn];
                *++ocstack.l_mark = oclval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                ocerror_loc_range[2] = oclloc;
                OCLLOC_DEFAULT(ocloc, ocerror_loc_range, 2); /* position of error span */
                *++ocstack.p_mark = ocloc;
#endif
                goto ocloop;
            }
            else
            {
#if OCDEBUG
                if (ocdebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    OCDEBUGSTR, ocdepth, *ocstack.s_mark);
#endif
                if (ocstack.s_mark <= ocstack.s_base) goto ocabort;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                ocerror_loc_range[1] = *ocstack.p_mark;
#endif
#if defined(OCDESTRUCT_CALL)
#if OCBTYACC
                if (!octrial)
#endif /* OCBTYACC */
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                    OCDESTRUCT_CALL("error: discarding state",
                                    ocstos[*ocstack.s_mark], ocstack.l_mark, ocstack.p_mark);
#else
                    OCDESTRUCT_CALL("error: discarding state",
                                    ocstos[*ocstack.s_mark], ocstack.l_mark);
#endif /* defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED) */
#endif /* defined(OCDESTRUCT_CALL) */
                --ocstack.s_mark;
                --ocstack.l_mark;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                --ocstack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (occhar == OCEOF) goto ocabort;
#if OCDEBUG
        if (ocdebug)
        {
            if ((ocs = ocname[OCTRANSLATE(occhar)]) == NULL) ocs = ocname[OCUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            OCDEBUGSTR, ocdepth, ocstate, occhar, ocs);
        }
#endif
#if defined(OCDESTRUCT_CALL)
#if OCBTYACC
        if (!octrial)
#endif /* OCBTYACC */
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
            OCDESTRUCT_CALL("error: discarding token", occhar, &oclval, &oclloc);
#else
            OCDESTRUCT_CALL("error: discarding token", occhar, &oclval);
#endif /* defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED) */
#endif /* defined(OCDESTRUCT_CALL) */
        occhar = OCEMPTY;
        goto ocloop;
    }

ocreduce:
    ocm = oclen[ocn];
#if OCDEBUG
    if (ocdebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        OCDEBUGSTR, ocdepth, ocstate, ocn, ocrule[ocn]);
#ifdef OCSTYPE_TOSTRING
#if OCBTYACC
        if (!octrial)
#endif /* OCBTYACC */
            if (ocm > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = ocm; i > 0; i--)
                {
                    if (i != ocm) fputs(", ", stderr);
                    fputs(OCSTYPE_TOSTRING(ocstos[ocstack.s_mark[1-i]],
                                           ocstack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (ocm > 0)
        ocval = ocstack.l_mark[1-ocm];
    else
        memset(&ocval, 0, sizeof ocval);
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&ocloc, 0, sizeof(ocloc));
#if OCBTYACC
    if (!octrial)
#endif /* OCBTYACC */
    {
        OCLLOC_DEFAULT(ocloc, &ocstack.p_mark[-ocm], ocm);
        /* just in case OCERROR is invoked within the action, save
           the start of the rhs as the error start position */
        ocerror_loc_range[1] = ocstack.p_mark[1-ocm];
    }
#endif

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
    default:
        break;
    }
    ocstack.s_mark -= ocm;
    ocstate = *ocstack.s_mark;
    ocstack.l_mark -= ocm;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    ocstack.p_mark -= ocm;
#endif
    ocm = oclhs[ocn];
    if (ocstate == 0 && ocm == 0)
    {
#if OCDEBUG
        if (ocdebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", OCDEBUGSTR, ocdepth);
#ifdef OCSTYPE_TOSTRING
#if OCBTYACC
            if (!octrial)
#endif /* OCBTYACC */
                fprintf(stderr, "result is <%s>, ", OCSTYPE_TOSTRING(ocstos[OCFINAL], ocval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", OCFINAL);
        }
#endif
        ocstate = OCFINAL;
        *++ocstack.s_mark = OCFINAL;
        *++ocstack.l_mark = ocval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        *++ocstack.p_mark = ocloc;
#endif
        if (occhar < 0)
        {
#if OCBTYACC
            do {
            if (oclvp < oclve)
            {
                /* we're currently re-reading tokens */
                oclval = *oclvp++;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                oclloc = *oclpp++;
#endif
                occhar = *oclexp++;
                break;
            }
            if (ocps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (oclvp == oclvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (oclvp - oclvals);
                    size_t s = (size_t) (oclvlim - oclvals);

                    s += OCLVQUEUEGROWTH;
                    if ((oclexemes = (OCINT *)realloc(oclexemes, s * sizeof(OCINT))) == NULL)
                        goto ocenomem;
                    if ((oclvals   = (OCSTYPE *)realloc(oclvals, s * sizeof(OCSTYPE))) == NULL)
                        goto ocenomem;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                    if ((oclpsns   = (OCLTYPE *)realloc(oclpsns, s * sizeof(OCLTYPE))) == NULL)
                        goto ocenomem;
#endif
                    oclvp   = oclve = oclvals + p;
                    oclvlim = oclvals + s;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                    oclpp   = oclpe = oclpsns + p;
                    oclplim = oclpsns + s;
#endif
                    oclexp  = oclexemes + p;
                }
                *oclexp = (OCINT) OCLEX;
                *oclvp++ = oclval;
                oclve++;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
                *oclpp++ = oclloc;
                oclpe++;
#endif
                occhar = *oclexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* OCBTYACC */
            occhar = OCLEX;
#if OCBTYACC
            } while (0);
#endif /* OCBTYACC */
            if (occhar < 0) occhar = OCEOF;
#if OCDEBUG
            if (ocdebug)
            {
                if ((ocs = ocname[OCTRANSLATE(occhar)]) == NULL) ocs = ocname[OCUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                OCDEBUGSTR, ocdepth, OCFINAL, occhar, ocs);
            }
#endif
        }
        if (occhar == OCEOF) goto ocaccept;
        goto ocloop;
    }
    if (((ocn = ocgindex[ocm]) != 0) && (ocn += ocstate) >= 0 &&
            ocn <= OCTABLESIZE && occheck[ocn] == (OCINT) ocstate)
        ocstate = octable[ocn];
    else
        ocstate = ocdgoto[ocm];
#if OCDEBUG
    if (ocdebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", OCDEBUGSTR, ocdepth);
#ifdef OCSTYPE_TOSTRING
#if OCBTYACC
        if (!octrial)
#endif /* OCBTYACC */
            fprintf(stderr, "result is <%s>, ", OCSTYPE_TOSTRING(ocstos[ocstate], ocval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *ocstack.s_mark, ocstate);
    }
#endif
    if (ocstack.s_mark >= ocstack.s_last && ocgrowstack(&ocstack) == OCENOMEM) goto ocoverflow;
    *++ocstack.s_mark = (OCINT) ocstate;
    *++ocstack.l_mark = ocval;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    *++ocstack.p_mark = ocloc;
#endif
    goto ocloop;
#if OCBTYACC

    /* Reduction declares that this path is valid. Set ocpath and do a full parse */
ocvalid:
    if (ocpath) OCABORT;
    while (ocps->save)
    {
        OCParseState *save = ocps->save;
        ocps->save = save->save;
        save->save = ocpath;
        ocpath = save;
    }
#if OCDEBUG
    if (ocdebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        OCDEBUGSTR, ocdepth, ocstate, ocpath->state, (int)(oclvp - oclvals - ocpath->lexeme));
#endif
    if (ocerrctx)
    {
        ocFreeState(ocerrctx);
        ocerrctx = NULL;
    }
    oclvp          = oclvals + ocpath->lexeme;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    oclpp          = oclpsns + ocpath->lexeme;
#endif
    oclexp         = oclexemes + ocpath->lexeme;
    occhar         = OCEMPTY;
    ocstack.s_mark = ocstack.s_base + (ocpath->ocstack.s_mark - ocpath->ocstack.s_base);
    memcpy (ocstack.s_base, ocpath->ocstack.s_base, (size_t) (ocstack.s_mark - ocstack.s_base + 1) * sizeof(OCINT));
    ocstack.l_mark = ocstack.l_base + (ocpath->ocstack.l_mark - ocpath->ocstack.l_base);
    memcpy (ocstack.l_base, ocpath->ocstack.l_base, (size_t) (ocstack.l_mark - ocstack.l_base + 1) * sizeof(OCSTYPE));
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
    ocstack.p_mark = ocstack.p_base + (ocpath->ocstack.p_mark - ocpath->ocstack.p_base);
    memcpy (ocstack.p_base, ocpath->ocstack.p_base, (size_t) (ocstack.p_mark - ocstack.p_base + 1) * sizeof(OCLTYPE));
#endif
    ocstate        = ocpath->state;
    goto ocloop;
#endif /* OCBTYACC */

ocoverflow:
    OCERROR_CALL("yacc stack overflow");
#if OCBTYACC
    goto ocabort_nomem;
ocenomem:
    OCERROR_CALL("memory exhausted");
ocabort_nomem:
#endif /* OCBTYACC */
    ocresult = 2;
    goto ocreturn;

ocabort:
    ocresult = 1;
    goto ocreturn;

ocaccept:
#if OCBTYACC
    if (ocps->save) goto ocvalid;
#endif /* OCBTYACC */
    ocresult = 0;

ocreturn:
#if defined(OCDESTRUCT_CALL)
    if (occhar != OCEOF && occhar != OCEMPTY)
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        OCDESTRUCT_CALL("cleanup: discarding token", occhar, &oclval, &oclloc);
#else
        OCDESTRUCT_CALL("cleanup: discarding token", occhar, &oclval);
#endif /* defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED) */

    {
        OCSTYPE *pv;
#if defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED)
        OCLTYPE *pp;

        for (pv = ocstack.l_base, pp = ocstack.p_base; pv <= ocstack.l_mark; ++pv, ++pp)
             OCDESTRUCT_CALL("cleanup: discarding state",
                             ocstos[*(ocstack.s_base + (pv - ocstack.l_base))], pv, pp);
#else
        for (pv = ocstack.l_base; pv <= ocstack.l_mark; ++pv)
             OCDESTRUCT_CALL("cleanup: discarding state",
                             ocstos[*(ocstack.s_base + (pv - ocstack.l_base))], pv);
#endif /* defined(OCLTYPE) || defined(OCLTYPE_IS_DECLARED) */
    }
#endif /* defined(OCDESTRUCT_CALL) */

#if OCBTYACC
    if (ocerrctx)
    {
        ocFreeState(ocerrctx);
        ocerrctx = NULL;
    }
    while (ocps)
    {
        OCParseState *save = ocps;
        ocps = save->save;
        save->save = NULL;
        ocFreeState(save);
    }
    while (ocpath)
    {
        OCParseState *save = ocpath;
        ocpath = save->save;
        save->save = NULL;
        ocFreeState(save);
    }
#endif /* OCBTYACC */
    ocfreestack(&ocstack);
    return (ocresult);
}
