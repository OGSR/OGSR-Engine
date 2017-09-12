// unit test file sinhc.hpp for the special functions test suite

//  (C) Copyright Hubert Holin 2003. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.


#include <functional>
#include <iomanip>
#include <iostream>
#include <complex>


#include <boost/math/special_functions/sinhc.hpp>


#include <boost/test/unit_test.hpp>


template<typename T>
void    sinhc_pi_test(const char * more_blurb)
{
    using    ::std::abs;
        
    using    ::std::numeric_limits;
    
    using    ::boost::math::sinhc_pi;
    
    
    BOOST_MESSAGE("Testing sinhc_pi in the real domain for "
        << more_blurb << ".");
    
    BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
        (
            abs(sinhc_pi<T>(static_cast<T>(0))-static_cast<T>(1)),
            numeric_limits<T>::epsilon()
        ));
}


template<typename T>
void    sinhc_pi_complex_test(const char * more_blurb)
{
    using    ::std::abs;
    using    ::std::sin;
        
    using    ::std::numeric_limits;
    
    using    ::boost::math::sinhc_pi;
    
    
    BOOST_MESSAGE("Testing sinhc_pi in the complex domain for "
        << more_blurb << ".");
    
    BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
        (
            abs(sinhc_pi<T>(::std::complex<T>(0, 1))-
                ::std::complex<T>(sin(static_cast<T>(1)))),
            numeric_limits<T>::epsilon()
        ));
}


void    sinhc_pi_manual_check()
{
    using    ::boost::math::sinhc_pi;
    
    
    BOOST_MESSAGE("sinc_pi");
    
    for    (int i = 0; i <= 100; i++)
    {
        BOOST_MESSAGE( ::std::setw(15)
                    << sinhc_pi<float>(static_cast<float>(i-50)/
                                                static_cast<float>(50))
                    << ::std::setw(15)
                    << sinhc_pi<double>(static_cast<double>(i-50)/
                                                static_cast<double>(50))
                    << ::std::setw(15)
                    << sinhc_pi<long double>(static_cast<long double>(i-50)/
                                                static_cast<long double>(50)));
    }
    
    BOOST_MESSAGE(" ");
}
    

