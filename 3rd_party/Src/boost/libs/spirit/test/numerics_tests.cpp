/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Joel de Guzman
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include "boost/spirit/core.hpp"
#include <iostream>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

//////////////////////////////////
template <typename T>
struct ts_real_parser_policies : public ureal_parser_policies<T>
{
    //  These policies can be used to parse thousand separated
    //  numbers with at most 2 decimal digits after the decimal
    //  point. e.g. 123,456,789.01

    typedef uint_parser<int, 10, 1, 2>  uint2_t;
    typedef uint_parser<T, 10, 1, -1>   uint_parser_t;
    typedef int_parser<int, 10, 1, -1>  int_parser_t;

    //////////////////////////////////  2 decimal places Max
    template <typename ScannerT>
    static typename parser_result<uint2_t, ScannerT>::type
    parse_frac_n(ScannerT& scan)
    { return uint2_t().parse(scan); }

    //////////////////////////////////
    template <typename ScannerT>
    static typename parser_result<chlit<>, ScannerT>::type
    parse_exp(ScannerT& scan)
    { return scan.no_match(); }

    //////////////////////////////////
    template <typename ScannerT>
    static typename parser_result<int_parser_t, ScannerT>::type
    parse_exp_n(ScannerT& scan)
    { return scan.no_match(); }

    //////////////////////////////////  Thousands separated numbers
    template <typename ScannerT>
    static typename parser_result<uint_parser_t, ScannerT>::type
    parse_n(ScannerT& scan)
    {
        typedef typename parser_result<uint_parser_t, ScannerT>::type RT;
        uint_parser<unsigned, 10, 1, 3> uint3_p;
        uint_parser<unsigned, 10, 3, 3> uint3_3_p;

        if (RT hit = uint3_p.parse(scan))
        {
            T n;
            while (match<> next = (',' >> uint3_3_p[assign(n)]).parse(scan))
            {
                hit.value() *= 1000;
                hit.value() += n;
                scan.concat_match(hit, next);
            }
            return hit;
        }
        return scan.no_match();
    }
};

real_parser<double, ts_real_parser_policies<double> > const
    ts_real_p = real_parser<double, ts_real_parser_policies<double> >();

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tNumeric tests...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    //  *** The following assumes 32 bit integers. Modify these constant
    //  *** strings when appropriate. BEWARE PLATFORM DEPENDENT!

    char const* max_unsigned = "4294967295";
    char const* unsigned_overflow = "4294967296";
    char const* max_int = "2147483647";
    char const* int_overflow = "2147483648";
    char const* min_int = "-2147483648";
    char const* int_underflow = "-2147483649";
    char const* max_binary = "11111111111111111111111111111111";
    char const* binary_overflow = "100000000000000000000000000000000";
    char const* max_octal = "77777777777";
    char const* octal_overflow = "100000000000";
    char const* max_hex = "FFFFFFFF";
    char const* hex_overflow = "100000000";

#ifdef BOOST_HAS_LONG_LONG

    char const* max_long_long = "9223372036854775807";
    char const* long_long_overflow = "9223372036854775808";
    char const* min_long_long = "-9223372036854775808";
    char const* long_long_underflow = "-9223372036854775809";

#endif

//  BEGIN TESTS...

    unsigned u;

//  unsigned integer

    parse("123456", uint_p[assign(u)]);
    assert(u == 123456);

    parse(max_unsigned, uint_p[assign(u)]);
    assert(u == UINT_MAX);

    assert(!parse(unsigned_overflow, uint_p).full);

//  signed integer

    int i;

    parse("123456", int_p[assign(i)]);
    assert(i == 123456);

    parse("-123456", int_p[assign(i)]);
    assert(i == -123456);

    parse(max_int, int_p[assign(i)]);
    assert(i == INT_MAX);

    parse(min_int, int_p[assign(i)]);
    assert(i == INT_MIN);

    assert(!parse(int_overflow, int_p).full);
    assert(!parse(int_underflow, int_p).full);

    assert(!parse("-", int_p).hit);

//  binary

    parse("11111110", bin_p[assign(u)]);
    assert(u == 0xFE);

    parse(max_binary, bin_p[assign(u)]);
    assert(u == UINT_MAX);

    assert(!parse(binary_overflow, bin_p).full);

//  octal

    parse("12545674515", oct_p[assign(u)]);
    assert(u == 012545674515);

    parse(max_octal, oct_p[assign(u)]);
    assert(u == UINT_MAX);

    assert(!parse(octal_overflow, oct_p).full);

//  hex

    parse("95BC8DF", hex_p[assign(u)]);
    assert(u == 0x95BC8DF);

    parse("abcdef12", hex_p[assign(u)]);
    assert(u == 0xabcdef12);

    parse(max_hex, hex_p[assign(u)]);
    assert(u == UINT_MAX);

    assert(!parse(hex_overflow, hex_p).full);

//  limited fieldwidth

    uint_parser<unsigned, 10, 1, 3> uint3_p;
    parse("123456", uint3_p[assign(u)]);
    assert(u == 123);

    uint_parser<unsigned, 10, 1, 4> uint4_p;
    parse("123456", uint4_p[assign(u)]);
    assert(u == 1234);

    uint_parser<unsigned, 10, 3, 3> uint3_3_p;

//  thousand separated numbers

#define r (uint3_p >> *(',' >> uint3_3_p))

    assert(parse("1,234,567,890", r).full);     //  OK
    assert(parse("12,345,678,900", r).full);    //  OK
    assert(parse("123,456,789,000", r).full);   //  OK
    assert(!parse("1000,234,567,890", r).full); //  Bad
    assert(!parse("1,234,56,890", r).full);     //  Bad

//  long long

#ifdef BOOST_HAS_LONG_LONG

// Some compilers have long long, but don't define the
// LONG_LONG_MIN and LONG_LONG_MAX macros in limits.h.  This
// assumes that long long is 64 bits.
#if !defined(LONG_LONG_MIN) && !defined(LONG_LONG_MAX) \
            && !defined(ULONG_LONG_MAX)
#define ULONG_LONG_MAX 0xffffffffffffffffLLU
#define LONG_LONG_MAX 0x7fffffffffffffffLL
#define LONG_LONG_MIN (-LONG_LONG_MAX - 1)
#endif

    long long ll;
    int_parser<long long> long_long_p;

    parse("1234567890123456789", long_long_p[assign(ll)]);
    assert(ll == 1234567890123456789LL);

    parse("-1234567890123456789", long_long_p[assign(ll)]);
    assert(ll == -1234567890123456789LL);

    parse(max_long_long, long_long_p[assign(ll)]);
    assert(ll == LONG_LONG_MAX);

    parse(min_long_long, long_long_p[assign(ll)]);
    assert(ll == LONG_LONG_MIN);

    assert(!parse(long_long_overflow, long_long_p).full);
    assert(!parse(long_long_underflow, long_long_p).full);

#endif

//  real numbers

    double  d;

    assert(parse("1234", ureal_p[assign(d)]).full && d == 1234);        //  Good.
    assert(parse("1.2e3", ureal_p[assign(d)]).full && d == 1.2e3);      //  Good.
    assert(parse("1.2e-3", ureal_p[assign(d)]).full && d == 1.2e-3);    //  Good.
    assert(parse("1.e2", ureal_p[assign(d)]).full && d == 1.e2);        //  Good.
    assert(parse(".2e3", ureal_p[assign(d)]).full && d == .2e3);        //  Good.
    assert(parse("2e3", ureal_p[assign(d)]).full && d == 2e3);          //  Good. No fraction
    assert(!parse("e3", ureal_p).full);                                 //  Bad! No number
    assert(!parse("-1.2e3", ureal_p).full);                             //  Bad! Negative number
    assert(!parse("+1.2e3", ureal_p).full);                             //  Bad! Positive sign
    assert(!parse("1.2e", ureal_p).full);                               //  Bad! No exponent
    assert(!parse("-.3", ureal_p).full);                                //  Bad! Negative

    assert(parse("-1234", real_p[assign(d)]).full && d == -1234);       //  Good.
    assert(parse("-1.2e3", real_p[assign(d)]).full && d == -1.2e3);     //  Good.
    assert(parse("+1.2e3", real_p[assign(d)]).full && d == 1.2e3);      //  Good.
    assert(parse("-0.1", real_p[assign(d)]).full && d == -0.1);         //  Good.
    assert(parse("-1.2e-3", real_p[assign(d)]).full && d == -1.2e-3);   //  Good.
    assert(parse("-1.e2", real_p[assign(d)]).full && d == -1.e2);       //  Good.
    assert(parse("-.2e3", real_p[assign(d)]).full && d == -.2e3);       //  Good.
    assert(parse("-2e3", real_p[assign(d)]).full && d == -2e3);         //  Good. No fraction
    assert(!parse("-e3", real_p).full);                                 //  Bad! No number
    assert(!parse("-1.2e", real_p).full);                               //  Bad! No exponent

    assert(!parse("1234", strict_ureal_p[assign(d)]).full);             //  Bad. Strict real
    assert(parse("1.2", strict_ureal_p[assign(d)]).full && d == 1.2);   //  Good.
    assert(!parse("-1234", strict_real_p[assign(d)]).full);             //  Bad. Strict real
    assert(parse("123.", strict_real_p[assign(d)]).full && d == 123);   //  Good.
    assert(parse("3.E6", strict_real_p[assign(d)]).full && d == 3e6);   //  Good.

//  Special thousands separated numbers

    assert(parse("123,456,789.01", ts_real_p[assign(d)]).full && d == 123456789.01);    //  Good.
    assert(parse("12,345,678.90", ts_real_p[assign(d)]).full && d == 12345678.90);      //  Good.
    assert(parse("1,234,567.89", ts_real_p[assign(d)]).full && d == 1234567.89);        //  Good.
    assert(!parse("1234,567,890", ts_real_p).full);     //  Bad.
    assert(!parse("1,234,5678,9", ts_real_p).full);     //  Bad.
    assert(!parse("1,234,567.89e6", ts_real_p).full);   //  Bad.

//  END TESTS.

/////////////////////////////////////////////////////////////////
    cout << "success!\n";
    return 0;
}
