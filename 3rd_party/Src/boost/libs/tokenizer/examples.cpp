// Boost tokenizer examples  -------------------------------------------------//

// © Copyright John R. Bandela 2001. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// See http://www.boost.org for updates, documentation, and revision history.

#include <iostream>
#include <iterator>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/array.hpp>

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

int test_main(int, char*[])
{
  using namespace std;
  using namespace boost;
  
  // Use tokenizer
  {
                const string test_string = ";;Hello|world||-foo--bar;yow;baz|";
                string answer[] = { "Hello", "world",  "foo", "bar", "yow",  "baz" };
                typedef tokenizer<char_separator<char> > Tok;
                char_separator<char> sep("-;|");
                Tok t(test_string, sep);
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));               
  }
  {
                const string test_string = ";;Hello|world||-foo--bar;yow;baz|";
                string answer[] = { "", "", "Hello", "|", "world", "|", "", "|", "",
                                                                                                "foo", "", "bar", "yow", "baz", "|", "" };
                typedef tokenizer<char_separator<char> > Tok;
                char_separator<char> sep("-;", "|", boost::keep_empty_tokens);
                Tok t(test_string, sep);
    BOOST_CRITICAL_TEST(equal(t.begin(), t.end(), answer));
  }
  {
    const string test_string = "This,,is, a.test..";
    string answer[] = {"This","is","a","test"};
    typedef tokenizer<> Tok;
    Tok t(test_string);
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));
  }

  {
    const string test_string = "Field 1,\"embedded,comma\",quote \\\", escape \\\\";
    string answer[] = {"Field 1","embedded,comma","quote \""," escape \\"};
    typedef tokenizer<escaped_list_separator<char> > Tok;
    Tok t(test_string);
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));

  }

  {
    const string test_string = ",1,;2\\\";3\\;,4,5^\\,\'6,7\';";
    string answer[] = {"","1","","2\"","3;","4","5\\","6,7",""};
    typedef tokenizer<escaped_list_separator<char> > Tok;
    escaped_list_separator<char> sep("\\^",",;","\"\'");
    Tok t(test_string,sep);
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));

  }

  {
    const string test_string = "12252001";
    string answer[] = {"12","25","2001"};
    typedef tokenizer<offset_separator > Tok;
    boost::array<int,3> offsets = {{2,2,4}};
    offset_separator func(offsets.begin(),offsets.end());
    Tok t(test_string,func);
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));

  }

  // Use token_iterator_generator
  {
    
    const string test_string = "This,,is, a.test..";
    string answer[] = {"This","is","a","test"};
    typedef token_iterator_generator<char_delimiters_separator<char> >::type Iter;
    Iter begin = make_token_iterator<string>(test_string.begin(),
      test_string.end(),char_delimiters_separator<char>());
    Iter end;
    BOOST_CRITICAL_TEST(equal(begin,end,answer));
  }

  {
    const string test_string = "Field 1,\"embedded,comma\",quote \\\", escape \\\\";
    string answer[] = {"Field 1","embedded,comma","quote \""," escape \\"};
    typedef token_iterator_generator<escaped_list_separator<char> >::type Iter;
    Iter begin = make_token_iterator<string>(test_string.begin(),
      test_string.end(),escaped_list_separator<char>());
    Iter end;
    BOOST_CRITICAL_TEST(equal(begin,end,answer));

  }

  {
    const string test_string = "12252001";
    string answer[] = {"12","25","2001"};
    typedef token_iterator_generator<offset_separator>::type Iter;
    boost::array<int,3> offsets = {{2,2,4}};
    offset_separator func(offsets.begin(),offsets.end());
    Iter begin = make_token_iterator<string>(test_string.begin(),
      test_string.end(),func);
    Iter end= make_token_iterator<string>(test_string.end(),
      test_string.end(),func);
    BOOST_CRITICAL_TEST(equal(begin,end,answer));

  }
  
  // Test copying
  {
    string s = "abcdef";
    token_iterator_generator<offset_separator>::type beg, end, other;
    boost::array<int,3> ar = {{1,2,3}};
    offset_separator f(ar.begin(),ar.end());
    beg = make_token_iterator<string>(s.begin(),s.end(),f);
    
    ++beg;
    other = beg;
    ++other;

    BOOST_CRITICAL_TEST(*beg=="bc");
    BOOST_CRITICAL_TEST(*other=="def");
    
    other =  make_token_iterator<string>(s.begin(),s.end(),f);

    BOOST_CRITICAL_TEST(*other=="a");
  }

  // Test non-default constructed char_delimiters_separator
  {
    const string test_string = "how,are you, doing";
    string answer[] = {"how",",","are you",","," doing"};
    tokenizer<> t(test_string,char_delimiters_separator<char>(true,",",""));
    BOOST_CRITICAL_TEST(equal(t.begin(),t.end(),answer));
  }

  return 0;
}

