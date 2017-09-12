%{
//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Lie-Quan Lee
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================



#ifdef GRAPHVIZ_DIRECTED
    #if GRAPHVIZ_DIRECTED == 0
        #define yyerror bgl_undir_error
        #include "graphviz_graph_parser.h"
    #else GRAPHIVZ == 1
        #define yyerror bgl_dir_error
        #include "graphviz_digraph_parser.h"
    #endif
#else
    #include "graphviz_parser.h"
#endif


#include <string>
#include <iostream>

static std::string literal;
static void begin_literal();
static void add_string(const char* str);
static void end_literal();

#include "yystype.h"
#define YY_DECL   int yylex(YYSTYPE* lvalp)
%}

%option noyywrap
%x Comment
%x Literal

DIGIT     [0-9]
ALPHABET  [a-zA-Z_]
NUMBER    [-+]?({DIGIT}+(\.{DIGIT}*)?|\.{DIGIT}+)([eE][-+]?{DIGIT}+)?
VARIABLE  {ALPHABET}+({DIGIT}|{ALPHABET})*
ID        {VARIABLE}|{NUMBER}

%%

"/*"            { BEGIN Comment; }
<Comment>"*/"   { BEGIN INITIAL; }
<Comment>.*     ;
"//".*$         ;
"#".*$          ;
"digraph"       { return DIGRAPH_T; }
"graph"         { return GRAPH_T; }
"node"          { return NODE_T; }
"edge"          { return EDGE_T; }
"strict"        ;
"subgraph"      { return SUBGRAPH_T; }
"->"|"--"       { return EDGEOP_T; }
{ID}            { lvalp->ptr = (void*)(new std::string(yytext)); return ID_T; }
["]             { BEGIN Literal; begin_literal();  }
<Literal>["]     { BEGIN INITIAL; end_literal(); lvalp->ptr = (void*)(new std::string(literal)); return ID_T; }
<Literal>[\\][\n]     ;
<Literal>([^"\\]*|[\\].)  { add_string(yytext); }
[ \t]           ;
\n              ;
.               { return yytext[0]; }
%%


void begin_literal() {
   literal = "";
}

void add_string(const char* str) {
   literal += str;
}

void end_literal() {
}

void yyerror(char *str)
{
  std::cout << str << std::endl;
}
