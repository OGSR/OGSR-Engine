
/*  A Bison parser, made from graphviz_parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse bgl_undir_parse
#define bgl_undir_lex bgl_undir_lex
#define bgl_undir_error bgl_undir_error
#define bgl_undir_lval bgl_undir_lval
#define bgl_undir_char bgl_undir_char
#define bgl_undir_debug bgl_undir_debug
#define bgl_undir_nerrs bgl_undir_nerrs
#define GRAPH_T 257
#define NODE_T  258
#define EDGE_T  259
#define DIGRAPH_T       260
#define EDGEOP_T        261
#define SUBGRAPH_T      262
#define ID_T    263

#line 1 "graphviz_parser.y"

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

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <boost/config.hpp>
#include <boost/graph/graphviz.hpp>

#if defined BOOST_NO_STRINGSTREAM 
  //#include <strstream> //We cannot use it since there is a bug in strstream  
#include <stdlib.h>
#else
#include <sstream>
#endif

#ifndef GRAPHVIZ_GRAPH
#error Need to define the GRAPHVIZ_GRAPH macro to either GraphvizGraph or GraphvizDigraph.
#endif 

#define YYPARSE_PARAM g

#include "yystype.h"

  extern void bgl_undir_error(char* str);
  extern void bgl_undir_restart(FILE* str);
  extern int bgl_undir_lex(YYSTYPE* lvalp);

  enum AttrState {GRAPH_GRAPH_A, GRAPH_NODE_A, GRAPH_EDGE_A, NODE_A, EDGE_A};

  using boost::GraphvizAttrList;

  namespace graphviz {

    typedef boost::graph_traits<GRAPHVIZ_GRAPH>::vertex_descriptor Vertex;
    typedef boost::graph_traits<GRAPHVIZ_GRAPH>::edge_descriptor   Edge;
    typedef GRAPHVIZ_GRAPH Subgraph;

    static Vertex current_vertex;
    static Edge   current_edge;
    static Subgraph* current_graph = NULL;
    static Subgraph* previous_graph = NULL;

    static std::vector< std::pair<void*, bool>* > vlist;//store a list of rhs 

    static std::map<std::string,std::string> attributes;//store attributes temporarily
    static AttrState attribute_state;

    static std::map<std::string, Subgraph*> subgraphs;  //store the names of subgraphs
    static std::map<std::string, Vertex> nodes;         //store the names of nodes

    typedef std::map<std::string, Subgraph*>::iterator It; 
    typedef std::map<std::string, Vertex>::iterator Iter; 

    static const std::string& get_graph_name(const Subgraph& g) {
      const boost::graph_property<Subgraph, boost::graph_name_t>::type&
        name = boost::get_property(g, boost::graph_name);
      return name; 
    }

    static std::pair<Iter, bool> lookup(const std::string& name) {
      //lookup in the top level
      Iter it = nodes.find(name);
      bool found = (it != nodes.end() );
      return std::make_pair(it, found);
    }
    
    static Vertex add_name(const std::string& name, GRAPHVIZ_GRAPH& g) {
      Vertex v = boost::add_vertex(*current_graph);
      v = current_graph->local_to_global(v);

      //set the label of vertex, it could be overwritten later.
      boost::property_map<GRAPHVIZ_GRAPH, boost::vertex_attribute_t>::type
        va = boost::get(boost::vertex_attribute, g); 
      va[v]["label"] = name; 
      
      //add v into the map so next time we will find it.
      nodes[name] = v; 
      return v;
    }

    static std::pair<It, bool> lookup_subgraph(const std::string& name) {
      It it = subgraphs.find(name);
      bool found = (it != subgraphs.end() );
      return std::make_pair(it, found);
    }
    
    static Subgraph* create_subgraph(const std::string& name) { 

      Subgraph* new_subgraph = &(current_graph->create_subgraph()); 

      subgraphs[name]        = new_subgraph;
      return new_subgraph;
    }

    
    static void set_attribute(GraphvizAttrList& p,
                              const GraphvizAttrList& attr) {
      GraphvizAttrList::const_iterator i, end;
      for ( i=attr.begin(), end=attr.end(); i!=end; ++i)
        p[i->first]=i->second;
    }
  
    static void set_attribute(Subgraph& g,
                              AttrState s, bool clear_attribute = true) {
      typedef Subgraph Graph;
      switch ( s ) {
      case GRAPH_GRAPH_A: 
        {
          boost::graph_property<Graph, boost::graph_graph_attribute_t>::type&
            gga = boost::get_property(g, boost::graph_graph_attribute);
          set_attribute(gga, attributes); 
        }
        break;
      case GRAPH_NODE_A: 
        {
          boost::graph_property<Graph, boost::graph_vertex_attribute_t>::type&
            gna = boost::get_property(g, boost::graph_vertex_attribute);
          set_attribute(gna, attributes); 
        }
        break;
      case GRAPH_EDGE_A: 
        {
          boost::graph_property<Graph, boost::graph_edge_attribute_t>::type&
            gea = boost::get_property(g, boost::graph_edge_attribute);
          set_attribute(gea, attributes); 
        }
        break;
      case NODE_A:
        {
          boost::property_map<Graph, boost::vertex_attribute_t>::type
            va = boost::get(boost::vertex_attribute, g);    //va[v]
          set_attribute(va[current_vertex], attributes);
        }
        break;
      case EDGE_A: 
        {
          boost::property_map<Graph, boost::edge_attribute_t>::type
            ea = boost::get(boost::edge_attribute, g);      //ea[e]
          set_attribute(ea[current_edge], attributes); 
        }
        break;
      }
      if ( clear_attribute )
        attributes.clear();
    }


    static void add_edges(const Vertex& u,
                          const Vertex& v, GRAPHVIZ_GRAPH& g) {
      graphviz::current_edge = boost::add_edge(u, v, g).first; 
      graphviz::set_attribute(g, EDGE_A, false);
    }
    
    static void add_edges(Subgraph* G1, Subgraph* G2,
                          GRAPHVIZ_GRAPH& g) {
      boost::graph_traits<Subgraph>::vertex_iterator i, j, m, n;
      for ( boost::tie(i, j) = boost::vertices(*G1); i != j; ++i) {
        for ( boost::tie(m, n) = boost::vertices(*G2); m != n; ++m) {
          graphviz::add_edges(G1->local_to_global(*i),
                              G2->local_to_global(*m), g);
        }
      }
    }

    static void add_edges(Subgraph* G, const Vertex& v, GRAPHVIZ_GRAPH& g) {
      boost::graph_traits<Subgraph>::vertex_iterator i, j;
      for ( boost::tie(i, j) = boost::vertices(*G); i != j; ++i) {
        graphviz::add_edges(G->local_to_global(*i), v, g);
      }
    }

    static void add_edges(const Vertex& u, Subgraph* G, GRAPHVIZ_GRAPH& g) {
      boost::graph_traits<Subgraph>::vertex_iterator i, j;
      for ( boost::tie(i, j) = boost::vertices(*G); i != j; ++i) {
        graphviz::add_edges(u, G->local_to_global(*i), g);
      }
    }

    static std::string random_string() {
      static int i=0;
#if defined BOOST_NO_STRINGSTREAM
      //std::strstream out;
      char buf[256];
      sprintf(buf, "default%i\0", i);
      ++i;
      return  string(buf);
#else
      std::stringstream out;
      out << "default" << i;
      ++i;
      return out.str();
#endif
    }


    static void set_graph_name(const std::string& name) {
      boost::graph_property<Subgraph, boost::graph_name_t>::type&
        gea = boost::get_property(*current_graph, boost::graph_name);
      gea = name;
    }

  } //namespace detail {

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define YYFINAL         66
#define YYFLAG          -32768
#define YYNTBASE        18

#define YYTRANSLATE(x) ((unsigned)(x) <= 263 ? yytranslate[x] : 45)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    16,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    17,    12,     2,
    15,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    13,     2,    14,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    10,     2,    11,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     7,    10,    12,    14,    16,    17,    20,    22,
    24,    25,    28,    31,    36,    38,    40,    42,    44,    48,
    52,    54,    56,    57,    59,    61,    63,    65,    67,    70,
    74,    75,    77,    79,    83,    87,    90,    92,    95,    97,
    99,   102,   103,   106,   109,   111
};

static const short yyrhs[] = {    20,
    19,     0,    10,    23,    11,     0,    21,    22,     0,     3,
     0,     6,     0,     9,     0,     0,    23,    25,     0,    25,
     0,    12,     0,     0,    26,    24,     0,    31,    24,     0,
    27,    13,    28,    14,     0,     3,     0,     4,     0,     5,
     0,    29,     0,    28,    30,    29,     0,     9,    15,     9,
     0,    12,     0,    16,     0,     0,    33,     0,    37,     0,
    32,     0,    41,     0,    29,     0,    35,    34,     0,    13,
    28,    14,     0,     0,     9,     0,    36,     0,     9,    17,
     9,     0,    40,    39,    34,     0,     7,    40,     0,    38,
     0,    39,    38,     0,    35,     0,    41,     0,    43,    44,
     0,     0,    42,    19,     0,     8,     9,     0,    19,     0,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   246,   249,   252,   262,   262,   265,   265,   268,   268,   271,
   271,   274,   274,   278,   285,   286,   287,   290,   290,   293,
   303,   303,   303,   306,   306,   306,   306,   309,   317,   328,
   328,   331,   346,   349,   368,   413,   417,   417,   420,   429,
   440,   447,   457,   462,   483,   483
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","GRAPH_T",
"NODE_T","EDGE_T","DIGRAPH_T","EDGEOP_T","SUBGRAPH_T","ID_T","'{'","'}'","';'",
"'['","']'","'='","','","':'","graph","graph_body","graph_header","graph_type",
"graph_name","stmt_list","semicolon","stmt","attr_stmt","attr_header","attr_list",
"attr","attr_separator","compound_stmt","graph_attr","node_stmt","opt_attr",
"node_id","node_port","edge_stmt","edge_rhs_one","edge_rhs","edge_endpoint",
"subgraph","@1","subgraph_header","opt_graph_body", NULL
};
#endif

static const short yyr1[] = {     0,
    18,    19,    20,    21,    21,    22,    22,    23,    23,    24,
    24,    25,    25,    26,    27,    27,    27,    28,    28,    29,
    30,    30,    30,    31,    31,    31,    31,    32,    33,    34,
    34,    35,    35,    36,    37,    38,    39,    39,    40,    40,
    41,    42,    41,    43,    44,    44
};

static const short yyr2[] = {     0,
     2,     3,     2,     1,     1,     1,     0,     2,     1,     1,
     0,     2,     2,     4,     1,     1,     1,     1,     3,     3,
     1,     1,     0,     1,     1,     1,     1,     1,     2,     3,
     0,     1,     1,     3,     3,     2,     1,     2,     1,     1,
     2,     0,     2,     2,     1,     0
};

static const short yydefact[] = {     0,
     4,     5,     0,     7,    42,     1,     6,     3,    15,    16,
    17,     0,    32,    42,     9,    11,     0,    28,    11,    26,
    24,    31,    33,    25,     0,    27,     0,    46,    44,     0,
     0,     2,     8,    10,    12,     0,    13,     0,    29,    42,
    37,    31,    43,    45,    41,    20,    34,     0,    23,    18,
    23,    32,    39,    36,    40,    35,    38,    21,    14,    22,
     0,    30,    19,     0,     0,     0
};

static const short yydefgoto[] = {    64,
     6,     3,     4,     8,    14,    35,    15,    16,    17,    49,
    18,    61,    19,    20,    21,    39,    22,    23,    24,    41,
    42,    25,    26,    27,    28,    45
};

static const short yypact[] = {    23,
-32768,-32768,    -4,     6,     9,-32768,-32768,-32768,-32768,-32768,
-32768,    18,    13,     0,-32768,    19,    21,-32768,    19,-32768,
-32768,    -6,-32768,-32768,    28,    29,    -4,    -4,-32768,    30,
    31,-32768,-32768,-32768,-32768,    32,-32768,    32,-32768,    24,
-32768,     3,-32768,-32768,-32768,-32768,-32768,    27,     7,-32768,
     8,    26,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    32,-32768,-32768,    44,    45,-32768
};

static const short yypgoto[] = {-32768,
    10,-32768,-32768,-32768,-32768,    33,    34,-32768,-32768,    11,
   -36,-32768,-32768,-32768,-32768,     4,    14,-32768,-32768,     5,
-32768,    15,    16,-32768,-32768,-32768
};


#define YYLAST          56


static const short yytable[] = {    50,
   -39,    50,     9,    10,    11,     5,    38,    12,    13,    40,
    32,     9,    10,    11,     7,    38,    12,    13,    58,    58,
    59,    62,    60,    60,    63,     1,    29,    30,     2,    31,
    34,    12,    52,    36,    40,   -40,    43,    44,    46,    47,
    48,    30,    31,    65,    66,    56,    57,    33,    51,     0,
     0,    37,     0,    53,    54,    55
};

static const short yycheck[] = {    36,
     7,    38,     3,     4,     5,    10,    13,     8,     9,     7,
    11,     3,     4,     5,     9,    13,     8,     9,    12,    12,
    14,    14,    16,    16,    61,     3,     9,    15,     6,    17,
    12,     8,     9,    13,     7,     7,    27,    28,     9,     9,
     9,    15,    17,     0,     0,    42,    42,    14,    38,    -1,
    -1,    19,    -1,    40,    40,    40
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
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

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (bgl_undir_char = YYEMPTY)
#define YYEMPTY         -2
#define YYEOF           0
#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrlab1
/* Like YYERROR except do call bgl_undir_error.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL          goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do                                                              \
  if (bgl_undir_char == YYEMPTY && yylen == 1)                          \
    { bgl_undir_char = (token), bgl_undir_lval = (value);                       \
      bgl_undir_char1 = YYTRANSLATE (bgl_undir_char);                           \
      YYPOPSTACK;                                               \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    { bgl_undir_error ("syntax error: cannot back up"); YYERROR; }      \
while (0)

#define YYTERROR        1
#define YYERRCODE       256

#ifndef YYPURE
#define YYLEX           bgl_undir_lex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX           bgl_undir_lex(&bgl_undir_lval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX           bgl_undir_lex(&bgl_undir_lval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX           bgl_undir_lex(&bgl_undir_lval, YYLEX_PARAM)
#else
#define YYLEX           bgl_undir_lex(&bgl_undir_lval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int     bgl_undir_char;                 /*  the lookahead symbol                */
YYSTYPE bgl_undir_lval;                 /*  the semantic value of the           */
                                /*  lookahead symbol                    */

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;                 /*  location data for the lookahead     */
                                /*  symbol                              */
#endif

int bgl_undir_nerrs;                    /*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int bgl_undir_debug;                    /*  nonzero means print parse trace     */
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks       */

#ifndef YYINITDEPTH
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

#if __GNUC__ > 1                /* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)      __builtin_memcpy(TO,FROM,COUNT)
#else                           /* not GNU C or C++ */
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

#line 217 "/usr/lib/bison.simple"

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
  int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  int bgl_undir_char1 = 0;              /*  lookahead token as an internal (translated) token number */

  short yyssa[YYINITDEPTH];     /*  the state stack                     */
  YYSTYPE yyvsa[YYINITDEPTH];   /*  the semantic value stack            */

  short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;        /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];   /*  the location stack                  */
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int bgl_undir_char;
  YYSTYPE bgl_undir_lval;
  int bgl_undir_nerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;                /*  the variable used to return         */
                                /*  semantic values from the action     */
                                /*  routines                            */

  int yylen;

#if YYDEBUG != 0
  if (bgl_undir_debug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  bgl_undir_nerrs = 0;
  bgl_undir_char = YYEMPTY;             /* Cause a token to be read.  */

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
          bgl_undir_error("parser stack overflow");
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
      if (bgl_undir_debug)
        fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
        YYABORT;
    }

#if YYDEBUG != 0
  if (bgl_undir_debug)
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

  /* bgl_undir_char is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (bgl_undir_char == YYEMPTY)
    {
#if YYDEBUG != 0
      if (bgl_undir_debug)
        fprintf(stderr, "Reading a token: ");
#endif
      bgl_undir_char = YYLEX;
    }

  /* Convert token to internal form (in bgl_undir_char1) for indexing tables with */

  if (bgl_undir_char <= 0)              /* This means end of input. */
    {
      bgl_undir_char1 = 0;
      bgl_undir_char = YYEOF;           /* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (bgl_undir_debug)
        fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      bgl_undir_char1 = YYTRANSLATE(bgl_undir_char);

#if YYDEBUG != 0
      if (bgl_undir_debug)
        {
          fprintf (stderr, "Next token is %d (%s", bgl_undir_char, yytname[bgl_undir_char1]);
          /* Give the individual parser a way to print the precise meaning
             of a token, for further debugging info.  */
#ifdef YYPRINT
          YYPRINT (stderr, bgl_undir_char, bgl_undir_lval);
#endif
          fprintf (stderr, ")\n");
        }
#endif
    }

  yyn += bgl_undir_char1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != bgl_undir_char1)
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
  if (bgl_undir_debug)
    fprintf(stderr, "Shifting token %d (%s), ", bgl_undir_char, yytname[bgl_undir_char1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (bgl_undir_char != YYEOF)
    bgl_undir_char = YYEMPTY;

  *++yyvsp = bgl_undir_lval;
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
  if (bgl_undir_debug)
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

case 2:
#line 249 "graphviz_parser.y"
{yyval.i=0;;
    break;}
case 3:
#line 253 "graphviz_parser.y"
{
    std::string* name = static_cast<std::string*>(yyvsp[0].ptr);
    graphviz::previous_graph = static_cast<graphviz::Subgraph*>(g);
    graphviz::current_graph = static_cast<graphviz::Subgraph*>(g);
    graphviz::set_graph_name(*name);
    delete name;
  ;
    break;}
case 6:
#line 265 "graphviz_parser.y"
{yyval.ptr = yyvsp[0].ptr; ;
    break;}
case 7:
#line 265 "graphviz_parser.y"
{yyval.ptr=(void*)(new std::string("G")); ;
    break;}
case 14:
#line 279 "graphviz_parser.y"
{ 
    graphviz::set_attribute(*graphviz::current_graph,
                          graphviz::attribute_state); 
  ;
    break;}
case 15:
#line 285 "graphviz_parser.y"
{ graphviz::attribute_state = GRAPH_GRAPH_A; ;
    break;}
case 16:
#line 286 "graphviz_parser.y"
{ graphviz::attribute_state = GRAPH_NODE_A; ;
    break;}
case 17:
#line 287 "graphviz_parser.y"
{ graphviz::attribute_state = GRAPH_EDGE_A; ;
    break;}
case 20:
#line 294 "graphviz_parser.y"
{ 
    std::string* name  = static_cast<std::string*>(yyvsp[-2].ptr);
    std::string* value = static_cast<std::string*>(yyvsp[0].ptr);
    graphviz::attributes[*name] = *value; 
    delete name;
    delete value;
  ;
    break;}
case 27:
#line 306 "graphviz_parser.y"
{ yyval.i = 0; ;
    break;}
case 28:
#line 310 "graphviz_parser.y"
{ 
    graphviz::set_attribute(
         *static_cast<graphviz::Subgraph*>(graphviz::current_graph),
                            GRAPH_GRAPH_A);
  ;
    break;}
case 29:
#line 318 "graphviz_parser.y"
{ 
    graphviz::Vertex* temp   = static_cast<graphviz::Vertex*>(yyvsp[-1].ptr); 
    graphviz::current_vertex = *temp;
    graphviz::set_attribute(*static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM),
                            NODE_A); 
    delete temp;
    yyval.i = 0;
  ;
    break;}
case 30:
#line 328 "graphviz_parser.y"
{ yyval.i=0; ;
    break;}
case 31:
#line 328 "graphviz_parser.y"
{ yyval.i=0; ;
    break;}
case 32:
#line 332 "graphviz_parser.y"
{
    std::string* name  = static_cast<std::string*>(yyvsp[0].ptr);
    std::pair<graphviz::Iter, bool> result = graphviz::lookup(*name); 
    if (result.second) {
      graphviz::current_vertex = result.first->second; 
      if (! graphviz::current_graph->is_root())
        boost::add_vertex(graphviz::current_vertex, *graphviz::current_graph);
    } else
      graphviz::current_vertex = graphviz::add_name(*name, *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM)) ; 
    graphviz::Vertex* temp = new graphviz::Vertex(graphviz::current_vertex);
    yyval.ptr = (void *)temp;
    graphviz::attribute_state = NODE_A;  
    delete name;
  ;
    break;}
case 33:
#line 346 "graphviz_parser.y"
{ yyval.ptr=yyvsp[0].ptr; ;
    break;}
case 34:
#line 350 "graphviz_parser.y"
{
    //consider port as a special properties ?? --need work here
    std::string* name = static_cast<std::string*>(yyvsp[-2].ptr);
    std::string* port = static_cast<std::string*>(yyvsp[0].ptr);

    std::pair<graphviz::Iter, bool> result = graphviz::lookup(*name); 
    if (result.second) 
      graphviz::current_vertex = result.first->second; 
    else
      graphviz::current_vertex = graphviz::add_name(*name, *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM)) ; 
    graphviz::Vertex* temp = new graphviz::Vertex(graphviz::current_vertex);
    yyval.ptr = (void *)temp;
    graphviz::attribute_state = NODE_A;  
    delete name;
    delete port;
  ;
    break;}
case 35:
#line 369 "graphviz_parser.y"
{

    typedef std::pair<void*, bool>* Ptr;
    Ptr source = static_cast<Ptr>(yyvsp[-2].ptr);

    for (std::vector<Ptr>::iterator it=graphviz::vlist.begin();
         it !=graphviz::vlist.end(); ++it) { 
      if ( source->second ) {
        if ( (*it)->second )
          graphviz::add_edges(static_cast<graphviz::Subgraph*>(source->first),
                            static_cast<graphviz::Subgraph*>((*it)->first),
                            *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM));
        else
          graphviz::add_edges(static_cast<graphviz::Subgraph*>(source->first),
                            *static_cast<graphviz::Vertex*>((*it)->first),
                            *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM));
      } else {
        graphviz::Vertex* temp = static_cast<graphviz::Vertex*>(source->first);
        if ( (*it)->second )
          graphviz::add_edges(*temp,
                            static_cast<graphviz::Subgraph*>((*it)->first),
                            *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM));
        else
          graphviz::add_edges(*temp,
                            *static_cast<graphviz::Vertex*>((*it)->first),
                            *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM));
        delete temp;
      }

      delete source; 
      source = *it; 
    } 
    
    if ( ! source->second ) {
      graphviz::Vertex* temp = static_cast<graphviz::Vertex*>(source->first);
      delete temp;
    }
    delete source;

    graphviz::attributes.clear();
    graphviz::vlist.clear(); 
  ;
    break;}
case 36:
#line 414 "graphviz_parser.y"
{ graphviz::vlist.push_back(static_cast<std::pair<void*, bool>*>(yyvsp[0].ptr)); ;
    break;}
case 39:
#line 421 "graphviz_parser.y"
{ 
    std::pair<void*, bool>* temp = new std::pair<void*, bool>;
    temp->first = yyvsp[0].ptr;
    temp->second = false;
    yyval.ptr = (void*)temp;

    graphviz::attribute_state = EDGE_A; 
  ;
    break;}
case 40:
#line 430 "graphviz_parser.y"
{ 
    std::pair<void*, bool>* temp = new std::pair<void*, bool>;
    temp->first = yyvsp[0].ptr;
    temp->second = true;
    yyval.ptr = (void*)temp;

    graphviz::attribute_state = EDGE_A; 
  ;
    break;}
case 41:
#line 441 "graphviz_parser.y"
{
    if ( yyvsp[0].i )
      graphviz::current_graph = &graphviz::current_graph->parent();
    else
      graphviz::current_graph = graphviz::previous_graph;
  ;
    break;}
case 42:
#line 448 "graphviz_parser.y"
{
    graphviz::previous_graph = graphviz::current_graph;
    std::string name = graphviz::random_string();
    graphviz::Subgraph* temp = graphviz::create_subgraph(name);
    graphviz::current_graph = temp;
    graphviz::set_graph_name(name);

    yyval.ptr = (void *) graphviz::current_graph;
  ;
    break;}
case 43:
#line 457 "graphviz_parser.y"
{
    graphviz::current_graph = &graphviz::current_graph->parent();
  ;
    break;}
case 44:
#line 463 "graphviz_parser.y"
{
    //lookup ID_T if it is already in the subgraph,
    //if it is not, add a new subgraph
    std::string* name  = static_cast<std::string*>(yyvsp[0].ptr);

    std::pair<graphviz::It, bool> temp = graphviz::lookup_subgraph(*name);

    graphviz::previous_graph = graphviz::current_graph;
    if ( temp.second )  {//found 
      graphviz::current_graph = (temp.first)->second;
    } else {
      graphviz::current_graph = graphviz::create_subgraph(*name);
      graphviz::set_graph_name(*name);
    }

    yyval.ptr = (void *) graphviz::current_graph;
    delete name;
  ;
    break;}
case 45:
#line 483 "graphviz_parser.y"
{yyval.i = 1; ;
    break;}
case 46:
#line 483 "graphviz_parser.y"
{ yyval.i = 0; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (bgl_undir_debug)
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
      ++bgl_undir_nerrs;

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
              bgl_undir_error(msg);
              free(msg);
            }
          else
            bgl_undir_error ("parse error; also virtual memory exceeded");
        }
      else
#endif /* YYERROR_VERBOSE */
        bgl_undir_error("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (bgl_undir_char == YYEOF)
        YYABORT;

#if YYDEBUG != 0
      if (bgl_undir_debug)
        fprintf(stderr, "Discarding token %d (%s).\n", bgl_undir_char, yytname[bgl_undir_char1]);
#endif

      bgl_undir_char = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;              /* Each real token shifted decrements this */

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
  if (bgl_undir_debug)
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
  if (bgl_undir_debug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = bgl_undir_lval;
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
#line 485 "graphviz_parser.y"


namespace boost {
  
  void read_graphviz(const std::string& filename, GRAPHVIZ_GRAPH& g) {
    FILE* file = fopen(filename.c_str(), "r");
    bgl_undir_restart(file);
    void* in = static_cast<void*>(file);
    yyparse(static_cast<void*>(&g));
  }

  void read_graphviz(FILE* file, GRAPHVIZ_GRAPH& g) {
    void* in = static_cast<void*>(file);
    bgl_undir_restart(file);
    yyparse(static_cast<void*>(&g));
  }
    
}

