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

#ifndef GRAPHVIZ_DIRECTED
#error Need to define the GRAPHVIZ_DIRECTED macro to either 0 or 1
#endif 

#if GRAPHVIZ_DIRECTED == 0
#define GRAPHVIZ_GRAPH boost::GraphvizGraph
#define yyrestart bgl_undir_restart
#else
#define GRAPHVIZ_GRAPH boost::GraphvizDigraph
#define yyrestart bgl_dir_restart
#endif

#define YYPARSE_PARAM g

#include "yystype.h"

  extern void yyerror(char* str);
  extern void yyrestart(FILE* str);
  extern int yylex(YYSTYPE* lvalp);

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
      return std::string(buf);
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

%}


%token<i>   GRAPH_T NODE_T EDGE_T DIGRAPH_T EDGEOP_T SUBGRAPH_T
%token<ptr> ID_T

%type<i>    graph graph_header graph_type stmt_list stmt attr_stmt attr_header
%type<i>    attr_list attr compound_stmt node_stmt edge_stmt opt_attr
%type<i>    graph_body edge_rhs_one edge_rhs graph_attr opt_graph_body
%type<ptr>  graph_name
%type<ptr>  edge_endpoint node_id  node_port subgraph subgraph_header

%pure_parser

%%
graph:        graph_header graph_body
              ;

graph_body:  '{' stmt_list '}' {$$=0;}
              ;

graph_header: graph_type graph_name
  {
    std::string* name = static_cast<std::string*>($2);
    graphviz::previous_graph = static_cast<graphviz::Subgraph*>(g);
    graphviz::current_graph = static_cast<graphviz::Subgraph*>(g);
    graphviz::set_graph_name(*name);
    delete name;
  }
              ;

graph_type:   GRAPH_T | DIGRAPH_T
              ;

graph_name:   ID_T {$$ = $1; } | {$$=(void*)(new std::string("G")); }
              ;

stmt_list:    stmt_list stmt | stmt
              ;

semicolon:    ';' | 
              ;

stmt:         attr_stmt semicolon | 
              compound_stmt  semicolon
              ;

attr_stmt:    attr_header '[' attr_list ']' 
  { 
    graphviz::set_attribute(*graphviz::current_graph,
			  graphviz::attribute_state); 
  }
              ;

attr_header:  GRAPH_T  { graphviz::attribute_state = GRAPH_GRAPH_A; } 
              | NODE_T { graphviz::attribute_state = GRAPH_NODE_A; }
              | EDGE_T { graphviz::attribute_state = GRAPH_EDGE_A; }
              ;

attr_list:    attr | attr_list attr_separator attr
              ;

attr:         ID_T '=' ID_T 
  { 
    std::string* name  = static_cast<std::string*>($1);
    std::string* value = static_cast<std::string*>($3);
    graphviz::attributes[*name] = *value; 
    delete name;
    delete value;
  }
              ;

attr_separator:    ';' | ',' |
              ;

compound_stmt: node_stmt | edge_stmt  | graph_attr | subgraph { $$ = 0; }
              ;

graph_attr    : attr 
  { 
    graphviz::set_attribute(
         *static_cast<graphviz::Subgraph*>(graphviz::current_graph),
			    GRAPH_GRAPH_A);
  }
              ;

node_stmt:    node_id opt_attr
  { 
    graphviz::Vertex* temp   = static_cast<graphviz::Vertex*>($1); 
    graphviz::current_vertex = *temp;
    graphviz::set_attribute(*static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM),
			    NODE_A); 
    delete temp;
    $$ = 0;
  }
              ;

opt_attr:     '[' attr_list ']' { $$=0; } | { $$=0; }
              ;

node_id:      ID_T 
  {
    std::string* name  = static_cast<std::string*>($1);
    std::pair<graphviz::Iter, bool> result = graphviz::lookup(*name); 
    if (result.second) {
      graphviz::current_vertex = result.first->second; 
      if (! graphviz::current_graph->is_root())
	boost::add_vertex(graphviz::current_vertex, *graphviz::current_graph);
    } else
      graphviz::current_vertex = graphviz::add_name(*name, *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM)) ; 
    graphviz::Vertex* temp = new graphviz::Vertex(graphviz::current_vertex);
    $$ = (void *)temp;
    graphviz::attribute_state = NODE_A;  
    delete name;
  }
              | node_port  { $$=$1; }
              ;

node_port:    ID_T ':' ID_T
  {
    //consider port as a special properties ?? --need work here
    std::string* name = static_cast<std::string*>($1);
    std::string* port = static_cast<std::string*>($3);

    std::pair<graphviz::Iter, bool> result = graphviz::lookup(*name); 
    if (result.second) 
      graphviz::current_vertex = result.first->second; 
    else
      graphviz::current_vertex = graphviz::add_name(*name, *static_cast<GRAPHVIZ_GRAPH*>(YYPARSE_PARAM)) ; 
    graphviz::Vertex* temp = new graphviz::Vertex(graphviz::current_vertex);
    $$ = (void *)temp;
    graphviz::attribute_state = NODE_A;  
    delete name;
    delete port;
  }
              ;

edge_stmt:    edge_endpoint edge_rhs opt_attr 
  {

    typedef std::pair<void*, bool>* Ptr;
    Ptr source = static_cast<Ptr>($1);

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
  }
              ;

edge_rhs_one: EDGEOP_T edge_endpoint   
  { graphviz::vlist.push_back(static_cast<std::pair<void*, bool>*>($2)); }
; 

edge_rhs:     edge_rhs_one | edge_rhs edge_rhs_one
              ;

edge_endpoint:   node_id 
  { 
    std::pair<void*, bool>* temp = new std::pair<void*, bool>;
    temp->first = $1;
    temp->second = false;
    $$ = (void*)temp;

    graphviz::attribute_state = EDGE_A; 
  }
                 | subgraph 
  { 
    std::pair<void*, bool>* temp = new std::pair<void*, bool>;
    temp->first = $1;
    temp->second = true;
    $$ = (void*)temp;

    graphviz::attribute_state = EDGE_A; 
  }
              ;

subgraph:    subgraph_header opt_graph_body 
  {
    if ( $2 )
      graphviz::current_graph = &graphviz::current_graph->parent();
    else
      graphviz::current_graph = graphviz::previous_graph;
  }
             |
  {
    graphviz::previous_graph = graphviz::current_graph;
    std::string name = graphviz::random_string();
    graphviz::Subgraph* temp = graphviz::create_subgraph(name);
    graphviz::current_graph = temp;
    graphviz::set_graph_name(name);

    $$ = (void *) graphviz::current_graph;
  } graph_body
  {
    graphviz::current_graph = &graphviz::current_graph->parent();
  }
              ;

subgraph_header: SUBGRAPH_T ID_T
  {
    //lookup ID_T if it is already in the subgraph,
    //if it is not, add a new subgraph
    std::string* name  = static_cast<std::string*>($2);

    std::pair<graphviz::It, bool> temp = graphviz::lookup_subgraph(*name);

    graphviz::previous_graph = graphviz::current_graph;
    if ( temp.second )  {//found 
      graphviz::current_graph = (temp.first)->second;
    } else {
      graphviz::current_graph = graphviz::create_subgraph(*name);
      graphviz::set_graph_name(*name);
    }

    $$ = (void *) graphviz::current_graph;
    delete name;
  }
              ;

opt_graph_body: graph_body {$$ = 1; } |  { $$ = 0; }
              ;
%%

namespace boost {
  
  void read_graphviz(const std::string& filename, GRAPHVIZ_GRAPH& g) {
    FILE* file = fopen(filename.c_str(), "r");
    yyrestart(file);
    void* in = static_cast<void*>(file);
    yyparse(static_cast<void*>(&g));
  }

  void read_graphviz(FILE* file, GRAPHVIZ_GRAPH& g) {
    void* in = static_cast<void*>(file);
    yyrestart(file);
    yyparse(static_cast<void*>(&g));
  }
    
}

