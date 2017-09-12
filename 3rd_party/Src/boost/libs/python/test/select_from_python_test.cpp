#include <boost/python/converter/arg_from_python.hpp>
#include <boost/python/type_id.hpp>
#include <iostream>

// gcc 2.95.x and MIPSpro 7.3.1.3 linker seem to demand this definition
#if ((defined(__GNUC__) && __GNUC__ < 3)) \
 || (defined(__sgi) && defined(__EDG_VERSION__) && (__EDG_VERSION__ == 238))
namespace boost { namespace python {
BOOST_PYTHON_DECL bool handle_exception_impl(function0<void>)
{
    return true;
}
}}
#endif

int result;

#define ASSERT_SAME(T1,T2) \
       if (!is_same< T1, T2 >::value) { \
             std::cout << "*********************\n"; \
             std::cout << python::type_id< T1 >() << " != " << python::type_id< T2 >() << "\n"; \
             std::cout << "*********************\n"; \
             result = 1; \
       }

int main()
{
    using namespace boost::python::converter;
    using namespace boost;


    ASSERT_SAME(
        select_arg_from_python<int>::type, arg_rvalue_from_python<int>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const>::type, arg_rvalue_from_python<int const>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile>::type, arg_rvalue_from_python<int volatile>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile>::type, arg_rvalue_from_python<int const volatile>
        );



    ASSERT_SAME(
        select_arg_from_python<int*>::type, pointer_arg_from_python<int*>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*>::type, pointer_arg_from_python<int const*>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*>::type, pointer_arg_from_python<int volatile*>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*>::type, pointer_arg_from_python<int const volatile*>
        );




    ASSERT_SAME(
        select_arg_from_python<int&>::type, reference_arg_from_python<int&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const&>::type, arg_rvalue_from_python<int const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile&>::type, reference_arg_from_python<int volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile&>::type, reference_arg_from_python<int const volatile&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*&>::type, reference_arg_from_python<int*&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*&>::type, reference_arg_from_python<int const*&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*&>::type, reference_arg_from_python<int volatile*&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*&>::type, reference_arg_from_python<int const volatile*&>
        );



    ASSERT_SAME(
        select_arg_from_python<int* const&>::type, pointer_cref_arg_from_python<int*const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const* const&>::type, pointer_cref_arg_from_python<int const*const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile* const&>::type, pointer_cref_arg_from_python<int volatile*const&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile* const&>::type, pointer_cref_arg_from_python<int const volatile*const&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*volatile&>::type, reference_arg_from_python<int*volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*volatile&>::type, reference_arg_from_python<int const*volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*volatile&>::type, reference_arg_from_python<int volatile*volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*volatile&>::type, reference_arg_from_python<int const volatile*volatile&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*const volatile&>::type, reference_arg_from_python<int*const volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*const volatile&>::type, reference_arg_from_python<int const*const volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*const volatile&>::type, reference_arg_from_python<int volatile*const volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*const volatile&>::type, reference_arg_from_python<int const volatile*const volatile&>
        );
    return result;
}
