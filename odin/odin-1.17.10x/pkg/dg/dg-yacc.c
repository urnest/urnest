/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20140715

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}
#line 31 "y.tab.c"

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
typedef short YYINT;
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
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 256
#define YYUNDFTOKEN 288
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,"TOK_BANNER","TOK_NEEDS","TOK_Word","TOK_Colon","TOK_Plus",
"TOK_Equals","TOK_LeftParen","TOK_RightParen","TOK_Percent","TOK_Slash",
"TOK_Semicolon","TOK_Question","TOK_LeftAngle","TOK_RightAngle","TOK_Ampersand",
"TOK_At","TOK_Asterisk","TOK_Dollar",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,"illegal-symbol",
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

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

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

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
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

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

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
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 36 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(1,1);}
break;
case 2:
#line 42 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 4:
#line 47 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,0);}
break;
case 5:
#line 50 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(2,1);}
break;
case 6:
#line 55 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(3,2);}
break;
case 7:
#line 61 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(5,3);}
break;
case 8:
#line 67 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(4,3);}
break;
case 9:
#line 73 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(7,3);}
break;
case 10:
#line 80 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(6,4);}
break;
case 11:
#line 85 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 13:
#line 92 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(8,1);}
break;
case 14:
#line 95 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(9,1);}
break;
case 16:
#line 102 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,0);}
break;
case 19:
#line 119 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 21:
#line 126 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(10,1);}
break;
case 22:
#line 132 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(11,1);}
break;
case 24:
#line 138 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(12,1);}
break;
case 25:
#line 143 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 27:
#line 150 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(13,1);}
break;
case 28:
#line 151 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(13,0);}
break;
case 29:
#line 156 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 31:
#line 162 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(16,1);}
break;
case 32:
#line 164 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(16,1);}
break;
case 33:
#line 169 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 36:
#line 183 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(17,1);}
break;
case 39:
#line 192 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(14,1);}
break;
case 40:
#line 195 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(15,1);}
break;
case 41:
#line 201 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(18,1);}
break;
case 43:
#line 208 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 45:
#line 217 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(21,0);}
break;
case 47:
#line 221 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(19,1);}
break;
case 48:
#line 223 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(20,1);}
break;
case 49:
#line 225 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(22,0);}
break;
case 50:
#line 228 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(23,1);}
break;
case 51:
#line 231 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(30,1);}
break;
case 52:
#line 236 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(24,2);}
break;
case 53:
#line 239 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(25,2);}
break;
case 54:
#line 241 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(27,1);}
break;
case 55:
#line 245 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(28,2);}
break;
case 56:
#line 248 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(31,1);}
break;
case 57:
#line 250 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(32,0);}
break;
case 58:
#line 253 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(30,1);}
break;
case 60:
#line 260 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,0);}
break;
case 61:
#line 265 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(26,1);}
break;
case 62:
#line 270 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(0,2);}
break;
case 67:
#line 285 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(33,1);}
break;
case 68:
#line 287 "/home/xju/tmp/odc930/xjutv/FILES/a/drvgrf.ygi.108793.y"
	{Action(34,-1);}
break;
#line 714 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
