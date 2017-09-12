/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _BANG_t = 258,
     _BANG_EQUALS_t = 259,
     _AMPER_t = 260,
     _AMPERAMPER_t = 261,
     _LPAREN_t = 262,
     _RPAREN_t = 263,
     _PLUS_EQUALS_t = 264,
     _COLON_t = 265,
     _SEMIC_t = 266,
     _LANGLE_t = 267,
     _LANGLE_EQUALS_t = 268,
     _EQUALS_t = 269,
     _RANGLE_t = 270,
     _RANGLE_EQUALS_t = 271,
     _QUESTION_EQUALS_t = 272,
     _LBRACKET_t = 273,
     _RBRACKET_t = 274,
     ACTIONS_t = 275,
     BIND_t = 276,
     CASE_t = 277,
     DEFAULT_t = 278,
     ELSE_t = 279,
     EXISTING_t = 280,
     FOR_t = 281,
     IF_t = 282,
     IGNORE_t = 283,
     IN_t = 284,
     INCLUDE_t = 285,
     LOCAL_t = 286,
     MODULE_t = 287,
     ON_t = 288,
     PIECEMEAL_t = 289,
     QUIETLY_t = 290,
     RETURN_t = 291,
     RULE_t = 292,
     SWITCH_t = 293,
     TOGETHER_t = 294,
     UPDATED_t = 295,
     WHILE_t = 296,
     _LBRACE_t = 297,
     _BAR_t = 298,
     _BARBAR_t = 299,
     _RBRACE_t = 300,
     ARG = 301,
     STRING = 302
   };
#endif
#define _BANG_t 258
#define _BANG_EQUALS_t 259
#define _AMPER_t 260
#define _AMPERAMPER_t 261
#define _LPAREN_t 262
#define _RPAREN_t 263
#define _PLUS_EQUALS_t 264
#define _COLON_t 265
#define _SEMIC_t 266
#define _LANGLE_t 267
#define _LANGLE_EQUALS_t 268
#define _EQUALS_t 269
#define _RANGLE_t 270
#define _RANGLE_EQUALS_t 271
#define _QUESTION_EQUALS_t 272
#define _LBRACKET_t 273
#define _RBRACKET_t 274
#define ACTIONS_t 275
#define BIND_t 276
#define CASE_t 277
#define DEFAULT_t 278
#define ELSE_t 279
#define EXISTING_t 280
#define FOR_t 281
#define IF_t 282
#define IGNORE_t 283
#define IN_t 284
#define INCLUDE_t 285
#define LOCAL_t 286
#define MODULE_t 287
#define ON_t 288
#define PIECEMEAL_t 289
#define QUIETLY_t 290
#define RETURN_t 291
#define RULE_t 292
#define SWITCH_t 293
#define TOGETHER_t 294
#define UPDATED_t 295
#define WHILE_t 296
#define _LBRACE_t 297
#define _BAR_t 298
#define _BARBAR_t 299
#define _RBRACE_t 300
#define ARG 301
#define STRING 302




/* Copy the first part of user declarations.  */
#line 97 "jamgram.y"

#include "jam.h"

#include "lists.h"
#include "parse.h"
#include "scan.h"
#include "compile.h"
#include "newstr.h"
#include "rules.h"

# define YYMAXDEPTH 10000	/* for OSF and other less endowed yaccs */

# define F0 (LIST *(*)(PARSE *, FRAME *))0
# define P0 (PARSE *)0
# define S0 (char *)0

# define pappend( l,r )    	parse_make( compile_append,l,r,P0,S0,S0,0 )
# define peval( c,l,r )	parse_make( compile_eval,l,r,P0,S0,S0,c )
# define pfor( s,l,r,x )    	parse_make( compile_foreach,l,r,P0,s,S0,x )
# define pif( l,r,t )	  	parse_make( compile_if,l,r,t,S0,S0,0 )
# define pincl( l )       	parse_make( compile_include,l,P0,P0,S0,S0,0 )
# define plist( s )	  	parse_make( compile_list,P0,P0,P0,s,S0,0 )
# define plocal( l,r,t )  	parse_make( compile_local,l,r,t,S0,S0,0 )
# define pmodule( l,r )	  	parse_make( compile_module,l,r,P0,S0,S0,0 )
# define pnull()	  	parse_make( compile_null,P0,P0,P0,S0,S0,0 )
# define pon( l,r )	  	parse_make( compile_on,l,r,P0,S0,S0,0 )
# define prule( s,p )     	parse_make( compile_rule,p,P0,P0,s,S0,0 )
# define prules( l,r )	  	parse_make( compile_rules,l,r,P0,S0,S0,0 )
# define pset( l,r,a )          parse_make( compile_set,l,r,P0,S0,S0,a )
# define pset1( l,r,t,a )	parse_make( compile_settings,l,r,t,S0,S0,a )
# define psetc( s,p,a,l )     	parse_make( compile_setcomp,p,a,P0,s,S0,l )
# define psete( s,l,s1,f ) 	parse_make( compile_setexec,l,P0,P0,s,s1,f )
# define pswitch( l,r )   	parse_make( compile_switch,l,r,P0,S0,S0,0 )
# define pwhile( l,r )   	parse_make( compile_while,l,r,P0,S0,S0,0 )

# define pnode( l,r )    	parse_make( F0,l,r,P0,S0,S0,0 )
# define psnode( s,l )     	parse_make( F0,l,P0,P0,s,S0,0 )



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 220 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  41
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   226

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  24
/* YYNRULES -- Number of rules. */
#define YYNRULES  74
/* YYNRULES -- Number of states. */
#define YYNSTATES  154

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   302

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    10,    12,    15,    21,
      22,    25,    27,    31,    32,    34,    35,    39,    43,    47,
      52,    59,    63,    72,    78,    84,    90,    96,   104,   110,
     114,   115,   116,   126,   128,   130,   132,   135,   137,   141,
     145,   149,   153,   157,   161,   165,   169,   173,   177,   181,
     184,   188,   189,   192,   197,   199,   203,   205,   206,   209,
     211,   212,   217,   220,   225,   230,   231,   234,   236,   238,
     240,   242,   244,   246,   247
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      49,     0,    -1,    -1,    51,    -1,    52,    -1,    51,    -1,
      56,    -1,    56,    51,    -1,    31,    64,    53,    11,    50,
      -1,    -1,    14,    64,    -1,    52,    -1,     7,    63,     8,
      -1,    -1,    31,    -1,    -1,    42,    50,    45,    -1,    30,
      64,    11,    -1,    46,    63,    11,    -1,    66,    59,    64,
      11,    -1,    66,    33,    64,    59,    64,    11,    -1,    36,
      64,    11,    -1,    26,    55,    46,    29,    64,    42,    50,
      45,    -1,    38,    64,    42,    61,    45,    -1,    27,    60,
      42,    50,    45,    -1,    32,    64,    42,    50,    45,    -1,
      41,    60,    42,    50,    45,    -1,    27,    60,    42,    50,
      45,    24,    56,    -1,    55,    37,    46,    54,    56,    -1,
      33,    66,    56,    -1,    -1,    -1,    20,    69,    46,    71,
      42,    57,    47,    58,    45,    -1,    14,    -1,     9,    -1,
      17,    -1,    23,    14,    -1,    66,    -1,    60,    14,    60,
      -1,    60,     4,    60,    -1,    60,    12,    60,    -1,    60,
      13,    60,    -1,    60,    15,    60,    -1,    60,    16,    60,
      -1,    60,     5,    60,    -1,    60,     6,    60,    -1,    60,
      43,    60,    -1,    60,    44,    60,    -1,    66,    29,    64,
      -1,     3,    60,    -1,     7,    60,     8,    -1,    -1,    62,
      61,    -1,    22,    46,    10,    50,    -1,    64,    -1,    64,
      10,    63,    -1,    65,    -1,    -1,    65,    66,    -1,    46,
      -1,    -1,    18,    67,    68,    19,    -1,    66,    63,    -1,
      33,    66,    66,    63,    -1,    33,    66,    36,    64,    -1,
      -1,    69,    70,    -1,    40,    -1,    39,    -1,    28,    -1,
      35,    -1,    34,    -1,    25,    -1,    -1,    21,    64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   139,   139,   141,   152,   154,   158,   160,   162,   167,
     170,   172,   176,   179,   182,   185,   188,   190,   192,   194,
     196,   198,   200,   202,   204,   206,   208,   210,   212,   214,
     217,   219,   216,   228,   230,   232,   234,   241,   243,   245,
     247,   249,   251,   253,   255,   257,   259,   261,   263,   265,
     267,   279,   280,   284,   293,   295,   305,   310,   311,   315,
     317,   317,   326,   328,   330,   341,   342,   346,   348,   350,
     352,   354,   356,   366,   367
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "_BANG_t", "_BANG_EQUALS_t", "_AMPER_t", 
  "_AMPERAMPER_t", "_LPAREN_t", "_RPAREN_t", "_PLUS_EQUALS_t", "_COLON_t", 
  "_SEMIC_t", "_LANGLE_t", "_LANGLE_EQUALS_t", "_EQUALS_t", "_RANGLE_t", 
  "_RANGLE_EQUALS_t", "_QUESTION_EQUALS_t", "_LBRACKET_t", "_RBRACKET_t", 
  "ACTIONS_t", "BIND_t", "CASE_t", "DEFAULT_t", "ELSE_t", "EXISTING_t", 
  "FOR_t", "IF_t", "IGNORE_t", "IN_t", "INCLUDE_t", "LOCAL_t", "MODULE_t", 
  "ON_t", "PIECEMEAL_t", "QUIETLY_t", "RETURN_t", "RULE_t", "SWITCH_t", 
  "TOGETHER_t", "UPDATED_t", "WHILE_t", "_LBRACE_t", "_BAR_t", 
  "_BARBAR_t", "_RBRACE_t", "ARG", "STRING", "$accept", "run", "block", 
  "rules", "null", "assign_list_opt", "arglist_opt", "local_opt", "rule", 
  "@1", "@2", "assign", "expr", "cases", "case", "lol", "list", "listp", 
  "arg", "@3", "func", "eflags", "eflag", "bindlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    48,    49,    49,    50,    50,    51,    51,    51,    52,
      53,    53,    54,    54,    55,    55,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      57,    58,    56,    59,    59,    59,    59,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    61,    61,    62,    63,    63,    64,    65,    65,    66,
      67,    66,    68,    68,    68,    69,    69,    70,    70,    70,
      70,    70,    70,    71,    71
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     2,     5,     0,
       2,     1,     3,     0,     1,     0,     3,     3,     3,     4,
       6,     3,     8,     5,     5,     5,     5,     7,     5,     3,
       0,     0,     9,     1,     1,     1,     2,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     0,     2,     4,     1,     3,     1,     0,     2,     1,
       0,     4,     2,     4,     4,     0,     2,     1,     1,     1,
       1,     1,     1,     0,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,    60,    65,    15,     0,    57,    57,    57,     0,    57,
      57,     0,     9,    59,     0,     3,     0,     6,     0,     0,
       0,    14,     0,     0,     0,    59,     0,    37,     0,    56,
       9,     0,    15,     0,     0,     0,     0,     5,     4,     0,
      54,     1,     0,     7,    34,    33,    35,     0,    57,    57,
       0,    57,     0,    72,    69,    71,    70,    68,    67,    73,
      66,     0,    49,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     9,     0,     0,    57,    17,    58,    57,    11,
       0,     9,    29,    21,    51,     9,    16,    18,    57,    13,
      36,     0,     0,     0,    62,    61,    57,     0,    57,    50,
      39,    44,    45,    40,    41,    38,    42,    43,     0,    46,
      47,    48,    10,     9,     0,     0,     0,    51,     0,    55,
      57,    15,    57,    19,    57,    57,    74,    30,     0,    24,
       8,    25,     0,    23,    52,    26,     0,    28,     0,    64,
      63,     0,     9,    15,     9,    12,    20,    31,     0,    27,
      53,     0,    22,    32
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    14,    36,    37,    38,    80,   121,    16,    17,   141,
     151,    49,    26,   116,   117,    39,    40,    29,    27,    19,
      52,    20,    60,    97
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -66
static const short yypact[] =
{
     120,   -66,   -66,   -13,     6,   -66,   -16,   -66,     5,   -66,
     -66,     6,   120,    29,    22,   -66,     9,   120,   165,    -3,
     167,   -66,    30,     6,     6,   -66,    21,    13,    44,     5,
      66,    40,   145,    72,    42,    54,    43,   -66,   -66,    75,
      77,   -66,    45,   -66,   -66,   -66,   -66,    76,   -66,   -66,
       5,   -66,    70,   -66,   -66,   -66,   -66,   -66,   -66,    74,
     -66,    81,   -66,   111,     6,     6,     6,     6,     6,     6,
       6,     6,   120,     6,     6,   -66,   -66,   -66,   -66,   -66,
      88,   120,   -66,   -66,    79,   120,   -66,   -66,   -66,   107,
     -66,   122,   117,    -8,   -66,   -66,   -66,    87,   -66,   -66,
      41,   210,   210,   -66,   -66,    41,   -66,   -66,    85,   204,
     204,   -66,   -66,   120,    89,    86,    90,    79,    92,   -66,
     -66,   145,   -66,   -66,   -66,   -66,   -66,   -66,    91,   124,
     -66,   -66,   131,   -66,   -66,   -66,   141,   -66,   148,   -66,
     -66,   113,   120,   145,   120,   -66,   -66,   -66,   119,   -66,
     -66,   123,   -66,   -66
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
     -66,   -66,   -65,    14,   137,   -66,   -66,   166,   -21,   -66,
     -66,    82,    38,    53,   -66,   -47,    -4,   -66,     0,   -66,
     -66,   -66,   -66,   -66
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -58
static const short yytable[] =
{
      18,    28,    30,    31,    94,    33,    34,   108,    32,    23,
       1,    82,    18,    24,    15,     1,   114,    18,    21,    51,
     118,   -14,    41,     1,     1,    64,    65,    66,   124,    77,
      50,    43,    18,    67,    68,    69,    70,    71,    25,   -57,
     -57,   119,    75,    25,    91,    92,    42,   -57,   130,    35,
      93,    25,    25,    67,    68,    76,    70,    71,    64,    65,
      66,    62,    63,    72,    73,    74,    67,    68,    69,    70,
      71,   111,    18,   136,   112,   -57,    61,   148,   140,   150,
      78,    18,    81,    83,    84,    18,    87,    88,    86,    95,
      90,    89,   126,   125,   128,    96,    85,    73,    74,   113,
     137,   115,   100,   101,   102,   103,   104,   105,   106,   107,
      98,   109,   110,    18,   120,    64,    65,    66,   138,    99,
     139,    18,   149,    67,    68,    69,    70,    71,   123,   127,
     129,    44,   132,   142,   131,   133,    45,   135,     1,    46,
       2,   144,    18,    18,    18,    47,     3,     4,   143,   145,
       5,     6,     7,     8,    73,    74,     9,   -15,    10,   146,
     147,    11,    12,     1,   152,     2,    13,    79,   153,    22,
     134,     3,     4,   122,    44,     5,    21,     7,     8,    45,
       0,     9,    46,    10,     0,     0,    11,    12,    47,     0,
       0,    13,    53,     0,     0,    54,     0,     0,    48,     0,
       0,    55,    56,     0,     0,     0,    57,    58,    64,    65,
      66,     0,     0,    59,    64,     0,    67,    68,    69,    70,
      71,     0,    67,    68,    69,    70,    71
};

static const short yycheck[] =
{
       0,     5,     6,     7,    51,     9,    10,    72,     8,     3,
      18,    32,    12,     7,     0,    18,    81,    17,    31,    19,
      85,    37,     0,    18,    18,     4,     5,     6,    36,    29,
      33,    17,    32,    12,    13,    14,    15,    16,    46,    10,
      11,    88,    29,    46,    48,    49,    37,    18,   113,    11,
      50,    46,    46,    12,    13,    11,    15,    16,     4,     5,
       6,    23,    24,    42,    43,    44,    12,    13,    14,    15,
      16,    75,    72,   120,    78,    46,    46,   142,   125,   144,
      14,    81,    42,    11,    42,    85,    11,    10,    45,    19,
      14,    46,    96,    93,    98,    21,    42,    43,    44,    11,
     121,    22,    64,    65,    66,    67,    68,    69,    70,    71,
      29,    73,    74,   113,     7,     4,     5,     6,   122,     8,
     124,   121,   143,    12,    13,    14,    15,    16,    11,    42,
      45,     9,    46,    42,    45,    45,    14,    45,    18,    17,
      20,    10,   142,   143,   144,    23,    26,    27,    24,     8,
      30,    31,    32,    33,    43,    44,    36,    37,    38,    11,
      47,    41,    42,    18,    45,    20,    46,    30,    45,     3,
     117,    26,    27,    91,     9,    30,    31,    32,    33,    14,
      -1,    36,    17,    38,    -1,    -1,    41,    42,    23,    -1,
      -1,    46,    25,    -1,    -1,    28,    -1,    -1,    33,    -1,
      -1,    34,    35,    -1,    -1,    -1,    39,    40,     4,     5,
       6,    -1,    -1,    46,     4,    -1,    12,    13,    14,    15,
      16,    -1,    12,    13,    14,    15,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    18,    20,    26,    27,    30,    31,    32,    33,    36,
      38,    41,    42,    46,    49,    51,    55,    56,    66,    67,
      69,    31,    55,     3,     7,    46,    60,    66,    64,    65,
      64,    64,    66,    64,    64,    60,    50,    51,    52,    63,
      64,     0,    37,    51,     9,    14,    17,    23,    33,    59,
      33,    66,    68,    25,    28,    34,    35,    39,    40,    46,
      70,    46,    60,    60,     4,     5,     6,    12,    13,    14,
      15,    16,    42,    43,    44,    29,    11,    66,    14,    52,
      53,    42,    56,    11,    42,    42,    45,    11,    10,    46,
      14,    64,    64,    66,    63,    19,    21,    71,    29,     8,
      60,    60,    60,    60,    60,    60,    60,    60,    50,    60,
      60,    64,    64,    11,    50,    22,    61,    62,    50,    63,
       7,    54,    59,    11,    36,    66,    64,    42,    64,    45,
      50,    45,    46,    45,    61,    45,    63,    56,    64,    64,
      63,    57,    42,    24,    10,     8,    11,    47,    50,    56,
      50,    58,    45,    45
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 142 "jamgram.y"
    { parse_save( yyvsp[0].parse ); }
    break;

  case 4:
#line 153 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; }
    break;

  case 5:
#line 155 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; }
    break;

  case 6:
#line 159 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; }
    break;

  case 7:
#line 161 "jamgram.y"
    { yyval.parse = prules( yyvsp[-1].parse, yyvsp[0].parse ); }
    break;

  case 8:
#line 163 "jamgram.y"
    { yyval.parse = plocal( yyvsp[-3].parse, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 9:
#line 167 "jamgram.y"
    { yyval.parse = pnull(); }
    break;

  case 10:
#line 171 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; yyval.number = ASSIGN_SET; }
    break;

  case 11:
#line 173 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; yyval.number = ASSIGN_APPEND; }
    break;

  case 12:
#line 177 "jamgram.y"
    { yyval.parse = yyvsp[-1].parse; }
    break;

  case 13:
#line 179 "jamgram.y"
    { yyval.parse = P0; }
    break;

  case 14:
#line 183 "jamgram.y"
    { yyval.number = 1; }
    break;

  case 15:
#line 185 "jamgram.y"
    { yyval.number = 0; }
    break;

  case 16:
#line 189 "jamgram.y"
    { yyval.parse = yyvsp[-1].parse; }
    break;

  case 17:
#line 191 "jamgram.y"
    { yyval.parse = pincl( yyvsp[-1].parse ); }
    break;

  case 18:
#line 193 "jamgram.y"
    { yyval.parse = prule( yyvsp[-2].string, yyvsp[-1].parse ); }
    break;

  case 19:
#line 195 "jamgram.y"
    { yyval.parse = pset( yyvsp[-3].parse, yyvsp[-1].parse, yyvsp[-2].number ); }
    break;

  case 20:
#line 197 "jamgram.y"
    { yyval.parse = pset1( yyvsp[-5].parse, yyvsp[-3].parse, yyvsp[-1].parse, yyvsp[-2].number ); }
    break;

  case 21:
#line 199 "jamgram.y"
    { yyval.parse = yyvsp[-1].parse; }
    break;

  case 22:
#line 201 "jamgram.y"
    { yyval.parse = pfor( yyvsp[-5].string, yyvsp[-3].parse, yyvsp[-1].parse, yyvsp[-6].number ); }
    break;

  case 23:
#line 203 "jamgram.y"
    { yyval.parse = pswitch( yyvsp[-3].parse, yyvsp[-1].parse ); }
    break;

  case 24:
#line 205 "jamgram.y"
    { yyval.parse = pif( yyvsp[-3].parse, yyvsp[-1].parse, pnull() ); }
    break;

  case 25:
#line 207 "jamgram.y"
    { yyval.parse = pmodule( yyvsp[-3].parse, yyvsp[-1].parse ); }
    break;

  case 26:
#line 209 "jamgram.y"
    { yyval.parse = pwhile( yyvsp[-3].parse, yyvsp[-1].parse ); }
    break;

  case 27:
#line 211 "jamgram.y"
    { yyval.parse = pif( yyvsp[-5].parse, yyvsp[-3].parse, yyvsp[0].parse ); }
    break;

  case 28:
#line 213 "jamgram.y"
    { yyval.parse = psetc( yyvsp[-2].string, yyvsp[0].parse, yyvsp[-1].parse, yyvsp[-4].number ); }
    break;

  case 29:
#line 215 "jamgram.y"
    { yyval.parse = pon( yyvsp[-1].parse, yyvsp[0].parse ); }
    break;

  case 30:
#line 217 "jamgram.y"
    { yymode( SCAN_STRING ); }
    break;

  case 31:
#line 219 "jamgram.y"
    { yymode( SCAN_NORMAL ); }
    break;

  case 32:
#line 221 "jamgram.y"
    { yyval.parse = psete( yyvsp[-6].string,yyvsp[-5].parse,yyvsp[-2].string,yyvsp[-7].number ); }
    break;

  case 33:
#line 229 "jamgram.y"
    { yyval.number = ASSIGN_SET; }
    break;

  case 34:
#line 231 "jamgram.y"
    { yyval.number = ASSIGN_APPEND; }
    break;

  case 35:
#line 233 "jamgram.y"
    { yyval.number = ASSIGN_DEFAULT; }
    break;

  case 36:
#line 235 "jamgram.y"
    { yyval.number = ASSIGN_DEFAULT; }
    break;

  case 37:
#line 242 "jamgram.y"
    { yyval.parse = peval( EXPR_EXISTS, yyvsp[0].parse, pnull() ); }
    break;

  case 38:
#line 244 "jamgram.y"
    { yyval.parse = peval( EXPR_EQUALS, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 39:
#line 246 "jamgram.y"
    { yyval.parse = peval( EXPR_NOTEQ, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 40:
#line 248 "jamgram.y"
    { yyval.parse = peval( EXPR_LESS, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 41:
#line 250 "jamgram.y"
    { yyval.parse = peval( EXPR_LESSEQ, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 42:
#line 252 "jamgram.y"
    { yyval.parse = peval( EXPR_MORE, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 43:
#line 254 "jamgram.y"
    { yyval.parse = peval( EXPR_MOREEQ, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 44:
#line 256 "jamgram.y"
    { yyval.parse = peval( EXPR_AND, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 45:
#line 258 "jamgram.y"
    { yyval.parse = peval( EXPR_AND, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 46:
#line 260 "jamgram.y"
    { yyval.parse = peval( EXPR_OR, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 47:
#line 262 "jamgram.y"
    { yyval.parse = peval( EXPR_OR, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 48:
#line 264 "jamgram.y"
    { yyval.parse = peval( EXPR_IN, yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 49:
#line 266 "jamgram.y"
    { yyval.parse = peval( EXPR_NOT, yyvsp[0].parse, pnull() ); }
    break;

  case 50:
#line 268 "jamgram.y"
    { yyval.parse = yyvsp[-1].parse; }
    break;

  case 51:
#line 279 "jamgram.y"
    { yyval.parse = P0; }
    break;

  case 52:
#line 281 "jamgram.y"
    { yyval.parse = pnode( yyvsp[-1].parse, yyvsp[0].parse ); }
    break;

  case 53:
#line 285 "jamgram.y"
    { yyval.parse = psnode( yyvsp[-2].string, yyvsp[0].parse ); }
    break;

  case 54:
#line 294 "jamgram.y"
    { yyval.parse = pnode( P0, yyvsp[0].parse ); }
    break;

  case 55:
#line 296 "jamgram.y"
    { yyval.parse = pnode( yyvsp[0].parse, yyvsp[-2].parse ); }
    break;

  case 56:
#line 306 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; yymode( SCAN_NORMAL ); }
    break;

  case 57:
#line 310 "jamgram.y"
    { yyval.parse = pnull(); yymode( SCAN_PUNCT ); }
    break;

  case 58:
#line 312 "jamgram.y"
    { yyval.parse = pappend( yyvsp[-1].parse, yyvsp[0].parse ); }
    break;

  case 59:
#line 316 "jamgram.y"
    { yyval.parse = plist( yyvsp[0].string ); }
    break;

  case 60:
#line 317 "jamgram.y"
    { yymode( SCAN_NORMAL ); }
    break;

  case 61:
#line 318 "jamgram.y"
    { yyval.parse = yyvsp[-1].parse; }
    break;

  case 62:
#line 327 "jamgram.y"
    { yyval.parse = prule( yyvsp[-1].string, yyvsp[0].parse ); }
    break;

  case 63:
#line 329 "jamgram.y"
    { yyval.parse = pon( yyvsp[-2].parse, prule( yyvsp[-1].string, yyvsp[0].parse ) ); }
    break;

  case 64:
#line 331 "jamgram.y"
    { yyval.parse = pon( yyvsp[-2].parse, yyvsp[0].parse ); }
    break;

  case 65:
#line 341 "jamgram.y"
    { yyval.number = 0; }
    break;

  case 66:
#line 343 "jamgram.y"
    { yyval.number = yyvsp[-1].number | yyvsp[0].number; }
    break;

  case 67:
#line 347 "jamgram.y"
    { yyval.number = EXEC_UPDATED; }
    break;

  case 68:
#line 349 "jamgram.y"
    { yyval.number = EXEC_TOGETHER; }
    break;

  case 69:
#line 351 "jamgram.y"
    { yyval.number = EXEC_IGNORE; }
    break;

  case 70:
#line 353 "jamgram.y"
    { yyval.number = EXEC_QUIETLY; }
    break;

  case 71:
#line 355 "jamgram.y"
    { yyval.number = EXEC_PIECEMEAL; }
    break;

  case 72:
#line 357 "jamgram.y"
    { yyval.number = EXEC_EXISTING; }
    break;

  case 73:
#line 366 "jamgram.y"
    { yyval.parse = pnull(); }
    break;

  case 74:
#line 368 "jamgram.y"
    { yyval.parse = yyvsp[0].parse; }
    break;


    }

/* Line 991 of yacc.c.  */
#line 1604 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}



