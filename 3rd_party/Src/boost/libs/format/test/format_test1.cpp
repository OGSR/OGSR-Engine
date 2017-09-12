// -*- C++ -*-
//  Boost general library 'format'   ---------------------------
//  See http://www.boost.org for updates, documentation, and revision history.

//  (C) Samuel Krempp 2001
//                  krempp@crans.ens-cachan.fr
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "boost/format.hpp"

#define BOOST_INCLUDE_MAIN 
#include <boost/test/test_tools.hpp>


int test_main(int, char* [])
{

  using boost::format;
  using boost::io::str;

  if(str( format("  %%  ") ) != "  %  ")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("nothing") ) != "nothing")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("%%  ") ) != "%  ")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("  %%") ) != "  %")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("  %n  ") ) != "    ")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("%n  ") ) != "  ")
      BOOST_ERROR("Basic parsing without arguments Failed");
  if(str( format("  %n") ) != "  ")
      BOOST_ERROR("Basic parsing without arguments Failed");

  if(str( format("%%##%%##%%1 %1%00") % "Escaped OK" ) != "%##%##%1 Escaped OK00")
      BOOST_ERROR("Basic parsing Failed");
  if(str( format("%%##%#x ##%%1 %s00") % 20 % "Escaped OK" ) != "%##0x14 ##%1 Escaped OK00")
      BOOST_ERROR("Basic p-parsing Failed") ;

  return 0;
}
