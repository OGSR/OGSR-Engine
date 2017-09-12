//#include <stdio.h>
#include <cassert>
#include <boost/python/detail/indirect_traits.hpp>

//#define print(expr) printf("%s ==> %s\n", #expr, expr)

// not all the compilers can handle an incomplete class type here.
struct X {};

int main()
{
    using namespace boost::python::detail;

    typedef void (X::*pmf)();

    assert(is_reference_to_function<int (&)()>::value);
    assert(!is_reference_to_function<int (*)()>::value);
    assert(!is_reference_to_function<int&>::value);
    assert(!is_reference_to_function<pmf>::value);
    
    assert(!is_pointer_to_function<int (&)()>::value);
    assert(is_pointer_to_function<int (*)()>::value);
    assert(!is_pointer_to_function<int (*&)()>::value);
    assert(!is_pointer_to_function<int (*const&)()>::value);
    assert(!is_pointer_to_function<pmf>::value);
    
    assert(!is_reference_to_function_pointer<int (&)()>::value);
    assert(!is_reference_to_function_pointer<int (*)()>::value);
    assert(!is_reference_to_function_pointer<int&>::value);
    assert(is_reference_to_function_pointer<int (*&)()>::value);
    assert(is_reference_to_function_pointer<int (*const&)()>::value);
    assert(!is_reference_to_function_pointer<pmf>::value);

    assert(is_reference_to_pointer<int*&>::value);
    assert(is_reference_to_pointer<int* const&>::value);
    assert(is_reference_to_pointer<int*volatile&>::value);
    assert(is_reference_to_pointer<int*const volatile&>::value);
    assert(is_reference_to_pointer<int const*&>::value);
    assert(is_reference_to_pointer<int const* const&>::value);
    assert(is_reference_to_pointer<int const*volatile&>::value);
    assert(is_reference_to_pointer<int const*const volatile&>::value);
    assert(!is_reference_to_pointer<pmf>::value);
    
    assert(!is_reference_to_pointer<int const volatile>::value);
    assert(!is_reference_to_pointer<int>::value);
    assert(!is_reference_to_pointer<int*>::value);

    assert(!is_reference_to_const<int*&>::value);
    assert(is_reference_to_const<int* const&>::value);
    assert(!is_reference_to_const<int*volatile&>::value);
    assert(is_reference_to_const<int*const volatile&>::value);
    
    assert(!is_reference_to_const<int const volatile>::value);
    assert(!is_reference_to_const<int>::value);
    assert(!is_reference_to_const<int*>::value);

    assert(is_reference_to_non_const<int*&>::value);
    assert(!is_reference_to_non_const<int* const&>::value);
    assert(is_reference_to_non_const<int*volatile&>::value);
    assert(!is_reference_to_non_const<int*const volatile&>::value);
    
    assert(!is_reference_to_non_const<int const volatile>::value);
    assert(!is_reference_to_non_const<int>::value);
    assert(!is_reference_to_non_const<int*>::value);
    
    assert(!is_reference_to_volatile<int*&>::value);
    assert(!is_reference_to_volatile<int* const&>::value);
    assert(is_reference_to_volatile<int*volatile&>::value);
    assert(is_reference_to_volatile<int*const volatile&>::value);
    
    assert(!is_reference_to_volatile<int const volatile>::value);
    assert(!is_reference_to_volatile<int>::value);
    assert(!is_reference_to_volatile<int*>::value);

    assert(!is_reference_to_class<int>::value);
    assert(!is_reference_to_class<int&>::value);
    assert(!is_reference_to_class<int*>::value);
    
    assert(!is_reference_to_class<X>::value);
    assert(is_reference_to_class<X&>::value);
    assert(is_reference_to_class<X const&>::value);
    assert(is_reference_to_class<X volatile&>::value);
    assert(is_reference_to_class<X const volatile&>::value);
    
    assert(!is_pointer_to_class<int>::value);
    assert(!is_pointer_to_class<int*>::value);
    assert(!is_pointer_to_class<int&>::value);
    
    assert(!is_pointer_to_class<X>::value);
    assert(!is_pointer_to_class<X&>::value);
    assert(is_pointer_to_class<X*>::value);
    assert(is_pointer_to_class<X const*>::value);
    assert(is_pointer_to_class<X volatile*>::value);
    assert(is_pointer_to_class<X const volatile*>::value);

    assert(is_reference_to_member_function_pointer<pmf&>::value);
    assert(is_reference_to_member_function_pointer<pmf const&>::value);
    assert(is_reference_to_member_function_pointer<pmf volatile&>::value);
    assert(is_reference_to_member_function_pointer<pmf const volatile&>::value);
    assert(!is_reference_to_member_function_pointer<pmf[2]>::value);
    assert(!is_reference_to_member_function_pointer<pmf(&)[2]>::value);
    assert(!is_reference_to_member_function_pointer<pmf>::value);
    
    return 0;
}
