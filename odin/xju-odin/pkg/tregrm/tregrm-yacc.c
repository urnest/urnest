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

#line 2 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
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

#define TOK_EOF_ 2
#define TOK_ERR_ 3
#define TOK_INCLUDE 4
#define TOK_SCANNER 5
#define TOK_NODES 6
#define TOK_RULES 7
#define TOK_Name 8
#define TOK_AString 9
#define TOK_QString 10
#define TOK_Equals 11
#define TOK_DoubleArrow 12
#define TOK_SingleArrow 13
#define TOK_Plus 14
#define TOK_Star 15
#define TOK_DoubleSlash 16
#define TOK_SemiColon 17
#define TOK_Question 18
#define TOK_LeftParen 19
#define TOK_RightParen 20
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    5,    5,    6,    6,    7,    7,    2,   10,
   10,   11,   11,   12,   12,   12,   12,    3,   14,   14,
   15,   15,    4,   16,   16,   17,   17,   18,   19,   20,
   20,   21,   23,   23,   22,   25,   25,   26,   26,   26,
   26,   26,   27,   27,   27,   27,   24,   24,   24,   28,
   13,    8,    9,
};
static const YYINT yylen[] = {                            2,
    4,    1,    2,    0,    2,    1,    1,    1,    1,    2,
    0,    2,    1,    3,    3,    3,    5,    1,    2,    0,
    2,    1,    1,    2,    0,    2,    1,    3,    1,    2,
    1,    3,    1,    0,    1,    2,    0,    3,    2,    2,
    2,    1,    3,    1,    1,    1,    2,    3,    4,    1,
    1,    1,    1,
};
static const YYINT yydefred[] = {                         0,
    0,    0,    0,    2,   52,   53,    0,    6,    7,    8,
    0,    0,    9,    5,    0,    0,    0,    0,    0,   13,
    0,    0,   18,    0,    0,    0,    0,   12,   22,    0,
    0,    1,   23,   51,   14,   15,   16,    0,   21,    0,
    0,   27,    0,   37,    0,    0,   31,   26,   17,    0,
    0,   28,   30,    0,   32,   33,   37,   46,   45,   44,
   36,    0,    0,   50,    0,    0,   39,   40,    0,   41,
    0,   48,   43,   38,   49,
};
static const YYINT yydgoto[] = {                          2,
    3,   12,   22,   32,    4,    7,    8,    9,   10,   13,
   19,   20,   40,   23,   30,   33,   41,   42,   45,   46,
   47,   50,   55,   56,   51,   61,   62,   65,
};
static const YYINT yysindex[] = {                         2,
   24,    0,   12,    0,    0,    0,   24,    0,    0,    0,
   28,   30,    0,    0,   40,   50,   58,   67,   28,    0,
   29,   35,    0,   62,   62,   62,   62,    0,    0,   29,
   62,    0,    0,    0,    0,    0,    0,   56,    0,   66,
   62,    0,   29,    0,   64,   66,    0,    0,    0,   70,
   16,    0,    0,    8,    0,    0,    0,    0,    0,    0,
    0,   59,   29,    0,   68,   69,    0,    0,   16,    0,
   71,    0,    0,    0,    0,
};
static const YYINT yyrindex[] = {                        49,
    0,    0,   60,    0,    0,    0,   57,    0,    0,    0,
    0,    7,    0,    0,    0,    0,    0,    0,   65,    0,
    0,   83,    0,    0,    0,    0,    0,    0,    0,   21,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   84,    0,    0,    0,    0,   73,    0,    0,    0,   31,
   33,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    3,    0,    0,   63,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
    0,    0,    0,    0,    0,    0,   78,  -10,  -11,    0,
    0,   74,  -22,    0,    0,    0,    0,   46,    0,    0,
   42,   37,    0,    0,    0,    0,   23,   32,
};
#define YYTABLESIZE 95
static const YYINT yytable[] = {                         18,
   17,   35,   36,   37,   38,    1,   20,   18,   17,   29,
   42,   42,   42,   20,   42,   42,   11,    6,   39,   42,
   19,   42,   42,   34,    5,    6,   63,   19,   60,   15,
   16,   49,    5,    6,   57,   21,    5,    6,    6,   59,
   58,   31,   64,   34,   35,   35,   60,   34,    4,   35,
   24,   64,   35,    4,    4,    4,    3,   59,   58,   11,
   25,    3,    3,    3,   10,   11,   11,   43,   26,   34,
   10,   10,   67,   68,   69,   47,   70,   27,   44,   47,
   52,   54,   25,   24,   14,   72,   48,   53,   73,   29,
   75,   74,   28,   66,   71,
};
static const YYINT yycheck[] = {                         11,
   11,   24,   25,   26,   27,    4,    0,   19,   19,   21,
    8,    9,   10,    7,   12,   13,    5,   10,   30,   17,
    0,   19,   20,    8,    9,   10,   19,    7,   51,    2,
    3,   43,    9,   10,   19,    6,    9,   10,   10,   51,
   51,    7,   54,   13,   12,   13,   69,   17,    0,   17,
   11,   63,   20,    5,    6,    7,    0,   69,   69,    0,
   11,    5,    6,    7,    0,    6,    7,   12,   11,    8,
    6,    7,   14,   15,   16,   13,   18,   11,   13,   17,
   17,   12,    0,    0,    7,   18,   41,   46,   20,   17,
   20,   69,   19,   57,   63,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 256
#define YYUNDFTOKEN 287
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,"TOK_EOF_","TOK_ERR_","TOK_INCLUDE","TOK_SCANNER","TOK_NODES",
"TOK_RULES","TOK_Name","TOK_AString","TOK_QString","TOK_Equals",
"TOK_DoubleArrow","TOK_SingleArrow","TOK_Plus","TOK_Star","TOK_DoubleSlash",
"TOK_SemiColon","TOK_Question","TOK_LeftParen","TOK_RightParen",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : xTreeGrammar",
"xTreeGrammar : xIncludes xScanner xNodes xRules",
"xIncludes : xIncludes_1",
"xIncludes_1 : TOK_INCLUDE xIncludes_101",
"xIncludes_1 :",
"xIncludes_101 : xIncludes_101 xIncludeFile",
"xIncludes_101 : xIncludeFile",
"xIncludeFile : TOK_AStringleaf",
"xIncludeFile : TOK_QStringleaf",
"xScanner : xScanner_1",
"xScanner_1 : TOK_SCANNER xScanner_101",
"xScanner_1 :",
"xScanner_101 : xScanner_101 xLexicalDefinition",
"xScanner_101 : xLexicalDefinition",
"xLexicalDefinition : TOK_EOF_ TOK_Equals TOK_Nameleaf",
"xLexicalDefinition : TOK_ERR_ TOK_Equals TOK_Nameleaf",
"xLexicalDefinition : TOK_AStringleaf TOK_Equals TOK_Nameleaf",
"xLexicalDefinition : TOK_QStringleaf TOK_Equals TOK_Nameleaf TOK_DoubleArrow TOK_QStringleaf",
"xNodes : xNodes_1",
"xNodes_1 : TOK_NODES xNodes_101",
"xNodes_1 :",
"xNodes_101 : xNodes_101 TOK_QStringleaf",
"xNodes_101 : TOK_QStringleaf",
"xRules : xRules_1",
"xRules_1 : TOK_RULES xRules_101",
"xRules_1 :",
"xRules_101 : xRules_101 xRule",
"xRules_101 : xRule",
"xRule : TOK_Nameleaf xAlternateList TOK_SemiColon",
"xAlternateList : xAlternateList_1",
"xAlternateList_1 : xAlternateList_1 xAlternate",
"xAlternateList_1 : xAlternate",
"xAlternate : TOK_SingleArrow xExpression xAlternate_1",
"xAlternate_1 : xTreeSpec",
"xAlternate_1 :",
"xExpression : xExpression_1",
"xExpression_1 : xExpression_1 xTerm",
"xExpression_1 :",
"xTerm : xElement TOK_DoubleSlash xElement",
"xTerm : xElement TOK_Plus",
"xTerm : xElement TOK_Star",
"xTerm : xElement TOK_Question",
"xTerm : xElement",
"xElement : TOK_LeftParen xExpression TOK_RightParen",
"xElement : TOK_Nameleaf",
"xElement : TOK_QStringleaf",
"xElement : TOK_AStringleaf",
"xTreeSpec : TOK_DoubleArrow xNodeName",
"xTreeSpec : TOK_DoubleArrow xNodeName TOK_Question",
"xTreeSpec : TOK_DoubleArrow TOK_LeftParen xNodeName TOK_RightParen",
"xNodeName : TOK_QStringleaf",
"TOK_Nameleaf : TOK_Name",
"TOK_AStringleaf : TOK_AString",
"TOK_QStringleaf : TOK_QString",

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
#line 40 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(1,4);}
break;
case 2:
#line 44 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(2,1);}
break;
case 4:
#line 52 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 5:
#line 57 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 9:
#line 69 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(3,1);}
break;
case 11:
#line 77 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 12:
#line 82 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 14:
#line 90 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(4,1);}
break;
case 15:
#line 94 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(5,1);}
break;
case 16:
#line 98 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(6,2);}
break;
case 17:
#line 104 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(7,3);}
break;
case 18:
#line 108 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(8,1);}
break;
case 20:
#line 116 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 21:
#line 121 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 23:
#line 127 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(9,1);}
break;
case 25:
#line 135 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 26:
#line 140 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 28:
#line 148 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(10,2);}
break;
case 29:
#line 152 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(11,1);}
break;
case 30:
#line 157 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 32:
#line 165 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(12,2);}
break;
case 34:
#line 172 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 35:
#line 176 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(-13,1);}
break;
case 36:
#line 181 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,2);}
break;
case 37:
#line 183 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(0,0);}
break;
case 38:
#line 189 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(14,2);}
break;
case 39:
#line 192 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(15,1);}
break;
case 40:
#line 195 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(16,1);}
break;
case 41:
#line 198 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(17,1);}
break;
case 48:
#line 221 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(18,1);}
break;
case 49:
#line 226 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(19,1);}
break;
case 51:
#line 232 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(20,-1);}
break;
case 52:
#line 234 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(21,-1);}
break;
case 53:
#line 236 "/home/xju/tmp/odc930/xjutv/FILES/e/tregrm.ygi.160653.y"
	{Action(22,-1);}
break;
#line 623 "y.tab.c"
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
