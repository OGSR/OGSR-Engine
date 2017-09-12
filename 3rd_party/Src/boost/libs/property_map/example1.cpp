//  (C) Copyright Jeremy Siek 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <iostream>
#include <map>
#include <string>
#include <boost/property_map.hpp>


template <typename AddressMap>
void foo(AddressMap address)
{
  typedef typename boost::property_traits<AddressMap>::value_type value_type;
  typedef typename boost::property_traits<AddressMap>::key_type key_type;

  value_type old_address, new_address;
  key_type fred = "Fred";
  old_address = get(address, fred);
  new_address = "384 Fitzpatrick Street";
  put(address, fred, new_address);

  key_type joe = "Joe";
  value_type& joes_address = address[joe];
  joes_address = "325 Cushing Avenue";
}

int
main()
{
  std::map<std::string, std::string> name2address;
  boost::associative_property_map< std::map<std::string, std::string> >
    address_map(name2address);

  name2address.insert(make_pair(std::string("Fred"), 
                                std::string("710 West 13th Street")));
  name2address.insert(make_pair(std::string("Joe"), 
                                std::string("710 West 13th Street")));

  foo(address_map);
  
  for (std::map<std::string, std::string>::iterator i = name2address.begin();
       i != name2address.end(); ++i)
    std::cout << i->first << ": " << i->second << "\n";

  return EXIT_SUCCESS;
}
