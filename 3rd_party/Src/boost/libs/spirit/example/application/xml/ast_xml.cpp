/*=============================================================================
    parse_tree_xml.hpp

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

/*
 * This example will parse an XML file into an ast.  Then it will print out
 * the xml using the tree to cout.
 */


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

typedef vector<wchar_t>::iterator iterator_t;
typedef tree_match<iterator_t, node_iter_data_factory<> > 
            parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator node_iter_t;
typedef ast_match_policy<iterator_t, node_iter_data_factory<> > match_policy_t;
typedef scanner<iterator_t, scanner_policies<iteration_policy, match_policy_t> > scanner_t;
typedef rule<scanner_t> rule_t;

void print_tree(parse_tree_match_t hit);
void print_node(node_iter_t const& i);

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

    iterator_t first = data.begin();
    iterator_t last = data.end();
    scanner_t scan(first, last);
    parse_tree_match_t hit = rule.parse(scan);

    if (hit && first == last)
    {
        cout << "\t\t" << filename << " Parses OK\n\n\n";

        print_tree(hit);
    }
    else
    {
        cout << "\t\t" << filename << " Fails Parsing\n" << endl;
        for (int i = 0; i < 50; ++i)
        {
            if (first == last)
                break;
            cout << char(*first++);
        }
        cout << endl;
    }
}

void print_tree(parse_tree_match_t hit)
{
    print_node(hit.trees.begin());
}

void print_node(node_iter_t const& i)
{
    if (i->children.begin() == i->children.end()) // terminal node
    {
        std::copy(i->value.begin(), i->value.end(), 
                ostream_iterator<char>(std::cout));
    }
    else
    {
        for(node_iter_t chi = i->children.begin();
                chi != i->children.end(); ++chi)
        {
            print_node(chi);
        }

    }
}


int main(int argc, char* argv[])
{
    xml_grammar g;
    if (argc == 2)
    {
        ParseFile(g, argv[1]);
    }
    else
    {
        cout << "Error: no filename given\n";
        return 1;
    }

    return 0;
}
