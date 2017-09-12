/*=============================================================================
    Lexer sample

    Spirit v1.6.0
    Copyright (c) 2001, Daniel C. Nuffer

    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.
=============================================================================*/

//#define BOOST_SPIRIT_DEBUG
#include "lexer.hpp"
#include <iostream>

using namespace boost::spirit;
using namespace std;

char to_char(wchar_t c)
{
    if (boost::spirit::impl::isgraph_(char(c)) || boost::spirit::impl::isspace_(char(c)))
        return char(c);
    else
        return '.';
}

ostream& operator<<(ostream& out, wchar_t const* c)
{
    while (*c)
        out << to_char(*c++);
    return out;
}

template <typename LexerT, typename CharT>
void test_match(LexerT& lexer, const CharT* str)
{
    int token;
    const CharT* end = str;
    while (*end)
        ++end;

    cout << "testing string: " << str << endl;
    while ((token = lexer.next_token(str, end)) != -1 && str != end)
    {
        cout << "matched token #" << token << endl;
    }
    if (token == -1)
    {
        cout << "lexing failed at: " << str << endl;
    }
    else
    {
        cout << "matched token #" << token << endl;
    }
}

int main(int , char** )
{
    /*
    g.parse("[[:blank:]]+");
    g.parse("[[:blank:]]*");
    g.parse("[^[:blank:]\\n]");
    g.parse("\\r?\\n");
    g.parse("([[:alpha:]_][[:alnum:]_-]*)");
    g.parse("[^[:alpha:]_*\\n]+");
    g.parse("(\\\\([^\\n]|[0-7]{1,3}|x[[:xdigit:]]{1,2}))");
    g.parse("(\"[:\"[[:alpha:]]+\":]\")");
    node* n = g.parse("a*b*c*");
    n = g.parse("a*b*c*|c*b*a*");
    n = g.parse("(a|b)*abb+c?");

    */

    //this is pathological! Generates ~65,000 dfa states
    //dfa_t dfa = g.createdfa("(a|b)*a(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)");
    //
    //g.register_regex("(a|b)*abb+c?(ab(c|d)*)?");
    //g.register_regex("abcd");
    //g.register_regex("dabcd");
    //g.register_regex("abcde");
    //g.register_regex("(abcd)|(dabcd)");
    //g.register_regex("[\\a-zA-\\x5a][a-zA-Z0-9_]*"); // identifier
    //g.register_regex("[0-9]*"); // integer

    lexer<> lex;
    lex.register_regex("(a|b)*abb+c?(ab(c|d)*)?", 99);
    lex.register_regex("[\\a-zA-\\x5a][a-zA-Z0-9_]*", 100); // identifier
    lex.register_regex("[0-9]*", 200); // integer
    test_match(lex, "aaabb");
    test_match(lex, "aaabbd");
    test_match(lex, "aaabbabd");
    test_match(lex, "aaabbde");
    test_match(lex, "baaabcabd");
    test_match(lex, "baaabcabde");
    test_match(lex, "abc123");
    test_match(lex, "1234");
    test_match(lex, "12a45");

    lexer<> lex2;
    lex2.register_regex("do", 1);
    lex2.register_regex("do1", 2);
    lex2.register_regex("do_more", 3);
    lex2.register_regex("_more", 4);
    lex2.register_regex("some_more", 5);
    lex2.register_regex("some_", 6);
    test_match(lex2, "do");
    test_match(lex2, "do1");
    test_match(lex2, "do_more");
    test_match(lex2, "_more");
    test_match(lex2, "some_more");
    test_match(lex2, "some_");
    test_match(lex2, "dodo1");
    test_match(lex2, "do_more_more");
    test_match(lex2, "some_more_moresome__more");

#define WCHAR_TESTS
#ifdef WCHAR_TESTS
    lexer<wchar_t const *> wlex;
    //wg.register_regex(L"(a|b)*abb+c?(ab(c|d)*)?");
    //wg.register_regex(L"d*a*b*c*d*");
    //wg.register_regex(L"[^\x01020304-\x05060708]+");
    //wg.register_regex(L"[^a-z]+");
    wlex.register_regex(L"[a-z]+", 2);

    test_match(wlex, L"aaabb");
    test_match(wlex, L"abc123");
    test_match(wlex, L"1234");
    test_match(wlex, L"12a45");
    test_match(wlex, L"abc");

// only compile this if the compiler has 4-byte wide chars.
#if defined(__GNUC__) && defined(linux)
    lexer<wchar_t const *> wlex2;
    //wlex2.register_regex(L"[^\\x07020304-\\x08060708]*", 1);
    wlex2.register_regex(L"[\\x07020304-\\x08060708]*", 1);

    test_match(wlex2, L"\x01020304");
    test_match(wlex2, L"\x01020303");
    test_match(wlex2, L"\x01020305");
    test_match(wlex2, L"\x02020305");
    test_match(wlex2, L"\x05060707");
    test_match(wlex2, L"\x05060708");
    test_match(wlex2, L"\x05060709");
    test_match(wlex2, L"\x07020303");
    test_match(wlex2, L"\x07020304");
    test_match(wlex2, L"\x07020305");
    test_match(wlex2, L"\x07120305");
    test_match(wlex2, L"\x08060707");
    test_match(wlex2, L"\x08060708");
    test_match(wlex2, L"\x08060709");
#endif
#ifdef BOOST_SPIRIT_DEBUG
    wlex.dump(cout);
#endif

#endif

}





