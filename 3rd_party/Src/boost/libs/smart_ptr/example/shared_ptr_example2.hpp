//  Boost shared_ptr_example2 header file  -----------------------------------//

//  (C) Copyright Beman Dawes 2001. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/shared_ptr.hpp>

//  This example demonstrates the handle/body idiom (also called pimpl and
//  several other names).  It separates the interface (in this header file)
//  from the implementation (in shared_ptr_example2.cpp).

//  Note that even though example::implementation is an incomplete type in
//  some translation units using this header, shared_ptr< implementation >
//  is still valid because the type is complete where it counts - in the
//  shared_ptr_example2.cpp translation unit where functions requiring a
//  complete type are actually instantiated.

class example
{
public:
  example();
  void do_something();
private:
  class implementation;
  boost::shared_ptr< implementation > _imp; // hide implementation details
};

