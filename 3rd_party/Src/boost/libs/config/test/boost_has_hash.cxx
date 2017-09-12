//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_HASH
//  TITLE:         <hashset> and <hashmap>
//  DESCRIPTION:   The C++ implementation provides the (SGI) hash_set
//                 or hash_map classes.

#include <hash_set>
#include <hash_map>

#ifndef BOOST_STD_EXTENSION_NAMESPACE
#define BOOST_STD_EXTENSION_NAMESPACE std
#endif

namespace boost_has_hash{

template <class Key, class Eq, class Hash, class Alloc>
void foo(const BOOST_STD_EXTENSION_NAMESPACE::hash_set<Key,Eq,Hash,Alloc>& )
{
}

template <class Key, class T, class Eq, class Hash, class Alloc>
void foo(const BOOST_STD_EXTENSION_NAMESPACE::hash_map<Key,T,Eq,Hash,Alloc>& )
{
}

int test()
{
   BOOST_STD_EXTENSION_NAMESPACE::hash_set<int> hs;
   foo(hs);
   BOOST_STD_EXTENSION_NAMESPACE::hash_map<int, long> hm;
   foo(hm);
   return 0;
}

}





