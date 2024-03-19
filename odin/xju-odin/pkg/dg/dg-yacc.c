/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20221106

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#undef YYBTYACC
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}
#line 34 "y.tab.c"

#if ! defined(YYSTYPE) && ! defined(YYSTYPE_IS_DECLARED)
/* Default: YYSTYPE is the semantic value type. */
typedef int YYSTYPE;
# define YYSTYPE_IS_DECLARED 1
#endif

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define TOK_BANNER 2
#define TOK_NEEDS 3
#define TOK_Word 4
#define TOK_Colon 5
#define TOK_Plus 6
#define TOK_Equals 7
#define TOK_LeftParen 8
#define TOK_RightParen 9
#define TOK_Percent 10
#define TOK_Slash 11
#define TOK_Semicolon 12
#define TOK_Question 13
#define TOK_LeftAngle 14
#define TOK_RightAngle 15
#define TOK_Ampersand 16
#define TOK_At 17
#define TOK_Asterisk 18
#define TOK_Dollar 19
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    1,    2,    2,    2,    2,    2,    2,    2,
    7,    7,    4,    4,   14,   14,    8,    5,   15,    6,
    6,   13,   13,   10,   16,   16,   11,   11,   18,   18,
   12,   12,   19,   19,   21,   20,   17,   17,   17,   17,
   23,   24,   24,   24,   25,   25,   25,   25,   25,   25,
   25,   26,   26,   26,   26,   26,   26,   26,   27,   27,
   28,   29,   29,    9,    9,   22,   22,    3,
};
static const YYINT yylen[] = {                            2,
    1,    3,    1,    0,    2,    4,    5,    5,    5,    6,
    2,    1,    2,    2,    1,    0,    2,    2,    4,    2,
    1,    3,    1,    1,    2,    1,    2,    0,    2,    1,
    1,    1,    2,    1,    3,    3,    1,    1,    2,    2,
    3,    1,    2,    3,    2,    1,    1,    1,    1,    2,
    2,    2,    2,    1,    3,    2,    1,    2,    1,    0,
    2,    2,    1,    1,    1,    1,    2,    1,
};
static const YYINT yydefred[] = {                         0,
    0,   68,    0,    0,    0,    0,    0,    0,    3,    0,
    0,    0,    0,    5,   18,   17,   15,   14,    0,    0,
    0,   13,    0,   66,    0,    0,   26,   37,    0,    0,
    0,    0,    0,    0,    2,    0,    0,    0,   47,   48,
   46,    0,   42,   67,    0,    0,   25,   40,   39,    0,
   20,    0,    0,    0,   45,    0,   51,   50,   41,    0,
    0,    0,    0,   43,   30,    0,    0,    6,    0,    0,
    9,   64,   65,   44,   58,   56,    0,    0,   53,   52,
   59,   29,    0,    0,   23,    0,   12,    8,   55,   63,
    0,    0,   10,    0,   32,   34,    0,   11,   62,    0,
    0,    0,    0,   33,   22,    0,   35,   36,    0,   19,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
    2,    4,    5,    6,   18,   19,  258,  259,  260,  261,
  262,  263,  266,  261,  261,  261,  261,  272,  261,   12,
    8,   18,   19,  261,  268,  274,  275,  280,  281,    7,
  261,  264,  264,  264,  260,    8,   10,   11,  263,  266,
  280,  282,  283,  261,    3,  269,  275,   16,   17,   15,
   13,    7,    7,    7,    9,  282,  280,  280,    9,   10,
   11,  263,  266,  284,  275,  276,    7,  263,   15,   15,
  267,  280,  281,    9,  280,  280,    7,    7,  281,  285,
  286,  275,   15,   14,  263,  265,  271,  263,  263,  267,
  287,    8,  270,  277,  278,  279,  263,  271,  267,    5,
  263,  273,    8,  279,   15,  261,    9,    9,   18,  261,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                          7,
    8,    9,   24,   11,   12,   32,   86,   13,   71,   25,
   46,   93,   87,   18,  102,   26,   27,   66,   94,   95,
   96,   28,   29,   42,   43,   64,   80,   81,   91,
};
static const YYINT yysindex[] = {                        30,
    8,    0,    8,    8,    8,    8,    0,    5,    0,   34,
   15,    8,    8,    0,    0,    0,    0,    0,    8,   30,
   88,    0,    8,    0,   36,   57,    0,    0,   50,   65,
   72,   70,   84,   98,    0,   78,   11,   11,    0,    0,
    0,  103,    0,    0,   57,   99,    0,    0,    0,   90,
    0,   96,  102,   57,    0,  110,    0,    0,    0,   11,
   11,  111,   95,    0,    0,   57,  109,    0,   64,   90,
    0,    0,    0,    0,    0,    0,   90,   57,    0,    0,
    0,    0,  123,   90,    0,   64,    0,    0,    0,    0,
   57,  120,    0,  130,    0,    0,  137,    0,    0,    8,
  136,  144,   90,    0,    0,  114,    0,    0,    8,    0,
};
static const YYINT yyrindex[] = {                        60,
    0,    0,    0,    0,  132,    0,    0,  146,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   60,
    0,    0,    0,    0,  150,   97,    0,    0,   51,    0,
  151,    0,    0,    0,    0,    0,    0,  117,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  124,  131,  138,    0,    0,  152,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   62,    0,    0,    0,    0,
  145,    0,    0,   63,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  153,    0,    0,    0,    0,
};
#if YYBTYACC
static const YYINT yycindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    0,  140,    1,    0,  -13,   28,    0,  -11,  -75,    0,
    0,    0,   75,    0,    0,    0,  -26,    0,    0,    0,
   69,  -10,  -45,  128,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 164
static const YYINT yytable[] = {                         47,
   10,   14,   90,   15,   16,   17,   19,   39,   73,   40,
   41,    2,   31,   31,    2,   99,   20,   79,   65,   31,
   10,   30,   39,   44,   40,   41,   57,   58,   62,   23,
   63,    1,   73,    2,    3,    4,   68,    2,   45,   82,
   33,   21,   62,   72,   63,   73,   34,    5,    6,   75,
   76,   22,   23,   38,   38,   85,   88,   38,   38,    4,
    2,    7,   31,   89,   21,   48,   49,   72,    3,   38,
   97,    4,   85,    7,   31,   23,   52,   84,  101,   50,
   72,    2,    3,    4,   51,   36,   55,   37,   38,  101,
   53,    2,    3,    4,    3,   36,   23,   37,   38,   24,
  106,   78,   21,   24,   54,   67,   23,    3,    4,  110,
   69,   59,   60,   61,    3,    4,   70,   77,   74,   60,
   61,   49,   49,   83,  100,   49,   49,   49,   57,   57,
   92,  109,   57,   57,   57,   54,   54,  103,   16,   54,
   54,   54,   60,   60,  107,    1,   60,   60,   60,   61,
   61,  105,  108,   61,   61,   61,   28,   21,   27,   35,
   98,   18,  104,   56,
};
static const YYINT yycheck[] = {                         26,
    0,    1,   78,    3,    4,    5,    6,   21,   54,   21,
   21,    4,   12,   13,    4,   91,   12,   63,   45,   19,
   20,    7,   36,   23,   36,   36,   37,   38,   42,   19,
   42,    2,   78,    4,    5,    6,   50,    4,    3,   66,
   13,    8,   56,   54,   56,   91,   19,   18,   19,   60,
   61,   18,   19,    3,    4,   69,   70,    7,    8,    0,
    4,    0,    0,   77,    8,   16,   17,   78,    5,   19,
   84,   12,   86,   12,   12,   19,    7,   14,   92,   15,
   91,    4,    5,    6,   13,    8,    9,   10,   11,  103,
    7,    4,    5,    6,    5,    8,   19,   10,   11,    3,
  100,    7,    8,    7,    7,    7,   19,    5,    6,  109,
   15,    9,   10,   11,    5,    6,   15,    7,    9,   10,
   11,    5,    6,   15,    5,    9,   10,   11,    5,    6,
    8,   18,    9,   10,   11,    5,    6,    8,    7,    9,
   10,   11,    5,    6,    9,    0,    9,   10,   11,    5,
    6,   15,    9,    9,   10,   11,    7,    7,    7,   20,
   86,    9,   94,   36,
};
#if YYBTYACC
static const YYINT yyctable[] = {                        -1,
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
   -1,   -1,   -1,
};
#endif
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 256
#define YYUNDFTOKEN 288
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"$end",0,"TOK_BANNER","TOK_NEEDS","TOK_Word","TOK_Colon","TOK_Plus",
"TOK_Equals","TOK_LeftParen","TOK_RightParen","TOK_Percent","TOK_Slash",
"TOK_Semicolon","TOK_Question","TOK_LeftAngle","TOK_RightAngle","TOK_Ampersand",
"TOK_At","TOK_Asterisk","TOK_Dollar",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error","$accept","xDerivationGraph",
"xDerivationGraph_1","xDGEntry","TOK_Wordleaf","xSourceType","xObjectType",
"xDesc","xDGEntry_1","xParameterType","xParameterValue","xArgs","xNeeds",
"xResults","xSuperType","xSourceType_1","xForeignObjectType","xArgs_1","xArg",
"xNeeds_1","xResults_1","xForeignResult","xResult","xVarWord","xFileArg",
"xOdinExpr1","xRoot","xOperation","xOperation_1","xParameterValues",
"xParameterValues_1","illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : xDerivationGraph",
"xDerivationGraph : xDerivationGraph_1",
"xDerivationGraph_1 : xDerivationGraph_1 TOK_Semicolon xDGEntry",
"xDerivationGraph_1 : xDGEntry",
"xDGEntry :",
"xDGEntry : TOK_BANNER TOK_Wordleaf",
"xDGEntry : xSourceType TOK_Equals TOK_RightAngle xObjectType",
"xDGEntry : xObjectType xDesc TOK_Equals TOK_RightAngle xDGEntry_1",
"xDGEntry : xParameterType xDesc TOK_Equals TOK_RightAngle xObjectType",
"xDGEntry : TOK_Dollar TOK_Wordleaf xDesc TOK_Equals xParameterValue",
"xDGEntry : TOK_Wordleaf xArgs xNeeds TOK_Equals TOK_RightAngle xResults",
"xDGEntry_1 : xDGEntry_1 xSuperType",
"xDGEntry_1 : xSuperType",
"xSourceType : TOK_Wordleaf TOK_Asterisk",
"xSourceType : TOK_Asterisk xSourceType_1",
"xSourceType_1 : TOK_Wordleaf",
"xSourceType_1 :",
"xParameterType : TOK_Plus TOK_Wordleaf",
"xObjectType : TOK_Colon TOK_Wordleaf",
"xForeignObjectType : TOK_Colon TOK_Wordleaf TOK_Asterisk TOK_Wordleaf",
"xDesc : TOK_Wordleaf TOK_Question",
"xDesc : TOK_Wordleaf",
"xSuperType : TOK_LeftAngle xObjectType TOK_RightAngle",
"xSuperType : xObjectType",
"xArgs : xArgs_1",
"xArgs_1 : xArgs_1 xArg",
"xArgs_1 : xArg",
"xNeeds : TOK_NEEDS xNeeds_1",
"xNeeds :",
"xNeeds_1 : xNeeds_1 xArg",
"xNeeds_1 : xArg",
"xResults : xResults_1",
"xResults : xForeignResult",
"xResults_1 : xResults_1 xResult",
"xResults_1 : xResult",
"xResult : TOK_LeftParen xObjectType TOK_RightParen",
"xForeignResult : TOK_LeftParen xForeignObjectType TOK_RightParen",
"xArg : xVarWord",
"xArg : xFileArg",
"xArg : xFileArg TOK_At",
"xArg : xFileArg TOK_Ampersand",
"xFileArg : TOK_LeftParen xOdinExpr1 TOK_RightParen",
"xOdinExpr1 : xRoot",
"xOdinExpr1 : xOdinExpr1 xOperation",
"xOdinExpr1 : TOK_LeftParen xOdinExpr1 TOK_RightParen",
"xRoot : TOK_LeftParen TOK_RightParen",
"xRoot : xVarWord",
"xRoot : xObjectType",
"xRoot : xParameterType",
"xRoot : TOK_Slash",
"xRoot : TOK_Slash xVarWord",
"xRoot : TOK_Percent xVarWord",
"xOperation : xParameterType xOperation_1",
"xOperation : xParameterType xFileArg",
"xOperation : xObjectType",
"xOperation : xObjectType TOK_Equals xObjectType",
"xOperation : TOK_Slash xVarWord",
"xOperation : TOK_Slash",
"xOperation : TOK_Percent xVarWord",
"xOperation_1 : xParameterValues",
"xOperation_1 :",
"xParameterValues : TOK_Equals xParameterValues_1",
"xParameterValues_1 : xParameterValues_1 xParameterValue",
"xParameterValues_1 : xParameterValue",
"xParameterValue : xVarWord",
"xParameterValue : xFileArg",
"xVarWord : TOK_Wordleaf",
"xVarWord : TOK_Dollar TOK_Wordleaf",
"TOK_Wordleaf : TOK_Word",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE  yyloc; /* position returned by actions */
YYLTYPE  yylloc; /* position from the lexer */
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 0).last_line; \
        (loc).first_column = YYRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = YYRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 1).first_line; \
        (loc).first_column = YYRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = YYRHSLOC(rhs, n).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* YYLLOC_DEFAULT */
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#if YYBTYACC

#ifndef YYLVQUEUEGROWTH
#define YYLVQUEUEGROWTH 32
#endif
#endif /* YYBTYACC */

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#ifndef YYINITSTACKSIZE
#define YYINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  *p_base;
    YYLTYPE  *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
    struct YYParseState_s *save;    /* Previously saved parser state */
    YYSTACKDATA            yystack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    YYINT                  ctry;    /* saved index in yyctable[] for this conflict */
};
typedef struct YYParseState_s YYParseState;
#endif /* YYBTYACC */
/* variables for the parser stack */
static YYSTACKDATA yystack;
#if YYBTYACC

/* Current parser state */
static YYParseState *yyps = 0;

/* yypath != NULL: do the full parse, starting at *yypath parser state. */
static YYParseState *yypath = 0;

/* Base of the lexical value queue */
static YYSTYPE *yylvals = 0;

/* Current position at lexical value queue */
static YYSTYPE *yylvp = 0;

/* End position of lexical value queue */
static YYSTYPE *yylve = 0;

/* The last allocated position at the lexical value queue */
static YYSTYPE *yylvlim = 0;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static YYLTYPE *yylpsns = 0;

/* Current position at lexical position queue */
static YYLTYPE *yylpp = 0;

/* End position of lexical position queue */
static YYLTYPE *yylpe = 0;

/* The last allocated position at the lexical position queue */
static YYLTYPE *yylplim = 0;
#endif

/* Current position at lexical token queue */
static YYINT  *yylexp = 0;

static YYINT  *yylexemes = 0;
#endif /* YYBTYACC */

/* For use in generated program */
#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif /* YYBTYACC */

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE *newps;
#endif

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == 0)
        return YYENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif /* YYPURE || defined(YY_NO_LEAKS) */
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
    YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
    if (p == NULL) return NULL;

    p->yystack.stacksize = size;
    if (size == 0)
    {
        p->yystack.s_base = NULL;
        p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        p->yystack.p_base = NULL;
#endif
        return p;
    }
    p->yystack.s_base    = (YYINT *) malloc(size * sizeof(YYINT));
    if (p->yystack.s_base == NULL) return NULL;
    p->yystack.l_base    = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
    if (p->yystack.l_base == NULL) return NULL;
    memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    p->yystack.p_base    = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
    if (p->yystack.p_base == NULL) return NULL;
    memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

    return p;
}

static void
yyFreeState(YYParseState *p)
{
    yyfreestack(&p->yystack);
    free(p);
}
#endif /* YYBTYACC */

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#if YYBTYACC
#define YYVALID        do { if (yyps->save)            goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && \
                                yyps->save->save == 0) goto yyvalid; } while(0)
#endif /* YYBTYACC */

int
YYPARSE_DECL()
{
    int yym, yyn, yystate, yyresult;
#if YYBTYACC
    int yynewerrflag;
    YYParseState *yyerrctx = NULL;
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  yyerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
    if (yydebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

#if YYBTYACC
    yyps = yyNewState(0); if (yyps == 0) goto yyenomem;
    yyps->save = 0;
#endif /* YYBTYACC */
    yym = 0;
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base;
#endif
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
#if YYBTYACC
        do {
        if (yylvp < yylve)
        {
            /* we're currently re-reading tokens */
            yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc = *yylpp++;
#endif
            yychar = *yylexp++;
            break;
        }
        if (yyps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (yylvp == yylvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (yylvp - yylvals);
                size_t s = (size_t) (yylvlim - yylvals);

                s += YYLVQUEUEGROWTH;
                if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
                if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
                yylvp   = yylve = yylvals + p;
                yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp   = yylpe = yylpsns + p;
                yylplim = yylpsns + s;
#endif
                yylexp  = yylexemes + p;
            }
            *yylexp = (YYINT) YYLEX;
            *yylvp++ = yylval;
            yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *yylpp++ = yylloc;
            yylpe++;
#endif
            yychar = *yylexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
        yychar = YYLEX;
#if YYBTYACC
        } while (0);
#endif /* YYBTYACC */
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if YYBTYACC

    /* Do we have a conflict? */
    if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
        yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        YYINT ctry;

        if (yypath)
        {
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                YYDEBUGSTR, yydepth, yystate);
#endif
            /* Switch to the next conflict context */
            save = yypath;
            yypath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != yystate) YYABORT;
            yyFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
                if (yyps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (save == NULL) goto yyenomem;
            save->save            = yyps->save;
            save->state           = yystate;
            save->errflag         = yyerrflag;
            save->yystack.s_mark  = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            save->yystack.l_mark  = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            save->yystack.p_mark  = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            ctry                  = yytable[yyn];
            if (yyctable[ctry] == -1)
            {
#if YYDEBUG
                if (yydebug && yychar >= YYEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (yyps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!yylexemes)
                {
                    yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
                    if (yylexemes == NULL) goto yyenomem;
                    yylvals   = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
                    if (yylvals == NULL) goto yyenomem;
                    yylvlim   = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpsns   = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
                    if (yylpsns == NULL) goto yyenomem;
                    yylplim   = yylpsns + YYLVQUEUEGROWTH;
#endif
                }
                if (yylvp == yylve)
                {
                    yylvp  = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp  = yylpe = yylpsns;
#endif
                    yylexp = yylexemes;
                    if (yychar >= YYEOF)
                    {
                        *yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                        *yylpe++ = yylloc;
#endif
                        *yylexp  = (YYINT) yychar;
                        yychar   = YYEMPTY;
                    }
                }
            }
            if (yychar >= YYEOF)
            {
                yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp--;
#endif
                yylexp--;
                yychar = YYEMPTY;
            }
            save->lexeme = (int) (yylvp - yylvals);
            yyps->save   = save;
        }
        if (yytable[yyn] == ctry)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
            if (yychar < 0)
            {
                yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp++;
#endif
                yylexp++;
            }
            if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                goto yyoverflow;
            yystate = yyctable[ctry];
            *++yystack.s_mark = (YYINT) yystate;
            *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *++yystack.p_mark = yylloc;
#endif
            yychar  = YYEMPTY;
            if (yyerrflag > 0) --yyerrflag;
            goto yyloop;
        }
        else
        {
            yyn = yyctable[ctry];
            goto yyreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* YYBTYACC */
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yylloc;
#endif
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

    yynewerrflag = 1;
    goto yyerrhandler;
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */

yyerrlab:
    /* explicit YYERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif

    yynewerrflag = 0;
yyerrhandler:
    while (yyps->save)
    {
        int ctry;
        YYParseState *save = yyps->save;
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            YYDEBUGSTR, yydepth, yystate, yyps->save->state,
                    (int)(yylvp - yylvals - yyps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
        {
            /* Free old saved error context state */
            if (yyerrctx) yyFreeState(yyerrctx);
            /* Create and fill out new saved error context state */
            yyerrctx                 = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (yyerrctx == NULL) goto yyenomem;
            yyerrctx->save           = yyps->save;
            yyerrctx->state          = yystate;
            yyerrctx->errflag        = yyerrflag;
            yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yyerrctx->lexeme         = (int) (yylvp - yylvals);
        }
        yylvp          = yylvals   + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yylpp          = yylpsns   + save->lexeme;
#endif
        yylexp         = yylexemes + save->lexeme;
        yychar         = YYEMPTY;
        yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
        memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
        yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
        memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
        memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
        ctry           = ++save->ctry;
        yystate        = save->state;
        /* We tried shift, try reduce now */
        if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
        yyps->save     = save->save;
        save->save     = NULL;
        yyFreeState(save);

        /* Nothing left on the stack -- error */
        if (!yyps->save)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                YYPREFIX, yydepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            yylvp          = yylvals   + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylpp          = yylpsns   + yyerrctx->lexeme;
#endif
            yylexp         = yylexemes + yyerrctx->lexeme;
            yychar         = yylexp[-1];
            yylval         = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc         = yylpp[-1];
#endif
            yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
            memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
            memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
            memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yystate        = yyerrctx->state;
            yyFreeState(yyerrctx);
            yyerrctx       = NULL;
        }
        yynewerrflag = 1;
    }
    if (yynewerrflag == 0) goto yyinrecovery;
#endif /* YYBTYACC */

    YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yyerror_loc_range[1] = yylloc; /* lookahead position is error start position */
#endif

#if !YYBTYACC
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
#endif
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                yyerror_loc_range[2] = yylloc;
                YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2); /* position of error span */
                *++yystack.p_mark = yyloc;
#endif
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
                if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
                --yystack.s_mark;
                --yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                --yystack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
        }
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
    yym = yylen[yyn];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            if (yym > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = yym; i > 0; i--)
                {
                    if (i != yym) fputs(", ", stderr);
                    fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
                                           yystack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
    if (!yytrial)
#endif /* YYBTYACC */
    {
        YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);
        /* just in case YYERROR is invoked within the action, save
           the start of the rhs as the error start position */
        yyerror_loc_range[1] = yystack.p_mark[1-yym];
    }
#endif

    switch (yyn)
    {
case 1:
#line 36 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(1,1);}
#line 1154 "y.tab.c"
break;
case 2:
#line 42 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1159 "y.tab.c"
break;
case 4:
#line 47 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,0);}
#line 1164 "y.tab.c"
break;
case 5:
#line 50 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(2,1);}
#line 1169 "y.tab.c"
break;
case 6:
#line 55 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(3,2);}
#line 1174 "y.tab.c"
break;
case 7:
#line 61 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(5,3);}
#line 1179 "y.tab.c"
break;
case 8:
#line 67 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(4,3);}
#line 1184 "y.tab.c"
break;
case 9:
#line 73 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(7,3);}
#line 1189 "y.tab.c"
break;
case 10:
#line 80 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(6,4);}
#line 1194 "y.tab.c"
break;
case 11:
#line 85 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1199 "y.tab.c"
break;
case 13:
#line 92 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(8,1);}
#line 1204 "y.tab.c"
break;
case 14:
#line 95 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(9,1);}
#line 1209 "y.tab.c"
break;
case 16:
#line 102 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,0);}
#line 1214 "y.tab.c"
break;
case 19:
#line 119 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1219 "y.tab.c"
break;
case 21:
#line 126 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(10,1);}
#line 1224 "y.tab.c"
break;
case 22:
#line 132 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(11,1);}
#line 1229 "y.tab.c"
break;
case 24:
#line 138 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(12,1);}
#line 1234 "y.tab.c"
break;
case 25:
#line 143 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1239 "y.tab.c"
break;
case 27:
#line 150 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(13,1);}
#line 1244 "y.tab.c"
break;
case 28:
#line 151 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(13,0);}
#line 1249 "y.tab.c"
break;
case 29:
#line 156 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1254 "y.tab.c"
break;
case 31:
#line 162 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(16,1);}
#line 1259 "y.tab.c"
break;
case 32:
#line 164 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(16,1);}
#line 1264 "y.tab.c"
break;
case 33:
#line 169 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1269 "y.tab.c"
break;
case 36:
#line 183 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(17,1);}
#line 1274 "y.tab.c"
break;
case 39:
#line 192 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(14,1);}
#line 1279 "y.tab.c"
break;
case 40:
#line 195 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(15,1);}
#line 1284 "y.tab.c"
break;
case 41:
#line 201 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(18,1);}
#line 1289 "y.tab.c"
break;
case 43:
#line 208 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1294 "y.tab.c"
break;
case 45:
#line 217 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(21,0);}
#line 1299 "y.tab.c"
break;
case 47:
#line 221 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(19,1);}
#line 1304 "y.tab.c"
break;
case 48:
#line 223 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(20,1);}
#line 1309 "y.tab.c"
break;
case 49:
#line 225 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(22,0);}
#line 1314 "y.tab.c"
break;
case 50:
#line 228 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(23,1);}
#line 1319 "y.tab.c"
break;
case 51:
#line 231 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(30,1);}
#line 1324 "y.tab.c"
break;
case 52:
#line 236 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(24,2);}
#line 1329 "y.tab.c"
break;
case 53:
#line 239 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(25,2);}
#line 1334 "y.tab.c"
break;
case 54:
#line 241 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(27,1);}
#line 1339 "y.tab.c"
break;
case 55:
#line 245 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(28,2);}
#line 1344 "y.tab.c"
break;
case 56:
#line 248 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(31,1);}
#line 1349 "y.tab.c"
break;
case 57:
#line 250 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(32,0);}
#line 1354 "y.tab.c"
break;
case 58:
#line 253 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(30,1);}
#line 1359 "y.tab.c"
break;
case 60:
#line 260 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,0);}
#line 1364 "y.tab.c"
break;
case 61:
#line 265 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(26,1);}
#line 1369 "y.tab.c"
break;
case 62:
#line 270 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(0,2);}
#line 1374 "y.tab.c"
break;
case 67:
#line 285 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(33,1);}
#line 1379 "y.tab.c"
break;
case 68:
#line 287 "/home/xju/tmp/oc122/xjutv/FILES/x/q/drvgrf.ygi.41886701.y"
	{Action(34,-1);}
#line 1384 "y.tab.c"
break;
#line 1386 "y.tab.c"
    default:
        break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
        }
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yyloc;
#endif
        if (yychar < 0)
        {
#if YYBTYACC
            do {
            if (yylvp < yylve)
            {
                /* we're currently re-reading tokens */
                yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylloc = *yylpp++;
#endif
                yychar = *yylexp++;
                break;
            }
            if (yyps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (yylvp == yylvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (yylvp - yylvals);
                    size_t s = (size_t) (yylvlim - yylvals);

                    s += YYLVQUEUEGROWTH;
                    if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
                        goto yyenomem;
                    if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
                        goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
                        goto yyenomem;
#endif
                    yylvp   = yylve = yylvals + p;
                    yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp   = yylpe = yylpsns + p;
                    yylplim = yylpsns + s;
#endif
                    yylexp  = yylexemes + p;
                }
                *yylexp = (YYINT) YYLEX;
                *yylvp++ = yylval;
                yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                *yylpp++ = yylloc;
                yylpe++;
#endif
                yychar = *yylexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
            yychar = YYLEX;
#if YYBTYACC
            } while (0);
#endif /* YYBTYACC */
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
    }
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    *++yystack.p_mark = yyloc;
#endif
    goto yyloop;
#if YYBTYACC

    /* Reduction declares that this path is valid. Set yypath and do a full parse */
yyvalid:
    if (yypath) YYABORT;
    while (yyps->save)
    {
        YYParseState *save = yyps->save;
        yyps->save = save->save;
        save->save = yypath;
        yypath = save;
    }
#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    yylvp          = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yylpp          = yylpsns + yypath->lexeme;
#endif
    yylexp         = yylexemes + yypath->lexeme;
    yychar         = YYEMPTY;
    yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
    memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
    yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
    memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
    memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
    yystate        = yypath->state;
    goto yyloop;
#endif /* YYBTYACC */

yyoverflow:
    YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
    goto yyabort_nomem;
yyenomem:
    YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif /* YYBTYACC */
    yyresult = 2;
    goto yyreturn;

yyabort:
    yyresult = 1;
    goto yyreturn;

yyaccept:
#if YYBTYACC
    if (yyps->save) goto yyvalid;
#endif /* YYBTYACC */
    yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
    if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */

    {
        YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYLTYPE *pp;

        for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
        for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
    }
#endif /* defined(YYDESTRUCT_CALL) */

#if YYBTYACC
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    while (yyps)
    {
        YYParseState *save = yyps;
        yyps = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
    while (yypath)
    {
        YYParseState *save = yypath;
        yypath = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
#endif /* YYBTYACC */
    yyfreestack(&yystack);
    return (yyresult);
}
