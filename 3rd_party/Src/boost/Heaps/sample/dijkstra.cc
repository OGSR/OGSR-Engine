// -*-C++-*- heap/sample/dijstra.cc
// <!!----------------------------------------------------------------------> 
// <!! Copyright (C) 1999 Dietmar Kuehl, Claas Solutions GmbH > 
// <!!> 
// <!! Permission to use, copy, modify, distribute and sell this > 
// <!! software for any purpose is hereby granted without fee, provided > 
// <!! that the above copyright notice appears in all copies and that > 
// <!! both that copyright notice and this permission notice appear in > 
// <!! supporting documentation. Dietmar Kuehl and Claas Solutions make no > 
// <!! representations about the suitability of this software for any > 
// <!! purpose. It is provided "as is" without express or implied warranty. > 
// <!!----------------------------------------------------------------------> 

// Author: Dietmar Kuehl dietmar.kuehl@claas-solutions.de 
// Title:  A sample of the priority queues using Dijstra's algorithm
// Version: $Id: dijstra.cc,v 1.1 1999/07/13 01:42:12 kuehl Exp $ 

// -------------------------------------------------------------------------- 

#include "boost/heap.hpp"
#include <iostream>
#include <string>
#include <vector>

// -------------------------------------------------------------------------- 

int node_distance(std::string const& s1, std::string const& s2)
{
  int count = 0;
  for (std::string::size_type i = 0; i < s1.size(); ++i)
    if (s1[i] != s2[i])
      ++count;

  return count * count;
}

struct node_ptr
{
  int index;    // index of the string
  int distance; // current distance

  explicit node_ptr(int idx): index(idx), distance(INT_MAX) {}
  void operator= (int dist) { distance = dist; }

  bool operator< (node_ptr const& np) const { return distance > np.distance; }
};

template <template <typename T> class Heap, typename RandomAccessIt>
void dijkstra(RandomAccessIt begin, RandomAccessIt end, int start, int destination)
{
  Heap<node_ptr>                                heap;
  std::vector<typename Heap<node_ptr>::pointer> references;
  std::vector<int>                              distance;
  std::vector<int>                              predecessor;

  for (ptrdiff_t i = 0; i < end - begin; ++i)
    {
      references.push_back(heap.push(node_ptr(i)));
      distance.push_back(INT_MAX);
      predecessor.push_back(INT_MAX);
    }

  heap.change(references[start], 0);
  distance[start] = 0;
  predecessor[start] = 0;

  while (!heap.empty())
    {
      int dist;

      node_ptr np = heap.top();
      heap.pop();

      if (np.index == destination)
	break;

      for (ptrdiff_t i = 0; i < end - begin; ++i)
	{
	  dist = node_distance(begin[i], begin[np.index]) + np.distance;
	  if (dist < distance[i])
	    {
	      heap.change(references[i], dist);
	      distance[i] = dist;
	      predecessor[i] = np.index;
	    }
	}
    }

  for(int i = destination; i != start; i = predecessor[i])
    std::cout << begin[i] << " - ";
  std::cout << "\n";
}

// -------------------------------------------------------------------------- 

template <typename T, int sz> inline int size(T (&)[sz]) { return sz; }
template <typename T, int sz> inline T* begin(T (&array)[sz]) { return array; }
template <typename T, int sz> inline T* end(T (&array)[sz]) { return array + sz; }

int main()
{
  std::string nodes[] = { "heap", "help", "hold", "cold", "bold", "bolt", "boot" };

  dijkstra<boost::splay_heap>(begin(nodes), end(nodes), 0, size(nodes) - 1);
  dijkstra<boost::d_heap>(begin(nodes), end(nodes), 0, size(nodes) - 1);
  dijkstra<boost::fibonacci_heap>(begin(nodes), end(nodes), 0, size(nodes) - 1);
  dijkstra<boost::lazy_fibonacci_heap>(begin(nodes), end(nodes), 0, size(nodes) - 1);
  dijkstra<boost::pairing_heap>(begin(nodes), end(nodes), 0, size(nodes) - 1);
}
