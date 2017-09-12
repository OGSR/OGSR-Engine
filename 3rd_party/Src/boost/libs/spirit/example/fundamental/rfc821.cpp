/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Martijn W. van der Lee
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  RFC821-compliant SMTP e-mail address parser
//  ===========================================
//  Martijn W. van der Lee (martijn@v-d-l.com)
//  Ported to Spirit v1.5 [ JDG 9/17/2002 ]
//
//  This is an example for using Spirit to verify RFC821-compliant e-mail
//  addresses.
//
//  Although the author has taken utmost care to check the correctness of this
//  code we can make no guarantees whatsoever. Use at your own risk.
//
//  Please report any corrections or problems with the implementation of RFC821
//  to spirit.sourceforge.net and/or the author and/or the spirit mailing list.
//
//  What this example demonstrates:
//  - Conversion of BNF to Spirit code.
//  - How to use exceptions for implementing semantics.
//
//  --------------------------------------------------- original RFC821 BNF ---
//
//  <reverse-path>  ::= <path>
//  <forward-path>  ::= <path>
//  <path>          ::= "<" [ <a-d-l> ":" ] <mailbox> ">"
//  <a-d-l>         ::= <at-domain> | <at-domain> "," <a-d-l>
//  <at-domain>     ::= "@" <domain>
//  <domain>        ::=  <element> | <element> "." <domain>
//  <element>       ::= <name> | "#" <number> | "[" <dotnum> "]"
//  <mailbox>       ::= <local-part> "@" <domain>
//  <local-part>    ::= <dot-string> | <quoted-string>
//  <name>          ::= <a> <ldh-str> <let-dig>
//  <ldh-str>       ::= <let-dig-hyp> | <let-dig-hyp> <ldh-str>
//  <let-dig>       ::= <a> | <d>
//  <let-dig-hyp>   ::= <a> | <d> | "-"
//  <dot-string>    ::= <string> | <string> "." <dot-string>
//  <string>        ::= <char> | <char> <string>
//  <quoted-string> ::=  """ <qtext> """
//  <qtext>         ::=  "\" <x> | "\" <x> <qtext> | <q> | <q> <qtext>
//  <char>          ::= <c> | "\" <x>
//  <dotnum>        ::= <snum> "." <snum> "." <snum> "." <snum>
//  <number>        ::= <d> | <d> <number>
//  <CRLF>          ::= <CR> <LF>
//  <CR>            ::= the carriage return character (ASCII code 13)
//  <LF>            ::= the line feed character (ASCII code 10)
//  <SP>            ::= the space character (ASCII code 32)
//  <snum>          ::= one, two, or three digits representing a decimal
//                      integer value in the range 0 through 255
//  <a>             ::= any one of the 52 alphabetic characters A through Z
//                      in upper case and a through z in lower case
//  <c>             ::= any one of the 128 ASCII characters, but not any
//                      <special> or <SP>
//  <d>             ::= any one of the ten digits 0 through 9
//  <q>             ::= any one of the 128 ASCII characters except <CR>,
//                      <LF>, quote ("), or backslash (\)
//  <x>             ::= any one of the 128 ASCII characters (no exceptions)
//  <special>       ::= "<" | ">" | "(" | ")" | "[" | "]" | "\" | "."
//                      | "," | ";" | ":" | "@"  """ | the control
//                      characters (ASCII codes 0 through 31 inclusive and
//                      127)
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/loops.hpp>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  My grammar
//
///////////////////////////////////////////////////////////////////////////////
struct rfc821 : public grammar<rfc821>
{
    template <typename ScannerT>
    struct definition
    {
        definition(rfc821 const& /*self*/)
        {
            range<>
                ascii(char(0x01), char(0x7F))
                ;

            e_path
                =   lexeme_d
                    [
                        '<'
                        >>  !(e_a_d_l >> ':')
                        >>  e_mailbox
                        >>  '>'
                    ]
                ;

            e_a_d_l
                =   e_at_domain
                >>  *(',' >> e_at_domain)
                ;

            e_mailbox
                =   e_local_part
                >>  e_at_domain
                ;

            e_at_domain
                =   '@'
                >>  e_domain
                ;

            e_domain
                =   e_element
                >>  *('.' >> e_element)
                ;

            e_element
                =   e_name
                |   '#' >> +digit_p
                |   '[' >> e_dotnum >> ']';

            e_local_part
                =   e_dot_string
                |   e_quoted_string
                ;

            e_dot_string
                =   +e_char
                >>  *('.' >> +e_char)
                ;

            e_quoted_string
                =   '\"'
                >>  e_qtext
                >>  '\"'
                ;

            e_qtext
                =   +(e_q | ('\\' >> ascii));

            // e_name forces domain-name parts to be 2 characters minimum, RFC0821
            //  is a bit unclear about this. Could also be 1 or 3.
            e_name
                =   alpha_p
                >>  +e_alnum_hyp
                ;

            e_char
                =   e_c
                |   ('\\' >> ascii)
                ;

            e_crlf
                =   ch_p('\r')
                >>  '\n'
                ;

            e_c
                =   ascii - e_special - ' '
                ;

            e_q
                =   ascii - '\\' - '\r' - '\n' - '\"'
                ;

            e_alnum_hyp
                =   *ch_p('-')
                >>  alnum_p
                ;

            e_dotnum
                =   e_snum
                >>  repeat_p(3)['.' >> e_snum]
                ;

            uint_parser<unsigned, 10, 1, 3> uint3_p;    // 3 digit unsigned

            e_snum
                =   max_limit_d(255u)[uint3_p];         // 0-255!

            e_special
                = ch_p('<') | '>' | '(' | ')' | '[' | ']'
                | '\\' | '.' | ',' | ';' | ':' | '@' | '\"'
                | cntrl_p;
        }

        rule<ScannerT>
            e_path, e_a_d_l, e_mailbox, e_at_domain, e_domain, e_element,
            e_local_part, e_dot_string, e_quoted_string, e_qtext, e_name,
            e_char, e_crlf, e_c, e_q, e_alnum_hyp, e_dotnum, e_snum,
            e_special
            ;

        rule<ScannerT> const&
        start() const { return e_path; }
    };
};

///////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    struct check
    {
        char address[32];
        bool prediction;
    };

    check checks[] =
    {
    //   12345678901234567890123456789012
        {"<user@server.com>", true},
        {"<user\\@@server.com>", true},
        {"<user\\\\@server.com>", true},
        {"<@serv2.com:user@server.com>", true},
        {"<user@#1234>", true},
        {"<user@mail.[12.34.56.255]>", true},
        {"<user@[255.0.0.0].#123.com>", true},
        {"<\"user name\"@server.com>", true},
        {"diddledoo", false},
        {"<polka@dot...dot>", false}
    };

    // Header
    cout    << "E-mail validation based on RFC821" << endl
            << "---------------------------------" << endl;

    // Tests
    int failed = 0;
    unsigned i;
    rfc821 g;

    for (i = 0; i < sizeof(checks) / sizeof(check); ++i)
    {
        //  This parser is an implicit lexeme. Passing in a
        //  space_p or any skipper will not work unless the
        //  grammar is explicitly wrapped inside a lexeme_d

        const bool success = parse(checks[i].address, g).full;

        cout    << "Test " << (int)i
                << " \"" << checks[i].address << "\" should "
                << (checks[i].prediction? "" : "not") << " match. Result: "
                << (success == checks[i].prediction? "success" : "failure")
                << endl;

        if (success != checks[i].prediction)
            ++failed;
    }

    // Footer
    cout << "---------------------------------" << endl;
    cout << failed << " test failed";

    return 0;
}


