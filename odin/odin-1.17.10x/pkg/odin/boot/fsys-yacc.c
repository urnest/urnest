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

#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}

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
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    0,    2,    2,    1,    1,    1,    5,    7,    7,
    9,    8,    8,    8,   11,   11,   10,   10,    3,    3,
    4,   12,   12,   12,   13,   13,   13,   13,   13,   13,
   13,   13,   14,   14,   14,   14,   14,   14,   14,   18,
   18,   19,   20,   20,   21,   21,   16,   17,    6,   22,
   15,
};
static const YYINT yylen[] = {                            2,
    1,    1,    3,    1,    3,    1,    2,    5,    1,    0,
    1,    1,    3,    4,    1,    0,    1,    0,    1,    0,
    1,    1,    2,    3,    2,    1,    1,    1,    2,    2,
    2,    2,    3,    4,    2,    5,    2,    1,    2,    1,
    0,    2,    2,    1,    1,    3,    1,    1,    1,    1,
    1,
};
static const YYINT yydefred[] = {                         0,
   49,   51,    0,    0,    0,    0,    0,    0,    1,    0,
    4,   19,    6,    0,    0,   22,   27,    7,    0,   47,
   30,    0,   25,    0,   26,    0,    0,   32,   29,    0,
   11,    0,    9,    0,    0,    0,    0,   23,   31,   24,
    5,    3,    0,   39,    0,    0,   48,    0,   37,    0,
    0,    0,    0,   33,   40,    0,    0,   12,    8,    0,
   34,    0,   45,    0,   44,    0,    0,   36,    0,   43,
    0,   17,   13,   46,   15,   14,
};
static const YYINT yydgoto[] = {                          8,
    9,   10,   11,   12,   13,   25,   32,   59,   33,   73,
   76,   15,   16,   38,   17,   21,   48,   54,   55,   64,
   65,    0,
};
static const YYINT yysindex[] = {                       145,
    0,    0,    8,    8,  130,   -2,    8,    0,    0,   -3,
    0,    0,    0,   10,   48,    0,    0,    0,   10,    0,
    0,    8,    0,    8,    0,   44,  160,    0,    0,  160,
    0,   15,    0,    8,    8,    9,    8,    0,    0,    0,
    0,    0,   26,    0,   28,  160,    0,   31,    0,  115,
   36,   16,   14,    0,    0,   42,   57,    0,    0,    8,
    0,  160,    0,   14,    0,   58,    8,    0,   40,    0,
    8,    0,    0,    0,    0,    0,
};
static const YYINT yyrindex[] = {                        33,
    0,    0,    0,    0,    0,    0,    6,    0,    0,   70,
    0,    0,    0,   34,   18,    0,    0,    0,   63,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   33,
    0,    0,    0,    0,    0,    0,   75,    0,    0,    0,
    0,    0,    0,    0,   84,    0,    0,   93,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  102,    0,    0,   73,    0,    0,    0,
   76,    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
    0,    0,   50,  -18,   79,    1,    0,    0,    0,    0,
    0,   80,    0,    0,    0,  -33,    0,    0,    0,    0,
   23,    0,
};
#define YYTABLESIZE 176
static const YYINT yytable[] = {                          1,
   14,   45,   30,   19,   20,   28,   28,   29,   41,    1,
    1,   28,   27,   28,   31,    1,   28,   21,   28,   28,
   46,   28,   39,   21,   28,   62,   68,   52,   61,   43,
   21,   58,   20,   26,   44,   20,   47,   49,   20,   26,
   50,   26,   51,   69,   26,   53,   60,   26,   10,   26,
   66,   34,   74,   63,   35,   34,   40,   36,   35,   37,
   20,   36,   31,   37,   63,   67,   71,   72,   31,    2,
   31,   75,   18,   31,   38,   16,   31,   10,   31,   42,
   38,   18,   38,   35,   26,   38,   70,   38,   38,   35,
   38,   35,   41,    0,   35,    0,   35,   35,   41,   35,
   41,   42,    0,   41,    0,   41,   41,   42,   41,   42,
    0,    0,   42,    0,   42,   42,    1,   42,    2,   56,
    0,    0,   22,   57,    0,    4,    5,    0,    0,   24,
    7,    1,    0,    2,    0,    0,    0,   22,    0,    0,
    4,    5,   23,    0,   24,    7,    1,    0,    2,    0,
    0,    0,    3,    0,    0,    4,    5,    0,    0,    6,
    7,    1,    0,    2,    0,    0,    0,   22,    0,    0,
    4,    5,    0,    0,   24,    7,
};
static const YYINT yycheck[] = {                          2,
    0,   35,    6,    3,    4,    0,    6,    7,   27,    2,
    2,    6,   15,    8,    5,    2,   11,    0,   13,   14,
   12,   16,   22,    6,   24,   12,   60,   46,   13,   15,
   13,   50,    0,    0,   34,   35,   36,   37,    6,    6,
   15,    8,   15,   62,   11,   15,   11,   14,   15,   16,
    9,    8,   13,   53,   11,    8,   13,   14,   11,   16,
   60,   14,    0,   16,   64,    9,    9,   67,    6,    0,
    8,   71,    0,   11,    0,    0,   14,   15,   16,   30,
    6,    3,    8,    0,    5,   11,   64,   13,   14,    6,
   16,    8,    0,   -1,   11,   -1,   13,   14,    6,   16,
    8,    0,   -1,   11,   -1,   13,   14,    6,   16,    8,
   -1,   -1,   11,   -1,   13,   14,    2,   16,    4,    5,
   -1,   -1,    8,    9,   -1,   11,   12,   -1,   -1,   15,
   16,    2,   -1,    4,   -1,   -1,   -1,    8,   -1,   -1,
   11,   12,   13,   -1,   15,   16,    2,   -1,    4,   -1,
   -1,   -1,    8,   -1,   -1,   11,   12,   -1,   -1,   15,
   16,    2,   -1,    4,   -1,   -1,   -1,    8,   -1,   -1,
   11,   12,   -1,   -1,   15,   16,
};
#define YYFINAL 8
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 256
#define YYUNDFTOKEN 281
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,"WORDTK","HOSTWD","OBJTID","EXCLPT","SCOLON","QUESMK","PERCNT",
"LANGLE","RANGLE","COLON","LPAREN","RPAREN","PLUS","EQUALS","SLASH",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : xParser",
"xParser : xDefinition",
"xParser : xParser_1",
"xParser_1 : xParser_1 SCOLON xOdinExprNull",
"xParser_1 : xOdinExprNull",
"xDefinition : EQUALS EQUALS xOdinExpr",
"xDefinition : xDefHeader",
"xDefinition : PERCNT xDefHeader",
"xDefHeader : WORDTKleaf xDefHeader_1 EQUALS EQUALS xValue",
"xDefHeader_1 : xExecDef",
"xDefHeader_1 :",
"xExecDef : EXCLPT",
"xValue : xOdinExpr",
"xValue : LANGLE LANGLE xValue_1",
"xValue : EXCLPT LANGLE LANGLE xValue_2",
"xValue_2 : WORDTKleaf",
"xValue_2 :",
"xValue_1 : WORDTKleaf",
"xValue_1 :",
"xOdinExprNull : xOdinExpr",
"xOdinExprNull :",
"xOdinExpr : xOdinExpr1",
"xOdinExpr1 : xRoot",
"xOdinExpr1 : xOdinExpr1 xOperation",
"xOdinExpr1 : LPAREN xOdinExpr1 RPAREN",
"xRoot : LPAREN RPAREN",
"xRoot : WORDTKleaf",
"xRoot : OBJTIDleaf",
"xRoot : SLASH",
"xRoot : SLASH WORDTKleaf",
"xRoot : COLON xFileType",
"xRoot : PERCNT WORDTKleaf",
"xRoot : EQUALS WORDTKleaf",
"xOperation : PLUS xPrmType xOperation_1",
"xOperation : PLUS LPAREN xOdinExpr RPAREN",
"xOperation : COLON xFileType",
"xOperation : COLON xFileType EQUALS COLON xFileType",
"xOperation : SLASH WORDTKleaf",
"xOperation : SLASH",
"xOperation : PERCNT WORDTKleaf",
"xOperation_1 : xPrmVals",
"xOperation_1 :",
"xPrmVals : EQUALS xPrmVals_1",
"xPrmVals_1 : xPrmVals_1 xPrmVal",
"xPrmVals_1 : xPrmVal",
"xPrmVal : WORDTKleaf",
"xPrmVal : LPAREN xOdinExpr RPAREN",
"xFileType : WORDTKleaf",
"xPrmType : WORDTKleaf",
"WORDTKleaf : WORDTK",
"HOSTWDleaf : HOSTWD",
"OBJTIDleaf : OBJTID",

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
case 2:
	{Action(-7,1);}
break;
case 3:
	{Action(0,2);}
break;
case 5:
	{Action(1,1);}
break;
case 6:
	{Action(2,1);}
break;
case 7:
	{Action(3,1);}
break;
case 8:
	{Action(0,3);}
break;
case 10:
	{Action(0,0);}
break;
case 11:
	{Action(4,0);}
break;
case 13:
	{Action(5,1);}
break;
case 14:
	{Action(6,1);}
break;
case 16:
	{Action(0,0);}
break;
case 18:
	{Action(0,0);}
break;
case 20:
	{Action(0,0);}
break;
case 21:
	{Action(8,1);}
break;
case 23:
	{Action(0,2);}
break;
case 25:
	{Action(9,0);}
break;
case 28:
	{Action(10,0);}
break;
case 29:
	{Action(11,1);}
break;
case 30:
	{Action(14,1);}
break;
case 31:
	{Action(18,1);}
break;
case 32:
	{Action(21,1);}
break;
case 33:
	{Action(12,2);}
break;
case 34:
	{Action(13,1);}
break;
case 35:
	{Action(14,1);}
break;
case 36:
	{Action(15,2);}
break;
case 37:
	{Action(16,1);}
break;
case 38:
	{Action(17,0);}
break;
case 39:
	{Action(18,1);}
break;
case 41:
	{Action(0,0);}
break;
case 42:
	{Action(20,1);}
break;
case 43:
	{Action(0,2);}
break;
case 49:
	{Action(23,-1);}
break;
case 50:
	{Action(22,-1);}
break;
case 51:
	{Action(24,-1);}
break;
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
