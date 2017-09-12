/*=============================================================================
    Lexer test with callbacks

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
#include <boost/function.hpp>
#include <iostream>

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

void lex_callback(const char* const& tok_begin, const char*& tok_end,
        const char* const& /*last*/, const int& token, lexer_control<int>&)
{
    cout << "matched token #" << token << " token = " << std::string(tok_begin, tok_end) << endl;

}

void ignore_callback(const char* const& tok_begin, const char*& tok_end,
        const char* const& /*last*/, const int& token, lexer_control<int>& ctl)
{
    cout << "ignoring token #" << token << " token = " << std::string(tok_begin, tok_end) << endl;

    ctl.ignore_current_token();
}

void change_callback(const char* const& tok_begin, const char*& tok_end,
        const char* const& /*last*/, const int& token, lexer_control<int>& ctl)
{
    cout << "changing token #" << token << " to 150. token = " <<
        std::string(tok_begin, tok_end) << endl;

    ctl.set_token(150);
}

void term_callback(const char* const& tok_begin, const char*& tok_end,
        const char* const& /*last*/, const int& token, lexer_control<int>& ctl)
{
    cout << "matched token #" << token << " token = " <<
        std::string(tok_begin, tok_end) << endl;
    cout << "terminating lexer" << endl;

    ctl.terminate();
}

struct term_callback_ftr
{
    void operator()(const char* const& tok_begin, const char*& tok_end,
        const char* const& /*last*/, const int& token, lexer_control<int>& ctl)
    {
        cout << "matched token #" << token << " token = " <<
            std::string(tok_begin, tok_end) << endl;
        cout << "terminating lexer" << endl;

        ctl.terminate();
    }
};

int main(int , char** )
{

    {
        lexer<const char*> lex;
        lex.register_regex("(a|b)*abb+c?(ab(c|d)*)?", 99, &lex_callback);
        lex.register_regex("[\\a-zA-\\x5a][a-zA-Z0-9_]*", 100, &lex_callback); // identifier
        lex.register_regex("1zz", 101, &change_callback);
        lex.register_regex("2zz", 102, &term_callback);
        lex.register_regex("[0-9]{3}", 300, &ignore_callback); // integer
        lex.register_regex("[0-9]*", 200, &lex_callback); // integer
        test_match(lex, "aaabb");
        test_match(lex, "aaabbd");
        test_match(lex, "aaabbabd");
        test_match(lex, "aaabbde");
        test_match(lex, "baaabcabd");
        test_match(lex, "baaabcabde");
        test_match(lex, "abc123");
        test_match(lex, "123abc123");
        test_match(lex, "1234");
        test_match(lex, "12a45");
        test_match(lex, "1zz");
        test_match(lex, "2zzbdef23");
    }

    {
        typedef boost::function5<void,
                        const char* const&,
                        const char*&,
                        const char* const&,
                        const int&,
                        lexer_control<int>&> func_callback_t;

        func_callback_t func_lex_cb = &lex_callback;
        func_callback_t func_ignore_cb = &ignore_callback;
        func_callback_t func_change_cb = &change_callback;
        func_callback_t func_term_cb = term_callback_ftr();

        lexer<const char*, int, func_callback_t> lex;
        lex.register_regex("(a|b)*abb+c?(ab(c|d)*)?", 99, &lex_callback);
        lex.register_regex("[\\a-zA-\\x5a][a-zA-Z0-9_]*", 100, func_lex_cb); // identifier
        lex.register_regex("1zz", 101, func_change_cb);
        lex.register_regex("2zz", 102, func_term_cb);
        lex.register_regex("[0-9]{3}", 300, func_ignore_cb); // integer
        lex.register_regex("[0-9]*", 200, func_lex_cb); // integer
        test_match(lex, "aaabb");
        test_match(lex, "aaabbd");
        test_match(lex, "aaabbabd");
        test_match(lex, "aaabbde");
        test_match(lex, "baaabcabd");
        test_match(lex, "baaabcabde");
        test_match(lex, "abc123");
        test_match(lex, "123abc123");
        test_match(lex, "1234");
        test_match(lex, "12a45");
        test_match(lex, "1zz");
        test_match(lex, "2zzbdef23");
    }

    lexer<const char*> lex2;
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

}





