/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_QUICKDOC_HPP)
#define BOOST_SPIRIT_QUICKDOC_HPP

///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/utility/chset.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/confix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace quickdoc
{
    using namespace boost::spirit;

    ///////////////////////////////////////////////////////////////////////////
    //
    //  quickdoc grammar
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ActionsT>
    struct quickdoc_grammar
    : public grammar<quickdoc_grammar<ActionsT> >
    {
        quickdoc_grammar(ActionsT& actions_)
        : actions(actions_) {}

        template <typename ScannerT>
        struct definition
        {
            definition(quickdoc_grammar const& self)
            {
                document =
                    doc_info >> blocks >> space
                    ;

                blocks =
                   +(   block_markup
                    |   code
                    |   unordered_list              [self.actions.unordered_list]
                    |   ordered_list                [self.actions.ordered_list]
                    |   hr                          [self.actions.hr]
                    |   comment >> *eol_p
                    |   paragraph                   [self.actions.paragraph]
                    |   eol_p
                    )
                    ;

                space =
                    *(blank_p | comment)
                    ;

                comment =
                    "[/" >> *(anychar_p - ']') >> ']'
                    ;

                doc_info =
                        *(space_p | comment)
                    >> "[doc" >> space
                    >> (*(anychar_p - ']'))         [self.actions.doc_title]
                    >> ']' >> +eol_p
                    ;

                hr =
                    str_p("----")
                    >> *(anychar_p - eol_p)
                    >> +eol_p
                    ;

                block_markup =
                        '['
                    >>  (   page
                        |   headings
                        |   blurb
                        |   blockquote
                        |   preformatted
                        |   def_macro
                        |   table
                        )
                    >>  ']'
                    >> +eol_p
                    ;

                page =
                        "page"
                    >>  !(':' >> uint_p             [self.actions.page_level])
                    >>  space
                    >>  (*(anychar_p - ']'))        [self.actions.page]
                    ;

                headings =
                    h1 | h2 | h3 | h4 | h5 | h6
                    ;

                h1 = "h1" >> space >> phrase        [self.actions.h1];
                h2 = "h2" >> space >> phrase        [self.actions.h2];
                h3 = "h3" >> space >> phrase        [self.actions.h3];
                h4 = "h4" >> space >> phrase        [self.actions.h4];
                h5 = "h5" >> space >> phrase        [self.actions.h5];
                h6 = "h6" >> space >> phrase        [self.actions.h6];

                blurb =
                    "blurb" >> space
                    >> phrase                       [self.actions.blurb]
                    ;

                blockquote =
                    ':' >> space >>
                    phrase                          [self.actions.blockquote]
                    ;

                preformatted =
                    "pre" >> space >>
                    phrase                          [self.actions.preformatted]
                    ;

                def_macro =
                    "def" >> space >> identifier    [self.actions.identifier]
                     >> space >> phrase             [self.actions.macro_def]
                    ;

                table =
                    "table" >> space
                    >>  (*(anychar_p - eol_p))      [self.actions.table_title]
                    >>  +eol_p                      [self.actions.start_row]
                    >> *(   table_row
                            >> +eol_p               [self.actions.end_row]
                        )
                    >>  eps_p                       [self.actions.table]
                    ;

                table_row =
                   *(       space
                        >>  ch_p('[')               [self.actions.start_cell]
                        >>  phrase
                        >>  ch_p(']')               [self.actions.end_cell]
                        >>  space
                    )
                    ;

                identifier =
                    *(anychar_p - (space_p | ']'))
                    ;

                code =
                    (
                        code_line
                        >> *(*eol_p >> code_line)
                    )                               [self.actions.code]
                    >> +eol_p
                    ;

                code_line =
                    ((ch_p(' ') | '\t'))
                    >> *(anychar_p - eol_p) >> eol_p
                    ;

                unordered_list =
                    +('*' >> space >> line)         [self.actions.list_item]
                    ;

                ordered_list =
                    +('#' >> space >> line)         [self.actions.list_item]
                    ;

                common =
                        self.actions.macro          [self.actions.do_macro]
                    |   phrase_markup
                    |   escape
                    |   comment
                    ;

                line =
                   *(   common
                    |   (anychar_p - eol_p)         [self.actions.plain_char]
                    )
                    >> +eol_p
                    ;

                paragraph =
                   *(   common
                    |   (   anychar_p
                            - (eol_p >> eol_p)
                        )                           [self.actions.plain_char]
                    )
                    >> +eol_p
                    ;

                phrase =
                   *(   common
                    |   comment
                    |   (anychar_p - ']')           [self.actions.plain_char]
                    )
                    ;

                phrase_markup =
                        '['
                    >>  (   image
                        |   link
                        |   bold
                        |   italic
                        |   underline
                        |   teletype
                        |   str_p("br")             [self.actions.break_]
                        |   unexpected              [self.actions.unexpected]
                        )
                    >>  ']'
                    ;

                escape =
                        (
                            "'''"
                        >>  *(anychar_p - "'''")    [self.actions.plain_char]
                        >>  "'''"
                        )
                    |   (
                            "\"'"
                        >>  *(anychar_p - "\"'")    [self.actions.plain_char]
                        >>  "\"'"
                        )
                    ;

                image =
                        '$' >> space
                    >> (*(anychar_p - ']'))         [self.actions.image]
                    ;

                link =
                        '@'
                    >>  (*(anychar_p - space))      [self.actions.link_pre]
                    >>  space >> phrase             [self.actions.link_post]
                    ;

                bold =
                        ch_p('*')                   [self.actions.bold_pre]
                    >>  space >> phrase             [self.actions.bold_post]
                    ;

                italic =
                        ch_p('\'')                  [self.actions.italic_pre]
                    >>  space >> phrase             [self.actions.italic_post]
                    ;

                underline =
                        ch_p('_')                   [self.actions.underline_pre]
                    >>  space >> phrase             [self.actions.underline_post]
                    ;

                teletype =
                        ch_p('^')                   [self.actions.teletype_pre]
                    >>  space >> phrase             [self.actions.teletype_post]
                    ;

                unexpected =
                    (*(anychar_p - ']'))
                    ;
           }

            rule<ScannerT>  document, blocks, doc_info, block_markup, code,
                            code_line, paragraph, space, comment, headings,
                            page, h1, h2, h3, h4, h5, h6, hr, blurb, blockquote,
                            phrase, phrase_markup, image, unordered_list,
                            ordered_list, bold, italic, underline, teletype,
                            escape, def_macro, identifier, link, table, table_row,
                            unexpected, preformatted, line, common
                            ;

            rule<ScannerT> const&
            start() const { return document; }
        };

        ActionsT&   actions;
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  code to html grammar
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ProcessT, typename UnexpectedT, typename OutT>
    struct code_to_html
    : public grammar<code_to_html<ProcessT, UnexpectedT, OutT> >
    {
        code_to_html(OutT& out_)
        : out(out_) {}

        template <typename ScannerT>
        struct definition
        {
            definition(code_to_html const& self)
            {
                program
                    =
                   *(   preprocessor    [ProcessT("preprocessor", self.out)]
                    |   comment         [ProcessT("comment", self.out)]
                    |   keyword         [ProcessT("keyword", self.out)]
                    |   identifier      [ProcessT("identifier", self.out)]
                    |   special         [ProcessT("special", self.out)]
                    |   string_         [ProcessT("string", self.out)]
                    |   literal         [ProcessT("literal", self.out)]
                    |   number          [ProcessT("number", self.out)]
                    |   anychar_p       [UnexpectedT(self.out)]
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

            rule<ScannerT>  program, preprocessor, comment, special,
                            string_, literal, number, identifier, keyword;
            symbols<>       keyword_;

            rule<ScannerT> const&
            start() const { return program; }
        };

        OutT& out;
    };
}

#endif // BOOST_SPIRIT_QUICKDOC_HPP

