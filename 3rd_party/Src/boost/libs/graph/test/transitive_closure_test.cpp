// Copyright (C) 2001 Vladimir Prus <ghost@cs.msu.su>
// Copyright (C) 2001 Jeremy Siek <jsiek@cs.indiana.edu>
// Permission to copy, use, modify, sell and distribute this software is
// granted, provided this copyright notice appears in all copies and 
// modified version are clearly marked as such. This software is provided
// "as is" without express or implied warranty, and with no claim as to its
// suitability for any purpose.


#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/transitive_closure.hpp>

#include <iostream>

#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <cstdlib>
#include <ctime>
#include <boost/progress.hpp>
using namespace std;
using namespace boost;

void generate_graph(int n, double p, vector< vector<int> >& r1)
{
  static class {
  public:
    double operator()() {
      return double(rand())/RAND_MAX;
    }
  } gen;  
  r1.clear();
  r1.resize(n);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) 
      if (gen() < p)
        r1[i].push_back(j);
}

// (i,j) is in E' if j is reachable from i
// Hmm, is_reachable does not detect when there is a non-trivial path
// from i to i. It always returns true for is_reachable(i,i).
// This needs to be fixed/worked around.
template <typename Graph, typename GraphTC>
bool check_transitive_closure(Graph& g, GraphTC& tc)
{
  std::vector<default_color_type> color_map_vec(num_vertices(g));
  typename graph_traits<GraphTC>::vertex_iterator i, i_end;
  for (tie(i, i_end) = vertices(tc); i != i_end; ++i) {
    typename graph_traits<GraphTC>::out_edge_iterator j, j_end;
    for (tie(j, j_end) = out_edges(*i, tc); j != j_end; ++j)
      if (!is_reachable(source(*j, g), target(*j, g), g, &color_map_vec[0]))
        return false;
  }
  return true;
}

bool test(int n, double p)
{
  vector< vector<int> > g1, g1_tc;
  generate_graph(n, p, g1);
  cout << "Created graph with " << n << " vertices.\n";

  vector< vector<int> > g1_c(g1);

  {
    progress_timer t;
    cout << "transitive_closure" << endl;
    transitive_closure(g1, g1_tc, vertex_index_map(identity_property_map()));
  }

  if(check_transitive_closure(g1, g1_tc))
    return true;
  else {
    //cout << "Original graph was " << multiline << g1_c << endl;
    //cout << "Result is " << multiline << g1 << endl;
    return false;
  }
}


int main()
{
  srand(time(0));
  static class {
  public:
    double operator()() {
      return double(rand())/RAND_MAX;
    }
  } gen;  


  for (size_t i = 0; i < 100; ++i) {
    int n = 0 + int(20*gen());
    double p = gen();
    if (!test(n, p)) {
      cout << "Failed." << endl;
      return 1; 
    }
  }
  cout << "Passed." << endl;
}

