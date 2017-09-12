/*=============================================================================
    xml.cpp

    Spirit V1.2
    URL: http://spirit.sourceforge.net/

    Copyright (c) 2001, Daniel C. Nuffer

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the copyright holder be held liable for
    any damages arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute
    it freely, subject to the following restrictions:

    1.  The origin of this software must not be misrepresented; you must
        not claim that you wrote the original software. If you use this
        software in a product, an acknowledgment in the product documentation
        would be appreciated but is not required.

    2.  Altered source versions must be plainly marked as such, and must
        not be misrepresented as being the original software.

    3.  This notice may not be removed or altered from any source
        distribution.
=============================================================================*/

//#define BOOST_SPIRIT_DEBUG  ///$$$ DEFINE THIS WHEN DEBUGGING $$$///

#include "xml_grammar.hpp"

#include <vector>
#include <iostream>
#include <fstream>

using namespace boost::spirit;
using namespace std;

/* This is provided so that we can *see* the debug output */
ostream& operator<<(ostream& out, wchar_t c)
{
    out << char(c);
    return out;
}

ostream& operator<<(ostream& out, wchar_t const* c)
{
    while (*c)
        out << char(*c++);
    return out;
}


template <typename RuleT>
static void ParseFileWithMultiPass(RuleT const& rule, char const* filename)
{
    cout << "////////////// Parsing with multi_pass //////////////////\n\n";

    ifstream in(filename);
    if (!in)
    {
        cout << "Could not open file: " << filename << endl;
        return;
    }

    // Turn of white space skipping on the stream
    in.unsetf(ios::skipws);

    typedef unsigned char char_t;
    typedef multi_pass<istream_iterator<char_t> > iterator_t;
    iterator_t first = make_multi_pass(istream_iterator<char_t>(in));
    iterator_t last = make_multi_pass(istream_iterator<char_t>());
    scanner<iterator_t> scan(first, last);

    match<> m = rule.parse(scan);

    if (m && first == last)
    {
        cout << "\t\t" << filename << " Parses OK\n\n\n";
    }
    else
    {
      cout << "\t\t" << filename << " Fails Parsing\n" << endl;
      for (int i = 0; i < 50; ++i)
      {
          cout << *first++;
      }
      cout << endl;
    }
}

template <typename RuleT>
static void ParseFile(RuleT const& rule, char const* filename)
{
    cout << "////////////// Parsing with std::vector /////////////////\n\n";

    ifstream in(filename);
    if (!in)
    {
        cout << "Could not open file: " << filename << endl;
        return;
    }

    // Turn of white space skipping on the stream
    in.unsetf(ios::skipws);

    vector<unsigned char> temp = vector<unsigned char>(
            istream_iterator<unsigned char>(in),
            istream_iterator<unsigned char>());

    // This is a rather pathetic attempt to work around gcc 2.95.x's lacking
    // library, which doesn't have wifstream.  The correct thing to do here
    // would be to translate the file into wide characters, but for now it
    // works just fine with ASCII files.
    vector<wchar_t> data = vector<wchar_t>(temp.begin(),
            temp.end());

    typedef vector<wchar_t>::const_iterator iterator_t;

    parse_info<iterator_t> pi = parse(data.begin(), data.end(), rule);

    if (pi.full)
    {
        cout << "\t\t" << filename << " Parses OK\n\n\n";
    }
    else
    {
        cout << "\t\t" << filename << " Fails Parsing\n" << endl;
        for (int i = 0; i < 50; ++i)
        {
            if (pi.stop == data.end())
                break;
            cout << char(*pi.stop++);
        }
        cout << endl;
    }
}

template <typename RuleT>
static void Parse(RuleT const& rule, wchar_t const *expr)
{
    cout << "/////////////////////////////////////////////////////////\n\n";

    typedef wchar_t const* iterator_t;

    parse_info<iterator_t> result = parse(expr, rule);

    if (result.full)
    {
        cout << "\t\t" << expr << " Parses OK\n\n\n";
    }
    else
    {
        cout << "\t\t" << expr << " Fails Parsing\n";
        cout << "\t\t";
        for (int i = 0; i < (result.stop - expr); i++)
            cout << " ";
        cout << "^--Here\n\n\n";
    }
}


int main(int argc, char* argv[])
{
    xml_grammar g;
    if (argc == 2)
    {
        ParseFile(g, argv[1]);
        // commented out to save compilation time.
        //ParseFileWithMultiPass(g, argv[1]);
    }
    /*
    else
    {
        cout << "The following should fail parsing:\n";

        Parse(g, L"<!-- declarations for <head> & <body> -->");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting ["
                L"<!ELEMENT greeting (#PCDATA)>"
                L"]>"
                L"<greeting>Hello, world!</greeting>"
                L"<!-- B+, B, or B--->");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
                L"<greetingHello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\"? >"
                L" <greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
                L"<![CDATA[<greeting>Hello, world!</greeting>]]>");

        cout << "The following should parse OK:\n";

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
                L"<greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
                L"<greeting>Hello, world!</greeting>"
                L"<!-- declarations for <head> & <body> -->");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                L"<!DOCTYPE greeting ["
                L"<!ELEMENT greeting (#PCDATA)>"
                L"]>"
                L"<greeting>Hello, world!"
                L"<![CDATA[<greeting>Hello, world!</greeting>]]>"
                L"</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\"?>"
                L" <greeting>Hello, world!</greeting>");

        Parse(g, L"<greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\"?> "
                L"<!DOCTYPE greeting SYSTEM \"hello.dtd\"> "
                L"<greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\" standalone='yes'?> "
                L"<greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\" standalone='yes'?> "
                L"<!DOCTYPE greeting SYSTEM \"hello.dtd\"> "
                L"<greeting>Hello, world!</greeting>");

        Parse(g,
                L"<?xml version=\"1.0\" encoding=\"ISO8859-1\" ?>"
                L"<note>"
                L"<to>Tove</to>"
                L"<from>Jani</from>"
                L"<heading>Reminder</heading>"
                L"<body>Don't forget me this weekend!</body>"
                L"</note>" );

        Parse(g,
                L"<?xml version=\"1.0\"?>"
                L"<!DOCTYPE note ["
                L"<!ELEMENT note    (to,from,heading,body)>"
                L"<!ELEMENT to      (#PCDATA)>"
                L"<!ELEMENT from    (#PCDATA)>"
                L"<!ELEMENT heading (#PCDATA)>"
                L"<!ELEMENT body    (#PCDATA)>"
                L"]>"
                L"<note>"
                L"<to>Tove</to>"
                L"<from>Jani</from>"
                L"<heading>Reminder</heading>"
                L"<body>Don't forget me this weekend!</body>"
                L"</note>");

    }
*/

    return 0;
}
