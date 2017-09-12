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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



