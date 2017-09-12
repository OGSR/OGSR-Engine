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

#if !defined(BOOST_NO_STD_WSTRING) && !defined(BOOST_NO_STD_WSTREAMBUF)
  using boost::wformat;
  if(str( wformat(L"%%##%%##%%1 %1%00") % L"Escaped OK" ) != L"%##%##%1 Escaped OK00")
      BOOST_ERROR("Basic w-parsing Failed");
  if(str( wformat(L"%%##%#x ##%%1 %s00") % 20 % L"Escaped OK" ) != L"%##0x14 ##%1 Escaped OK00")
      BOOST_ERROR("Basic wp-parsing Failed") ;
#endif // wformat tests


  return 0;
}
