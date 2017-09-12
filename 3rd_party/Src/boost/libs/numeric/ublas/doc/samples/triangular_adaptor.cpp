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

#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>

int main () {
    using namespace boost::numeric::ublas;
    matrix<double> m (3, 3);
    triangular_adaptor<matrix<double>, lower> tal (m);
    for (unsigned i = 0; i < tal.size1 (); ++ i)
        for (unsigned j = 0; j <= i; ++ j)
            tal (i, j) = 3 * i + j;
    std::cout << tal << std::endl;
    triangular_adaptor<matrix<double>, upper> tau (m);
    for (unsigned i = 0; i < tau.size1 (); ++ i)
        for (unsigned j = i; j < tau.size2 (); ++ j)
            tau (i, j) = 3 * i + j;
    std::cout << tau << std::endl;
}

