//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_NL_TYPES_H
//  TITLE:         <nl_types.h>
//  DESCRIPTION:   The platform has an <nl_types.h>.

#include <nl_types.h>


namespace boost_has_nl_types_h{

int test()
{
   nl_catd cat = catopen("foo", 0);
   if(cat >= 0) catclose(cat);
   return 0;
}

}




