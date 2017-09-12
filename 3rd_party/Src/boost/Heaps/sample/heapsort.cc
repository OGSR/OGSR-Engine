// -*-C++-*- heap/sample/heapsort.cc
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
// Title:  Heapsort as an example for the use of priority queues
// Version: $Id: heapsort.cc,v 1.1 1999/07/13 01:42:12 kuehl Exp $ 

// -------------------------------------------------------------------------- 

#include "boost/heap.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

// -------------------------------------------------------------------------- 

template <typename Heap, typename ForwardIterator>
void heap_sort(ForwardIterator begin, ForwardIterator end)
{
  Heap heap; // use default constructor for construction
  
  // insert all elements into the heap:
  for (ForwardIterator it = begin; it != end; ++it)
    heap.push(*it); 
  
  // finally, extract them again:
  for (ForwardIterator it = begin; !heap.empty(); ++it)
    {
      *it = heap.top();
      heap.pop();
    }
}
                        
// -------------------------------------------------------------------------- 

int main(int ac, char* av[])
{
  if (ac != 2)
    {
      std::cerr << "usage: " << av[0] << " <heap-no.>\n";
      std::cerr << "  0: boost::priority_queue<int, ..., std::less<int> >\n";
      std::cerr << "  1: boost::priority_queue<int, ..., std::greater<int> >\n";
      std::cerr << "  2: boost::d_heap<int, std::less<int> >\n";
      std::cerr << "  3: boost::d_heap<int, std::greater<int> >\n";
      std::cerr << "  4: boost::fibonacci_heap<int, std::less<int> >\n";
      std::cerr << "  5: boost::fibonacci_heap<int, std::greater<int> >\n";
      std::cerr << "  6: boost::pairing_heap<int, std::less<int> >\n";
      std::cerr << "  7: boost::pairing_heap<int, std::greater<int> >\n";
      std::cerr << "  8: boost::splay_heap<int, std::less<int> >\n";
      std::cerr << "  9: boost::splay_heap<int, std::greater<int> >\n";
      
      return EXIT_FAILURE;
    }
  
  int const no_elements = 20;
  std::vector<int> vec;
  std::generate_n(std::back_inserter(vec), no_elements, std::rand);
  
  std::vector<int>::iterator beg = vec.begin();
  std::vector<int>::iterator end = vec.end();
  
  switch (std::strtol(av[1], 0, 10))
    {
    case 0:
      heap_sort<boost::priority_queue<int, std::vector<int>, std::less<int> > >(beg, end);
      break;
    case 1:
      heap_sort<boost::priority_queue<int, std::vector<int>, std::greater<int> > >(beg, end);
      break;
    case 2:
      heap_sort<boost::d_heap<int, std::less<int> > >(beg, end);
      break;
    case 3:
      heap_sort<boost::d_heap<int, std::greater<int> > >(beg, end);
      break;
    case 4:
      heap_sort<boost::fibonacci_heap<int, std::less<int> > >(beg, end);
      break;
    case 5:
      heap_sort<boost::fibonacci_heap<int, std::greater<int> > >(beg, end);
      break;
    case 6:
      heap_sort<boost::pairing_heap<int, std::less<int> > >(beg, end);
      break;
    case 7:
      heap_sort<boost::pairing_heap<int, std::greater<int> > >(beg, end);
      break;
    case 8:
      heap_sort<boost::splay_heap<int, std::less<int> > >(beg, end);
      break;
    case 9:
      heap_sort<boost::splay_heap<int, std::greater<int> > >(beg, end);
      break;
    }
  
  std::copy(beg, end, std::ostream_iterator<int>(std::cout, "\n"));
}
                     
