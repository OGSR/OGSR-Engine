// Copyright (C) Vladimir Prus 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>

using namespace boost;

class copier {
public:
    template<class V1, class V2>
    void operator()(const V1&, const V2&) const {}
};

int main()
{
    adjacency_list<vecS, vecS, directedS, property<vertex_root_t, int> > g1, g2;
    adjacency_list<vecS, setS, directedS, property<vertex_index_t, int> > g3;

    copy_graph(g1, g2);
    copier c;
    copy_graph(g3, g1, vertex_copy(c));
}
