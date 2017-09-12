/*=============================================================================
    simplexml.cpp

    Spirit V1.3
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

#include <boost/spirit.hpp>
#include <iostream>

using namespace boost::spirit;
using namespace std;

// JDG 2-14-2003. Made Parse a template to make Borland happy.
template <typename RuleT, typename InputT>
static void
Parse(RuleT const& rule, InputT expr)
{
    cout << "/////////////////////////////////////////////////////////\n\n";

    parse_info<char const*> result = parse(expr, rule);

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

typedef chset<unsigned char> chset_t;
typedef chlit<unsigned char> chlit_t;

int
main()
{

    // Start grammar definition
    rule<>    prolog, element, Misc, Reference,
        CData, doctypedecl, XMLDecl, SDDecl, VersionInfo, EncodingDecl,
        VersionNum, Eq, EmptyElemTag, STag, content, ETag, Attribute,
        AttValue, CharData, Comment, CDSect,
        CharRef, EntityRef, EncName, document,
        Name, Comment1, S;

    // XML Character classes
    chset_t Char("\x9\xA\xD\x20-\xFF");
    chset_t Letter("\x41-\x5A\x61-\x7A\xC0-\xD6\xD8-\xF6\xF8-\xFF");
    chset_t Digit("0-9");
    chset_t Extender('\xB7');
    chset_t NameChar = Letter | Digit | chset_t("._:-") | Extender;
    chset_t Sch("\x20\x9\xD\xA");

    // This is the start rule for XML
    document =
        prolog >> element >> *Misc
    ;

    S =
        +(Sch)
    ;

    Name =
        (Letter | '_' | ':')
        >> *(NameChar)
    ;

    AttValue =
            '"'
            >> *(
                    (anychar_p - (chset_t('<') | '&' | '"'))
                  | Reference
                )
            >> '"'
        |   '\''
            >> *(
                    (anychar_p - (chset_t('<') | '&' | '\''))
                  | Reference
                )
            >> '\''
    ;

    chset_t CharDataChar(anychar_p - (chset_t('<') | chset_t('&')));

    CharData =
        *(CharDataChar - str_p("]]>"))
    ;

    Comment1 =
        *(
              (Char - ch_p('-'))
            | (ch_p('-') >> (Char - ch_p('-')))
         )
    ;

    Comment =
        str_p("<!--") >> Comment1 >> str_p("-->")
    ;

    CDSect =
        str_p("<![CDATA[") >> CData >> str_p("]]>")
    ;

    CData =
        *(Char - str_p("]]>"))
    ;

    prolog =
        !XMLDecl >> *Misc >> !(doctypedecl >> *Misc)
    ;

    XMLDecl =
           str_p("<?xml")
        >> VersionInfo
        >> !EncodingDecl
        >> !SDDecl
        >> !S
        >> str_p("?>")
    ;

    VersionInfo =
           S
        >> str_p("version")
        >> Eq
        >> (
                ch_p('\'') >> VersionNum >> '\''
            |   ch_p('"')  >> VersionNum >> '"'
           )
    ;

    Eq =
        !S >> '=' >> !S
    ;

    chset_t VersionNumCh("A-Za-z0-9_.:-");

    VersionNum =
        +(VersionNumCh)
    ;

    Misc =
        Comment | S
    ;

    doctypedecl =
           str_p("<!DOCTYPE")
        >> *(Char - (chset_t('[') | '>'))
        >> !('[' >> *(Char - ']') >> ']')
        >> '>'
    ;

    SDDecl =
           S
        >> str_p("standalone")
        >> Eq
        >> (
                (ch_p('\'') >> (str_p("yes") | str_p("no")) >> '\'')
             |  (ch_p('"')  >> (str_p("yes") | str_p("no")) >> '"')
           )
    ;

    element =
            EmptyElemTag
        |   STag >> content >> ETag
    ;

    STag =
           '<'
        >> Name
        >> *(S >> Attribute)
        >> !S
        >> '>'
    ;

    Attribute =
        Name >> Eq >> AttValue
    ;

    ETag =
        str_p("</") >> Name >> !S >> '>'
    ;

    content =
        !CharData
        >> *(
              (
                 element
                 | Reference
                 | CDSect
                 | Comment
              )
              >> !CharData
            )
    ;

    EmptyElemTag =
           '<'
        >> Name
        >> *(S >> Attribute)
        >> !S
        >> str_p("/>")
    ;

    CharRef =
          str_p("&#") >> +digit_p >> ';'
        | str_p("&#x") >> +xdigit_p >> ';'
    ;

    Reference =
          EntityRef
        | CharRef
    ;

    EntityRef =
        '&' >> Name >> ';'
    ;

    EncodingDecl =
           S
        >> str_p("encoding")
        >> Eq
        >> (   ch_p('"')  >> EncName >> '"'
             | ch_p('\'') >> EncName >> '\''
           )
    ;

    chset_t EncNameCh = VersionNumCh - chset_t(':');

    EncName =
        alpha_p >> *(EncNameCh)
    ;





    cout << "The following should fail parsing:\n";

    Parse(document, "<!-- declarations for <head> & <body> -->");

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting ["
                "<!ELEMENT greeting (#PCDATA)>"
            "]>"
            "<greeting>Hello, world!</greeting>"
            "<!-- B+, B, or B--->");

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
            "<greetingHello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\"? >"
            " <greeting>Hello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
            "<![CDATA[<greeting>Hello, world!</greeting>]]>");

    cout << "The following should parse OK:\n";

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
            "<greeting>Hello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting [<!ELEMENT greeting (#PCDATA)>]>"
            "<greeting>Hello, world!</greeting>"
            "<!-- declarations for <head> & <body> -->");

    Parse(CDSect,
            "<![CDATA[<greeting>Hello, world!</greeting>]]>");

    Parse(document,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<!DOCTYPE greeting ["
                "<!ELEMENT greeting (#PCDATA)>"
            "]>"
            "<greeting>Hello, world!"
                "<![CDATA[<greeting>Hello, world!</greeting>]]>"
            "</greeting>");

    Parse(document,
            "<?xml version=\"1.0\"?>"
            " <greeting>Hello, world!</greeting>");

    Parse(document, "<greeting>Hello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\"?> "
            "<!DOCTYPE greeting SYSTEM \"hello.dtd\"> "
            "<greeting>Hello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\" standalone='yes'?> "
            "<greeting>Hello, world!</greeting>");

    Parse(document,
            "<?xml version=\"1.0\" standalone='yes'?> "
            "<!DOCTYPE greeting SYSTEM \"hello.dtd\"> "
            "<greeting>Hello, world!</greeting>");

    Parse(document,
                        "<?xml version=\"1.0\" encoding=\"ISO8859-1\" ?>"
                            "<note>"
                                  "<to>Tove</to>"
                                "<from>Jani</from>"
                                "<heading>Reminder</heading>"
                                "<body>Don't forget me this weekend!</body>"
                            "</note>" );

    Parse(document,
                       "<?xml version=\"1.0\"?>"
                            "<!DOCTYPE note ["
                                "<!ELEMENT note    (to,from,heading,body)>"
                                "<!ELEMENT to      (#PCDATA)>"
                                "<!ELEMENT from    (#PCDATA)>"
                                "<!ELEMENT heading (#PCDATA)>"
                                "<!ELEMENT body    (#PCDATA)>"
                            "]>"
                            "<note>"
                                "<to>Tove</to>"
                                "<from>Jani</from>"
                                "<heading>Reminder</heading>"
                                 "<body>Don't forget me this weekend!</body>"
                             "</note>");

    return 0;
}
