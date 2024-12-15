/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"

#include "inc/GMC.h"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}

#line 83 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_BANNER = 2,                /* TOK_BANNER  */
    TOK_NEEDS = 3,                 /* TOK_NEEDS  */
    TOK_Word = 4,                  /* TOK_Word  */
    TOK_Colon = 5,                 /* TOK_Colon  */
    TOK_Plus = 6,                  /* TOK_Plus  */
    TOK_Equals = 7,                /* TOK_Equals  */
    TOK_LeftParen = 8,             /* TOK_LeftParen  */
    TOK_RightParen = 9,            /* TOK_RightParen  */
    TOK_Percent = 10,              /* TOK_Percent  */
    TOK_Slash = 11,                /* TOK_Slash  */
    TOK_Semicolon = 12,            /* TOK_Semicolon  */
    TOK_Question = 13,             /* TOK_Question  */
    TOK_LeftAngle = 14,            /* TOK_LeftAngle  */
    TOK_RightAngle = 15,           /* TOK_RightAngle  */
    TOK_Ampersand = 16,            /* TOK_Ampersand  */
    TOK_At = 17,                   /* TOK_At  */
    TOK_Asterisk = 18,             /* TOK_Asterisk  */
    TOK_Dollar = 19                /* TOK_Dollar  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOK_BANNER = 3,                 /* TOK_BANNER  */
  YYSYMBOL_TOK_NEEDS = 4,                  /* TOK_NEEDS  */
  YYSYMBOL_TOK_Word = 5,                   /* TOK_Word  */
  YYSYMBOL_TOK_Colon = 6,                  /* TOK_Colon  */
  YYSYMBOL_TOK_Plus = 7,                   /* TOK_Plus  */
  YYSYMBOL_TOK_Equals = 8,                 /* TOK_Equals  */
  YYSYMBOL_TOK_LeftParen = 9,              /* TOK_LeftParen  */
  YYSYMBOL_TOK_RightParen = 10,            /* TOK_RightParen  */
  YYSYMBOL_TOK_Percent = 11,               /* TOK_Percent  */
  YYSYMBOL_TOK_Slash = 12,                 /* TOK_Slash  */
  YYSYMBOL_TOK_Semicolon = 13,             /* TOK_Semicolon  */
  YYSYMBOL_TOK_Question = 14,              /* TOK_Question  */
  YYSYMBOL_TOK_LeftAngle = 15,             /* TOK_LeftAngle  */
  YYSYMBOL_TOK_RightAngle = 16,            /* TOK_RightAngle  */
  YYSYMBOL_TOK_Ampersand = 17,             /* TOK_Ampersand  */
  YYSYMBOL_TOK_At = 18,                    /* TOK_At  */
  YYSYMBOL_TOK_Asterisk = 19,              /* TOK_Asterisk  */
  YYSYMBOL_TOK_Dollar = 20,                /* TOK_Dollar  */
  YYSYMBOL_YYACCEPT = 21,                  /* $accept  */
  YYSYMBOL_xDerivationGraph = 22,          /* xDerivationGraph  */
  YYSYMBOL_xDerivationGraph_1 = 23,        /* xDerivationGraph_1  */
  YYSYMBOL_xDGEntry = 24,                  /* xDGEntry  */
  YYSYMBOL_xDGEntry_1 = 25,                /* xDGEntry_1  */
  YYSYMBOL_xSourceType = 26,               /* xSourceType  */
  YYSYMBOL_xSourceType_1 = 27,             /* xSourceType_1  */
  YYSYMBOL_xParameterType = 28,            /* xParameterType  */
  YYSYMBOL_xObjectType = 29,               /* xObjectType  */
  YYSYMBOL_xForeignObjectType = 30,        /* xForeignObjectType  */
  YYSYMBOL_xDesc = 31,                     /* xDesc  */
  YYSYMBOL_xSuperType = 32,                /* xSuperType  */
  YYSYMBOL_xArgs = 33,                     /* xArgs  */
  YYSYMBOL_xArgs_1 = 34,                   /* xArgs_1  */
  YYSYMBOL_xNeeds = 35,                    /* xNeeds  */
  YYSYMBOL_xNeeds_1 = 36,                  /* xNeeds_1  */
  YYSYMBOL_xResults = 37,                  /* xResults  */
  YYSYMBOL_xResults_1 = 38,                /* xResults_1  */
  YYSYMBOL_xResult = 39,                   /* xResult  */
  YYSYMBOL_xForeignResult = 40,            /* xForeignResult  */
  YYSYMBOL_xArg = 41,                      /* xArg  */
  YYSYMBOL_xFileArg = 42,                  /* xFileArg  */
  YYSYMBOL_xOdinExpr1 = 43,                /* xOdinExpr1  */
  YYSYMBOL_xRoot = 44,                     /* xRoot  */
  YYSYMBOL_xOperation = 45,                /* xOperation  */
  YYSYMBOL_xOperation_1 = 46,              /* xOperation_1  */
  YYSYMBOL_xParameterValues = 47,          /* xParameterValues  */
  YYSYMBOL_xParameterValues_1 = 48,        /* xParameterValues_1  */
  YYSYMBOL_xParameterValue = 49,           /* xParameterValue  */
  YYSYMBOL_xVarWord = 50,                  /* xVarWord  */
  YYSYMBOL_TOK_Wordleaf = 51               /* TOK_Wordleaf  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   121

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  69
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  112

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   257


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
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
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    36,    36,    40,    44,    47,    49,    52,    57,    63,
      69,    75,    84,    87,    91,    94,   100,   102,   106,   111,
     116,   123,   126,   130,   134,   138,   142,   145,   149,   151,
     155,   158,   162,   164,   168,   171,   175,   181,   187,   189,
     191,   194,   199,   205,   207,   210,   216,   219,   221,   223,
     225,   227,   230,   235,   238,   241,   243,   247,   250,   252,
     258,   260,   264,   269,   272,   276,   278,   282,   284,   287
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOK_BANNER",
  "TOK_NEEDS", "TOK_Word", "TOK_Colon", "TOK_Plus", "TOK_Equals",
  "TOK_LeftParen", "TOK_RightParen", "TOK_Percent", "TOK_Slash",
  "TOK_Semicolon", "TOK_Question", "TOK_LeftAngle", "TOK_RightAngle",
  "TOK_Ampersand", "TOK_At", "TOK_Asterisk", "TOK_Dollar", "$accept",
  "xDerivationGraph", "xDerivationGraph_1", "xDGEntry", "xDGEntry_1",
  "xSourceType", "xSourceType_1", "xParameterType", "xObjectType",
  "xForeignObjectType", "xDesc", "xSuperType", "xArgs", "xArgs_1",
  "xNeeds", "xNeeds_1", "xResults", "xResults_1", "xResult",
  "xForeignResult", "xArg", "xFileArg", "xOdinExpr1", "xRoot",
  "xOperation", "xOperation_1", "xParameterValues", "xParameterValues_1",
  "xParameterValue", "xVarWord", "TOK_Wordleaf", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-49)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      35,     5,   -49,     5,     5,     5,     5,    43,    36,   -49,
      48,     5,     5,    53,   -49,   -49,   -49,   -49,   -49,     5,
     -49,    35,    41,    51,    61,    59,    83,   -49,     5,    64,
      11,   -49,    -3,   -49,   -49,    68,   -49,    71,    70,   -49,
      75,    73,    46,    46,   -49,   -49,    94,   -49,   -49,   -49,
      11,    79,   -49,   -49,   -49,    11,   -49,    71,    54,   -49,
     101,   -49,   -49,   -49,    46,    46,    18,    88,   -49,    11,
     -49,    81,   -49,   -49,   -49,   -49,    71,    54,   -49,   -49,
     -49,   -49,   -49,    11,   -49,   -49,   -49,    71,   -49,    89,
      93,   -49,    11,   -49,   -49,   108,   -49,    90,   -49,   -49,
     -49,   -49,     5,   105,   106,    71,   -49,    98,   -49,   -49,
       5,   -49
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       5,     0,    69,     0,     0,    17,     0,     0,     2,     4,
       0,     0,     0,     0,     6,    19,    18,    15,    16,     0,
       1,     5,     0,     0,    22,     0,     0,    14,     0,    29,
      25,    27,    39,    38,    67,     0,     3,     0,     0,    21,
       0,     0,     0,    50,    49,    48,     0,    43,    47,    68,
       0,     0,    26,    41,    40,     0,     7,     0,     0,    46,
       0,    52,    51,    42,     0,    58,    61,    55,    44,    28,
      31,     0,    66,    10,    65,     9,     0,     8,    24,    13,
      45,    59,    57,     0,    54,    53,    60,     0,    30,     0,
       0,    12,    62,    64,    56,     0,    11,    32,    35,    33,
      23,    63,     0,     0,     0,     0,    34,    19,    36,    37,
       0,    20
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -49,   -49,   -49,    97,   -49,   -49,   -49,     4,   -24,   -49,
      20,    42,   -49,   -49,   -49,   -49,   -49,   -49,    23,   -49,
     -21,   -48,    80,   -49,   -49,   -49,   -49,   -49,   -22,   -18,
       0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     7,     8,     9,    77,    10,    17,    11,    12,   104,
      23,    79,    29,    30,    51,    69,    96,    97,    98,    99,
      31,    32,    46,    47,    68,    85,    86,    92,    73,    33,
      34
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      13,    14,    45,    15,    16,    18,    19,    72,    48,    52,
       2,    24,    24,    56,    53,    54,     2,    45,    84,    24,
      26,    13,    67,    48,    61,    62,    83,    26,    49,    70,
      44,    28,    25,    75,    78,    72,    67,    74,     1,    35,
       2,     3,     4,    20,    72,    44,    81,    82,    88,    21,
      66,     2,    90,    78,     5,     6,    22,    37,     2,    38,
       3,    93,    26,    94,    66,    74,    28,    40,    50,    76,
     101,   103,    27,    28,    74,    39,    55,     3,     2,     3,
       4,   103,    41,    59,    42,    43,    57,    71,     2,     3,
       4,    58,    41,    28,    42,    43,    87,    89,    95,   105,
       3,     4,   107,    28,    63,    64,    65,     3,     4,   100,
     111,    80,    64,    65,   102,   108,   109,   110,    36,    91,
     106,    60
};

static const yytype_int8 yycheck[] =
{
       0,     1,    26,     3,     4,     5,     6,    55,    26,    30,
       5,    11,    12,    37,    17,    18,     5,    41,    66,    19,
       9,    21,    46,    41,    42,    43,     8,     9,    28,    50,
      26,    20,    12,    57,    58,    83,    60,    55,     3,    19,
       5,     6,     7,     0,    92,    41,    64,    65,    69,    13,
      46,     5,    76,    77,    19,    20,     8,    16,     5,     8,
       6,    83,     9,    87,    60,    83,    20,     8,     4,    15,
      92,    95,    19,    20,    92,    14,     8,     6,     5,     6,
       7,   105,     9,    10,    11,    12,    16,     8,     5,     6,
       7,    16,     9,    20,    11,    12,     8,    16,     9,     9,
       6,     7,   102,    20,    10,    11,    12,     6,     7,    16,
     110,    10,    11,    12,     6,    10,    10,    19,    21,    77,
      97,    41
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     6,     7,    19,    20,    22,    23,    24,
      26,    28,    29,    51,    51,    51,    51,    27,    51,    51,
       0,    13,     8,    31,    51,    31,     9,    19,    20,    33,
      34,    41,    42,    50,    51,    31,    24,    16,     8,    14,
       8,     9,    11,    12,    28,    29,    43,    44,    50,    51,
       4,    35,    41,    17,    18,     8,    29,    16,    16,    10,
      43,    50,    50,    10,    11,    12,    28,    29,    45,    36,
      41,     8,    42,    49,    50,    29,    15,    25,    29,    32,
      10,    50,    50,     8,    42,    46,    47,     8,    41,    16,
      29,    32,    48,    49,    29,     9,    37,    38,    39,    40,
      16,    49,     6,    29,    30,     9,    39,    51,    10,    10,
      19,    51
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    21,    22,    23,    23,    24,    24,    24,    24,    24,
      24,    24,    25,    25,    26,    26,    27,    27,    28,    29,
      30,    31,    31,    32,    32,    33,    34,    34,    35,    35,
      36,    36,    37,    37,    38,    38,    39,    40,    41,    41,
      41,    41,    42,    43,    43,    43,    44,    44,    44,    44,
      44,    44,    44,    45,    45,    45,    45,    45,    45,    45,
      46,    46,    47,    48,    48,    49,    49,    50,    50,    51
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     1,     0,     2,     4,     5,     5,
       5,     6,     2,     1,     2,     2,     1,     0,     2,     2,
       4,     2,     1,     3,     1,     1,     2,     1,     2,     0,
       2,     1,     1,     1,     2,     1,     3,     3,     1,     1,
       2,     2,     3,     1,     2,     3,     2,     1,     1,     1,
       1,     2,     2,     2,     2,     1,     3,     2,     1,     2,
       1,     0,     2,     2,     1,     1,     1,     1,     2,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* xDerivationGraph: xDerivationGraph_1  */
#line 36 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                       {Action(1,1);}
#line 1278 "y.tab.c"
    break;

  case 3: /* xDerivationGraph_1: xDerivationGraph_1 TOK_Semicolon xDGEntry  */
#line 42 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(0,2);}
#line 1284 "y.tab.c"
    break;

  case 5: /* xDGEntry: %empty  */
#line 47 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
 {Action(0,0);}
#line 1290 "y.tab.c"
    break;

  case 6: /* xDGEntry: TOK_BANNER TOK_Wordleaf  */
#line 50 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(2,1);}
#line 1296 "y.tab.c"
    break;

  case 7: /* xDGEntry: xSourceType TOK_Equals TOK_RightAngle xObjectType  */
#line 55 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                {Action(3,2);}
#line 1302 "y.tab.c"
    break;

  case 8: /* xDGEntry: xObjectType xDesc TOK_Equals TOK_RightAngle xDGEntry_1  */
#line 61 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
               {Action(5,3);}
#line 1308 "y.tab.c"
    break;

  case 9: /* xDGEntry: xParameterType xDesc TOK_Equals TOK_RightAngle xObjectType  */
#line 67 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                {Action(4,3);}
#line 1314 "y.tab.c"
    break;

  case 10: /* xDGEntry: TOK_Dollar TOK_Wordleaf xDesc TOK_Equals xParameterValue  */
#line 73 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                    {Action(7,3);}
#line 1320 "y.tab.c"
    break;

  case 11: /* xDGEntry: TOK_Wordleaf xArgs xNeeds TOK_Equals TOK_RightAngle xResults  */
#line 80 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(6,4);}
#line 1326 "y.tab.c"
    break;

  case 12: /* xDGEntry_1: xDGEntry_1 xSuperType  */
#line 85 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
               {Action(0,2);}
#line 1332 "y.tab.c"
    break;

  case 14: /* xSourceType: TOK_Wordleaf TOK_Asterisk  */
#line 92 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(8,1);}
#line 1338 "y.tab.c"
    break;

  case 15: /* xSourceType: TOK_Asterisk xSourceType_1  */
#line 95 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                  {Action(9,1);}
#line 1344 "y.tab.c"
    break;

  case 17: /* xSourceType_1: %empty  */
#line 102 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
     {Action(0,0);}
#line 1350 "y.tab.c"
    break;

  case 20: /* xForeignObjectType: TOK_Colon TOK_Wordleaf TOK_Asterisk TOK_Wordleaf  */
#line 119 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(0,2);}
#line 1356 "y.tab.c"
    break;

  case 22: /* xDesc: TOK_Wordleaf  */
#line 126 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(10,1);}
#line 1362 "y.tab.c"
    break;

  case 23: /* xSuperType: TOK_LeftAngle xObjectType TOK_RightAngle  */
#line 132 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(11,1);}
#line 1368 "y.tab.c"
    break;

  case 25: /* xArgs: xArgs_1  */
#line 138 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
            {Action(12,1);}
#line 1374 "y.tab.c"
    break;

  case 26: /* xArgs_1: xArgs_1 xArg  */
#line 143 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
         {Action(0,2);}
#line 1380 "y.tab.c"
    break;

  case 28: /* xNeeds: TOK_NEEDS xNeeds_1  */
#line 150 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(13,1);}
#line 1386 "y.tab.c"
    break;

  case 29: /* xNeeds: %empty  */
#line 151 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
 {Action(13,0);}
#line 1392 "y.tab.c"
    break;

  case 30: /* xNeeds_1: xNeeds_1 xArg  */
#line 156 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
         {Action(0,2);}
#line 1398 "y.tab.c"
    break;

  case 32: /* xResults: xResults_1  */
#line 162 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
               {Action(16,1);}
#line 1404 "y.tab.c"
    break;

  case 33: /* xResults: xForeignResult  */
#line 164 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(16,1);}
#line 1410 "y.tab.c"
    break;

  case 34: /* xResults_1: xResults_1 xResult  */
#line 169 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
            {Action(0,2);}
#line 1416 "y.tab.c"
    break;

  case 37: /* xForeignResult: TOK_LeftParen xForeignObjectType TOK_RightParen  */
#line 183 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(17,1);}
#line 1422 "y.tab.c"
    break;

  case 40: /* xArg: xFileArg TOK_At  */
#line 192 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
           {Action(14,1);}
#line 1428 "y.tab.c"
    break;

  case 41: /* xArg: xFileArg TOK_Ampersand  */
#line 195 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                  {Action(15,1);}
#line 1434 "y.tab.c"
    break;

  case 42: /* xFileArg: TOK_LeftParen xOdinExpr1 TOK_RightParen  */
#line 201 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(18,1);}
#line 1440 "y.tab.c"
    break;

  case 44: /* xOdinExpr1: xOdinExpr1 xOperation  */
#line 208 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
               {Action(0,2);}
#line 1446 "y.tab.c"
    break;

  case 46: /* xRoot: TOK_LeftParen TOK_RightParen  */
#line 217 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(21,0);}
#line 1452 "y.tab.c"
    break;

  case 48: /* xRoot: xObjectType  */
#line 221 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                {Action(19,1);}
#line 1458 "y.tab.c"
    break;

  case 49: /* xRoot: xParameterType  */
#line 223 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                   {Action(20,1);}
#line 1464 "y.tab.c"
    break;

  case 50: /* xRoot: TOK_Slash  */
#line 225 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
              {Action(22,0);}
#line 1470 "y.tab.c"
    break;

  case 51: /* xRoot: TOK_Slash xVarWord  */
#line 228 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(23,1);}
#line 1476 "y.tab.c"
    break;

  case 52: /* xRoot: TOK_Percent xVarWord  */
#line 231 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(30,1);}
#line 1482 "y.tab.c"
    break;

  case 53: /* xOperation: xParameterType xOperation_1  */
#line 236 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(24,2);}
#line 1488 "y.tab.c"
    break;

  case 54: /* xOperation: xParameterType xFileArg  */
#line 239 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(25,2);}
#line 1494 "y.tab.c"
    break;

  case 55: /* xOperation: xObjectType  */
#line 241 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                {Action(27,1);}
#line 1500 "y.tab.c"
    break;

  case 56: /* xOperation: xObjectType TOK_Equals xObjectType  */
#line 245 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                {Action(28,2);}
#line 1506 "y.tab.c"
    break;

  case 57: /* xOperation: TOK_Slash xVarWord  */
#line 248 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(31,1);}
#line 1512 "y.tab.c"
    break;

  case 58: /* xOperation: TOK_Slash  */
#line 250 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
              {Action(32,0);}
#line 1518 "y.tab.c"
    break;

  case 59: /* xOperation: TOK_Percent xVarWord  */
#line 253 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
             {Action(30,1);}
#line 1524 "y.tab.c"
    break;

  case 61: /* xOperation_1: %empty  */
#line 260 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
     {Action(0,0);}
#line 1530 "y.tab.c"
    break;

  case 62: /* xParameterValues: TOK_Equals xParameterValues_1  */
#line 265 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                       {Action(26,1);}
#line 1536 "y.tab.c"
    break;

  case 63: /* xParameterValues_1: xParameterValues_1 xParameterValue  */
#line 270 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                    {Action(0,2);}
#line 1542 "y.tab.c"
    break;

  case 68: /* xVarWord: TOK_Dollar TOK_Wordleaf  */
#line 285 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                 {Action(33,1);}
#line 1548 "y.tab.c"
    break;

  case 69: /* TOK_Wordleaf: TOK_Word  */
#line 287 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"
                         {Action(34,-1);}
#line 1554 "y.tab.c"
    break;


#line 1558 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 289 "/home/xju/tmp/oc122/xjutv/FILES/p/drvgrf.ygi.1120288.y"

