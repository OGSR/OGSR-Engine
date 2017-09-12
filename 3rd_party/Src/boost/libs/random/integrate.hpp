/* integrate.hpp header file
 *
 * Copyright Jens Maurer 2000
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * Jens Maurer makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * $Id: integrate.hpp,v 1.4 2001/11/19 22:13:04 jmaurer Exp $
 *
 * Revision history
 *   01 April 2001: Modified to use new <boost/limits.hpp> header. (JMaddock)
 */

#ifndef INTEGRATE_HPP
#define INTEGRATE_HPP

#include <boost/limits.hpp>

template<class UnaryFunction>
inline typename UnaryFunction::result_type 
trapezoid(UnaryFunction f, typename UnaryFunction::argument_type a,
          typename UnaryFunction::argument_type b, int n)
{
  typename UnaryFunction::result_type tmp = 0;
  for(int i = 1; i <= n-1; ++i)
    tmp += f(a+(b-a)/n*i);
  return (b-a)/2/n * (f(a) + f(b) + 2*tmp);
}

template<class UnaryFunction>
inline typename UnaryFunction::result_type 
simpson(UnaryFunction f, typename UnaryFunction::argument_type a,
        typename UnaryFunction::argument_type b, int n)
{
  typename UnaryFunction::result_type tmp1 = 0;
  for(int i = 1; i <= n-1; ++i)
    tmp1 += f(a+(b-a)/n*i);
  typename UnaryFunction::result_type tmp2 = 0;
  for(int i = 1; i <= n ; ++i)
    tmp2 += f(a+(b-a)/2/n*(2*i-1));

  return (b-a)/6/n * (f(a) + f(b) + 2*tmp1 + 4*tmp2);
}

// compute b so that f(b) = y; assume f is monotone increasing
template<class UnaryFunction>
inline typename UnaryFunction::argument_type
invert_monotone_inc(UnaryFunction f, typename UnaryFunction::result_type y,
                    typename UnaryFunction::argument_type lower = -1,
                    typename UnaryFunction::argument_type upper = 1)
{
  while(upper-lower > 1e-6) {
    double middle = (upper+lower)/2;
    if(f(middle) > y)
      upper = middle;
    else
      lower = middle;
  }
  return (upper+lower)/2;
}

// compute b so that  I(f(x), a, b) == y
template<class UnaryFunction>
inline typename UnaryFunction::argument_type
quantil(UnaryFunction f, typename UnaryFunction::argument_type a,
        typename UnaryFunction::result_type y,
        typename UnaryFunction::argument_type step)
{
  typedef typename UnaryFunction::result_type result_type;
  if(y >= 1.0)
    return std::numeric_limits<result_type>::infinity();
  typename UnaryFunction::argument_type b = a;
  for(result_type result = 0; result < y; b += step)
    result += step*f(b);
  return b;
}


#endif /* INTEGRATE_HPP */
