// (C) Copyright Jeremy Siek 2001. 
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

#include <iostream>
#include <fstream>
#include <cmath> // for pow
#include <boost/dynamic_bitset.hpp>

#include <boost/test/test_tools.hpp>

#include "bitset_test.hpp"

template <typename Block>
void run_test_cases()
{
  typedef bitset_test< boost::dynamic_bitset<Block> > Tests;

  std::string long_string(101, '0');
  for (std::size_t i = 0; i < long_string.size(); ++i)
    long_string[i] = '0' + (i % 2);

  std::size_t ul_size = CHAR_BIT * sizeof(unsigned long);

  //=====================================================================
  // Test b.to_long()
  {
    boost::dynamic_bitset<Block> b;
    Tests::to_ulong(b);
  }
  {
    std::string ul_str(ul_size, '1');
    boost::dynamic_bitset<Block> b(ul_str);
    Tests::to_ulong(b);
  }
  { // case overflow
    boost::dynamic_bitset<Block> b(long_string);
    Tests::to_ulong(b);
  }
  //=====================================================================
  // Test to_string(b, str)
  {
    boost::dynamic_bitset<Block> b;
    Tests::to_string(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::to_string(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::to_string(b);
  }
  //=====================================================================
  // Test b.count()
  {
    boost::dynamic_bitset<Block> b;
    Tests::count(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::count(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::count(b);
  }
  //=====================================================================
  // Test b.size()
  {
    boost::dynamic_bitset<Block> b;
    Tests::size(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::size(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::size(b);
  }
  //=====================================================================
  // Test b.any()
  {
    boost::dynamic_bitset<Block> b;
    Tests::any(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::any(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::any(b);
  }
  //=====================================================================
  // Test b.none()
  {
    boost::dynamic_bitset<Block> b;
    Tests::none(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::none(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::none(b);
  }
  //=====================================================================
  // Test a.is_subset_of(b)
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::subset(a, b);
  }
  //=====================================================================
  // Test a.is_proper_subset_of(b)
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::proper_subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::proper_subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::proper_subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::proper_subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::proper_subset(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::proper_subset(a, b);
  }
  //=====================================================================
  // Test operator==
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_equal(a, b);
  }
  //=====================================================================
  // Test operator!=
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_not_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_not_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_not_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_not_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_not_equal(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_not_equal(a, b);
  }
  //=====================================================================
  // Test operator<
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_less_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_less_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_less_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_less_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_less_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_less_than(a, b);
  }
  // check for consistency with ulong behaviour
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 5ul);
    assert(a < b);
  }
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 4ul);
    assert(!(a < b));
  }
  {
    boost::dynamic_bitset<Block> a(3, 5ul), b(3, 4ul);
    assert(!(a < b));
  }
  //=====================================================================
  // Test operator<=
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_less_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_less_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_less_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_less_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_less_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_less_than_eq(a, b);
  }
  // check for consistency with ulong behaviour
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 5ul);
    assert(a <= b);
  }
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 4ul);
    assert(a <= b);
  }
  {
    boost::dynamic_bitset<Block> a(3, 5ul), b(3, 4ul);
    assert(!(a <= b));
  }
  //=====================================================================
  // Test operator>
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_greater_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_greater_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_greater_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_greater_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_greater_than(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_greater_than(a, b);
  }
  // check for consistency with ulong behaviour
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 5ul);
    assert(!(a > b));
  }
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 4ul);
    assert(!(a > b));
  }
  {
    boost::dynamic_bitset<Block> a(3, 5ul), b(3, 4ul);
    assert(a > b);
  }
  //=====================================================================
  // Test operator<=
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::operator_greater_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0")), b(std::string("0"));
    Tests::operator_greater_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("1"));
    Tests::operator_greater_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::operator_greater_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_greater_than_eq(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_greater_than_eq(a, b);
  }
  // check for consistency with ulong behaviour
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 5ul);
    assert(!(a >= b));
  }
  {
    boost::dynamic_bitset<Block> a(3, 4ul), b(3, 4ul);
    assert(a >= b);
  }
  {
    boost::dynamic_bitset<Block> a(3, 5ul), b(3, 4ul);
    assert(a >= b);
  }
  //=====================================================================
  // Test b.test(pos)  
  { // case pos >= b.size()
    boost::dynamic_bitset<Block> b;
    Tests::test_bit(b, 0);
  }
  { // case pos < b.size()
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::test_bit(b, 0);
  }
  { // case pos == b.size() / 2
    boost::dynamic_bitset<Block> b(long_string);
    Tests::test_bit(b, long_string.size()/2);
  }
  //=====================================================================
  // Test b << pos  
  { // case pos == 0
    std::size_t pos = 0;
    boost::dynamic_bitset<Block> b(std::string("1010"));
    Tests::operator_shift_left(b, pos);
  }
  { // case pos == size()/2
    std::size_t pos = long_string.size() / 2;
    boost::dynamic_bitset<Block> b(long_string);
    Tests::operator_shift_left(b, pos);
  }
  { // case pos >= n
    std::size_t pos = long_string.size();
    boost::dynamic_bitset<Block> b(long_string);
    Tests::operator_shift_left(b, pos);
  }
  //=====================================================================
  // Test b >> pos  
  { // case pos == 0
    std::size_t pos = 0;
    boost::dynamic_bitset<Block> b(std::string("1010"));
    Tests::operator_shift_right(b, pos);
  }
  { // case pos == size()/2
    std::size_t pos = long_string.size() / 2;
    boost::dynamic_bitset<Block> b(long_string);
    Tests::operator_shift_right(b, pos);
  }
  { // case pos >= n
    std::size_t pos = long_string.size();
    boost::dynamic_bitset<Block> b(long_string);
    Tests::operator_shift_right(b, pos);
  }
  //=====================================================================
  // Test a & b
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::operator_and(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::operator_and(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_and(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_and(lhs, rhs);
  }
  //=====================================================================
  // Test a | b
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::operator_or(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::operator_or(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_or(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_or(lhs, rhs);
  }
  //=====================================================================
  // Test a^b
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::operator_xor(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::operator_xor(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_xor(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_xor(lhs, rhs);
  }
  //=====================================================================
  // Test a-b
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::operator_sub(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::operator_sub(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_sub(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_sub(lhs, rhs);
  }
  //=====================================================================
  // Test stream operator<< and operator>>
  {
    boost::dynamic_bitset<Block> b;
    boost::dynamic_bitset<Block> x(b.size());
    Tests::stream_read_write(b, x);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    boost::dynamic_bitset<Block> x(b.size());
    Tests::stream_read_write(b, x);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    boost::dynamic_bitset<Block> x(b.size());
    Tests::stream_read_write(b, x);
  }
}

int
test_main(int, char*[])
{ 
  run_test_cases<unsigned char>();
  run_test_cases<unsigned short>();
  run_test_cases<unsigned long>();
  return EXIT_SUCCESS;
}
