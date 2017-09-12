// test file for quaternion.hpp

//  (C) Copyright Hubert Holin 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "quaternion_mi1.h"


#include <boost/math/quaternion.hpp>

void    quaternion_mi1()
{
    ::boost::math::quaternion<float>    q0;
    
    q0 *= q0;
}
