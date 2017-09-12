// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include <boost/config.hpp>
#include <iostream>
#include <algorithm>
#include <boost/iterator_adaptors.hpp>

int main(int, char*[])
{
  char letters_[] = "hello world!";
  const int N = sizeof(letters_)/sizeof(char) - 1;
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
  // Assume there won't be proper iterator traits for pointers. This
  // is just a wrapper for char* which has the right traits.
  typedef boost::iterator_adaptor<char*, boost::default_iterator_policies, char> base_iterator;
#else
  typedef char* base_iterator;
#endif
  base_iterator letters(letters_);
  
  std::cout << "original sequence of letters:\t"
            << letters_ << std::endl;

  std::sort(letters, letters + N);

  // Use reverse_iterator_generator to print a sequence
  // of letters in reverse order.
  
  boost::reverse_iterator_generator<base_iterator>::type
    reverse_letters_first(letters + N),
    reverse_letters_last(letters);

  std::cout << "letters in descending order:\t";
  std::copy(reverse_letters_first, reverse_letters_last,
            std::ostream_iterator<char>(std::cout));
  std::cout << std::endl;

  // Use make_reverse_iterator() to print the sequence
  // of letters in reverse-reverse order.

  std::cout << "letters in ascending order:\t";
  std::copy(boost::make_reverse_iterator(reverse_letters_last),
            boost::make_reverse_iterator(reverse_letters_first),
            std::ostream_iterator<char>(std::cout));
  std::cout << std::endl;

  return 0;
}
