//  Boost ios_state_test.cpp test file  --------------------------------------//

//  (C) Copyright Daryle Walker 2002.  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears 
//  in all copies.  This software is provided "as is" without express or implied 
//  warranty, and with no claim as to its suitability for any purpose. 

//  Revision History
//   26 Feb 2002  Initial version (Daryle Walker)

#define  BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>  // main, BOOST_TEST, etc.

#include <boost/cstdlib.hpp>       // for boost::exit_success
#include <boost/io/ios_state.hpp>  // for boost::io::ios_flags_saver, etc.

#include <iomanip>    // for std::setw
#include <ios>        // for std::ios_base, std::streamsize, etc.
#include <iostream>   // for std::cout, etc.
#include <istream>    // for std::istream
#include <locale>     // for std::numpunct_byname, std::locale
#include <ostream>    // for std::endl, std::ostream
#include <streambuf>  // for std::streambuf
#include <string>     // for std::string


// Facet with the bool names spelled backwards
class backward_bool_names
    : public std::numpunct_byname<char>
{
    typedef std::numpunct_byname<char>  base_type;

public:
    explicit  backward_bool_names( char const *name )
        : base_type( name )
        {}

protected:
    virtual  base_type::string_type  do_truename() const
        { return "eurt"; }
    virtual  base_type::string_type  do_falsename() const
        { return "eslaf"; }
};


// Index to test custom storage
int const  my_index = std::ios_base::xalloc();

// Test data
char const    test_string[] = "Hello world";
int const     test_num1 = -16;
double const  test_num2 = 34.5678901234;
bool const    test_bool = true;


// Function prototypes
void  saver_tests_1( std::istream &input, std::ostream &output,
 std::ostream &err );
void  saver_tests_2( std::istream &input, std::ostream &output,
 std::ostream &err );


// Test program
int
test_main
(
    int         ,   // "argc" is unused
    char *      []  // "argv" is unused
)
{
    using std::cout;
    using std::ios_base;
    using std::streamsize;
    using std::cin;

    cout << "The original data is:\n";
    cout << '\t' << test_string << '\n';
    cout << '\t' << test_num1 << '\n';
    cout << '\t' << test_num2 << '\n';
    cout << '\t' << std::boolalpha << test_bool << std::endl;

    // Save states for comparison later
    ios_base::fmtflags const  cout_flags = cout.flags();
    streamsize const          cout_precision = cout.precision();
    streamsize const          cout_width = cout.width();
    ios_base::iostate const   cout_iostate = cout.rdstate();
    ios_base::iostate const   cout_exceptions = cout.exceptions();
    std::ostream * const      cin_tie = cin.tie();
    std::streambuf * const    cout_sb = cout.rdbuf();
    char const                cout_fill = cout.fill();
    std::locale const         cout_locale = cout.getloc();

    cout.iword( my_index ) = 42L;
    cout.pword( my_index ) = &cin;

    // Run saver tests with changing separate from saving
    saver_tests_1( cin, cout, std::cerr );

    // Check if states are back to normal
    BOOST_TEST( &cin == cout.pword(my_index) );
    BOOST_TEST( 42L == cout.iword(my_index) );
    BOOST_TEST( cout_locale == cout.getloc() );
    BOOST_TEST( cout_fill == cout.fill() );
    BOOST_TEST( cout_sb == cout.rdbuf() );
    BOOST_TEST( cin_tie == cin.tie() );
    BOOST_TEST( cout_exceptions == cout.exceptions() );
    BOOST_TEST( cout_iostate == cout.rdstate() );
    BOOST_TEST( cout_width == cout.width() );
    BOOST_TEST( cout_precision == cout.precision() );
    BOOST_TEST( cout_flags == cout.flags() );

    // Run saver tests with combined saving and changing
    saver_tests_2( cin, cout, std::cerr );

    // Check if states are back to normal
    BOOST_TEST( &cin == cout.pword(my_index) );
    BOOST_TEST( 42L == cout.iword(my_index) );
    BOOST_TEST( cout_locale == cout.getloc() );
    BOOST_TEST( cout_fill == cout.fill() );
    BOOST_TEST( cout_sb == cout.rdbuf() );
    BOOST_TEST( cin_tie == cin.tie() );
    BOOST_TEST( cout_exceptions == cout.exceptions() );
    BOOST_TEST( cout_iostate == cout.rdstate() );
    BOOST_TEST( cout_width == cout.width() );
    BOOST_TEST( cout_precision == cout.precision() );
    BOOST_TEST( cout_flags == cout.flags() );

    return boost::exit_success;
}

// Save, change, and restore stream properties
void
saver_tests_1
(
    std::istream &  input,
    std::ostream &  output,
    std::ostream &  err
)
{
    using std::locale;
    using std::ios_base;
    using std::setw;

    boost::io::ios_flags_saver const      ifls( output );
    boost::io::ios_precision_saver const  iprs( output );
    boost::io::ios_width_saver const      iws( output );
    boost::io::ios_tie_saver const        its( input );
    boost::io::ios_rdbuf_saver const      irs( output );
    boost::io::ios_fill_saver const       ifis( output );
    boost::io::ios_locale_saver const     ils( output );
    boost::io::ios_iword_saver const      iis( output, my_index );
    boost::io::ios_pword_saver const      ipws( output, my_index );

    locale  loc( locale::classic(), new backward_bool_names("") );

    input.tie( &err );
    output.rdbuf( err.rdbuf() );
    output.iword( my_index ) = 69L;
    output.pword( my_index ) = &err;

    output << "The data is (again):\n";
    output.setf( ios_base::showpos | ios_base::boolalpha );
    output.setf( ios_base::internal, ios_base::adjustfield );
    output.fill( '@' );
    output.precision( 9 );
    output << '\t' << test_string << '\n';
    output << '\t' << setw( 10 ) << test_num1 << '\n';
    output << '\t' << setw( 15 ) << test_num2 << '\n';
    output.imbue( loc );
    output << '\t' << test_bool << '\n';

    BOOST_TEST( &err == output.pword(my_index) );
    BOOST_TEST( 69L == output.iword(my_index) );

    try
    {
        boost::io::ios_exception_saver const  ies( output );
        boost::io::ios_iostate_saver const    iis( output );

        output.exceptions( ios_base::eofbit );
        output.setstate( ios_base::eofbit );
        BOOST_ERROR( "previous line should have thrown" );
    }
    catch ( ios_base::failure &f )
    {
        err << "Got the expected I/O failure: \"" << f.what() << "\".\n";
        BOOST_TEST( output.exceptions() == ios_base::goodbit );
    }
    catch ( ... )
    {
        err << "Got an unknown error when doing exception test!\n";
        throw;
    }
}

// Save & change and restore stream properties
void
saver_tests_2
(
    std::istream &  input,
    std::ostream &  output,
    std::ostream &  err
)
{
    using std::locale;
    using std::ios_base;

    boost::io::ios_tie_saver const    its( input, &err );
    boost::io::ios_rdbuf_saver const  irs( output, err.rdbuf() );
    boost::io::ios_iword_saver const  iis( output, my_index, 69L );
    boost::io::ios_pword_saver const  ipws( output, my_index, &err );
    output << "The data is (a third time; adding the numbers):\n";

    boost::io::ios_flags_saver const      ifls( output, (output.flags()
     & ~ios_base::adjustfield) | ios_base::showpos | ios_base::boolalpha
     | (ios_base::internal & ios_base::adjustfield) );
    boost::io::ios_precision_saver const  iprs( output, 9 );
    boost::io::ios_fill_saver const       ifis( output, '@' );
    output << '\t' << test_string << '\n';

    boost::io::ios_width_saver const  iws( output, 12 );
    output.put( '\t' );
    output << test_num1 + test_num2;
    output.put( '\n' );

    locale                             loc( locale::classic(),
     new backward_bool_names("") );
    boost::io::ios_locale_saver const  ils( output, loc );
    output << '\t' << test_bool << '\n';

    BOOST_TEST( &err == output.pword(my_index) );
    BOOST_TEST( 69L == output.iword(my_index) );

    try
    {
        boost::io::ios_exception_saver const  ies( output, ios_base::eofbit  );
        boost::io::ios_iostate_saver const    iis( output, output.rdstate()
         | ios_base::eofbit );

        BOOST_ERROR( "previous line should have thrown" );
    }
    catch ( ios_base::failure &f )
    {
        err << "Got the expected I/O failure: \"" << f.what() << "\".\n";
        BOOST_TEST( output.exceptions() == ios_base::goodbit );
    }
    catch ( ... )
    {
        err << "Got an unknown error when doing exception test!\n";
        throw;
    }
}
