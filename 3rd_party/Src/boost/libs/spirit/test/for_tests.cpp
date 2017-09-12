/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// vi:ts=4:sw=4:et
// Tests for spirit::for_p
// [13-Jan-2003]
////////////////////////////////////////////////////////////////////////////////
#if defined (__BORLANDC__)
#define _RWSTD_COMPILE_INSTANTIATE // Borland Workaround [JDG]
#endif

#define qDebug 0
#include <iostream>
#include <cstring>
#if qDebug
#define SPIRIT_DEBUG
#endif
#include <string>
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic/for.hpp>
#include <boost/ref.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace local
{
    template <typename T>
    struct var_wrapper
        : public ::boost::reference_wrapper<T>
    {
        typedef ::boost::reference_wrapper<T> parent;

        explicit inline var_wrapper(T& t) : parent(t) {}

        inline T& operator()() const { return parent::get(); }
    };

    template <typename T>
    inline var_wrapper<T>
    var(T& t)
    {
        return var_wrapper<T>(t);
    }
}

namespace
{
    template <typename T>
    class add_actor
    {
    public:
        explicit add_actor(T &ref_) : ref(ref_) {}

        template <typename T2>
        void operator()(T2 const &val) const
        { ref += val; }

    private:
        T& ref;
    };

    template <typename T>
    inline add_actor<T> const
    add(T& ref)
    {
        return add_actor<T>(ref);
    }
}

typedef ::boost::spirit::rule<> rule_t;

unsigned int test_count = 0;
unsigned int error_count = 0;

unsigned int iterations_performed;
unsigned int iterations_desired;
std::string  input_matched;

static const unsigned int kError = 999;
static const bool good = true;
static const bool bad = false;

rule_t for_rule;
rule_t for_rule2;

void
test_for
(
    char const *s,
    bool succeed,
    rule_t const &r,
    unsigned int iterations_expected
)
{
    using namespace std;
    ++test_count;

    iterations_performed = 0;

    ::boost::spirit::parse_info<> m = ::boost::spirit::parse(s, s+strlen(s), r);

    bool result = (succeed==m.full)?good:bad;

    if (m.full && (m.length != strlen(s)))
        result = bad;

    result &= iterations_expected == iterations_performed;

    if (result==good)
        cout << "PASSED";
    else
    {
        ++error_count;
        cout << "FAILED";
    }

    cout << ": \"" << s << "\" ==> ";
    if (!m.full)
        cout << "<error>";
    else
        cout << '"' << input_matched << '"';

    cout << "     " << iterations_performed << " of "
         << iterations_desired << " iterations\n";
}

namespace
{
    void zero() { iterations_performed = 0; }

    struct inc
    {
        inline void operator()() const { ++iterations_performed; }
    };
    struct cmp
    {
        inline bool operator()() const
        {
            return iterations_performed<iterations_desired;
        }
    };
}

int
main()
{
    using namespace std;

    using boost::spirit::uint_p;
    using boost::spirit::for_p;
    using boost::spirit::assign;

#if qDebug
    SPIRIT_DEBUG_RULE(for_rule);
    SPIRIT_DEBUG_RULE(for_rule2);
#endif

    for_rule
        =   uint_p[assign(iterations_desired)] >> ':'
        >>  for_p(&zero, cmp(), inc())["xy"]
            [assign(input_matched)]
        ;

    for_rule2
        =   for_p(&zero, '.', inc())["xy"]
            [assign(input_matched)]
        ;

    cout << "/////////////////////////////////////////////////////////\n";
    cout << "\n";
    cout << "          for_p test\n";
    cout << "\n";
    cout << "/////////////////////////////////////////////////////////\n";
    cout << "\n";

    test_for("3:xyxyxy",    true, for_rule, 3);
    test_for("3:",         false, for_rule, 0);
    test_for("3:xyxy",     false, for_rule, 2);
    test_for("3:xyxyxyxy", false, for_rule, 3);

    test_for(".xy.xy.xy", true, for_rule2, 3);
    test_for(".xy.xy.xy.", false, for_rule2, 3);

    std::cout << "\n    ";
    if (error_count==0)
        cout << "All " << test_count << " for_p-tests passed.\n"
             << "Test concluded successfully\n";
    else
        cout << error_count << " of " << test_count << " for_p-tests failed\n"
             << "Test failed\n";

    return error_count!=0;
}
////////////////////////////////////////////////////////////////////////////////
// End of File
