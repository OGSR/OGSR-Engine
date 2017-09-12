// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifndef COMPLICATED_DWA20011215_HPP
# define COMPLICATED_DWA20011215_HPP
# include <iostream>

# include "simple_type.hpp"

struct complicated
{
    complicated(simple const&, int = 0);
    ~complicated();

    int get_n() const;

    char* s;
    int n;
};

inline complicated::complicated(simple const&s, int n)
    : s(s.s), n(n)
{
    std::cout << "constructing complicated: " << this->s << ", " << n << std::endl;
}

inline complicated::~complicated()
{
    std::cout << "destroying complicated: " << this->s << ", " << n << std::endl;
}

inline int complicated::get_n() const
{
    return n;
}

#endif // COMPLICATED_DWA20011215_HPP
