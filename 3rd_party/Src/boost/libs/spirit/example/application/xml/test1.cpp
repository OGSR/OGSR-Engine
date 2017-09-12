/*=============================================================================
    xml.hpp

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
#define NDEBUG // disable asserts

// Test out vector approach

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>

#include <vector>
#include <iostream>
#include <fstream>

#include "xml_grammar.hpp"

using namespace boost::spirit;
using namespace std;

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

    vector<unsigned char> temp = vector<unsigned char>(
            istreambuf_iterator<char>(in.rdbuf()),
            istreambuf_iterator<char>());

    typedef vector<unsigned char>::const_iterator iterator_t;

    parse_info<iterator_t> pi = parse(temp.begin(), temp.end(), rule);

    if (pi.full)
    {
        cout << "\t\t" << filename << " Parses OK\n\n\n";
    }
    else
    {
        cout << "\t\t" << filename << " Fails Parsing\n" << endl;
        for (int i = 0; i < 50; ++i)
        {
            if (pi.stop == temp.end())
                break;
            cout << char(*pi.stop++);
        }
        cout << endl;
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
        cout << "Error: No filename given" << endl;
        return 1;
    }

    return 0;
}
