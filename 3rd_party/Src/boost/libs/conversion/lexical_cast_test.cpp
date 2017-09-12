// Unit test for boost::lexical_cast.
//
// See http://www.boost.org for most recent version, including documentation.
//
// Copyright Terje Slettebø and Kevlin Henney, 2003.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#include <boost/config.hpp>

#if defined(__INTEL_COMPILER)
#pragma warning(disable: 193 383 488 981 1418 1419)
#elif defined(BOOST_MSVC)
#pragma warning(disable: 4097 4100 4121 4127 4146 4244 4245 4511 4512 4701 4800)
#endif

#include <boost/lexical_cast.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#if defined(BOOST_NO_STRINGSTREAM) || \
    defined(BOOST_NO_STD_WSTRING) || \
    defined(BOOST_NO_STD_LOCALE) || \
    defined(BOOST_NO_CWCHAR) || \
    defined(BOOST_MSVC) && (BOOST_MSVC <= 1200)
#define DISABLE_WIDE_CHAR_SUPPORT
#endif

using namespace boost;

void test_conversion_to_char();
void test_conversion_to_int();
void test_conversion_to_double();
void test_conversion_to_bool();
void test_conversion_to_string();
void test_conversion_to_pointer();
void test_conversion_from_wchar_t();
void test_conversion_to_wchar_t();
void test_conversion_from_wstring();
void test_conversion_to_wstring();

unit_test_framework::test_suite *init_unit_test_suite(int, char **)
{
    unit_test_framework::test_suite *suite =
        BOOST_TEST_SUITE("lexical_cast unit test");
    suite->add(BOOST_TEST_CASE(test_conversion_to_char));
    suite->add(BOOST_TEST_CASE(test_conversion_to_int));
    suite->add(BOOST_TEST_CASE(test_conversion_to_double));
    suite->add(BOOST_TEST_CASE(test_conversion_to_bool));
    suite->add(BOOST_TEST_CASE(test_conversion_to_pointer));
    suite->add(BOOST_TEST_CASE(test_conversion_to_string));
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    suite->add(BOOST_TEST_CASE(test_conversion_from_wchar_t));
    suite->add(BOOST_TEST_CASE(test_conversion_to_wchar_t));
    suite->add(BOOST_TEST_CASE(test_conversion_from_wstring));
    suite->add(BOOST_TEST_CASE(test_conversion_to_wstring));
    #endif
    return suite;
}

void test_conversion_to_char()
{
    BOOST_CHECK_EQUAL('A', lexical_cast<char>('A'));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(' '));
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(1));
    BOOST_CHECK_EQUAL('0', lexical_cast<char>(0));
    BOOST_CHECK_THROW(lexical_cast<char>(123), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(1.0));
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(true));
    BOOST_CHECK_EQUAL('0', lexical_cast<char>(false));
    BOOST_CHECK_EQUAL('A', lexical_cast<char>("A"));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(" "));
    BOOST_CHECK_THROW(lexical_cast<char>(""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<char>("Test"), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL('A', lexical_cast<char>(std::string("A")));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(std::string(" ")));
    BOOST_CHECK_THROW(
        lexical_cast<char>(std::string("")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<char>(std::string("Test")), boost::bad_lexical_cast);
}

void test_conversion_to_int()
{
    BOOST_CHECK_EQUAL(1,lexical_cast<int>('1'));
    BOOST_CHECK_EQUAL(0,lexical_cast<int>('0'));
    BOOST_CHECK_THROW(lexical_cast<int>('A'),boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(1,lexical_cast<int>(1));
    BOOST_CHECK_EQUAL(
        std::numeric_limits<int>::max(),
        lexical_cast<int>(std::numeric_limits<int>::max()));
    BOOST_CHECK_EQUAL(1,lexical_cast<int>(1.0));

    BOOST_CHECK_THROW(lexical_cast<int>(1.23), boost::bad_lexical_cast);

    BOOST_CHECK_THROW(lexical_cast<int>(1e20), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(true));
    BOOST_CHECK_EQUAL(0, lexical_cast<int>(false));
    BOOST_CHECK_EQUAL(123, lexical_cast<int>("123"));
    BOOST_CHECK_THROW(
        lexical_cast<int>(" 123"), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>(""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>("Test"), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(123, lexical_cast<int>("123"));
    BOOST_CHECK_EQUAL(123,lexical_cast<int>(std::string("123")));
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string(" 123")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string("")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string("Test")), boost::bad_lexical_cast);
}

void test_conversion_to_double()
{
    BOOST_CHECK_EQUAL(1.0, lexical_cast<double>('1'));
    BOOST_CHECK_THROW(lexical_cast<double>('A'), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(1.0, lexical_cast<double>(1));
    BOOST_CHECK_EQUAL(1.23, lexical_cast<double>(1.23));
    BOOST_CHECK_CLOSE(
        std::numeric_limits<double>::max() / 2,
        lexical_cast<double>(std::numeric_limits<double>::max() / 2),
        std::numeric_limits<double>::epsilon());
    BOOST_CHECK_EQUAL(1.0, lexical_cast<double>(true));
    BOOST_CHECK_EQUAL(0.0, lexical_cast<double>(false));
    BOOST_CHECK_EQUAL(1.23, lexical_cast<double>("1.23"));
    BOOST_CHECK_THROW(lexical_cast<double>(""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<double>("Test"), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(1.23, lexical_cast<double>(std::string("1.23")));
    BOOST_CHECK_THROW(
        lexical_cast<double>(std::string("")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<double>(std::string("Test")), boost::bad_lexical_cast);
}

void test_conversion_to_bool()
{
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>('1'));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>('0'));
    BOOST_CHECK_THROW(lexical_cast<bool>('A'), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(1));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(0));
    BOOST_CHECK_THROW(lexical_cast<bool>(123), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(1.0));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(0.0));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(true));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(false));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>("1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>("0"));
    BOOST_CHECK_THROW(lexical_cast<bool>(""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>("Test"), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>("1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>("0"));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(std::string("1")));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(std::string("0")));
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::string("")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::string("Test")), boost::bad_lexical_cast);
}

void test_conversion_to_string()
{
    BOOST_CHECK_EQUAL("A", lexical_cast<std::string>('A'));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(' '));
    BOOST_CHECK_EQUAL("123", lexical_cast<std::string>(123));
    BOOST_CHECK_EQUAL("1.23", lexical_cast<std::string>(1.23));
    BOOST_CHECK_EQUAL("1.111111111", lexical_cast<std::string>(1.111111111));
    BOOST_CHECK_EQUAL("1",lexical_cast<std::string>(true));
    BOOST_CHECK_EQUAL("0",lexical_cast<std::string>(false));
    BOOST_CHECK_EQUAL("Test", lexical_cast<std::string>("Test"));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(" "));
    BOOST_CHECK_EQUAL("", lexical_cast<std::string>(""));
    BOOST_CHECK_EQUAL("Test", lexical_cast<std::string>(std::string("Test")));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(std::string(" ")));
    BOOST_CHECK_EQUAL("", lexical_cast<std::string>(std::string("")));
}

void test_conversion_to_pointer()
{
    BOOST_CHECK_THROW(lexical_cast<char *>("Test"), boost::bad_lexical_cast);
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    BOOST_CHECK_THROW(lexical_cast<wchar_t *>("Test"), boost::bad_lexical_cast);
    #endif
}

void test_conversion_from_wchar_t()
{
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(L'1'));
    BOOST_CHECK_THROW(lexical_cast<int>(L'A'), boost::bad_lexical_cast);

    BOOST_CHECK_EQUAL(123, lexical_cast<int>(L"123"));
    BOOST_CHECK_THROW(lexical_cast<int>(L""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>(L"Test"), boost::bad_lexical_cast);

    BOOST_CHECK_EQUAL(1.0, lexical_cast<double>(L'1'));
    BOOST_CHECK_THROW(lexical_cast<double>(L'A'), boost::bad_lexical_cast);

    BOOST_CHECK_EQUAL(1.23, lexical_cast<double>(L"1.23"));
    BOOST_CHECK_THROW(lexical_cast<double>(L""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<double>(L"Test"), boost::bad_lexical_cast);

    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(L'1'));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(L'0'));
    BOOST_CHECK_THROW(lexical_cast<bool>(L'A'), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(L"1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(L"0"));
    BOOST_CHECK_THROW(lexical_cast<bool>(L""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(L"Test"), boost::bad_lexical_cast);
    #endif
}

void test_conversion_to_wchar_t()
{
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(1));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(0));
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(123), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(1.0));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(0.0));
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(true));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(false));
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(L'A'));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(L' '));
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(L"A"));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(L" "));
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(L""), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(L"Test"), boost::bad_lexical_cast);
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(std::wstring(L"A")));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(std::wstring(L" ")));
    BOOST_CHECK_THROW(
        lexical_cast<wchar_t>(std::wstring(L"")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<wchar_t>(std::wstring(L"Test")), boost::bad_lexical_cast);
    #endif
}

void test_conversion_from_wstring()
{
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    BOOST_CHECK_EQUAL(123, lexical_cast<int>(std::wstring(L"123")));
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::wstring(L"")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::wstring(L"Test")), boost::bad_lexical_cast);

    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(std::wstring(L"1")));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(std::wstring(L"0")));
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::wstring(L"")), boost::bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::wstring(L"Test")), boost::bad_lexical_cast);
    #endif
}

void test_conversion_to_wstring()
{
    #ifndef DISABLE_WIDE_CHAR_SUPPORT
    BOOST_CHECK(L"123" == lexical_cast<std::wstring>(123));
    BOOST_CHECK(L"1.23" == lexical_cast<std::wstring>(1.23));
    BOOST_CHECK(L"1.111111111" == lexical_cast<std::wstring>(1.111111111));
    BOOST_CHECK(L"1" == lexical_cast<std::wstring>(true));
    BOOST_CHECK(L"0" == lexical_cast<std::wstring>(false));
    BOOST_CHECK(L"A" == lexical_cast<std::wstring>(L'A'));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(L' '));
    BOOST_CHECK(L"Test" == lexical_cast<std::wstring>(L"Test"));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(L" "));
    BOOST_CHECK(L"" == lexical_cast<std::wstring>(L""));
    BOOST_CHECK(L"Test" == lexical_cast<std::wstring>(std::wstring(L"Test")));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(std::wstring(L" ")));
    BOOST_CHECK(L"" == lexical_cast<std::wstring>(std::wstring(L"")));
    #endif
}
