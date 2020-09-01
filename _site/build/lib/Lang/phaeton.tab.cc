/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 22 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

  #include <stdio.h>

  #include "phaeton.tab.hh"
  #include "lex.yy.h"

  void yyerror(yyscan_t scanner, const phaeton::Program *root, const char *msg) {
    fprintf(stderr, "%s\n", msg);
  }

#line 81 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED
# define YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 16 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

  #include "ph/AST/AST.h"

  typedef void* yyscan_t;

#line 121 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT2 = 258,
    INT8 = 259,
    INT16 = 260,
    INT32 = 261,
    INT64 = 262,
    FLOAT8 = 263,
    FLOAT16 = 264,
    FLOAT32 = 265,
    FLOAT64 = 266,
    KW_VAR = 267,
    KW_INPUT = 268,
    KW_OUTPUT = 269,
    KW_TYPE = 270,
    SYMBOL_COLON = 271,
    SYMBOL_LEFT_PAREN = 272,
    SYMBOL_RIGHT_PAREN = 273,
    SYMBOL_LEFT_BRACKET = 274,
    SYMBOL_RIGHT_BRACKET = 275,
    ARITHM_DOT = 276,
    ARITHM_ADD = 277,
    ARITHM_SUB = 278,
    ARITHM_MUL = 279,
    ARITHM_DIV = 280,
    ARITHM_PRODUCT = 281,
    EQUAL = 282,
    KW_ELEM = 283,
    KW_FIRST = 284,
    KW_LAST = 285,
    ARITHM_CARET = 286,
    INT = 287,
    ID = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 33 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

  const phaeton::Program *program;
  
  phaeton::DeclList *Decls;
  phaeton::StmtList *Stmts;
  phaeton::ExprList *Exprs;
 
  const phaeton::Decl *Decl;

  const phaeton::Stmt *Stmt;

  const phaeton::Expression *Expr;

  const phaeton::ElementDirective *ElemDirective;

  const phaeton::Identifier *Identifier;
  const phaeton::Integer *Integer;
  const phaeton::BrackExpr *Brack;
  const phaeton::ParenExpr *Paren;

  const char *string_literal;
  int integer_literal;

  int InOutSpec;
  int PosSpec;

#line 193 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (yyscan_t yyscanner, const phaeton::Program *&root);

#endif /* !YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   61

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  40
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  66

#define YYUNDEFTOK  2
#define YYMAXUTOK   288

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   113,   113,   115,   116,   118,   119,   121,   127,   128,
     129,   131,   135,   136,   138,   140,   141,   143,   144,   145,
     147,   148,   151,   154,   158,   159,   162,   166,   167,   168,
     169,   171,   173,   175,   177,   179,   180,   184,   185,   187,
     188
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT2", "INT8", "INT16", "INT32",
  "INT64", "FLOAT8", "FLOAT16", "FLOAT32", "FLOAT64", "KW_VAR", "KW_INPUT",
  "KW_OUTPUT", "KW_TYPE", "SYMBOL_COLON", "SYMBOL_LEFT_PAREN",
  "SYMBOL_RIGHT_PAREN", "SYMBOL_LEFT_BRACKET", "SYMBOL_RIGHT_BRACKET",
  "ARITHM_DOT", "ARITHM_ADD", "ARITHM_SUB", "ARITHM_MUL", "ARITHM_DIV",
  "ARITHM_PRODUCT", "EQUAL", "KW_ELEM", "KW_FIRST", "KW_LAST",
  "ARITHM_CARET", "INT", "ID", "$accept", "program", "decl_list", "decl",
  "var_decl", "in_out_spec", "type_decl", "stmt_list", "stmt", "type_expr",
  "expr", "term", "factor", "atom", "identifier", "integer", "brack_expr",
  "paren_expr", "expr_list", "pos_spec", "elem_direct", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288
};
# endif

#define YYPACT_NINF -33

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-33)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      21,    29,   -20,    35,     0,   -33,   -33,   -33,    29,    29,
     -20,   -33,    32,   -33,    18,   -33,   -20,   -33,   -33,    36,
      -8,   -33,    19,   -20,   -33,    26,    -8,   -33,   -33,   -33,
     -10,   -33,   -24,   -33,   -16,   -33,   -16,   -33,   -33,    22,
       6,     3,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
      37,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,     0,     0,    39,     4,     5,     6,     8,     8,
       0,    31,     0,     1,     0,     3,     0,     9,    10,     0,
       0,    35,     0,     2,    13,     0,     0,    11,    15,    16,
       0,    32,     0,    12,     0,     7,     0,    33,    36,    17,
      20,    24,    27,    28,    29,    30,    37,    38,    40,    14,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    18,
      19,    23,    21,    22,    25,    26
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -33,   -33,   -33,    50,   -33,    38,   -33,   -33,    33,    31,
     -32,   -14,    -7,   -33,    -2,    39,    12,   -33,   -33,   -33,
     -33
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,     5,     6,    10,     7,    23,    24,    27,
      38,    39,    40,    41,    42,    43,    44,    45,    30,    48,
      16
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      12,    36,    49,    21,    50,    46,    47,    36,    19,    21,
      37,    21,     1,    11,    25,     2,    31,    11,    28,    59,
      60,    25,    31,    11,    28,    11,    22,    53,    14,    56,
      54,    55,    29,     1,    57,    13,     2,    21,    29,    61,
      62,    63,     8,     9,    51,    52,    17,    18,    20,    64,
      65,    31,    26,    34,    15,    58,    33,    35,     0,     0,
       0,    32
};

static const yytype_int8 yycheck[] =
{
       2,    17,    34,    19,    36,    29,    30,    17,    10,    19,
      20,    19,    12,    33,    16,    15,    32,    33,    20,    51,
      52,    23,    32,    33,    26,    33,    14,    21,    28,    26,
      24,    25,    20,    12,    31,     0,    15,    19,    26,    53,
      54,    55,    13,    14,    22,    23,     8,     9,    16,    56,
      57,    32,    16,    27,     4,    18,    23,    26,    -1,    -1,
      -1,    22
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    12,    15,    35,    36,    37,    38,    40,    13,    14,
      39,    33,    48,     0,    28,    37,    54,    39,    39,    48,
      16,    19,    50,    41,    42,    48,    16,    43,    48,    50,
      52,    32,    49,    42,    27,    43,    17,    20,    44,    45,
      46,    47,    48,    49,    50,    51,    29,    30,    53,    44,
      44,    22,    23,    21,    24,    25,    26,    31,    18,    44,
      44,    45,    45,    45,    46,    46
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    36,    36,    37,    37,    38,    39,    39,
      39,    40,    41,    41,    42,    43,    43,    44,    44,    44,
      45,    45,    45,    45,    46,    46,    46,    47,    47,    47,
      47,    48,    49,    50,    51,    52,    52,    53,    53,    54,
      54
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     2,     1,     1,     1,     5,     0,     2,
       2,     4,     2,     1,     3,     1,     1,     1,     3,     3,
       1,     3,     3,     3,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     0,     2,     1,     1,     0,
       4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
        yyerror (yyscanner, root, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, yyscanner, root); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, yyscan_t yyscanner, const phaeton::Program *&root)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yyscanner);
  YYUSE (root);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, yyscan_t yyscanner, const phaeton::Program *&root)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, yyscanner, root);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, yyscan_t yyscanner, const phaeton::Program *&root)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , yyscanner, root);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, yyscanner, root); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, yyscan_t yyscanner, const phaeton::Program *&root)
{
  YYUSE (yyvaluep);
  YYUSE (yyscanner);
  YYUSE (root);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t yyscanner, const phaeton::Program *&root)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, yyscanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
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
  case 2:
#line 113 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { root = phaeton::Program::create((yyvsp[-2].Decls), (yyvsp[0].Stmts), (yyvsp[-1].ElemDirective)); }
#line 1341 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 3:
#line 115 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Decls) = phaeton::DeclList::append((yyvsp[-1].Decls), (yyvsp[0].Decl)); }
#line 1347 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 4:
#line 116 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Decls) = phaeton::DeclList::create((yyvsp[0].Decl)); }
#line 1353 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 7:
#line 121 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
             (yyval.Decl) = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_VarDecl,
                               (yyvsp[-2].Identifier), (yyvsp[0].Expr),
                               (phaeton::Decl::InOutSpecifier)(yyvsp[-3].InOutSpec));
           }
#line 1363 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 8:
#line 127 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.InOutSpec) = phaeton::Decl::IO_SPEC_Empty; }
#line 1369 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 9:
#line 128 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.InOutSpec) = phaeton::Decl::IO_SPEC_Input | (yyvsp[0].InOutSpec); }
#line 1375 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 10:
#line 129 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.InOutSpec) = phaeton::Decl::IO_SPEC_Output | (yyvsp[0].InOutSpec); }
#line 1381 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 11:
#line 131 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
              (yyval.Decl) = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_TypeDecl, (yyvsp[-2].Identifier), (yyvsp[0].Expr));
            }
#line 1389 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 12:
#line 135 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Stmts) = phaeton::StmtList::append((yyvsp[-1].Stmts), (yyvsp[0].Stmt)); }
#line 1395 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 13:
#line 136 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Stmts) = phaeton::StmtList::create((yyvsp[0].Stmt)); }
#line 1401 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 14:
#line 138 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Stmt) = phaeton::Stmt::create((yyvsp[-2].Identifier), (yyvsp[0].Expr)); }
#line 1407 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 15:
#line 140 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Identifier); }
#line 1413 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 16:
#line 141 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Brack); }
#line 1419 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 18:
#line 144 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_AddExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr)); }
#line 1425 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 19:
#line 145 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_SubExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr)); }
#line 1431 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 21:
#line 148 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
         (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_MulExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr));
       }
#line 1439 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 22:
#line 151 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
         (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_DivExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr));
       }
#line 1447 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 23:
#line 154 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
         (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ContractionExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr));
       }
#line 1455 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 25:
#line 159 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
           (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ProductExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr));
         }
#line 1463 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 26:
#line 162 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
           (yyval.Expr) = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_TranspositionExpr, (yyvsp[-2].Expr), (yyvsp[0].Expr));
         }
#line 1471 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 27:
#line 166 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Identifier); }
#line 1477 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 28:
#line 167 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Integer); }
#line 1483 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 29:
#line 168 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Brack); }
#line 1489 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 30:
#line 169 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Expr) = (const phaeton::Expression *)(yyvsp[0].Paren); }
#line 1495 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 31:
#line 171 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Identifier) = phaeton::Identifier::create((yyvsp[0].string_literal)); }
#line 1501 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 32:
#line 173 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Integer) = phaeton::Integer::create((yyvsp[0].integer_literal)); }
#line 1507 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 33:
#line 175 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Brack) = phaeton::BrackExpr::create((yyvsp[-1].Exprs)); }
#line 1513 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 34:
#line 177 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Paren) = phaeton::ParenExpr::create((yyvsp[-1].Expr)); }
#line 1519 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 35:
#line 179 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.Exprs) = phaeton::ExprList::create(); }
#line 1525 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 36:
#line 180 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
              (yyval.Exprs) = phaeton::ExprList::append((yyvsp[-1].Exprs), (yyvsp[0].Expr));
            }
#line 1533 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 37:
#line 184 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.PosSpec) = phaeton::ElementDirective::POS_First; }
#line 1539 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 38:
#line 185 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.PosSpec) = phaeton::ElementDirective::POS_Last; }
#line 1545 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 39:
#line 187 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    { (yyval.ElemDirective) = nullptr; }
#line 1551 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;

  case 40:
#line 188 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"
    {
                (yyval.ElemDirective) = phaeton::ElementDirective::create(phaeton::ASTNode::AST_NODE_KIND_ElementDirective,
                                        (phaeton::ElementDirective::POSSpecifier)(yyvsp[0].PosSpec), (yyvsp[-1].Integer), (yyvsp[-2].Brack));
              }
#line 1560 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"
    break;


#line 1564 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.cc"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (yyscanner, root, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yyscanner, root, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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
                      yytoken, &yylval, yyscanner, root);
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp, yyscanner, root);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (yyscanner, root, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, yyscanner, root);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yyscanner, root);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 201 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

