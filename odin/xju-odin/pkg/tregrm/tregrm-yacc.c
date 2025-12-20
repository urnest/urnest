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
#line 1 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"

#include "gmc/gmc.h"
#include "tregrm/inc/NodTyp_.h"
#include "gmc/nod.h"
#include "tregrm/inc/Func.hh"
extern int num_ParseErrors;
tp_Nod YY_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)yyparse(); Nod = End_ConstructTree();
return Nod;}
void yyerror(char* s)
{num_ParseErrors++; ParseError(s);}
int yylex() {return YY_Lex();}

#line 86 "y.tab.c"

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
    TOK_EOF_ = 2,                  /* TOK_EOF_  */
    TOK_ERR_ = 3,                  /* TOK_ERR_  */
    TOK_INCLUDE = 4,               /* TOK_INCLUDE  */
    TOK_SCANNER = 5,               /* TOK_SCANNER  */
    TOK_NODES = 6,                 /* TOK_NODES  */
    TOK_RULES = 7,                 /* TOK_RULES  */
    TOK_Name = 8,                  /* TOK_Name  */
    TOK_AString = 9,               /* TOK_AString  */
    TOK_QString = 10,              /* TOK_QString  */
    TOK_Equals = 11,               /* TOK_Equals  */
    TOK_DoubleArrow = 12,          /* TOK_DoubleArrow  */
    TOK_SingleArrow = 13,          /* TOK_SingleArrow  */
    TOK_Plus = 14,                 /* TOK_Plus  */
    TOK_Star = 15,                 /* TOK_Star  */
    TOK_DoubleSlash = 16,          /* TOK_DoubleSlash  */
    TOK_SemiColon = 17,            /* TOK_SemiColon  */
    TOK_Question = 18,             /* TOK_Question  */
    TOK_LeftParen = 19,            /* TOK_LeftParen  */
    TOK_RightParen = 20            /* TOK_RightParen  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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
  YYSYMBOL_TOK_EOF_ = 3,                   /* TOK_EOF_  */
  YYSYMBOL_TOK_ERR_ = 4,                   /* TOK_ERR_  */
  YYSYMBOL_TOK_INCLUDE = 5,                /* TOK_INCLUDE  */
  YYSYMBOL_TOK_SCANNER = 6,                /* TOK_SCANNER  */
  YYSYMBOL_TOK_NODES = 7,                  /* TOK_NODES  */
  YYSYMBOL_TOK_RULES = 8,                  /* TOK_RULES  */
  YYSYMBOL_TOK_Name = 9,                   /* TOK_Name  */
  YYSYMBOL_TOK_AString = 10,               /* TOK_AString  */
  YYSYMBOL_TOK_QString = 11,               /* TOK_QString  */
  YYSYMBOL_TOK_Equals = 12,                /* TOK_Equals  */
  YYSYMBOL_TOK_DoubleArrow = 13,           /* TOK_DoubleArrow  */
  YYSYMBOL_TOK_SingleArrow = 14,           /* TOK_SingleArrow  */
  YYSYMBOL_TOK_Plus = 15,                  /* TOK_Plus  */
  YYSYMBOL_TOK_Star = 16,                  /* TOK_Star  */
  YYSYMBOL_TOK_DoubleSlash = 17,           /* TOK_DoubleSlash  */
  YYSYMBOL_TOK_SemiColon = 18,             /* TOK_SemiColon  */
  YYSYMBOL_TOK_Question = 19,              /* TOK_Question  */
  YYSYMBOL_TOK_LeftParen = 20,             /* TOK_LeftParen  */
  YYSYMBOL_TOK_RightParen = 21,            /* TOK_RightParen  */
  YYSYMBOL_YYACCEPT = 22,                  /* $accept  */
  YYSYMBOL_xTreeGrammar = 23,              /* xTreeGrammar  */
  YYSYMBOL_xIncludes = 24,                 /* xIncludes  */
  YYSYMBOL_xIncludes_1 = 25,               /* xIncludes_1  */
  YYSYMBOL_xIncludes_101 = 26,             /* xIncludes_101  */
  YYSYMBOL_xIncludeFile = 27,              /* xIncludeFile  */
  YYSYMBOL_xScanner = 28,                  /* xScanner  */
  YYSYMBOL_xScanner_1 = 29,                /* xScanner_1  */
  YYSYMBOL_xScanner_101 = 30,              /* xScanner_101  */
  YYSYMBOL_xLexicalDefinition = 31,        /* xLexicalDefinition  */
  YYSYMBOL_xNodes = 32,                    /* xNodes  */
  YYSYMBOL_xNodes_1 = 33,                  /* xNodes_1  */
  YYSYMBOL_xNodes_101 = 34,                /* xNodes_101  */
  YYSYMBOL_xRules = 35,                    /* xRules  */
  YYSYMBOL_xRules_1 = 36,                  /* xRules_1  */
  YYSYMBOL_xRules_101 = 37,                /* xRules_101  */
  YYSYMBOL_xRule = 38,                     /* xRule  */
  YYSYMBOL_xAlternateList = 39,            /* xAlternateList  */
  YYSYMBOL_xAlternateList_1 = 40,          /* xAlternateList_1  */
  YYSYMBOL_xAlternate = 41,                /* xAlternate  */
  YYSYMBOL_xAlternate_1 = 42,              /* xAlternate_1  */
  YYSYMBOL_xExpression = 43,               /* xExpression  */
  YYSYMBOL_xExpression_1 = 44,             /* xExpression_1  */
  YYSYMBOL_xTerm = 45,                     /* xTerm  */
  YYSYMBOL_xElement = 46,                  /* xElement  */
  YYSYMBOL_xTreeSpec = 47,                 /* xTreeSpec  */
  YYSYMBOL_xNodeName = 48,                 /* xNodeName  */
  YYSYMBOL_TOK_Nameleaf = 49,              /* TOK_Nameleaf  */
  YYSYMBOL_TOK_AStringleaf = 50,           /* TOK_AStringleaf  */
  YYSYMBOL_TOK_QStringleaf = 51            /* TOK_QStringleaf  */
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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   60

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  54
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  77

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
      21,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
static const yytype_uint8 yyrline[] =
{
       0,    40,    40,    47,    52,    55,    59,    62,    66,    68,
      72,    77,    80,    84,    87,    91,    95,    99,   103,   111,
     116,   119,   123,   126,   130,   135,   138,   142,   145,   149,
     155,   159,   162,   166,   173,   175,   179,   183,   186,   190,
     194,   197,   200,   203,   207,   211,   213,   215,   219,   222,
     226,   233,   235,   237,   239
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
  "\"end of file\"", "error", "\"invalid token\"", "TOK_EOF_", "TOK_ERR_",
  "TOK_INCLUDE", "TOK_SCANNER", "TOK_NODES", "TOK_RULES", "TOK_Name",
  "TOK_AString", "TOK_QString", "TOK_Equals", "TOK_DoubleArrow",
  "TOK_SingleArrow", "TOK_Plus", "TOK_Star", "TOK_DoubleSlash",
  "TOK_SemiColon", "TOK_Question", "TOK_LeftParen", "TOK_RightParen",
  "$accept", "xTreeGrammar", "xIncludes", "xIncludes_1", "xIncludes_101",
  "xIncludeFile", "xScanner", "xScanner_1", "xScanner_101",
  "xLexicalDefinition", "xNodes", "xNodes_1", "xNodes_101", "xRules",
  "xRules_1", "xRules_101", "xRule", "xAlternateList", "xAlternateList_1",
  "xAlternate", "xAlternate_1", "xExpression", "xExpression_1", "xTerm",
  "xElement", "xTreeSpec", "xNodeName", "TOK_Nameleaf", "TOK_AStringleaf",
  "TOK_QStringleaf", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-18)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      14,    10,    17,    21,   -18,   -18,   -18,    10,   -18,   -18,
     -18,   -18,    12,    22,   -18,   -18,    18,    19,    12,   -18,
      24,    25,    23,    30,   -18,    33,    33,   -18,    33,    33,
      23,   -18,    33,   -18,   -18,   -18,   -18,   -18,   -18,    20,
     -18,    33,   -18,    31,    23,   -18,   -18,    26,    31,   -18,
     -18,    34,    -7,   -18,   -18,    -6,   -18,   -18,   -18,   -18,
       9,   -18,   -18,   -18,    23,    27,   -18,    28,   -18,   -18,
      -7,   -18,    29,   -18,   -18,   -18,   -18
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       5,     0,     0,    12,     3,    53,    54,     4,     7,     8,
       9,     1,     0,    21,    10,     6,     0,     0,    11,    14,
       0,     0,     0,    26,    19,     0,     0,    13,     0,     0,
      20,    23,     0,     2,    24,    52,    15,    16,    17,     0,
      22,    25,    28,     0,     0,    27,    38,     0,    30,    32,
      18,    35,    36,    29,    31,     0,    33,    34,    38,    37,
      43,    45,    47,    46,     0,    48,    51,     0,    40,    41,
       0,    42,     0,    49,    44,    39,    50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -18,   -18,   -18,   -18,   -18,    32,   -18,   -18,   -18,    36,
     -18,   -18,   -18,   -18,   -18,   -18,     7,   -18,   -18,     3,
     -18,    -3,   -18,   -18,   -14,   -18,    -4,   -17,   -11,   -12
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,     4,     7,     8,    13,    14,    18,    19,
      23,    24,    30,    33,    34,    41,    42,    47,    48,    49,
      56,    51,    52,    59,    60,    57,    65,    43,     9,    10
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      21,    20,    35,     5,     6,     6,    21,    20,    36,    37,
      31,    38,    39,    58,    64,    16,    17,    11,    40,     1,
       5,     6,     5,     6,    68,    69,    70,    12,    71,    22,
      25,    26,    50,    44,     6,    61,    28,    29,    32,    15,
      63,    62,    35,    66,    53,    46,    73,    55,    45,    74,
      76,    54,    66,    61,    27,    67,    75,     0,    63,    62,
      72
};

static const yytype_int8 yycheck[] =
{
      12,    12,     9,    10,    11,    11,    18,    18,    25,    26,
      22,    28,    29,    20,    20,     3,     4,     0,    30,     5,
      10,    11,    10,    11,    15,    16,    17,     6,    19,     7,
      12,    12,    44,    13,    11,    52,    12,    12,     8,     7,
      52,    52,     9,    55,    18,    14,    19,    13,    41,    21,
      21,    48,    64,    70,    18,    58,    70,    -1,    70,    70,
      64
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,    23,    24,    25,    10,    11,    26,    27,    50,
      51,     0,     6,    28,    29,    27,     3,     4,    30,    31,
      50,    51,     7,    32,    33,    12,    12,    31,    12,    12,
      34,    51,     8,    35,    36,     9,    49,    49,    49,    49,
      51,    37,    38,    49,    13,    38,    14,    39,    40,    41,
      51,    43,    44,    18,    41,    13,    42,    47,    20,    45,
      46,    49,    50,    51,    20,    48,    51,    43,    15,    16,
      17,    19,    48,    19,    21,    46,    21
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    22,    23,    24,    25,    25,    26,    26,    27,    27,
      28,    29,    29,    30,    30,    31,    31,    31,    31,    32,
      33,    33,    34,    34,    35,    36,    36,    37,    37,    38,
      39,    40,    40,    41,    42,    42,    43,    44,    44,    45,
      45,    45,    45,    45,    46,    46,    46,    46,    47,    47,
      47,    48,    49,    50,    51
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     4,     1,     2,     0,     2,     1,     1,     1,
       1,     2,     0,     2,     1,     3,     3,     3,     5,     1,
       2,     0,     2,     1,     1,     2,     0,     2,     1,     3,
       1,     2,     1,     3,     1,     0,     1,     2,     0,     3,
       2,     2,     2,     1,     3,     1,     1,     1,     2,     3,
       4,     1,     1,     1,     1
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
  case 2: /* xTreeGrammar: xIncludes xScanner xNodes xRules  */
#line 43 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
           {Action(1,4);}
#line 1254 "y.tab.c"
    break;

  case 3: /* xIncludes: xIncludes_1  */
#line 47 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                {Action(2,1);}
#line 1260 "y.tab.c"
    break;

  case 5: /* xIncludes_1: %empty  */
#line 55 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1266 "y.tab.c"
    break;

  case 6: /* xIncludes_101: xIncludes_101 xIncludeFile  */
#line 60 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(0,2);}
#line 1272 "y.tab.c"
    break;

  case 10: /* xScanner: xScanner_1  */
#line 72 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
               {Action(3,1);}
#line 1278 "y.tab.c"
    break;

  case 12: /* xScanner_1: %empty  */
#line 80 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1284 "y.tab.c"
    break;

  case 13: /* xScanner_101: xScanner_101 xLexicalDefinition  */
#line 85 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                       {Action(0,2);}
#line 1290 "y.tab.c"
    break;

  case 15: /* xLexicalDefinition: TOK_EOF_ TOK_Equals TOK_Nameleaf  */
#line 93 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(4,1);}
#line 1296 "y.tab.c"
    break;

  case 16: /* xLexicalDefinition: TOK_ERR_ TOK_Equals TOK_Nameleaf  */
#line 97 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(5,1);}
#line 1302 "y.tab.c"
    break;

  case 17: /* xLexicalDefinition: TOK_AStringleaf TOK_Equals TOK_Nameleaf  */
#line 101 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(6,2);}
#line 1308 "y.tab.c"
    break;

  case 18: /* xLexicalDefinition: TOK_QStringleaf TOK_Equals TOK_Nameleaf TOK_DoubleArrow TOK_QStringleaf  */
#line 107 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                    {Action(7,3);}
#line 1314 "y.tab.c"
    break;

  case 19: /* xNodes: xNodes_1  */
#line 111 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
             {Action(8,1);}
#line 1320 "y.tab.c"
    break;

  case 21: /* xNodes_1: %empty  */
#line 119 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1326 "y.tab.c"
    break;

  case 22: /* xNodes_101: xNodes_101 TOK_QStringleaf  */
#line 124 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                    {Action(0,2);}
#line 1332 "y.tab.c"
    break;

  case 24: /* xRules: xRules_1  */
#line 130 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
             {Action(9,1);}
#line 1338 "y.tab.c"
    break;

  case 26: /* xRules_1: %empty  */
#line 138 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1344 "y.tab.c"
    break;

  case 27: /* xRules_101: xRules_101 xRule  */
#line 143 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
          {Action(0,2);}
#line 1350 "y.tab.c"
    break;

  case 29: /* xRule: TOK_Nameleaf xAlternateList TOK_SemiColon  */
#line 151 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                  {Action(10,2);}
#line 1356 "y.tab.c"
    break;

  case 30: /* xAlternateList: xAlternateList_1  */
#line 155 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                     {Action(11,1);}
#line 1362 "y.tab.c"
    break;

  case 31: /* xAlternateList_1: xAlternateList_1 xAlternate  */
#line 160 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
               {Action(0,2);}
#line 1368 "y.tab.c"
    break;

  case 33: /* xAlternate: TOK_SingleArrow xExpression xAlternate_1  */
#line 168 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(12,2);}
#line 1374 "y.tab.c"
    break;

  case 35: /* xAlternate_1: %empty  */
#line 175 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1380 "y.tab.c"
    break;

  case 36: /* xExpression: xExpression_1  */
#line 179 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                  {Action(-13,1);}
#line 1386 "y.tab.c"
    break;

  case 37: /* xExpression_1: xExpression_1 xTerm  */
#line 184 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
          {Action(0,2);}
#line 1392 "y.tab.c"
    break;

  case 38: /* xExpression_1: %empty  */
#line 186 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
     {Action(0,0);}
#line 1398 "y.tab.c"
    break;

  case 39: /* xTerm: xElement TOK_DoubleSlash xElement  */
#line 192 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
             {Action(14,2);}
#line 1404 "y.tab.c"
    break;

  case 40: /* xTerm: xElement TOK_Plus  */
#line 195 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
             {Action(15,1);}
#line 1410 "y.tab.c"
    break;

  case 41: /* xTerm: xElement TOK_Star  */
#line 198 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
             {Action(16,1);}
#line 1416 "y.tab.c"
    break;

  case 42: /* xTerm: xElement TOK_Question  */
#line 201 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(17,1);}
#line 1422 "y.tab.c"
    break;

  case 49: /* xTreeSpec: TOK_DoubleArrow xNodeName TOK_Question  */
#line 224 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                 {Action(18,1);}
#line 1428 "y.tab.c"
    break;

  case 50: /* xTreeSpec: TOK_DoubleArrow TOK_LeftParen xNodeName TOK_RightParen  */
#line 229 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                   {Action(19,1);}
#line 1434 "y.tab.c"
    break;

  case 52: /* TOK_Nameleaf: TOK_Name  */
#line 235 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                         {Action(20,-1);}
#line 1440 "y.tab.c"
    break;

  case 53: /* TOK_AStringleaf: TOK_AString  */
#line 237 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                               {Action(21,-1);}
#line 1446 "y.tab.c"
    break;

  case 54: /* TOK_QStringleaf: TOK_QString  */
#line 239 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"
                               {Action(22,-1);}
#line 1452 "y.tab.c"
    break;


#line 1456 "y.tab.c"

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

#line 241 "/home/xju/tmp/octs/xjutv/FILES/a/m/cast.y.3541995.y"

