
/*  A Bison parser, made from /net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	TOK_EOF_	2
#define	TOK_ERR_	3
#define	TOK_INCLUDE	4
#define	TOK_SCANNER	5
#define	TOK_NODES	6
#define	TOK_RULES	7
#define	TOK_Name	8
#define	TOK_AString	9
#define	TOK_QString	10
#define	TOK_Equals	11
#define	TOK_DoubleArrow	12
#define	TOK_SingleArrow	13
#define	TOK_Plus	14
#define	TOK_Star	15
#define	TOK_DoubleSlash	16
#define	TOK_SemiColon	17
#define	TOK_Question	18
#define	TOK_LeftParen	19
#define	TOK_RightParen	20

#line 1 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"

#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		77
#define	YYFLAG		-32768
#define	YYNTBASE	22

#define YYTRANSLATE(x) ((unsigned)(x) <= 257 ? yytranslate[x] : 51)

static const char yytranslate[] = {     0,
     2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     5,     7,    10,    11,    14,    16,    18,    20,    22,
    25,    26,    29,    31,    35,    39,    43,    49,    51,    54,
    55,    58,    60,    62,    65,    66,    69,    71,    75,    77,
    80,    82,    86,    88,    89,    91,    94,    95,    99,   102,
   105,   108,   110,   114,   116,   118,   120,   123,   127,   132,
   134,   136,   138
};

static const short yyrhs[] = {    23,
    27,    31,    34,     0,    24,     0,     5,    25,     0,     0,
    25,    26,     0,    26,     0,    49,     0,    50,     0,    28,
     0,     6,    29,     0,     0,    29,    30,     0,    30,     0,
     3,    12,    48,     0,     4,    12,    48,     0,    49,    12,
    48,     0,    50,    12,    48,    13,    50,     0,    32,     0,
     7,    33,     0,     0,    33,    50,     0,    50,     0,    35,
     0,     8,    36,     0,     0,    36,    37,     0,    37,     0,
    48,    38,    18,     0,    39,     0,    39,    40,     0,    40,
     0,    14,    42,    41,     0,    46,     0,     0,    43,     0,
    43,    44,     0,     0,    45,    17,    45,     0,    45,    15,
     0,    45,    16,     0,    45,    19,     0,    45,     0,    20,
    42,    21,     0,    48,     0,    50,     0,    49,     0,    13,
    47,     0,    13,    47,    19,     0,    13,    20,    47,    21,
     0,    50,     0,     9,     0,    10,     0,    11,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    35,    42,    46,    50,    54,    57,    61,    63,    67,    71,
    75,    79,    82,    86,    90,    94,    98,   106,   110,   114,
   118,   121,   125,   129,   133,   137,   140,   144,   150,   154,
   157,   161,   167,   170,   174,   178,   181,   185,   189,   192,
   195,   198,   202,   206,   208,   210,   214,   217,   221,   228,
   232,   234,   236
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TOK_EOF_",
"TOK_ERR_","TOK_INCLUDE","TOK_SCANNER","TOK_NODES","TOK_RULES","TOK_Name","TOK_AString",
"TOK_QString","TOK_Equals","TOK_DoubleArrow","TOK_SingleArrow","TOK_Plus","TOK_Star",
"TOK_DoubleSlash","TOK_SemiColon","TOK_Question","TOK_LeftParen","TOK_RightParen",
"xTreeGrammar","xIncludes","xIncludes_1","xIncludes_101","xIncludeFile","xScanner",
"xScanner_1","xScanner_101","xLexicalDefinition","xNodes","xNodes_1","xNodes_101",
"xRules","xRules_1","xRules_101","xRule","xAlternateList","xAlternateList_1",
"xAlternate","xAlternate_1","xExpression","xExpression_1","xTerm","xElement",
"xTreeSpec","xNodeName","TOK_Nameleaf","TOK_AStringleaf","TOK_QStringleaf", NULL
};
#endif

static const short yyr1[] = {     0,
    22,    23,    24,    24,    25,    25,    26,    26,    27,    28,
    28,    29,    29,    30,    30,    30,    30,    31,    32,    32,
    33,    33,    34,    35,    35,    36,    36,    37,    38,    39,
    39,    40,    41,    41,    42,    43,    43,    44,    44,    44,
    44,    44,    45,    45,    45,    45,    46,    46,    46,    47,
    48,    49,    50
};

static const short yyr2[] = {     0,
     4,     1,     2,     0,     2,     1,     1,     1,     1,     2,
     0,     2,     1,     3,     3,     3,     5,     1,     2,     0,
     2,     1,     1,     2,     0,     2,     1,     3,     1,     2,
     1,     3,     1,     0,     1,     2,     0,     3,     2,     2,
     2,     1,     3,     1,     1,     1,     2,     3,     4,     1,
     1,     1,     1
};

static const short yydefact[] = {     4,
     0,    11,     2,    52,    53,     3,     6,     7,     8,     0,
    20,     9,     5,     0,     0,    10,    13,     0,     0,     0,
    25,    18,     0,     0,    12,     0,     0,    19,    22,     0,
     1,    23,    51,    14,    15,    16,     0,    21,    24,    27,
     0,     0,    26,    37,     0,    29,    31,    17,    34,    35,
    28,    30,     0,    32,    33,    37,    36,    42,    44,    46,
    45,     0,    47,    50,     0,    39,    40,     0,    41,     0,
    48,    43,    38,    49,     0,     0,     0
};

static const short yydefgoto[] = {    75,
     2,     3,     6,     7,    11,    12,    16,    17,    21,    22,
    28,    31,    32,    39,    40,    45,    46,    47,    54,    49,
    50,    57,    58,    55,    63,    41,     8,     9
};

static const short yypact[] = {    14,
    10,    11,-32768,-32768,-32768,    10,-32768,-32768,-32768,    12,
    20,-32768,-32768,    17,    18,    12,-32768,    19,    21,    23,
    28,-32768,    29,    29,-32768,    29,    29,    23,-32768,    29,
-32768,-32768,-32768,-32768,-32768,-32768,    24,-32768,    29,-32768,
    25,    23,-32768,-32768,    26,    25,-32768,-32768,    32,    -7,
-32768,-32768,    -6,-32768,-32768,-32768,-32768,     9,-32768,-32768,
-32768,    23,    27,-32768,    30,-32768,-32768,    -7,-32768,    33,
-32768,-32768,-32768,-32768,    42,    47,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,    43,-32768,-32768,-32768,    34,-32768,-32768,
-32768,-32768,-32768,-32768,    16,-32768,-32768,     2,-32768,     0,
-32768,-32768,   -11,-32768,    -2,   -15,    -9,   -10
};


#define	YYLAST		60


static const short yytable[] = {    19,
    18,    33,     4,     5,     5,    19,    18,    34,    35,    29,
    36,    37,    56,    62,    14,    15,    10,    38,     1,     4,
     5,     4,     5,    66,    67,    68,    20,    69,    23,    24,
    26,    48,    27,     5,    59,    30,    42,    33,    44,    61,
    60,    76,    64,    51,    53,    71,    77,    52,    13,    25,
    72,    64,    59,    74,    43,    65,    73,    61,    60,    70
};

static const short yycheck[] = {    10,
    10,     9,    10,    11,    11,    16,    16,    23,    24,    20,
    26,    27,    20,    20,     3,     4,     6,    28,     5,    10,
    11,    10,    11,    15,    16,    17,     7,    19,    12,    12,
    12,    42,    12,    11,    50,     8,    13,     9,    14,    50,
    50,     0,    53,    18,    13,    19,     0,    46,     6,    16,
    21,    62,    68,    21,    39,    56,    68,    68,    68,    62
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 40 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(1,4);;
    break;}
case 2:
#line 44 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(2,1);;
    break;}
case 4:
#line 52 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 5:
#line 57 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 9:
#line 69 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(3,1);;
    break;}
case 11:
#line 77 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 12:
#line 82 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 14:
#line 90 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(4,1);;
    break;}
case 15:
#line 94 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(5,1);;
    break;}
case 16:
#line 98 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(6,2);;
    break;}
case 17:
#line 104 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(7,3);;
    break;}
case 18:
#line 108 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(8,1);;
    break;}
case 20:
#line 116 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 21:
#line 121 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 23:
#line 127 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(9,1);;
    break;}
case 25:
#line 135 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 26:
#line 140 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 28:
#line 148 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(10,2);;
    break;}
case 29:
#line 152 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(11,1);;
    break;}
case 30:
#line 157 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 32:
#line 165 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(12,2);;
    break;}
case 34:
#line 172 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 35:
#line 176 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(-13,1);;
    break;}
case 36:
#line 181 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,2);;
    break;}
case 37:
#line 183 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(0,0);;
    break;}
case 38:
#line 189 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(14,2);;
    break;}
case 39:
#line 192 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(15,1);;
    break;}
case 40:
#line 195 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(16,1);;
    break;}
case 41:
#line 198 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(17,1);;
    break;}
case 48:
#line 221 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(18,1);;
    break;}
case 49:
#line 226 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(19,1);;
    break;}
case 51:
#line 232 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(20,-1);;
    break;}
case 52:
#line 234 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(21,-1);;
    break;}
case 53:
#line 236 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"
{Action(22,-1);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 238 "/net/tantalum/home/tantalum/gclemm/.ODIN/tantalum/FILES/e/tregrm.ygi.232243.y"

