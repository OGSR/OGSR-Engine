#include <iostream>

namespace operators {

struct C
{
    static double x;
    double value;

    const C operator+(const C other) const
    {
        C c;
        c.value = value + other.value;
        return c;
    }
    operator int() const
    {
        return value;    
    } 
    
    double operator()()
    {
        return C::x;
    }

    double operator()(double other)
    {
        return C::x + other;
    }
    

};

double C::x = 10;

const C operator*(const C& lhs, const C& rhs)
{
    C c;
    c.value = lhs.value * rhs.value;
    return c;
}

}
