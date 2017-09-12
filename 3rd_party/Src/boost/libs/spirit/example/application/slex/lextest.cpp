/*=============================================================================
    Lexer test

    Spirit v1.6.0
    Copyright (c) 2001, Daniel C. Nuffer

    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.
=============================================================================*/

#if defined (__BORLANDC__)
#define _RWSTD_COMPILE_INSTANTIATE // Borland Workaround
#endif

#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace boost::spirit;
using namespace std;

ostream& operator<<(ostream& out, wchar_t c)
{
    if (boost::spirit::impl::isgraph_(char(c)) || boost::spirit::impl::isspace_(char(c)))
        out << char(c);
    else
        out << '.';
    return out;
}

ostream& operator<<(ostream& out, wchar_t const* c)
{
    while (*c)
        out << char(*c++);
    return out;
}

ostream& operator<<(ostream& out, const std::basic_string<wchar_t>& c)
{
    for (std::basic_string<wchar_t>::const_iterator i = c.begin();
            i != c.end(); ++i)
    {
        if (boost::spirit::impl::isgraph_(char(*i)) || boost::spirit::impl::isspace_(char(*i)))
            out << char(*i);
        else
            out << '.';
    }
    return out;
}

template <typename IteratorT, typename StrT>
void test_match(lexer<IteratorT>& lexer, const StrT& str)
{
    typedef typename StrT::value_type CharT;

    int token;
    IteratorT end = &*str.end();
    IteratorT first = &*str.begin();
    IteratorT prev = first;

    cout << "testing string: " << str << endl;
    while ((token = lexer.next_token(first, end)) != -1 && first != end)
    {
        cout << "matched token #" << token << " :" <<
              basic_string<CharT>(prev, first) << '\n';
        prev = first;
    }
    if (token == -1)
    {
        cout << "lexing failed at: " << basic_string<CharT>(first, end) << endl;
    }
    else
    {
        cout << "matched token #" << token << " :" <<
              basic_string<CharT>(prev, first) << '\n';
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Usage: <lex file> <lex string>" << endl;
        return 1;
    }

    lexer<> lex;
    lexer<wchar_t const *> wlex;
    string line;
    int id = 0;
    int wid = 0;
    ifstream in(argv[1]);
    if (!in)
    {
        cout << "Could not open file: " << argv[1] << endl;
        return 1;
    }

    for (getline(in, line); in.good(); getline(in, line))
    {
        if (line.size() > 0)
        {
            if (line[0] == '%') // it's a special directive
            {
                if (line == "%i") // case insensitive
                {
                    lex.set_case_insensitive(true);
                    wlex.set_case_insensitive(true);
                }
            }
            else
            {
                cout << "token #" << id << ": " << line << '\n';
                lex.register_regex(line, id++);
                wlex.register_regex(
                        std::basic_string<wchar_t>(line.begin(), line.end()),
                        wid++);
            }
        }
    }

    lex.create_dfa();
    wlex.create_dfa();

#ifdef BOOST_SPIRIT_DEBUG
    lex.dump(cout);
    wlex.dump(cout);
#endif

    ifstream testfile(argv[2]);
    if (testfile)
    {
        string teststr, line;
        for (getline(testfile, line); testfile.good(); getline(testfile, line))
        {
            teststr += line;
            teststr += '\n';
        }
        test_match(lex, teststr);
        test_match(wlex,
                std::basic_string<wchar_t>(teststr.begin(), teststr.end()));
    }
    else
    {
        test_match(lex,
                std::basic_string<char>(argv[2], argv[2]+strlen(argv[2])));
        test_match(wlex,
                std::basic_string<wchar_t>(argv[2], argv[2]+strlen(argv[2])));
    }

    return 0;
}





