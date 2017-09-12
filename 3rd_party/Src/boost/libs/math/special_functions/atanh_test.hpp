// unit test file atanh.hpp for the special functions test suite

//  (C) Copyright Hubert Holin 2003. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.


#include <functional>
#include <iomanip>
#include <iostream>


#include <boost/math/special_functions/atanh.hpp>


#include <boost/test/unit_test.hpp>


template<typename T>
T    atanh_error_evaluator(T x)
{
    using    ::std::abs;
    using    ::std::tanh;
    using    ::std::cosh;
        
    using    ::std::numeric_limits;
    
    using    ::boost::math::atanh;
    
    
    static T const   epsilon = numeric_limits<float>::epsilon();
    
    T                y = tanh(x);
    T                z = atanh(y);
    
    T                absolute_error = abs(z-x);
    T                relative_error = absolute_error/(cosh(x)*cosh(x));
    T                scaled_error = relative_error/epsilon;
    
    return(scaled_error);
}


template<typename T>
void    atanh_test(const char * more_blurb)
{
    using    ::std::abs;
    using    ::std::tanh;
    using    ::std::log;
        
    using    ::std::numeric_limits;
    
    using    ::boost::math::atanh;
    
    
    BOOST_MESSAGE("Testing atanh in the real domain for "
        << more_blurb << ".");
    
    BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
        (
            abs(atanh<T>(static_cast<T>(0))),
            numeric_limits<T>::epsilon()
        ));
    
    BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
        (
            abs(atanh<T>(static_cast<T>(3)/5) - log(static_cast<T>(2))),
            numeric_limits<T>::epsilon()
        ));
    
    BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
        (
            abs(atanh<T>(static_cast<T>(-3)/5) + log(static_cast<T>(2))),
            numeric_limits<T>::epsilon()
        ));
    
    for    (int i = 0; i <= 100; i++)
    {
        T    x = static_cast<T>(i-50)/static_cast<T>(5);
        T    y = tanh(x);
        
        if    (
                (abs(y-static_cast<T>(1)) >= numeric_limits<T>::epsilon())&&
                (abs(y+static_cast<T>(1)) >= numeric_limits<T>::epsilon())
            )
        {
            BOOST_CHECK_PREDICATE(::std::less_equal<T>(), 2,
                (
                    atanh_error_evaluator(x),
                    static_cast<T>(4)
                ));
        }
    }
}


void    atanh_manual_check()
{
    using    ::std::abs;
    using    ::std::tanh;
        
    using    ::std::numeric_limits;
    
    
    BOOST_MESSAGE("atanh");
    
    for    (int i = 0; i <= 100; i++)
    {
        float        xf = static_cast<float>(i-50)/static_cast<float>(5);
        double       xd = static_cast<double>(i-50)/static_cast<double>(5);
        long double  xl =
                static_cast<long double>(i-50)/static_cast<long double>(5);
        
        float        yf = tanh(xf);
        double       yd = tanh(xd);
        long double  yl = tanh(xl);
        
        if    (
                std::numeric_limits<float>::has_infinity &&
                std::numeric_limits<double>::has_infinity &&
                std::numeric_limits<long double>::has_infinity
            )
        {
            BOOST_MESSAGE( ::std::setw(15)
                        << atanh_error_evaluator(xf)
                        << ::std::setw(15)
                        << atanh_error_evaluator(xd)
                        << ::std::setw(15)
                        << atanh_error_evaluator(xl));
        }
        else
        {
            if    (
                    (abs(yf-static_cast<float>(1)) <
                        numeric_limits<float>::epsilon())||
                    (abs(yf+static_cast<float>(1)) <
                        numeric_limits<float>::epsilon())||
                    (abs(yf-static_cast<double>(1)) <
                        numeric_limits<double>::epsilon())||
                    (abs(yf+static_cast<double>(1)) <
                        numeric_limits<double>::epsilon())||
                    (abs(yf-static_cast<long double>(1)) <
                        numeric_limits<long double>::epsilon())||
                    (abs(yf+static_cast<long double>(1)) <
                        numeric_limits<long double>::epsilon())
                )
            {
                BOOST_MESSAGE("Platform's numerics may lack precision.");
            }
            else
            {
                BOOST_MESSAGE( ::std::setw(15)
                            << atanh_error_evaluator(xf)
                            << ::std::setw(15)
                            << atanh_error_evaluator(xd)
                            << ::std::setw(15)
                            << atanh_error_evaluator(xl));
            }
        }
    }
    
    BOOST_MESSAGE(" ");
}

