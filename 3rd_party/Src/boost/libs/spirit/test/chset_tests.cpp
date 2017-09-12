/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 1998-2003 Joel de Guzman
    Copyright (c) 2001-2003 Daniel Nuffer
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <iostream>
#include <cassert>
#include "boost/config.hpp"
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

using namespace std;

//#define BOOST_SPIRIT_DEBUG
#include "boost/spirit/utility/chset.hpp"
using namespace boost::spirit;

namespace
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  chset tests
    //
    ///////////////////////////////////////////////////////////////////////////
    void
    DrawRuler(SSTREAM& out, char const* str)
    {
        out << std::endl << std::endl;
        out << "\t_____________________________________________________________\n";
        out << "\t" << str << std::endl;
        out << "\t";
        for (char i = '!'; i < '^'; i++)
            out << i;
        out << "\n";
        out << "\t_____________________________________________________________\n\n";
    }

    //////////////////////////////////
    template <typename CharT>
    void
    Draw(SSTREAM& out, chset<CharT> a, char const* str)
    {
        out << "\t";

        for (int i = '!'; i < '^'; i++)
            if (a.test(i))
                out << '*';
            else
                out << " ";

        out << "\t" << str << std::endl;
    }

    //////////////////////////////////
    template <typename CharT>
    void
    chset_tests(SSTREAM& out, CharT const* a_, CharT b1_, CharT b2_)
    {
        chset<CharT>    a(a_);
        range<CharT>    b_(b1_, b2_);
        chset<CharT>    b(b_);
        chset<CharT>    c(~a);  // ~char_parser code must not interfere
                                // with chset

        DrawRuler(out, "Initial");
        Draw(out, a, "a \tchset<CharT>  a(\"0-9A-Z\");");
        Draw(out, b, "b_ \trange<CharT>  b_('5', 'J');");
        Draw(out, b, "b \tchset<CharT>  b(b_);");

        DrawRuler(out, "Inverse");
        Draw(out, ~a, "~a");
        Draw(out, c, "chset<>(~a)");
        Draw(out, ~~a, "~~a");
        Draw(out, ~b, "~b");
        Draw(out, ~b_, "~b_");

        DrawRuler(out, "Union");
        Draw(out, a, "a");
        Draw(out, b, "b");
        Draw(out, a | b, "a | b");
        Draw(out, a | b_, "a | b_");
        Draw(out, b_ | a, "b_ | a");
        Draw(out, a | anychar_p, "a | anychar_p");
        Draw(out, b | anychar_p, "b | anychar_p");

        DrawRuler(out, "Intersection");
        Draw(out, a, "a");
        Draw(out, b, "b");
        Draw(out, a & b, "a & b");
        Draw(out, a & b_, "a & b_");
        Draw(out, b_ & a, "b_ & a");
        Draw(out, a & anychar_p, "a & anychar_p");
        Draw(out, b & anychar_p, "b & anychar_p");

        DrawRuler(out, "Difference");
        Draw(out, a, "a");
        Draw(out, b, "b");
        Draw(out, a - b, "a - b");
        Draw(out, b - a, "b - a");
        Draw(out, a - b_, "a - b_");
        Draw(out, b_ - a, "b_ - a");
        Draw(out, a - anychar_p, "a - anychar_p");
        Draw(out, anychar_p - a, "anychar_p - a");
        Draw(out, b - anychar_p, "b - anychar_p");
        Draw(out, anychar_p - b, "anychar_p - b");

        DrawRuler(out, "Xor");
        Draw(out, a, "a");
        Draw(out, b, "b");
        Draw(out, a ^ b, "a ^ b");
        Draw(out, a ^ b_, "a ^ b_");
        Draw(out, b_ ^ a, "b_ ^ a");
        Draw(out, a ^ nothing_p, "a ^ nothing_p");
        Draw(out, a ^ anychar_p, "a ^ anychar_p");
        Draw(out, b ^ nothing_p, "b ^ nothing_p");
        Draw(out, b ^ anychar_p, "b ^ anychar_p");
    }

    char const* test =
        "\n\n"
        "\t_____________________________________________________________\n"
        "\tInitial\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t               **********       **************************   \ta \tchset<CharT>  a(\"0-9A-Z\");\n"
        "\t                    **********************                   \tb_ \trange<CharT>  b_('5', 'J');\n"
        "\t                    **********************                   \tb \tchset<CharT>  b(b_);\n"
        "\n"
        "\n"
        "\t_____________________________________________________________\n"
        "\tInverse\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t***************          *******                          ***\t~a\n"
        "\t***************          *******                          ***\tchset<>(~a)\n"
        "\t               **********       **************************   \t~~a\n"
        "\t********************                      *******************\t~b\n"
        "\t********************                      *******************\t~b_\n"
        "\n"
        "\n"
        "\t_____________________________________________________________\n"
        "\tUnion\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t               **********       **************************   \ta\n"
        "\t                    **********************                   \tb\n"
        "\t               *******************************************   \ta | b\n"
        "\t               *******************************************   \ta | b_\n"
        "\t               *******************************************   \tb_ | a\n"
        "\t*************************************************************\ta | anychar_p\n"
        "\t*************************************************************\tb | anychar_p\n"
        "\n"
        "\n"
        "\t_____________________________________________________________\n"
        "\tIntersection\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t               **********       **************************   \ta\n"
        "\t                    **********************                   \tb\n"
        "\t                    *****       **********                   \ta & b\n"
        "\t                    *****       **********                   \ta & b_\n"
        "\t                    *****       **********                   \tb_ & a\n"
        "\t               **********       **************************   \ta & anychar_p\n"
        "\t                    **********************                   \tb & anychar_p\n"
        "\n"
        "\n"
        "\t_____________________________________________________________\n"
        "\tDifference\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t               **********       **************************   \ta\n"
        "\t                    **********************                   \tb\n"
        "\t               *****                      ****************   \ta - b\n"
        "\t                         *******                             \tb - a\n"
        "\t               *****                      ****************   \ta - b_\n"
        "\t                         *******                             \tb_ - a\n"
        "\t                                                             \ta - anychar_p\n"
        "\t***************          *******                          ***\tanychar_p - a\n"
        "\t                                                             \tb - anychar_p\n"
        "\t********************                      *******************\tanychar_p - b\n"
        "\n"
        "\n"
        "\t_____________________________________________________________\n"
        "\tXor\n"
        "\t!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
        "\t_____________________________________________________________\n"
        "\n"
        "\t               **********       **************************   \ta\n"
        "\t                    **********************                   \tb\n"
        "\t               *****     *******          ****************   \ta ^ b\n"
        "\t               *****     *******          ****************   \ta ^ b_\n"
        "\t               *****     *******          ****************   \tb_ ^ a\n"
        "\t               **********       **************************   \ta ^ nothing_p\n"
        "\t***************          *******                          ***\ta ^ anychar_p\n"
        "\t                    **********************                   \tb ^ nothing_p\n"
        "\t********************                      *******************\tb ^ anychar_p\n"
    ;

} // namespace

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tCharacter sets test...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    SSTREAM tout, aout, bout;

    tout << test;

    chset_tests(aout, "0-9A-Z", '5', 'J');
    chset_tests(bout, L"0-9A-Z", L'5', L'J');
    assert(GETSTRING(aout) == GETSTRING(tout));
    assert(GETSTRING(aout) == GETSTRING(bout));

    cout << GETSTRING(tout);
    std::cout << "\n\n/////////////////////////////////////////////////////////\n\n";
    cout << "Tests concluded successfully\n";
    return 0;
}

