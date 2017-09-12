#ifndef WRAPPER_TEST_WRAPPERS
#define WRAPPER_TEST_WRAPPERS 

#include <vector>
#include <boost/python.hpp>
#include "wrappertest.h"

using namespace boost::python;

template <class T>
list VectorToList(const std::vector<T> & v)
{
    list res;
    std::vector<T>::const_iterator it;
    for(it = v.begin(); it != v.end(); ++it){
        res.append(*it);
    }
    Py_XINCREF(res.ptr());
    return res;
}

list RangeWrapper(int count){
    return VectorToList(wrappertest::Range(count));
}

#endif
