// Boost shared_ptr_example2 implementation file  -----------------------------//

//  (C) Copyright Beman Dawes 2001. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include "shared_ptr_example2.hpp"
#include <iostream>

class example::implementation
{
 public:
  ~implementation() { std::cout << "destroying implementation\n"; }
};

example::example() : _imp( new implementation ) {}

void example::do_something()
  { std::cout << "use_count() is " << _imp.use_count() << "\n"; }
