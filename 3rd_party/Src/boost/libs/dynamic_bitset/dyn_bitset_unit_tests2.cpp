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

  //=====================================================================
  // Test operator&=
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::and_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::and_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::and_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::and_assignment(lhs, rhs);
  }
  //=====================================================================
  // Test operator |=
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::or_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::or_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::or_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::or_assignment(lhs, rhs);
  }
  //=====================================================================
  // Test operator^=
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::xor_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::xor_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("0")), rhs(std::string("1"));
    Tests::xor_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string), rhs(long_string);
    Tests::xor_assignment(lhs, rhs);
  }
  //=====================================================================
  // Test operator-=
  {
    boost::dynamic_bitset<Block> lhs, rhs;
    Tests::sub_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("1")), rhs(std::string("0"));
    Tests::sub_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("0")), rhs(std::string("1"));
    Tests::sub_assignment(lhs, rhs);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string), rhs(long_string);
    Tests::sub_assignment(lhs, rhs);
  }
  //=====================================================================
  // Test operator<<=
  { // case pos == 0
    std::size_t pos = 0;
    boost::dynamic_bitset<Block> b(std::string("1010"));
    Tests::shift_left_assignment(b, pos);
  }
  { // case pos == size()/2
    std::size_t pos = long_string.size() / 2;
    boost::dynamic_bitset<Block> b(long_string);
    Tests::shift_left_assignment(b, pos);
  }
  { // case pos >= n
    std::size_t pos = long_string.size();
    boost::dynamic_bitset<Block> b(long_string);
    Tests::shift_left_assignment(b, pos);
  }
  //=====================================================================
  // Test operator>>=
  { // case pos == 0
    std::size_t pos = 0;
    boost::dynamic_bitset<Block> b(std::string("1010"));
    Tests::shift_right_assignment(b, pos);
  }
  { // case pos == size()/2
    std::size_t pos = long_string.size() / 2;
    boost::dynamic_bitset<Block> b(long_string);
    Tests::shift_right_assignment(b, pos);
  }
  { // case pos >= n
    std::size_t pos = long_string.size();
    boost::dynamic_bitset<Block> b(long_string);
    Tests::shift_right_assignment(b, pos);
  }
  //=====================================================================
  // test b.set()
  {
    boost::dynamic_bitset<Block> b;
    Tests::set_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::set_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::set_all(b);
  }
  //=====================================================================
  // Test b.set(pos)
  { // case pos >= b.size()
    boost::dynamic_bitset<Block> b;
    Tests::set_one(b, 0, true);
  }
  { // case pos < b.size()
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::set_one(b, 0, true);
  }
  { // case pos == b.size() / 2
    boost::dynamic_bitset<Block> b(long_string);
    Tests::set_one(b, long_string.size()/2, true);
  }
  //=====================================================================
  // Test b.reset()
  {
    boost::dynamic_bitset<Block> b;
    Tests::reset_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::reset_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::reset_all(b);
  }
  //=====================================================================
  // Test b.reset(pos)  
  { // case pos >= b.size()
    boost::dynamic_bitset<Block> b;
    Tests::reset_one(b, 0);
  }
  { // case pos < b.size()
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::reset_one(b, 0);
  }
  { // case pos == b.size() / 2
    boost::dynamic_bitset<Block> b(long_string);
    Tests::reset_one(b, long_string.size()/2);
  }
  //=====================================================================
  // Test ~b
  {
    boost::dynamic_bitset<Block> b;
    Tests::operator_flip(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("1"));
    Tests::operator_flip(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::operator_flip(b);
  }
  //=====================================================================
  // Test b.flip()
  {
    boost::dynamic_bitset<Block> b;
    Tests::flip_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("1"));
    Tests::flip_all(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::flip_all(b);
  }
  //=====================================================================
  // Test b.flip(pos)  
  { // case pos >= b.size()
    boost::dynamic_bitset<Block> b;
    Tests::flip_one(b, 0);
  }
  { // case pos < b.size()
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::flip_one(b, 0);
  }
  { // case pos == b.size() / 2
    boost::dynamic_bitset<Block> b(long_string);
    Tests::flip_one(b, long_string.size()/2);
  }
}

int
test_main(int argc, char*[])
{ 
  run_test_cases<unsigned char>();
  run_test_cases<unsigned short>();
  run_test_cases<unsigned long>();
  return EXIT_SUCCESS;
}
 
