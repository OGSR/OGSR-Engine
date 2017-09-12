
# The files in this directory implement the read_graphviz() functions
# declared in <boost/graph/graphviz.hpp>. There are both directed
# and undirected graph versions of these functions. To use both, you
# will need to compile the code generated from graphviz_parser.y twice,
# once with -DGRAPHVIZ_GRAPH=boost::GraphvizDigraph and once with
#  -DGRAPHVIZ_GRAPH=boost::GraphvizGraph.
#
# This Makefile will most likely *not* work on your system.
# We have not yet had time to create a portable Makefile.
# This is the Makefile we currently use.
#

BOOST 		= ../../..

CXX		= g++ -ftemplate-depth-30
LEX		= flex
YACC		= bison
MV		= /bin/mv

EXTRAFLAGS 	= -g 

INCLUDES	= -I$(BOOST)

CXXFLAGS	= $(INCLUDES) $(EXTRAFLAGS)

LDFLAAGS	= 

LIBS		=

AR		= ar


default: libbgl-viz.a libbgl-viz.so

.SUFFIXES: .cpp .o .l .y .c

OBJS = graphviz_graph_lex.o graphviz_digraph_lex.o graphviz_digraph_parser.o graphviz_graph_parser.o

libbgl-viz.a: $(OBJS)
	$(AR) -rc libbgl-viz.a $(OBJS)

GRAPH_SONAME_VERSION = 1

libbgl-viz.so: $(OBJS)
	$(CXX) -shared -Wl,-soname -Wl,$@.$(GRAPH_SONAME_VERSION) -o $@.$(GRAPH_SONAME_VERSION) $(OBJS)
	ln -s $@.$(GRAPH_SONAME_VERSION) $@ 

# this next part is a bit strange. We compile graphviz_parser.cpp twice.
# Once with for undirected graphs with GRAPHVIZ_GRAPH=boost::GraphvizGraph 
# and once for directed graphs with GRAPHVIZ_GRAPH=boost::GraphvizDigraph.

graphviz_graph_parser.o: graphviz_graph_parser.cpp
	$(CXX) -DGRAPHVIZ_DIRECTED=0 $(CXXFLAGS) -c graphviz_graph_parser.cpp -o graphviz_graph_parser.o

graphviz_digraph_parser.o: graphviz_digraph_parser.cpp
	$(CXX) -DGRAPHVIZ_DIRECTED=1 $(CXXFLAGS) -c graphviz_digraph_parser.cpp -o graphviz_digraph_parser.o

graphviz_graph_lex.o: graphviz_graph_lex.cpp graphviz_parser.h
	$(CXX) $(CXXFLAGS) -c graphviz_graph_lex.cpp

graphviz_digraph_lex.o: graphviz_digraph_lex.cpp graphviz_parser.h
	$(CXX) $(CXXFLAGS) -c graphviz_digraph_lex.cpp

graphviz_graph_lex.cpp: graphviz_lex.l
	$(LEX) graphviz_lex.l
	rm -rf graphviz_graph_lex.cpp
	sed -f ./sed-undir lex.yy.c > graphviz_graph_lex.cpp

graphviz_digraph_lex.cpp: graphviz_lex.l
	$(LEX) graphviz_lex.l
	rm -rf graphviz_digraph_lex.cpp
	sed -f ./sed-dir lex.yy.c > graphviz_digraph_lex.cpp

graphviz_graph_parser.cpp: graphviz_parser.y
	$(YACC) -p bgl_undir_ -d -v graphviz_parser.y
	sed -f ./sed-undir graphviz_parser.tab.c > graphviz_graph_parser.cpp

graphviz_digraph_parser.cpp: graphviz_parser.y
	$(YACC) -p bgl_dir_ -d -v graphviz_parser.y
	sed -f ./sed-dir graphviz_parser.tab.c > graphviz_digraph_parser.cpp

graphviz_parser.h: graphviz_digraph_parser.cpp
	$(MV) graphviz_parser.tab.h graphviz_parser.h

dist:
	mkdir -p ./tmp/graphviz/boost/boost/graph
	cp boost/boost/graph/graphviz.hpp ./tmp/graphviz/boost/boost/graph
	cp graphviz.grammar ./tmp/graphviz
	cp graphviz_parser.cpp ./tmp/graphviz
	cp graphviz_lex.cpp  ./tmp/graphviz
	cp graphviz_parser.h ./tmp/graphviz
	cp graphviz_graph_type.h ./tmp/graphviz
	cp graphviz_lex.l  ./tmp/graphviz
	cp graphviz_parser.y ./tmp/graphviz
	cp graphviz.cpp  ./tmp/graphviz
	cp *.dot         ./tmp/graphviz
	cp Makefile      ./tmp/graphviz
	cd ./tmp; tar cvfz graphviz.tgz graphviz; $(MV) graphviz.tgz ..; cd ..
	/bin/rm -rf ./tmp


clean:
	/bin/rm -rf	*.o *.a *.so* *.output graphviz_digraph_parser.cpp graphviz_graph_parser.cpp graphviz_lex.cpp graphviz_parser.h
