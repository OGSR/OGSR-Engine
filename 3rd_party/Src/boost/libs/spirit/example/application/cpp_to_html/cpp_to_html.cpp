///////////////////////////////////////////////////////////////////////////////
//
//  C++ Source to HTML converter
//
//  [ JDG 9/01/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/utility/chset.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/confix.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Semantic actions
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void print_char(CharT ch, ostream& out)
{
    switch (ch)
    {
        case '<': out << "&lt;";    break;
        case '>': out << "&gt;";    break;
        case '&': out << "&amp;";   break;
        case '"': out << "&quot;";  break;
        default:  out << ch;        break;
    }
}

///////////////////////////////////////
struct process
{
    process(char const* name_, ostream& out_)
    : name(name_), out(out_) {}

    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        out << "<span class=" << name << ">";
        while (first != last)
            print_char(*first++, out);
        out << "</span>";
    }

    char const* name;
    ostream& out;
};

///////////////////////////////////////
struct unexpected_char
{
    unexpected_char(ostream& out_)
    : out(out_) {}

    template <typename CharT>
    void operator()(CharT) const
    {
        out << '#'; // print out an unexpected character
    }

    ostream& out;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Our C++ to HTML grammar
//
///////////////////////////////////////////////////////////////////////////////
struct cpp_to_html : public grammar<cpp_to_html>
{
    cpp_to_html(ostream& out_)
    : out(out_) {}

    template <typename ScannerT>
    struct definition
    {
        definition(cpp_to_html const& self)
        {
            program
                =
               *(   preprocessor    [process("preprocessor", self.out)]
                |   comment         [process("comment", self.out)]
                |   keyword         [process("keyword", self.out)]
                |   identifier      [process("identifier", self.out)]
                |   special         [process("special", self.out)]
                |   string_         [process("string", self.out)]
                |   literal         [process("literal", self.out)]
                |   number          [process("number", self.out)]
                |   anychar_p       [unexpected_char(self.out)]
                )
                ;

            preprocessor
                =   '#' >> ((alpha_p | '_') >> *(alnum_p | '_'))
                    >> *space_p
                ;

            comment
                =   +((comment_p("//") | comment_p("/*", "*/"))
                    >> *space_p)
                ;

            keyword
                =   keyword_ >> (eps_p - (alnum_p | '_')) >> *space_p
                ;   // make sure we recognize whole words only

            keyword_
                =   "and_eq", "and", "asm", "auto", "bitand", "bitor",
                    "bool", "break", "case", "catch", "char", "class",
                    "compl", "const_cast", "const", "continue", "default",
                    "delete", "do", "double", "dynamic_cast",  "else",
                    "enum", "explicit", "export", "extern", "false",
                    "float", "for", "friend", "goto", "if", "inline",
                    "int", "long", "mutable", "namespace", "new", "not_eq",
                    "not", "operator", "or_eq", "or", "private",
                    "protected", "public", "register", "reinterpret_cast",
                    "return", "short", "signed", "sizeof", "static",
                    "static_cast", "struct", "switch", "template", "this",
                    "throw", "true", "try", "typedef", "typeid",
                    "typename", "union", "unsigned", "using", "virtual",
                    "void", "volatile", "wchar_t", "while", "xor_eq", "xor"
                ;

            special
                =   +(chset_p("~!%^&*()+={[}]:;,<.>?/|\\-") >> *space_p)
                ;

            string_
                =   !as_lower_d['l'] >> confix_p('"', *c_escape_ch_p, '"')
                    >> *space_p
                ;

            literal
                =   !as_lower_d['l'] >> confix_p('\'', *c_escape_ch_p, '\'')
                    >> *space_p
                ;

            number
                =   (   real_p
                    |   as_lower_d["0x"] >> hex_p
                    |   '0' >> oct_p
                    )
                    >>  *as_lower_d[chset_p("ldfu")]
                    >>  *space_p
                ;

            identifier
                =   ((alpha_p | '_') >> *(alnum_p | '_'))
                    >> *space_p
                ;
        }

        rule<ScannerT>      program, preprocessor, comment, special,
                            string_, literal, number, identifier, keyword;
        symbols<>           keyword_;

        rule<ScannerT> const&
        start() const { return program; }
    };

    ostream& out;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a file
//
///////////////////////////////////////////////////////////////////////////////
static void
parse(char const* filename)
{
    ifstream in(filename);

    if (!in)
    {
        cerr << "Could not open input file: " << filename << endl;
        return;
    }

    string out_name(filename);
    out_name += ".html";
    ofstream out(out_name.c_str());

    if (!out)
    {
        cerr << "Could not open output file: " << out_name << endl;
        return;
    }

    in.unsetf(ios::skipws); //  Turn of white space skipping on the stream

    vector<char> vec;
    std::copy(
        istream_iterator<char>(in),
        istream_iterator<char>(),
        std::back_inserter(vec));

    out << "<html>\n" << "<head>\n";
    out << "<!-- Generated by the Spirit (http://spirit.sf.net) ";
    out << "C++ to HTML Converter -->\n";
    out << "<title>\n";
    out << out_name << "</title>\n";
    out << "<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\">";
    out << "</head>\n";
    out << "<body>\n" << "<pre>\n";

    vector<char>::const_iterator first = vec.begin();
    vector<char>::const_iterator last = vec.end();

    cpp_to_html p(out);
    parse_info<vector<char>::const_iterator> info =
        parse(first, last, p);

    if (!info.full)
    {
        cerr << "---PARSING FAILURE---\n";
        cerr << string(info.stop, last);
    }

    out << "</pre>\n" << "</body>\n" << "</html>\n";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            cout << argv[i] << endl;
            parse(argv[i]);
        }
    }
    else
    {
        cerr << "---NO FILENAME GIVEN---" << endl;
    }

    return 0;
}

