/*
 * (C) Copyright Christain Engstrom 2001.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */

#ifndef BOOST_ISO8859_1_REGEX_TRAITS_HPP
#define BOOST_ISO8859_1_REGEX_TRAITS_HPP
  
class iso8859_1_regex_traits : public boost::c_regex_traits<char> {
private:
  typedef boost::c_regex_traits<char> base;
public:
  iso8859_1_regex_traits() {}
  ~iso8859_1_regex_traits() {}

  bool is_class(char c, boost::uint_fast32_t f) const;
  char translate(char c, bool icase) const;
};

#endif


