//  (C) Copyright John Maddock 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_DIRENT_H
//  TITLE:         <dirent.h>
//  DESCRIPTION:   The platform has an <dirent.h>.

#include <dirent.h>


namespace boost_has_dirent_h{

int test()
{
   DIR* pd = opendir("foobar");
   if(pd) closedir(pd);
   return 0;
}

}





