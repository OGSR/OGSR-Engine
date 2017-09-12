#include "rfc-skipper.hpp"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
using namespace boost::spirit;

int main(int , char** )
    {
    // Strip brain-dead RFC style input down to the bones using
    // rfc_skipper.

    char weird[] =                                      \
        "From: (Some \r\n"                              \
        "       comment) simons (stuff) \r\n"           \
        "        @      computer (inserted) . (between) org(tokens)";
    string output;
    parse(weird, (    str_p("From:")
                      >> *( anychar_p [append(output)] )
                 ),
          rfc_skipper_p);
    cout << "Stripped address is: '" << output << "'" << endl;
    assert(output == "simons@computer.org");

    return 0;
    }
