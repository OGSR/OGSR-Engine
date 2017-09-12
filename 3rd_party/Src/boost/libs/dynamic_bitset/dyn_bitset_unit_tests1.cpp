// (C) Copyright Jeremy Siek 2001. 
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

#include <iostream>
#include <fstream>
#include <cmath> // for pow
#include <boost/limits.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/test/test_tools.hpp>

#include "bitset_test.hpp"


template <typename Block>
void test_from_ulong(std::size_t n, unsigned long number)
{
  boost::dynamic_bitset<Block> b(n, number);
  bitset_test< boost::dynamic_bitset<Block> >::from_unsigned_long(b, number);
}


template <typename Block>
void run_test_cases()
{
  typedef bitset_test< boost::dynamic_bitset<Block> > Tests;

  std::string long_string(101, '0');
  for (std::size_t j = 0; j < long_string.size(); ++j)
    long_string[j] = '0' + (j % 2);

  std::size_t N, ul_size = CHAR_BIT * sizeof(unsigned long), 
    block_size = CHAR_BIT * sizeof(Block);
  unsigned long numbers[] = { 0, 40247,
                              std::numeric_limits<unsigned long>::max() };

  //=====================================================================
  // Test construction from unsigned long
  for (std::size_t i = 0; i < 3; ++i) {
    unsigned long number = numbers[i];
    N = 0;
    test_from_ulong<Block>(N, number);
    
    N = std::size_t(0.7 * double(ul_size));
    test_from_ulong<Block>(N, number);
    
    N = 1 * ul_size;
    test_from_ulong<Block>(N, number);
    
    N = std::size_t(1.3 * double(ul_size));
    test_from_ulong<Block>(N, number);
    
    N = std::size_t(0.7 * double(block_size));
    test_from_ulong<Block>(N, number);
    
    N = block_size;
    test_from_ulong<Block>(N, number);
    
    N = std::size_t(1.3 * double(block_size));
    test_from_ulong<Block>(N, number);
    
    N = 3 * block_size;
    test_from_ulong<Block>(N, number);
  }
  //=====================================================================
  // Test construction from a string
  { 
    // case pos > str.size()
    Tests::from_string(std::string(""), 1, 1);

    // invalid arguments
    Tests::from_string(std::string("x11"), 0, 3);
    Tests::from_string(std::string("0y1"), 0, 3);
    Tests::from_string(std::string("10z"), 0, 3);

    // valid arguments
    Tests::from_string(std::string(""), 0, 0);
    Tests::from_string(std::string("0"), 0, 1);
    Tests::from_string(std::string("1"), 0, 1);
    Tests::from_string(long_string, 0, long_string.size());
  }
  //=====================================================================
  // Test construction from a block range
  {
    std::vector<Block> blocks;
    Tests::from_block_range(blocks);
  }
  {
    std::vector<Block> blocks(3);
    blocks[0] = static_cast<Block>(0);
    blocks[1] = static_cast<Block>(1);
    blocks[2] = ~Block(0);
    Tests::from_block_range(blocks);
  }
  {
    std::vector<Block> blocks(101);
    for (typename std::vector<Block>::size_type i = 0; 
         i < blocks.size(); ++i)
      blocks[i] = i;
    Tests::from_block_range(blocks);
  }
  //=====================================================================
  // Test copy constructor
  {
    boost::dynamic_bitset<Block> b;
    Tests::copy_constructor(b);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("0"));
    Tests::copy_constructor(b);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    Tests::copy_constructor(b);
  }
  //=====================================================================
  // Test assignment operator
  {
    boost::dynamic_bitset<Block> a, b;
    Tests::assignment_operator(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1")), b(std::string("0"));
    Tests::assignment_operator(a, b);
  }
  {
    boost::dynamic_bitset<Block> a(long_string), b(long_string);
    Tests::assignment_operator(a, b);
  }
  //=====================================================================
  // Test resize
  {
    boost::dynamic_bitset<Block> a;
    Tests::resize(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0"));
    Tests::resize(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1"));
    Tests::resize(a);
  }
  {
    boost::dynamic_bitset<Block> a(long_string);
    Tests::resize(a);
  }
  //=====================================================================
  // Test clear
  {
    boost::dynamic_bitset<Block> a;
    Tests::clear(a);
  }
  {
    boost::dynamic_bitset<Block> a(long_string);
    Tests::clear(a);
  }
  //=====================================================================
  // Test append bit
  {
    boost::dynamic_bitset<Block> a;
    Tests::append_bit(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0"));
    Tests::append_bit(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1"));
    Tests::append_bit(a);
  }
  {
    boost::dynamic_bitset<Block> a(long_string);
    Tests::append_bit(a);
  }
  //=====================================================================
  // Test append block
  {
    boost::dynamic_bitset<Block> a;
    Tests::append_block(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0"));
    Tests::append_block(a);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1"));
    Tests::append_block(a);
  }
  {
    boost::dynamic_bitset<Block> a(long_string);
    Tests::append_block(a);
  }
  //=====================================================================
  // Test append block range
  {
    boost::dynamic_bitset<Block> a;
    std::vector<Block> blocks;
    Tests::append_block_range(a, blocks);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("0"));
    std::vector<Block> blocks(3);
    blocks[0] = static_cast<Block>(0);
    blocks[1] = static_cast<Block>(1);
    blocks[2] = ~Block(0);
    Tests::append_block_range(a, blocks);
  }
  {
    boost::dynamic_bitset<Block> a(std::string("1"));
    std::vector<Block> blocks(101);
    for (typename std::vector<Block>::size_type i = 0; 
         i < blocks.size(); ++i)
      blocks[i] = i;
    Tests::append_block_range(a, blocks);
  }
  {
    boost::dynamic_bitset<Block> a(long_string);
    std::vector<Block> blocks(3);
    blocks[0] = static_cast<Block>(0);
    blocks[1] = static_cast<Block>(1);
    blocks[2] = ~Block(0);
    Tests::append_block_range(a, blocks);
  }
  //=====================================================================
  // Test bracket operator
  {
    boost::dynamic_bitset<Block> b1;
    std::vector<bool> bitvec1;
    Tests::operator_bracket(b1, bitvec1);
  }
  {
    boost::dynamic_bitset<Block> b(std::string("1"));
    std::vector<bool> bit_vec(1, true);
    Tests::operator_bracket(b, bit_vec);
  }
  {
    boost::dynamic_bitset<Block> b(long_string);
    std::size_t n = long_string.size();
    std::vector<bool> bit_vec(n);
    for (std::size_t i = 0; i < n; ++i)
      bit_vec[i] = long_string[n - 1 - i] == '0' ? 0 : 1;
    Tests::operator_bracket(b, bit_vec);
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
