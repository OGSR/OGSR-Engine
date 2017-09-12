//
//  Copyright (c) 2000-2002
//  Joerg Walter, Mathias Koch
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  The authors make no representations
//  about the suitability of this software for any purpose.
//  It is provided "as is" without express or implied warranty.
//
//  The authors gratefully acknowledge the support of
//  GeNeSys mbH & Co. KG in producing this work.
//

#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/io.hpp>

int main () {
    using namespace boost::numeric::ublas;
    matrix<double> m (3, 3);
    banded_adaptor<matrix<double> > ba (m, 1, 1);
    for (signed i = 0; i < signed (ba.size1 ()); ++ i)
        for (signed j = std::max (i - 1, 0); j < std::min (i + 2, signed (ba.size2 ())); ++ j)
            ba (i, j) = 3 * i + j;
    std::cout << ba << std::endl;
}

