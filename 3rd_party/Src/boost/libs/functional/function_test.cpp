// ------------------------------------------------------------------------------
// Tests for the Boost functional.hpp header file
//
// Note that functional.hpp relies on partial specialisation to be
// effective.  If your compiler lacks this feature, very few of the
// tests would compile, and so have been excluded from the test.
// ------------------------------------------------------------------------------
// Copyright (c) 2000
// Cadenza New Zealand Ltd
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appears in all copies
// and that both the copyright notice and this permission notice
// appear in supporting documentation.  Cadenza New Zealand Ltd makes
// no representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied
// warranty.
// ------------------------------------------------------------------------------
// $Id: function_test.cpp,v 1.2 2001/09/22 11:52:24 johnmaddock Exp $
// ------------------------------------------------------------------------------
// $Log: function_test.cpp,v $
// Revision 1.2  2001/09/22 11:52:24  johnmaddock
// Intel C++ fixes: no void return types supported.
//
// Revision 1.1.1.1  2000/07/07 16:04:18  beman
// 1.16.1 initial CVS checkin
//
// Revision 1.3  2000/06/26 09:44:01  mark
// Updated following feedback from Jens Maurer.
//
// Revision 1.2  2000/05/17 08:31:45  mark
// Added extra tests now that function traits work correctly.
// For compilers with no support for partial specialisation,
// excluded tests that won't work.
//
// Revision 1.1  2000/05/07 09:14:41  mark
// Initial revision
// ------------------------------------------------------------------------------

// To demonstrate what the boosted function object adapters do for
// you, try compiling with USE_STD defined.  This will endeavour to
// use the standard function object adapters, but is likely to result
// in numerous errors due to the fact that you cannot have references
// to references.
#ifdef USE_STD
#include <functional>
#define boost std
#else
#include <boost/functional.hpp>
#endif

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

class Person
{
  public:
    Person() {}
    Person(const char *n) : name(n) {}

    const std::string &get_name() const { return name; }
    void print(std::ostream &os) const { os << name << " "; }
    void set_name(const std::string &n) { name = n; std::cout << name << " "; }
    std::string clear_name() { std::string ret = name; name = ""; return ret; }
    void do_something(int) const {}

    bool is_fred() const { return name == "Fred"; }
    
  private:
    std::string name;
};

namespace
{
    bool is_equal(const std::string &s1, const std::string &s2)
    {
        return s1 == s2;
    }

    bool is_betty(const std::string &s)
    {
        return s == "Betty";
    }

    void do_set_name(Person *p, const std::string &name)
    {
        p->set_name(name);
    }
    
    void do_set_name_ref(Person &p, const std::string &name)
    {
        p.set_name(name);
    }
}

int main()
{
    std::vector<Person> v1;
    v1.push_back("Fred");
    v1.push_back("Wilma");
    v1.push_back("Barney");
    v1.push_back("Betty");

    const std::vector<Person> cv1(v1.begin(), v1.end());

    std::vector<std::string> v2;
    v2.push_back("Fred");
    v2.push_back("Wilma");
    v2.push_back("Barney");
    v2.push_back("Betty");

    Person person;
    Person &r = person;

    Person fred("Fred");
    Person wilma("Wilma");
    Person barney("Barney");
    Person betty("Betty");
    std::vector<Person*> v3;
    v3.push_back(&fred);
    v3.push_back(&wilma);
    v3.push_back(&barney);
    v3.push_back(&betty);

    const std::vector<Person*> cv3(v3.begin(), v3.end());
    std::vector<const Person*> v3c(v3.begin(), v3.end());

    std::ostream &os = std::cout;

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(__ICL)
    // unary_traits, unary_negate
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::not1(is_betty));

    std::cout << '\n';
    std::transform(v1.begin(), v1.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::not1(boost::mem_fun_ref(&Person::is_fred)));

    // binary_traits, binary_negate
    std::cout << '\n';
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::bind1st(boost::not2(is_equal), "Betty"));

    std::cout << '\n';
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::bind2nd(boost::not2(is_equal), "Betty"));

    // pointer_to_unary_function
    std::cout << '\n';
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::not1(boost::ptr_fun(is_betty)));

    // binary_traits, bind1st, bind2nd
    std::cout << '\n';
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::bind1st(is_equal, "Betty"));

    std::cout << '\n';
    std::transform(v2.begin(), v2.end(),
                   std::ostream_iterator<bool>(std::cout, " "),
                   boost::bind2nd(is_equal, "Betty"));

    // pointer_to_binary_function, bind1st
    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::ptr_fun(do_set_name), &person));

    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::ptr_fun(do_set_name_ref), person));

    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::ptr_fun(do_set_name_ref), r));

    // binary_traits
    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(do_set_name, &person));

    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(do_set_name_ref, person));

    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(do_set_name_ref, r));
#endif

    // const_mem_fun_t
    std::cout << '\n';
    std::transform(v3.begin(), v3.end(),
                   std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun(&Person::get_name));

    std::cout << '\n';
    std::transform(cv3.begin(), cv3.end(),
                   std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun(&Person::get_name));

    std::cout << '\n';
    std::transform(v3c.begin(), v3c.end(),
                   std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun(&Person::get_name));

    // const_mem_fun_ref_t
    std::cout << '\n';
    std::transform(v1.begin(), v1.end(),
                   std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun_ref(&Person::get_name));

    std::cout << '\n';
    std::transform(cv1.begin(), cv1.end(),
                   std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun_ref(&Person::get_name));

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    // const_mem_fun1_t, bind2nd
    std::cout << '\n';
    std::for_each(v3.begin(), v3.end(), boost::bind2nd(boost::mem_fun(&Person::print), std::cout));

    std::cout << '\n';
    std::for_each(v3.begin(), v3.end(), boost::bind2nd(boost::mem_fun(&Person::print), os));

    // const_mem_fun1_ref_t, bind2nd
    std::cout << '\n';
    std::for_each(v1.begin(), v1.end(), boost::bind2nd(boost::mem_fun_ref(&Person::print), std::cout));

    std::cout << '\n';
    std::for_each(v1.begin(), v1.end(), boost::bind2nd(boost::mem_fun_ref(&Person::print), os));

    // mem_fun1_t, bind1st
    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::mem_fun(&Person::set_name), &person));

    // mem_fun1_ref_t, bind1st
    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::mem_fun_ref(&Person::set_name), person));

    std::cout << '\n';
    std::for_each(v2.begin(), v2.end(), boost::bind1st(boost::mem_fun_ref(&Person::set_name), r));
#endif

    // mem_fun_t
    std::cout << '\n';
    std::transform(v3.begin(), v3.end(), std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun(&Person::clear_name));
    
    // mem_fun_ref_t
    std::cout << '\n';
    std::transform(v1.begin(), v1.end(), std::ostream_iterator<std::string>(std::cout, " "),
                   boost::mem_fun_ref(&Person::clear_name));    

    std::cout << '\n';
    return 0;
}
