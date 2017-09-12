#ifndef INCLUDED_RFC_SKIPPER_HPP
#define INCLUDED_RFC_SKIPPER_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/chset.hpp>

struct rfc_skipper : public boost::spirit::grammar<rfc_skipper>
    {
    rfc_skipper()
        {
        }
    template<typename scannerT>
    struct definition
        {
        definition(const rfc_skipper& /*self*/)
            {
            using namespace boost::spirit;

            first = lexeme_d
                    [
                        junk    = lwsp | comment,

                        lwsp    = +(    !str_p("\r\n")
                                        >> chset_p(" \t")
                                   ),

                        comment =  ch_p('(')
                                   >>  *(   lwsp
                                        |   ctext
                                        |   qpair
                                        |   comment
                                        )
                                   >> ')',

                        ctext   =  anychar_p - chset_p("()\\\r"),

                        qpair   =  ch_p('\\') >> anychar_p
                    ];
            }
        const boost::spirit::rule<scannerT>& start() const
            {
            return first;
            }
        boost::spirit::subrule<0>     junk;
        boost::spirit::subrule<1>     lwsp;
        boost::spirit::subrule<2>     comment;
        boost::spirit::subrule<3>     ctext;
        boost::spirit::subrule<4>     qpair;
        boost::spirit::rule<scannerT> first;
        };
    };
const rfc_skipper rfc_skipper_p;

#endif
