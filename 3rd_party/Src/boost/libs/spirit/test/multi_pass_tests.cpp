/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Daniel Nuffer
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include "boost/config.hpp"
#include "boost/spirit/iterator/multi_pass.hpp"
#include <iterator>
#include <string>
#ifdef BOOST_NO_STRINGSTREAM
#include <strstream>
#define SSTREAM strstream
std::string GETSTRING(std::strstream& ss)
{
    ss << ends;
    std::string rval = ss.str();
    ss.freeze(false);
    return rval;
}
#else
#include <sstream>
#define GETSTRING(ss) ss.str()
#define SSTREAM stringstream
#endif
#include <cassert>
#include <memory> // for auto_ptr

using namespace std;
using namespace boost::spirit;

SSTREAM res;

typedef multi_pass<istream_iterator<char> > default_multi_pass_t;

/*
typedef multi_pass<
    istream_iterator<char>,
    multi_pass_policies::input_iterator,
    multi_pass_policies::first_owner,
    multi_pass_policies::no_check,
    multi_pass_policies::fixed_size_queue<6>
> fixed_multi_pass_t;
*/
// this replaces the above
typedef look_ahead<istream_iterator<char>, 6> fixed_multi_pass_t;

typedef multi_pass<
    istream_iterator<char>,
    multi_pass_policies::input_iterator,
    multi_pass_policies::first_owner,
    multi_pass_policies::buf_id_check,
    multi_pass_policies::std_deque
> first_owner_multi_pass_t;


// a functor to test out the functor_multi_pass
class my_functor
{
    public:
        typedef char result_type;
        my_functor()
            : c('A')
        {}

        char operator()()
        {
            if (c == 'M')
                return eof;
            else
                return c++;
        }

        static result_type eof;
    private:
        char c;
};

my_functor::result_type my_functor::eof = '\0';

typedef multi_pass<
    my_functor,
    multi_pass_policies::functor_input,
    multi_pass_policies::first_owner,
    multi_pass_policies::no_check,
    multi_pass_policies::std_deque
> functor_multi_pass_t;

void test_default_multi_pass()
{
    res << "-*= test_default_multi_pass =*-\n";
    istream_iterator<char> end;
    auto_ptr<default_multi_pass_t> mpend(new default_multi_pass_t(end));

    {
        SSTREAM ss;
        ss << "test string";

        istream_iterator<char> a(ss);
        auto_ptr<default_multi_pass_t> mp1(new default_multi_pass_t(a));

        while (*mp1 != *mpend)
        {
            res << *((*mp1)++);
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(b));
        auto_ptr<default_multi_pass_t> mp3(new default_multi_pass_t(b));
        *mp3 = *mp2;

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<default_multi_pass_t> mp1(new default_multi_pass_t(a));
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(*mp1));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(b));
        auto_ptr<default_multi_pass_t> mp3(new default_multi_pass_t(b));
        *mp3 = *mp2;

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();
        ++*mp2;

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<default_multi_pass_t> mp1(new default_multi_pass_t(a));
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(*mp1));

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mp1)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<default_multi_pass_t> mp1(new default_multi_pass_t(a));
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(a));
        assert(*mp1 != *mp2);
        ++*mp1;
        assert(*mp1 != *mp2);

    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<default_multi_pass_t> mp2(new default_multi_pass_t(b));
        auto_ptr<default_multi_pass_t> mp3(new default_multi_pass_t(b));
        *mp3 = *mp2;

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp2->clear_queue();

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        try
        {
            res << **mp3; // this should throw illegal_backtracking
            assert(0);
        }
        catch (const boost::spirit::multi_pass_policies::illegal_backtracking& e)
        {
        }
        res << endl;
    }


}

void test_fixed_multi_pass()
{
    res << "-*= test_fixed_multi_pass =*-\n";
    istream_iterator<char> end;
    auto_ptr<fixed_multi_pass_t> mpend(new fixed_multi_pass_t(end));

    {
        SSTREAM ss;
        ss << "test string";

        istream_iterator<char> a(ss);
        auto_ptr<fixed_multi_pass_t> mp1(new fixed_multi_pass_t(a));

        while (*mp1 != *mpend)
        {
            res << *((*mp1)++);
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<fixed_multi_pass_t> mp2(new fixed_multi_pass_t(b));
        auto_ptr<fixed_multi_pass_t> mp3(new fixed_multi_pass_t(*mp2));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<fixed_multi_pass_t> mp1(new fixed_multi_pass_t(a));
        auto_ptr<fixed_multi_pass_t> mp2(new fixed_multi_pass_t(*mp1));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<fixed_multi_pass_t> mp2(new fixed_multi_pass_t(b));
        auto_ptr<fixed_multi_pass_t> mp3(new fixed_multi_pass_t(*mp2));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();
        ++*mp2;

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<fixed_multi_pass_t> mp1(new fixed_multi_pass_t(a));
        auto_ptr<fixed_multi_pass_t> mp2(new fixed_multi_pass_t(*mp1));

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mp1)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<fixed_multi_pass_t> mp1(new fixed_multi_pass_t(a));
        auto_ptr<fixed_multi_pass_t> mp2(new fixed_multi_pass_t(a));
        assert(*mp1 != *mp2);
        ++*mp1;
        assert(*mp1 != *mp2);

    }

}

void test_first_owner_multi_pass()
{
    res << "-*= test_first_owner_multi_pass =*-\n";
    istream_iterator<char> end;
    auto_ptr<first_owner_multi_pass_t> mpend(new first_owner_multi_pass_t(end));

    {
        SSTREAM ss;
        ss << "test string";

        istream_iterator<char> a(ss);
        auto_ptr<first_owner_multi_pass_t> mp1(new first_owner_multi_pass_t(a));

        while (*mp1 != *mpend)
        {
            res << *((*mp1)++);
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(b));
        auto_ptr<first_owner_multi_pass_t> mp3(new first_owner_multi_pass_t(*mp2));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<first_owner_multi_pass_t> mp1(new first_owner_multi_pass_t(a));
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(*mp1));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(b));
        auto_ptr<first_owner_multi_pass_t> mp3(new first_owner_multi_pass_t(*mp2));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp3.reset();
        ++*mp2;

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<first_owner_multi_pass_t> mp1(new first_owner_multi_pass_t(a));
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(*mp1));

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        for (int i = 0; i < 4; ++i)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mp1)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        while (*mp1 != *mpend)
        {
            res << **mp1;
            ++*mp1;
        }

        assert(*mp1 != *mp2);
        assert(*mp1 > *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp2 < *mp1);
        assert(*mp2 <= *mp1);
        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        assert(*mp1 == *mp2);
        assert(*mp1 >= *mp2);
        assert(*mp1 <= *mp2);
        res << endl;
    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> a(ss);
        auto_ptr<first_owner_multi_pass_t> mp1(new first_owner_multi_pass_t(a));
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(a));
        assert(*mp1 != *mp2);
        ++*mp1;
        assert(*mp1 != *mp2);

    }

    {
        SSTREAM ss;
        ss << "test string";
        istream_iterator<char> b(ss);
        auto_ptr<first_owner_multi_pass_t> mp2(new first_owner_multi_pass_t(b));
        auto_ptr<first_owner_multi_pass_t> mp3(new first_owner_multi_pass_t(*mp2));

        for (int i = 0; i < 4; ++i)
        {
            res << **mp2;
            ++*mp2;
        }

        mp2->clear_queue();

        while (*mp2 != *mpend)
        {
            res << **mp2;
            ++*mp2;
        }

        try
        {
            res << **mp3; // this should throw illegal_backtracking
            assert(0);
        }
        catch (const boost::spirit::multi_pass_policies::illegal_backtracking& e)
        {
        }
        res << endl;
    }

}


void test_functor_multi_pass()
{
    res << "-*= test_functor_multi_pass =*-\n";
    functor_multi_pass_t mpend;

    {
        functor_multi_pass_t mp1 = functor_multi_pass_t(my_functor());

        while (mp1 != mpend)
        {
            res << *(mp1++);
        }

        res << endl;
    }

    {
        functor_multi_pass_t mp1 = functor_multi_pass_t(my_functor());
        functor_multi_pass_t mp2 = functor_multi_pass_t(mp1);

        for (int i = 0; i < 4; ++i)
        {
            res << *mp1;
            ++mp1;
        }

        while (mp2 != mpend)
        {
            res << *mp2;
            ++mp2;
        }

        while (mp1 != mpend)
        {
            res << *mp1;
            ++mp1;
        }

        res << endl;
    }

    {
        functor_multi_pass_t mp1 = functor_multi_pass_t(my_functor());
        functor_multi_pass_t mp2 = functor_multi_pass_t(mp1);

        assert(mp1 == mp2);
        assert(mp1 >= mp2);
        assert(mp1 <= mp2);
        for (int i = 0; i < 4; ++i)
        {
            res << *mp1;
            ++mp1;
        }

        assert(mp1 != mp2);
        assert(mp1 > mp2);
        assert(mp1 >= mp2);
        assert(mp2 < mp1);
        assert(mp2 <= mp1);
        while (mp2 != mp1)
        {
            res << *mp2;
            ++mp2;
        }

        assert(mp1 == mp2);
        assert(mp1 >= mp2);
        assert(mp1 <= mp2);
        while (mp1 != mpend)
        {
            res << *mp1;
            ++mp1;
        }

        assert(mp1 != mp2);
        assert(mp1 > mp2);
        assert(mp1 >= mp2);
        assert(mp2 < mp1);
        assert(mp2 <= mp1);
        while (mp2 != mpend)
        {
            res << *mp2;
            ++mp2;
        }

        assert(mp1 == mp2);
        assert(mp1 >= mp2);
        assert(mp1 <= mp2);
        res << endl;
    }

    {
        functor_multi_pass_t mp1 = functor_multi_pass_t(my_functor());
        functor_multi_pass_t mp2 = functor_multi_pass_t(my_functor());
        assert(mp1 != mp2);
        ++mp1;
        assert(mp1 != mp2);

    }
}

int main(int, char**)
{

    test_default_multi_pass();
    test_fixed_multi_pass();
    test_first_owner_multi_pass();
    test_functor_multi_pass();

    assert(GETSTRING(res) == "-*= test_default_multi_pass =*-\n"
            "teststring\n"
            "teststring\n"
            "testteststringstring\n"
            "testtring\n"
            "testteststringstring\n"
            "teststring\n"
            "-*= test_fixed_multi_pass =*-\n"
            "teststring\n"
            "teststring\n"
            "testteststringstring\n"
            "testtring\n"
            "testteststringstring\n"
            "-*= test_first_owner_multi_pass =*-\n"
            "teststring\n"
            "teststring\n"
            "testteststringstring\n"
            "testtring\n"
            "testteststringstring\n"
            "teststring\n"
            "-*= test_functor_multi_pass =*-\n"
            "ABCDEFGHIJKL\n"
            "ABCDABCDEFGHIJKLEFGHIJKL\n"
            "ABCDABCDEFGHIJKLEFGHIJKL\n");
}

