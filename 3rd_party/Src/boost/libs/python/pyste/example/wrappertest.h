#ifndef WRAPPER_TEST
#define WRAPPER_TEST


#include <vector>

namespace wrappertest {

std::vector<int> Range(int count)
{
    std::vector<int> v;
    v.reserve(count);
    for (int i = 0; i < count; ++i){
        v.push_back(i);
    }
    return v;
}


struct C
{
    C() {}

    std::vector<int> Mul(int value)
    {
        std::vector<int> res;
        res.reserve(value);
        std::vector<int>::const_iterator it;
        std::vector<int> v(Range(value));
        for (it = v.begin(); it != v.end(); ++it){
            res.push_back(*it * value);
        }
        return res;
    }
};

} 
#endif

