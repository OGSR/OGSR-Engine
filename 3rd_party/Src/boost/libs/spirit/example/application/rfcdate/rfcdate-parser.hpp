#ifndef INCLUDED_RFCDATE_PARSER_HPP
#define INCLUDED_RFCDATE_PARSER_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include "boost/spirit/attribute.hpp"
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/binders.hpp>
#include "timestamp.hpp"

struct month_parser : boost::spirit::symbols<int>
    {
    month_parser()
        {
        add ("jan", 0)("feb", 1)("mar", 2)("apr", 3)
            ("may", 4)("jun", 5)("jul", 6)("aug", 7)
            ("sep", 8)("oct", 9)("nov", 10)("dec", 11);
        }
    };
const month_parser month_p;

struct wday_parser : boost::spirit::symbols<int>
    {
    wday_parser()
        {
        add ("sun", 0)("mon", 1)("tue", 2)("wed", 3)
            ("thu", 4)("fri", 5)("sat", 6);
        }
    };
const wday_parser wday_p;

struct timezone_parser : boost::spirit::symbols<int>
    {
    timezone_parser()
        {
        add ("ut", 0)("gmt", 0)
            ("est", -18000)("edt", -14400)
            ("cst", -21600)("cdt", -18000)
            ("mst", -25200)("mdt", -21600)
            ("pst", -28800)("pdt", -25200)
            ("z", 0)("a", -3600)("m", -43200)("n", +3600)("y", +43200);
        }
    };
const timezone_parser timezone_p;

struct rfcdate_closure : boost::spirit::closure<rfcdate_closure, timestamp>
    {
    member1 val;
    };

struct rfcdate_parser : public boost::spirit::grammar<rfcdate_parser, rfcdate_closure::context_t>
    {
    rfcdate_parser()
        {
        }
    template<typename scannerT>
    struct definition
        {
        definition(const rfcdate_parser& self)
            {
            using namespace boost::spirit;
            using namespace phoenix;

#define ASSIGN(member, what) bind(&timestamp::member)(self.val) = what

            first =
                (
                    date_time = !(    lexeme_d
                                      [
                                          as_lower_d
                                          [
                                              wday_p [ASSIGN(tm_wday, arg1)]
                                          ]
                                      ]
                                      >> ','
                                 )
                                 >> date
                                 >> !time
                                 >> !zone,

                    date      =  uint_p [ASSIGN(tm_mday, arg1)]
                                 >> lexeme_d
                                    [
                                        as_lower_d
                                        [
                                            month_p [ASSIGN(tm_mon, arg1)]
                                        ]
                                    ]
                                 >> (    limit_d(0u, 99u)
                                         [
                                             uint_p [ASSIGN(tm_year, arg1)]
                                         ]
                                    |    min_limit_d(1900u)
                                         [
                                             uint_p [ASSIGN(tm_year, arg1 - 1900)]
                                         ]
                                    ),

                    time      = uint_p [ASSIGN(tm_hour, arg1)]
                                >> ':'
                                >> uint_p [ASSIGN(tm_min, arg1)]
                                >> !(
                                        ':'
                                        >> uint_p [ASSIGN(tm_sec, arg1)]
                                    ),

                    zone      = ch_p('+')  >>  uint4_p
                                               [
                                                   ASSIGN(tzoffset,
                                                          ((arg1 / 100) * 60 + (arg1 % 100)) * 60)
                                               ]
                              | ch_p('-')  >>  uint4_p
                                               [
                                                   ASSIGN(tzoffset,
                                                          -((arg1 / 100) * 60 + (arg1 % 100)) * 60)
                                               ]
                              | lexeme_d
                                [
                                    as_lower_d
                                    [
                                        timezone_p [ASSIGN(tzoffset, arg1)]
                                    ]
                                ]
                );
#undef ASSIGN
            }
        const boost::spirit::rule<scannerT>& start() const
            {
            return first;
            }
        boost::spirit::subrule<0> date_time;
        boost::spirit::subrule<1> date;
        boost::spirit::subrule<2> time;
        boost::spirit::subrule<3> zone;
        boost::spirit::uint_parser<int, 10, 4, 4>  uint4_p;
        boost::spirit::rule<scannerT> first;
        };
    };
const rfcdate_parser rfcdate_p;

#endif
