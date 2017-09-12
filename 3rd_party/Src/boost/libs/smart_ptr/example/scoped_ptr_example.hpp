//  Boost scoped_ptr_example header file  ------------------------------------//

//  (C) Copyright Beman Dawes 2001. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

//  The point of this example is to prove that even though
//  example::implementation is an incomplete type in translation units using
//  this header, scoped_ptr< implementation > is still valid because the type
//  is complete where it counts - in the inplementation translation unit where
//  destruction is actually instantiated.

class example : private boost::noncopyable
{
 public:
  example();
  ~example();
  void do_something();
 private:
  class implementation;
  boost::scoped_ptr< implementation > _imp; // hide implementation details
};

