//  fstream_test.cpp  --------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/filesystem/fstream.hpp>
#include <string>
#include <cstdio> // for std::remove

namespace fs = boost::filesystem;

#include <boost/config.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
  namespace std { using ::remove; }
#endif

#include <boost/test/minimal.hpp>

int test_main( int, char*[] )
{
  { // basic_filebuf runtime results are ignored; as long as they don't crash
    // or throw we are satisfied
    fs::basic_filebuf<char> bfb;
    fs::filebuf cfb;

    bfb.open( "fstream_test_bffoo", std::ios_base::in );
    cfb.open( "fstream_test_bffoo", std::ios_base::in );

#   ifndef BOOST_NO_STD_WSTRING
    fs::wfilebuf wfb;
    wfb.open( "fstream_test_bffoo", std::ios_base::in );
#   endif
  }

  std::remove( "fstream_test_bfoo" );
  std::remove( "fstream_test_cfoo" );
# ifndef BOOST_NO_STD_WSTRING
  std::remove( "fstream_test_wfoo" );
# endif
  {
    fs::basic_ofstream<char> bofs( "fstream_test_bfoo" );
    fs::ofstream cofs( "fstream_test_cfoo" );

    BOOST_TEST( bofs.is_open() );
    BOOST_TEST( cofs.is_open() );

    bofs << "fstream_test_bfoo";
    cofs << "fstream_test_cfoo";

    // these will fail, but they still test the interface
    bofs.open( "fstream_test_bfoo" );
    cofs.open( "fstream_test_cfoo" );

#   ifndef BOOST_NO_STD_WSTRING
    fs::wofstream wofs( "fstream_test_wfoo" );
    BOOST_TEST( wofs.is_open() );
    wofs << L"fstream_test_wfoo";
    wofs.open( "fstream_test_wfoo" ); // expected to fail
#   endif
  }

  {
    fs::basic_ifstream<char> bifs( "fstream_test_bfoo" );
    fs::ifstream cifs( "fstream_test_cfoo" );

    BOOST_TEST( bifs.is_open() );
    BOOST_TEST( cifs.is_open() );

    std::string b;
    std::string c;

    bifs >> b;
    cifs >> c;

    BOOST_TEST( b == "fstream_test_bfoo" );
    BOOST_TEST( c == "fstream_test_cfoo" );

    // these will fail, but they still test the interface
    bifs.open( "fstream_test_bfoo" );
    cifs.open( "fstream_test_cfoo" );

#   ifndef BOOST_NO_STD_WSTRING
    fs::wifstream wifs( "fstream_test_wfoo" );
    BOOST_TEST( wifs.is_open() );
    std::wstring w;
    wifs >> w;
    BOOST_TEST( w == L"fstream_test_wfoo" );
    wifs.open( "fstream_test_wfoo" ); // expected to fail
#   endif
  }

  {
    fs::basic_fstream<char> bfs( "fstream_test_bfoo" );
    fs::fstream cfs( "fstream_test_cfoo" );

    BOOST_TEST( bfs.is_open() );
    BOOST_TEST( cfs.is_open() );

    std::string b;
    std::string c;

    bfs >> b;
    cfs >> c;

    BOOST_TEST( b == "fstream_test_bfoo" );
    BOOST_TEST( c == "fstream_test_cfoo" );

    // these will fail, but they still test the interface
    bfs.open( "fstream_test_bfoo" );
    cfs.open( "fstream_test_cfoo" );

#   ifndef BOOST_NO_STD_WSTRING
    fs::wfstream wfs( "fstream_test_wfoo" );
    BOOST_TEST( wfs.is_open() );
    std::wstring w;
    wfs >> w;
    BOOST_TEST( w == L"fstream_test_wfoo" );
    wfs.open( "fstream_test_wfoo" ); // expected to fail
#   endif
  }

//  std::remove( "fstream_test_bfoo" );
//  std::remove( "fstream_test_cfoo" );
//  #   ifndef BOOST_NO_STD_WSTRING
//  std::remove( "fstream_test_wfoo" );
//  #   endif
  return 0;
}
