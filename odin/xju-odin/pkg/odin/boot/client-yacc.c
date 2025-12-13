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
   especially those whose name start with OC_ or oc_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with oc or OC, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define OCBISON 30802

/* Bison version string.  */
#define OCBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define OCSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define OCPURE 0

/* Push parsers.  */
#define OCPUSH 0

/* Pull parsers.  */
#define OCPULL 1




/* First part of user prologue.  */

#include "gmc/nod.h"
extern int num_ParseErrors;
tp_Nod OC_Parse() {tp_Nod Nod;
num_ParseErrors = 0; Init_Lex(); Init_ConstructTree();
(void)ocparse(); Nod = End_ConstructTree();
return Nod;}
void ocerror(GMC_ARG(char*, s)) GMC_DCL(char*, s) 
{num_ParseErrors++; ParseError(s);}
int oclex() {return OC_Lex();}


# ifndef OC_CAST
#  ifdef __cplusplus
#   define OC_CAST(Type, Val) static_cast<Type> (Val)
#   define OC_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define OC_CAST(Type, Val) ((Type) (Val))
#   define OC_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef OC_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define OC_NULLPTR nullptr
#   else
#    define OC_NULLPTR 0
#   endif
#  else
#   define OC_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef OC_OC_Y_TAB_H_INCLUDED
# define OC_OC_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef OCDEBUG
# define OCDEBUG 0
#endif
#if OCDEBUG
extern int ocdebug;
#endif

/* Token kinds.  */
#ifndef OCTOKENTYPE
# define OCTOKENTYPE
  enum octokentype
  {
    OCEMPTY = -2,
    OCEOF = 0,                     /* "end of file"  */
    OCerror = 256,                 /* error  */
    OCUNDEF = 257,                 /* "invalid token"  */
    WORDTK = 2,                    /* WORDTK  */
    SCAN_HOSTWD = 3,               /* SCAN_HOSTWD  */
    SCAN_OBJTID = 4,               /* SCAN_OBJTID  */
    EXCLPT = 5,                    /* EXCLPT  */
    SCOLON = 6,                    /* SCOLON  */
    QUESMK = 7,                    /* QUESMK  */
    PERCNT = 8,                    /* PERCNT  */
    LANGLE = 9,                    /* LANGLE  */
    RANGLE = 10,                   /* RANGLE  */
    COLON = 11,                    /* COLON  */
    LPAREN = 12,                   /* LPAREN  */
    RPAREN = 13,                   /* RPAREN  */
    PLUS = 14,                     /* PLUS  */
    EQUALS = 15,                   /* EQUALS  */
    SLASH = 16                     /* SLASH  */
  };
  typedef enum octokentype octoken_kind_t;
#endif
/* Token kinds.  */
#define OCEMPTY -2
#define OCEOF 0
#define OCerror 256
#define OCUNDEF 257
#define WORDTK 2
#define SCAN_HOSTWD 3
#define SCAN_OBJTID 4
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

/* Value type.  */
#if ! defined OCSTYPE && ! defined OCSTYPE_IS_DECLARED
typedef int OCSTYPE;
# define OCSTYPE_IS_TRIVIAL 1
# define OCSTYPE_IS_DECLARED 1
#endif


extern OCSTYPE oclval;


int ocparse (void);


#endif /* !OC_OC_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum ocsymbol_kind_t
{
  OCSYMBOL_OCEMPTY = -2,
  OCSYMBOL_OCEOF = 0,                      /* "end of file"  */
  OCSYMBOL_OCerror = 1,                    /* error  */
  OCSYMBOL_OCUNDEF = 2,                    /* "invalid token"  */
  OCSYMBOL_WORDTK = 3,                     /* WORDTK  */
  OCSYMBOL_SCAN_HOSTWD = 4,                /* SCAN_HOSTWD  */
  OCSYMBOL_SCAN_OBJTID = 5,                /* SCAN_OBJTID  */
  OCSYMBOL_EXCLPT = 6,                     /* EXCLPT  */
  OCSYMBOL_SCOLON = 7,                     /* SCOLON  */
  OCSYMBOL_QUESMK = 8,                     /* QUESMK  */
  OCSYMBOL_PERCNT = 9,                     /* PERCNT  */
  OCSYMBOL_LANGLE = 10,                    /* LANGLE  */
  OCSYMBOL_RANGLE = 11,                    /* RANGLE  */
  OCSYMBOL_COLON = 12,                     /* COLON  */
  OCSYMBOL_LPAREN = 13,                    /* LPAREN  */
  OCSYMBOL_RPAREN = 14,                    /* RPAREN  */
  OCSYMBOL_PLUS = 15,                      /* PLUS  */
  OCSYMBOL_EQUALS = 16,                    /* EQUALS  */
  OCSYMBOL_SLASH = 17,                     /* SLASH  */
  OCSYMBOL_OCACCEPT = 18,                  /* $accept  */
  OCSYMBOL_xCommand = 19,                  /* xCommand  */
  OCSYMBOL_xCommand_1 = 20,                /* xCommand_1  */
  OCSYMBOL_xSingleCommand = 21,            /* xSingleCommand  */
  OCSYMBOL_xQuery = 22,                    /* xQuery  */
  OCSYMBOL_xExecute = 23,                  /* xExecute  */
  OCSYMBOL_xCopy = 24,                     /* xCopy  */
  OCSYMBOL_xVariable = 25,                 /* xVariable  */
  OCSYMBOL_xOdinExpr = 26,                 /* xOdinExpr  */
  OCSYMBOL_xOdinExpr1 = 27,                /* xOdinExpr1  */
  OCSYMBOL_xRoot = 28,                     /* xRoot  */
  OCSYMBOL_xOperation = 29,                /* xOperation  */
  OCSYMBOL_xOperation_1 = 30,              /* xOperation_1  */
  OCSYMBOL_xPrmVals = 31,                  /* xPrmVals  */
  OCSYMBOL_xPrmVals_1 = 32,                /* xPrmVals_1  */
  OCSYMBOL_xPrmVal = 33,                   /* xPrmVal  */
  OCSYMBOL_xFileType = 34,                 /* xFileType  */
  OCSYMBOL_xPrmType = 35,                  /* xPrmType  */
  OCSYMBOL_xWord = 36,                     /* xWord  */
  OCSYMBOL_xEPString = 37,                 /* xEPString  */
  OCSYMBOL_WORDTKleaf = 38,                /* WORDTKleaf  */
  OCSYMBOL_SCAN_HOSTWDleaf = 39,           /* SCAN_HOSTWDleaf  */
  OCSYMBOL_SCAN_OBJTIDleaf = 40            /* SCAN_OBJTIDleaf  */
};
typedef enum ocsymbol_kind_t ocsymbol_kind_t;




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
#  define OC_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ octype_int8;
#elif defined OC_STDINT_H
typedef int_least8_t octype_int8;
#else
typedef signed char octype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ octype_int16;
#elif defined OC_STDINT_H
typedef int_least16_t octype_int16;
#else
typedef short octype_int16;
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
typedef __UINT_LEAST8_TYPE__ octype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined OC_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t octype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char octype_uint8;
#else
typedef short octype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ octype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined OC_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t octype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short octype_uint16;
#else
typedef int octype_uint16;
#endif

#ifndef OCPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define OCPTRDIFF_T __PTRDIFF_TYPE__
#  define OCPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define OCPTRDIFF_T ptrdiff_t
#  define OCPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define OCPTRDIFF_T long
#  define OCPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef OCSIZE_T
# ifdef __SIZE_TYPE__
#  define OCSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define OCSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define OCSIZE_T size_t
# else
#  define OCSIZE_T unsigned
# endif
#endif

#define OCSIZE_MAXIMUM                                  \
  OC_CAST (OCPTRDIFF_T,                                 \
           (OCPTRDIFF_MAXIMUM < OC_CAST (OCSIZE_T, -1)  \
            ? OCPTRDIFF_MAXIMUM                         \
            : OC_CAST (OCSIZE_T, -1)))

#define OCSIZEOF(X) OC_CAST (OCPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef octype_int8 oc_state_t;

/* State numbers in computations.  */
typedef int oc_state_fast_t;

#ifndef OC_
# if defined OCENABLE_NLS && OCENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define OC_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef OC_
#  define OC_(Msgid) Msgid
# endif
#endif


#ifndef OC_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define OC_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define OC_ATTRIBUTE_PURE
# endif
#endif

#ifndef OC_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define OC_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define OC_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define OC_USE(E) ((void) (E))
#else
# define OC_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about oclval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define OC_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define OC_INITIAL_VALUE(Value) Value
#endif
#ifndef OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define OC_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef OC_INITIAL_VALUE
# define OC_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define OC_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define OC_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef OC_IGNORE_USELESS_CAST_BEGIN
# define OC_IGNORE_USELESS_CAST_BEGIN
# define OC_IGNORE_USELESS_CAST_END
#endif


#define OC_ASSERT(E) ((void) (0 && (E)))

#if !defined ocoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef OCSTACK_USE_ALLOCA
#  if OCSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define OCSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define OCSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define OCSTACK_ALLOC alloca
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

# ifdef OCSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define OCSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef OCSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define OCSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define OCSTACK_ALLOC OCMALLOC
#  define OCSTACK_FREE OCFREE
#  ifndef OCSTACK_ALLOC_MAXIMUM
#   define OCSTACK_ALLOC_MAXIMUM OCSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined OCMALLOC || defined malloc) \
             && (defined OCFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef OCMALLOC
#   define OCMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (OCSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef OCFREE
#   define OCFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined ocoverflow */

#if (! defined ocoverflow \
     && (! defined __cplusplus \
         || (defined OCSTYPE_IS_TRIVIAL && OCSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union ocalloc
{
  oc_state_t ocss_alloc;
  OCSTYPE ocvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define OCSTACK_GAP_MAXIMUM (OCSIZEOF (union ocalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define OCSTACK_BYTES(N) \
     ((N) * (OCSIZEOF (oc_state_t) + OCSIZEOF (OCSTYPE)) \
      + OCSTACK_GAP_MAXIMUM)

# define OCCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables OCSIZE and OCSTACKSIZE give the old and new number of
   elements in the stack, and OCPTR gives the new location of the
   stack.  Advance OCPTR to a properly aligned location for the next
   stack.  */
# define OCSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        OCPTRDIFF_T ocnewbytes;                                         \
        OCCOPY (&ocptr->Stack_alloc, Stack, ocsize);                    \
        Stack = &ocptr->Stack_alloc;                                    \
        ocnewbytes = ocstacksize * OCSIZEOF (*Stack) + OCSTACK_GAP_MAXIMUM; \
        ocptr += ocnewbytes / OCSIZEOF (*ocptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined OCCOPY_NEEDED && OCCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef OCCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define OCCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, OC_CAST (OCSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define OCCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          OCPTRDIFF_T oci;                      \
          for (oci = 0; oci < (Count); oci++)   \
            (Dst)[oci] = (Src)[oci];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !OCCOPY_NEEDED */

/* OCFINAL -- State number of the termination state.  */
#define OCFINAL  39
/* OCLAST -- Last index in OCTABLE.  */
#define OCLAST   137

/* OCNTOKENS -- Number of terminals.  */
#define OCNTOKENS  18
/* OCNNTS -- Number of nonterminals.  */
#define OCNNTS  23
/* OCNRULES -- Number of rules.  */
#define OCNRULES  60
/* OCNSTATES -- Number of states.  */
#define OCNSTATES  83

/* OCMAXUTOK -- Last valid token kind.  */
#define OCMAXUTOK   257


/* OCTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by oclex, with out-of-bounds checking.  */
#define OCTRANSLATE(OCX)                                \
  (0 <= (OCX) && (OCX) <= OCMAXUTOK                     \
   ? OC_CAST (ocsymbol_kind_t, octranslate[OCX])        \
   : OCSYMBOL_OCUNDEF)

/* OCTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by oclex.  */
static const octype_int8 octranslate[] =
{
       0,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     2,     2,     2,
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

#if OCDEBUG
/* OCRLINE[OCN] -- Source line where rule number OCN was defined.  */
static const octype_uint8 ocrline[] =
{
       0,    33,    33,    37,    41,    44,    46,    48,    50,    52,
      56,    60,    63,    65,    68,    70,    75,    81,    85,    89,
      92,    97,   100,   104,   110,   114,   116,   119,   125,   128,
     130,   132,   134,   137,   140,   143,   146,   149,   154,   158,
     163,   166,   172,   175,   177,   183,   185,   189,   194,   197,
     201,   203,   209,   213,   217,   219,   223,   226,   228,   230,
     232
};
#endif

/** Accessing symbol of state STATE.  */
#define OC_ACCESSING_SYMBOL(State) OC_CAST (ocsymbol_kind_t, ocstos[State])

#if OCDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   OCSYMBOL.  No bounds checking.  */
static const char *ocsymbol_name (ocsymbol_kind_t ocsymbol) OC_ATTRIBUTE_UNUSED;

/* OCTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at OCNTOKENS, nonterminals.  */
static const char *const octname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "WORDTK",
  "SCAN_HOSTWD", "SCAN_OBJTID", "EXCLPT", "SCOLON", "QUESMK", "PERCNT",
  "LANGLE", "RANGLE", "COLON", "LPAREN", "RPAREN", "PLUS", "EQUALS",
  "SLASH", "$accept", "xCommand", "xCommand_1", "xSingleCommand", "xQuery",
  "xExecute", "xCopy", "xVariable", "xOdinExpr", "xOdinExpr1", "xRoot",
  "xOperation", "xOperation_1", "xPrmVals", "xPrmVals_1", "xPrmVal",
  "xFileType", "xPrmType", "xWord", "xEPString", "WORDTKleaf",
  "SCAN_HOSTWDleaf", "SCAN_OBJTIDleaf", OC_NULLPTR
};

static const char *
ocsymbol_name (ocsymbol_kind_t ocsymbol)
{
  return octname[ocsymbol];
}
#endif

#define OCPACT_NINF (-45)

#define ocpact_value_is_default(Yyn) \
  ((Yyn) == OCPACT_NINF)

#define OCTABLE_NINF (-1)

#define octable_value_is_error(Yyn) \
  0

/* OCPACT[STATE-NUM] -- Index in OCTABLE of the portion describing
   STATE-NUM.  */
static const octype_int8 ocpact[] =
{
      75,   -45,   -45,   -45,     9,    25,    35,    35,   105,    35,
      35,     3,     7,   -45,   -45,   -45,   -45,   -45,    20,    47,
     -45,    16,   -45,    15,   -45,   -45,   -45,    35,   -45,   -45,
     -45,   -45,   -45,   -45,   -45,    27,   -45,   -45,   -45,   -45,
      75,    55,   120,   120,   -45,    35,    35,     5,    35,   -45,
      90,   -45,   -45,   -45,   -45,    35,   -45,   -45,   -45,    18,
     120,    19,   -45,   -45,    41,   -45,   -45,   -45,    38,    43,
      12,   -45,   -45,    35,   -45,   120,    12,   -45,   -45,   -45,
      44,   -45,   -45
};

/* OCDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when OCTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const octype_int8 ocdefact[] =
{
       5,    58,    59,    60,    14,    54,     0,     0,     0,     0,
      31,     0,     2,     4,     6,     7,     8,     9,    10,    24,
      25,    29,    12,    55,    57,    30,    56,     0,    34,    54,
      36,    55,    33,    52,    28,     0,    29,    37,    32,     1,
       5,    13,    20,    19,    11,     0,     0,     0,    43,    26,
      21,    35,    16,    27,     3,     0,    18,    17,    44,    40,
       0,    46,    53,    42,     0,    22,    23,    15,     0,     0,
       0,    38,    45,     0,    39,     0,    47,    49,    50,    41,
       0,    48,    51
};

/* OCPGOTO[NTERM-NUM].  */
static const octype_int8 ocpgoto[] =
{
     -45,   -45,   -45,    26,   -45,   -45,   -45,   -45,   -38,    52,
     -45,   -45,   -45,   -45,   -45,   -24,   -44,   -45,     0,   -17,
      11,   -45,   -45
};

/* OCDEFGOTO[NTERM-NUM].  */
static const octype_int8 ocdefgoto[] =
{
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    49,    71,    72,    76,    77,    32,    61,    36,    22,
      31,    24,    25
};

/* OCTABLE[OCPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If OCTABLE_NINF, syntax error.  */
static const octype_int8 octable[] =
{
      21,    44,    59,    39,    56,    57,    30,    33,     1,    37,
      38,    23,    65,    29,    40,     1,    28,    26,    60,    23,
      29,    27,    69,    51,     2,    75,    41,    52,     1,    79,
      42,    43,    50,    66,    68,    70,    45,    80,     1,    46,
      21,    53,    47,    29,    48,    58,    33,    62,    63,    26,
      73,    23,    81,    23,    23,    67,    45,    74,    82,    46,
      35,    23,    47,    26,    48,     0,    54,    55,     0,     0,
      78,    23,     0,    33,     0,     0,    78,     0,     1,     2,
       3,     4,     0,     5,     6,     0,    23,     7,     8,     0,
       0,     9,    10,     1,     2,     3,    64,     0,     5,     6,
       0,     0,     7,     8,     0,     0,     9,    10,     1,     0,
       3,     0,     0,     5,     6,     0,     0,     7,     8,    34,
       0,     9,    10,     1,     0,     3,     0,     0,     5,     6,
       0,     0,     7,     8,     0,     0,     9,    10
};

static const octype_int8 occheck[] =
{
       0,    18,    46,     0,    42,    43,     6,     7,     3,     9,
      10,     0,    50,     8,     7,     3,     5,     8,    13,     8,
       8,    12,    60,     8,     4,    13,     6,    27,     3,    73,
      10,    11,    16,    50,    16,    16,     9,    75,     3,    12,
      40,    14,    15,     8,    17,    45,    46,    47,    48,     8,
      12,    40,    76,    42,    43,    55,     9,    14,    14,    12,
       8,    50,    15,     8,    17,    -1,    40,    12,    -1,    -1,
      70,    60,    -1,    73,    -1,    -1,    76,    -1,     3,     4,
       5,     6,    -1,     8,     9,    -1,    75,    12,    13,    -1,
      -1,    16,    17,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,     3,    -1,
       5,    -1,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      -1,    16,    17,     3,    -1,     5,    -1,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    16,    17
};

/* OCSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const octype_int8 ocstos[] =
{
       0,     3,     4,     5,     6,     8,     9,    12,    13,    16,
      17,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    36,    37,    38,    39,    40,     8,    12,    38,     8,
      36,    38,    34,    36,    14,    27,    36,    36,    36,     0,
       7,     6,    10,    11,    37,     9,    12,    15,    17,    29,
      16,     8,    36,    14,    21,    12,    26,    26,    36,    34,
      13,    35,    36,    36,     6,    26,    37,    36,    16,    26,
      16,    30,    31,    12,    14,    13,    32,    33,    36,    34,
      26,    33,    14
};

/* OCR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const octype_int8 ocr1[] =
{
       0,    18,    19,    20,    20,    21,    21,    21,    21,    21,
      22,    23,    23,    23,    23,    23,    23,    24,    24,    24,
      24,    25,    25,    25,    26,    27,    27,    27,    28,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    29,    29,
      29,    29,    29,    29,    29,    30,    30,    31,    32,    32,
      33,    33,    34,    35,    36,    36,    37,    37,    38,    39,
      40
};

/* OCR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const octype_int8 ocr2[] =
{
       0,     2,     1,     3,     1,     0,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     4,     3,     3,     3,     2,
       2,     2,     3,     3,     1,     1,     2,     3,     2,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     3,     4,
       2,     5,     2,     1,     2,     1,     0,     2,     2,     1,
       1,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       1
};


enum { OCENOMEM = -2 };

#define ocerrok         (ocerrstatus = 0)
#define occlearin       (occhar = OCEMPTY)

#define OCACCEPT        goto ocacceptlab
#define OCABORT         goto ocabortlab
#define OCERROR         goto ocerrorlab
#define OCNOMEM         goto ocexhaustedlab


#define OCRECOVERING()  (!!ocerrstatus)

#define OCBACKUP(Token, Value)                                    \
  do                                                              \
    if (occhar == OCEMPTY)                                        \
      {                                                           \
        occhar = (Token);                                         \
        oclval = (Value);                                         \
        OCPOPSTACK (oclen);                                       \
        ocstate = *ocssp;                                         \
        goto ocbackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        ocerror (OC_("syntax error: cannot back up")); \
        OCERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use OCerror or OCUNDEF. */
#define OCERRCODE OCUNDEF


/* Enable debugging if requested.  */
#if OCDEBUG

# ifndef OCFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define OCFPRINTF fprintf
# endif

# define OCDPRINTF(Args)                        \
do {                                            \
  if (ocdebug)                                  \
    OCFPRINTF Args;                             \
} while (0)




# define OC_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (ocdebug)                                                            \
    {                                                                     \
      OCFPRINTF (stderr, "%s ", Title);                                   \
      oc_symbol_print (stderr,                                            \
                  Kind, Value); \
      OCFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on OCO.  |
`-----------------------------------*/

static void
oc_symbol_value_print (FILE *oco,
                       ocsymbol_kind_t ockind, OCSTYPE const * const ocvaluep)
{
  FILE *ocoutput = oco;
  OC_USE (ocoutput);
  if (!ocvaluep)
    return;
  OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  OC_USE (ockind);
  OC_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on OCO.  |
`---------------------------*/

static void
oc_symbol_print (FILE *oco,
                 ocsymbol_kind_t ockind, OCSTYPE const * const ocvaluep)
{
  OCFPRINTF (oco, "%s %s (",
             ockind < OCNTOKENS ? "token" : "nterm", ocsymbol_name (ockind));

  oc_symbol_value_print (oco, ockind, ocvaluep);
  OCFPRINTF (oco, ")");
}

/*------------------------------------------------------------------.
| oc_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
oc_stack_print (oc_state_t *ocbottom, oc_state_t *octop)
{
  OCFPRINTF (stderr, "Stack now");
  for (; ocbottom <= octop; ocbottom++)
    {
      int ocbot = *ocbottom;
      OCFPRINTF (stderr, " %d", ocbot);
    }
  OCFPRINTF (stderr, "\n");
}

# define OC_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (ocdebug)                                                  \
    oc_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the OCRULE is going to be reduced.  |
`------------------------------------------------*/

static void
oc_reduce_print (oc_state_t *ocssp, OCSTYPE *ocvsp,
                 int ocrule)
{
  int oclno = ocrline[ocrule];
  int ocnrhs = ocr2[ocrule];
  int oci;
  OCFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             ocrule - 1, oclno);
  /* The symbols being reduced.  */
  for (oci = 0; oci < ocnrhs; oci++)
    {
      OCFPRINTF (stderr, "   $%d = ", oci + 1);
      oc_symbol_print (stderr,
                       OC_ACCESSING_SYMBOL (+ocssp[oci + 1 - ocnrhs]),
                       &ocvsp[(oci + 1) - (ocnrhs)]);
      OCFPRINTF (stderr, "\n");
    }
}

# define OC_REDUCE_PRINT(Rule)          \
do {                                    \
  if (ocdebug)                          \
    oc_reduce_print (ocssp, ocvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int ocdebug;
#else /* !OCDEBUG */
# define OCDPRINTF(Args) ((void) 0)
# define OC_SYMBOL_PRINT(Title, Kind, Value, Location)
# define OC_STACK_PRINT(Bottom, Top)
# define OC_REDUCE_PRINT(Rule)
#endif /* !OCDEBUG */


/* OCINITDEPTH -- initial size of the parser's stacks.  */
#ifndef OCINITDEPTH
# define OCINITDEPTH 200
#endif

/* OCMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   OCSTACK_ALLOC_MAXIMUM < OCSTACK_BYTES (OCMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef OCMAXDEPTH
# define OCMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
ocdestruct (const char *ocmsg,
            ocsymbol_kind_t ockind, OCSTYPE *ocvaluep)
{
  OC_USE (ocvaluep);
  if (!ocmsg)
    ocmsg = "Deleting";
  OC_SYMBOL_PRINT (ocmsg, ockind, ocvaluep, oclocationp);

  OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  OC_USE (ockind);
  OC_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int occhar;

/* The semantic value of the lookahead symbol.  */
OCSTYPE oclval;
/* Number of syntax errors so far.  */
int ocnerrs;




/*----------.
| ocparse.  |
`----------*/

int
ocparse (void)
{
    oc_state_fast_t ocstate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int ocerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow ocoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    OCPTRDIFF_T ocstacksize = OCINITDEPTH;

    /* The state stack: array, bottom, top.  */
    oc_state_t ocssa[OCINITDEPTH];
    oc_state_t *ocss = ocssa;
    oc_state_t *ocssp = ocss;

    /* The semantic value stack: array, bottom, top.  */
    OCSTYPE ocvsa[OCINITDEPTH];
    OCSTYPE *ocvs = ocvsa;
    OCSTYPE *ocvsp = ocvs;

  int ocn;
  /* The return value of ocparse.  */
  int ocresult;
  /* Lookahead symbol kind.  */
  ocsymbol_kind_t octoken = OCSYMBOL_OCEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  OCSTYPE ocval;



#define OCPOPSTACK(N)   (ocvsp -= (N), ocssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int oclen = 0;

  OCDPRINTF ((stderr, "Starting parse\n"));

  occhar = OCEMPTY; /* Cause a token to be read.  */

  goto ocsetstate;


/*------------------------------------------------------------.
| ocnewstate -- push a new state, which is found in ocstate.  |
`------------------------------------------------------------*/
ocnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  ocssp++;


/*--------------------------------------------------------------------.
| ocsetstate -- set current state (the top of the stack) to ocstate.  |
`--------------------------------------------------------------------*/
ocsetstate:
  OCDPRINTF ((stderr, "Entering state %d\n", ocstate));
  OC_ASSERT (0 <= ocstate && ocstate < OCNSTATES);
  OC_IGNORE_USELESS_CAST_BEGIN
  *ocssp = OC_CAST (oc_state_t, ocstate);
  OC_IGNORE_USELESS_CAST_END
  OC_STACK_PRINT (ocss, ocssp);

  if (ocss + ocstacksize - 1 <= ocssp)
#if !defined ocoverflow && !defined OCSTACK_RELOCATE
    OCNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      OCPTRDIFF_T ocsize = ocssp - ocss + 1;

# if defined ocoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        oc_state_t *ocss1 = ocss;
        OCSTYPE *ocvs1 = ocvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if ocoverflow is a macro.  */
        ocoverflow (OC_("memory exhausted"),
                    &ocss1, ocsize * OCSIZEOF (*ocssp),
                    &ocvs1, ocsize * OCSIZEOF (*ocvsp),
                    &ocstacksize);
        ocss = ocss1;
        ocvs = ocvs1;
      }
# else /* defined OCSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (OCMAXDEPTH <= ocstacksize)
        OCNOMEM;
      ocstacksize *= 2;
      if (OCMAXDEPTH < ocstacksize)
        ocstacksize = OCMAXDEPTH;

      {
        oc_state_t *ocss1 = ocss;
        union ocalloc *ocptr =
          OC_CAST (union ocalloc *,
                   OCSTACK_ALLOC (OC_CAST (OCSIZE_T, OCSTACK_BYTES (ocstacksize))));
        if (! ocptr)
          OCNOMEM;
        OCSTACK_RELOCATE (ocss_alloc, ocss);
        OCSTACK_RELOCATE (ocvs_alloc, ocvs);
#  undef OCSTACK_RELOCATE
        if (ocss1 != ocssa)
          OCSTACK_FREE (ocss1);
      }
# endif

      ocssp = ocss + ocsize - 1;
      ocvsp = ocvs + ocsize - 1;

      OC_IGNORE_USELESS_CAST_BEGIN
      OCDPRINTF ((stderr, "Stack size increased to %ld\n",
                  OC_CAST (long, ocstacksize)));
      OC_IGNORE_USELESS_CAST_END

      if (ocss + ocstacksize - 1 <= ocssp)
        OCABORT;
    }
#endif /* !defined ocoverflow && !defined OCSTACK_RELOCATE */


  if (ocstate == OCFINAL)
    OCACCEPT;

  goto ocbackup;


/*-----------.
| ocbackup.  |
`-----------*/
ocbackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  ocn = ocpact[ocstate];
  if (ocpact_value_is_default (ocn))
    goto ocdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* OCCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (occhar == OCEMPTY)
    {
      OCDPRINTF ((stderr, "Reading a token\n"));
      occhar = oclex ();
    }

  if (occhar <= OCEOF)
    {
      occhar = OCEOF;
      octoken = OCSYMBOL_OCEOF;
      OCDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (occhar == OCerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      occhar = OCUNDEF;
      octoken = OCSYMBOL_OCerror;
      goto ocerrlab1;
    }
  else
    {
      octoken = OCTRANSLATE (occhar);
      OC_SYMBOL_PRINT ("Next token is", octoken, &oclval, &oclloc);
    }

  /* If the proper action on seeing token OCTOKEN is to reduce or to
     detect an error, take that action.  */
  ocn += octoken;
  if (ocn < 0 || OCLAST < ocn || occheck[ocn] != octoken)
    goto ocdefault;
  ocn = octable[ocn];
  if (ocn <= 0)
    {
      if (octable_value_is_error (ocn))
        goto ocerrlab;
      ocn = -ocn;
      goto ocreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (ocerrstatus)
    ocerrstatus--;

  /* Shift the lookahead token.  */
  OC_SYMBOL_PRINT ("Shifting", octoken, &oclval, &oclloc);
  ocstate = ocn;
  OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ocvsp = oclval;
  OC_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  occhar = OCEMPTY;
  goto ocnewstate;


/*-----------------------------------------------------------.
| ocdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
ocdefault:
  ocn = ocdefact[ocstate];
  if (ocn == 0)
    goto ocerrlab;
  goto ocreduce;


/*-----------------------------.
| ocreduce -- do a reduction.  |
`-----------------------------*/
ocreduce:
  /* ocn is the number of a rule to reduce with.  */
  oclen = ocr2[ocn];

  /* If OCLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets OCVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to OCVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that OCVAL may be used uninitialized.  */
  ocval = ocvsp[1-oclen];


  OC_REDUCE_PRINT (ocn);
  switch (ocn)
    {
  case 2: /* xCommand: xCommand_1  */
               {Action(-1,1);}
    break;

  case 3: /* xCommand_1: xCommand_1 SCOLON xSingleCommand  */
                   {Action(0,2);}
    break;

  case 5: /* xSingleCommand: %empty  */
 {Action(2,0);}
    break;

  case 11: /* xExecute: xOdinExpr xEPString  */
              {Action(7,2);}
    break;

  case 12: /* xExecute: xEPString  */
              {Action(7,1);}
    break;

  case 13: /* xExecute: xOdinExpr EXCLPT  */
           {Action(7,1);}
    break;

  case 14: /* xExecute: EXCLPT  */
           {Action(7,0);}
    break;

  case 15: /* xExecute: xOdinExpr EXCLPT COLON xWord  */
          {Action(8,2);}
    break;

  case 16: /* xExecute: EXCLPT COLON xWord  */
          {Action(8,1);}
    break;

  case 17: /* xCopy: xOdinExpr RANGLE xOdinExpr  */
              {Action(4,2);}
    break;

  case 18: /* xCopy: xOdinExpr LANGLE xOdinExpr  */
              {Action(6,2);}
    break;

  case 19: /* xCopy: xOdinExpr RANGLE  */
           {Action(3,1);}
    break;

  case 20: /* xCopy: xOdinExpr LANGLE  */
           {Action(5,1);}
    break;

  case 21: /* xVariable: xWord EQUALS  */
           {Action(9,1);}
    break;

  case 22: /* xVariable: xWord EQUALS xOdinExpr  */
              {Action(10,2);}
    break;

  case 23: /* xVariable: xWord EQUALS xEPString  */
              {Action(10,2);}
    break;

  case 24: /* xOdinExpr: xOdinExpr1  */
               {Action(15,1);}
    break;

  case 26: /* xOdinExpr1: xOdinExpr1 xOperation  */
               {Action(0,2);}
    break;

  case 28: /* xRoot: LPAREN RPAREN  */
           {Action(16,0);}
    break;

  case 31: /* xRoot: SLASH  */
          {Action(17,0);}
    break;

  case 32: /* xRoot: SLASH xWord  */
          {Action(18,1);}
    break;

  case 33: /* xRoot: COLON xFileType  */
              {Action(21,1);}
    break;

  case 34: /* xRoot: QUESMK WORDTKleaf  */
               {Action(14,1);}
    break;

  case 35: /* xRoot: WORDTKleaf QUESMK  */
           {Action(13,1);}
    break;

  case 36: /* xRoot: PERCNT xWord  */
          {Action(25,1);}
    break;

  case 37: /* xRoot: EQUALS xWord  */
          {Action(29,1);}
    break;

  case 38: /* xOperation: PLUS xPrmType xOperation_1  */
                 {Action(19,2);}
    break;

  case 39: /* xOperation: PLUS LPAREN xOdinExpr RPAREN  */
           {Action(20,1);}
    break;

  case 40: /* xOperation: COLON xFileType  */
              {Action(21,1);}
    break;

  case 41: /* xOperation: COLON xFileType EQUALS COLON xFileType  */
              {Action(22,2);}
    break;

  case 42: /* xOperation: SLASH xWord  */
          {Action(23,1);}
    break;

  case 43: /* xOperation: SLASH  */
          {Action(24,0);}
    break;

  case 44: /* xOperation: PERCNT xWord  */
          {Action(25,1);}
    break;

  case 46: /* xOperation_1: %empty  */
     {Action(0,0);}
    break;

  case 47: /* xPrmVals: EQUALS xPrmVals_1  */
               {Action(27,1);}
    break;

  case 48: /* xPrmVals_1: xPrmVals_1 xPrmVal  */
            {Action(0,2);}
    break;

  case 51: /* xPrmVal: LPAREN xOdinExpr RPAREN  */
           {Action(28,1);}
    break;

  case 54: /* xWord: QUESMK  */
           {Action(11,0);}
    break;

  case 56: /* xEPString: EXCLPT QUESMK  */
           {Action(12,0);}
    break;

  case 58: /* WORDTKleaf: WORDTK  */
                     {Action(31,-1);}
    break;

  case 59: /* SCAN_HOSTWDleaf: SCAN_HOSTWD  */
                               {Action(30,-1);}
    break;

  case 60: /* SCAN_OBJTIDleaf: SCAN_OBJTID  */
                               {Action(32,-1);}
    break;



      default: break;
    }
  /* User semantic actions sometimes alter occhar, and that requires
     that octoken be updated with the new translation.  We take the
     approach of translating immediately before every use of octoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     OCABORT, OCACCEPT, or OCERROR immediately after altering occhar or
     if it invokes OCBACKUP.  In the case of OCABORT or OCACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of OCERROR or OCBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  OC_SYMBOL_PRINT ("-> $$ =", OC_CAST (ocsymbol_kind_t, ocr1[ocn]), &ocval, &ocloc);

  OCPOPSTACK (oclen);
  oclen = 0;

  *++ocvsp = ocval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int oclhs = ocr1[ocn] - OCNTOKENS;
    const int oci = ocpgoto[oclhs] + *ocssp;
    ocstate = (0 <= oci && oci <= OCLAST && occheck[oci] == *ocssp
               ? octable[oci]
               : ocdefgoto[oclhs]);
  }

  goto ocnewstate;


/*--------------------------------------.
| ocerrlab -- here on detecting error.  |
`--------------------------------------*/
ocerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  octoken = occhar == OCEMPTY ? OCSYMBOL_OCEMPTY : OCTRANSLATE (occhar);
  /* If not already recovering from an error, report this error.  */
  if (!ocerrstatus)
    {
      ++ocnerrs;
      ocerror (OC_("syntax error"));
    }

  if (ocerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (occhar <= OCEOF)
        {
          /* Return failure if at end of input.  */
          if (occhar == OCEOF)
            OCABORT;
        }
      else
        {
          ocdestruct ("Error: discarding",
                      octoken, &oclval);
          occhar = OCEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto ocerrlab1;


/*---------------------------------------------------.
| ocerrorlab -- error raised explicitly by OCERROR.  |
`---------------------------------------------------*/
ocerrorlab:
  /* Pacify compilers when the user code never invokes OCERROR and the
     label ocerrorlab therefore never appears in user code.  */
  if (0)
    OCERROR;
  ++ocnerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this OCERROR.  */
  OCPOPSTACK (oclen);
  oclen = 0;
  OC_STACK_PRINT (ocss, ocssp);
  ocstate = *ocssp;
  goto ocerrlab1;


/*-------------------------------------------------------------.
| ocerrlab1 -- common code for both syntax error and OCERROR.  |
`-------------------------------------------------------------*/
ocerrlab1:
  ocerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      ocn = ocpact[ocstate];
      if (!ocpact_value_is_default (ocn))
        {
          ocn += OCSYMBOL_OCerror;
          if (0 <= ocn && ocn <= OCLAST && occheck[ocn] == OCSYMBOL_OCerror)
            {
              ocn = octable[ocn];
              if (0 < ocn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (ocssp == ocss)
        OCABORT;


      ocdestruct ("Error: popping",
                  OC_ACCESSING_SYMBOL (ocstate), ocvsp);
      OCPOPSTACK (1);
      ocstate = *ocssp;
      OC_STACK_PRINT (ocss, ocssp);
    }

  OC_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ocvsp = oclval;
  OC_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  OC_SYMBOL_PRINT ("Shifting", OC_ACCESSING_SYMBOL (ocn), ocvsp, oclsp);

  ocstate = ocn;
  goto ocnewstate;


/*-------------------------------------.
| ocacceptlab -- OCACCEPT comes here.  |
`-------------------------------------*/
ocacceptlab:
  ocresult = 0;
  goto ocreturnlab;


/*-----------------------------------.
| ocabortlab -- OCABORT comes here.  |
`-----------------------------------*/
ocabortlab:
  ocresult = 1;
  goto ocreturnlab;


/*-----------------------------------------------------------.
| ocexhaustedlab -- OCNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
ocexhaustedlab:
  ocerror (OC_("memory exhausted"));
  ocresult = 2;
  goto ocreturnlab;


/*----------------------------------------------------------.
| ocreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
ocreturnlab:
  if (occhar != OCEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      octoken = OCTRANSLATE (occhar);
      ocdestruct ("Cleanup: discarding lookahead",
                  octoken, &oclval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this OCABORT or OCACCEPT.  */
  OCPOPSTACK (oclen);
  OC_STACK_PRINT (ocss, ocssp);
  while (ocssp != ocss)
    {
      ocdestruct ("Cleanup: popping",
                  OC_ACCESSING_SYMBOL (+*ocssp), ocvsp);
      OCPOPSTACK (1);
    }
#ifndef ocoverflow
  if (ocss != ocssa)
    OCSTACK_FREE (ocss);
#endif

  return ocresult;
}


