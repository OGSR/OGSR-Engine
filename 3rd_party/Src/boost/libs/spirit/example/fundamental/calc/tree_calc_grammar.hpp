/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Daniel Nuffer
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_TREE_CALC_GRAMMAR_HPP_
#define BOOST_SPIRIT_TREE_CALC_GRAMMAR_HPP_

using namespace boost::spirit;

////////////////////////////////////////////////////////////////////////////
//
//  Our calculator grammar
//
////////////////////////////////////////////////////////////////////////////
struct calculator : public grammar<calculator>
{
    static const int integerID = 1;
    static const int factorID = 2;
    static const int termID = 3;
    static const int expressionID = 4;

    template <typename ScannerT>
    struct definition
    {
        definition(calculator const& /*self*/)
        {
            //  Start grammar definition
            integer     =   leaf_node_d[ lexeme_d[
                                (!ch_p('-') >> +digit_p)
                            ] ];

            factor      =   integer
                        |   inner_node_d[ch_p('(') >> expression >> ch_p(')')]
                        |   (root_node_d[ch_p('-')] >> factor);

            term        =   factor >>
                            *(  (root_node_d[ch_p('*')] >> factor)
                              | (root_node_d[ch_p('/')] >> factor)
                            );

            expression  =   term >>
                            *(  (root_node_d[ch_p('+')] >> term)
                              | (root_node_d[ch_p('-')] >> term)
                            );
            //  End grammar definition

            // turn on the debugging info.
            BOOST_SPIRIT_DEBUG_RULE(integer);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(term);
            BOOST_SPIRIT_DEBUG_RULE(expression);
        }

        rule<ScannerT, parser_context, parser_tag<expressionID> >   expression;
        rule<ScannerT, parser_context, parser_tag<termID> >         term;
        rule<ScannerT, parser_context, parser_tag<factorID> >       factor;
        rule<ScannerT, parser_context, parser_tag<integerID> >      integer;

        rule<ScannerT, parser_context, parser_tag<expressionID> > const&
        start() const { return expression; }
    };
};

#endif

