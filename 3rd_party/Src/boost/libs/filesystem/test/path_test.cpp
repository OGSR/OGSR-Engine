//  path_test program  -------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/utility.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

namespace fs = boost::filesystem;
using boost::filesystem::path;
using boost::next;
using boost::prior;

#include <boost/test/minimal.hpp>

#define PATH_CHECK( a, b ) check( a, b, __LINE__ )

namespace {
  int errors;

  void check( const fs::path & source,
              const std::string & expected, int line )
  {
    if ( source.string()== expected ) return;

    ++errors;

    std::cout << '(' << line << ") source.string(): \"" << source.string()
              << "\" != expected: \"" << expected
              << "\"" << std::endl;
  }

  void check_throw( const std::string & arg )
  {
    try
    {
      fs::path arg_path( arg );
      ++errors;
      std::cout << "failed to throw with argument \"" << arg
                << "\"" << std::endl;
    }
    catch ( const fs::filesystem_error & /*ex*/ )
    {
//      std::cout << ex.what() << "\n";
    }
  }

} // unnamed namespace

int test_main( int, char*[] )
{
  std::string platform( BOOST_PLATFORM );
  platform = ( platform == "Win32" || platform == "Win64" || platform == "Cygwin" )
             ? "Windows"
             : "POSIX";

  boost::function_requires< boost::ForwardIteratorConcept< fs::path::iterator > >();

  path p1( "fe/fi/fo/fum" );
  path p2( p1 );
  path p3;
  BOOST_TEST( p1.string() != p3.string() );
  p3 = p2;

//  p1.branch_path() = p2; // should fail
//  *p1.begin() = ""; // should fail

  // These verify various overloads don't cause compiler errors
  fs::exists( "foo" );
  fs::exists( std::string( "foo" ) );
  fs::exists( p1 );
  fs::exists( "foo" / p1 );
  fs::exists( std::string( "foo" ) / p1 );
  fs::exists( fs::check_posix_leaf( "foo" ) );

  BOOST_TEST( p1.string() == p2.string() );
  BOOST_TEST( p1.string() == p3.string() );
  BOOST_TEST( path( "foo" ).leaf() == "foo" );
  BOOST_TEST( path( "foo" ).branch_path().string() == "" );
  BOOST_TEST( p1.leaf() == "fum" );
  BOOST_TEST( p1.branch_path().string() == "fe/fi/fo" );
  BOOST_TEST( path( "" ).empty() == true );
  BOOST_TEST( path( "foo" ).empty() == false );

  PATH_CHECK( "", "" );

  PATH_CHECK( "foo", "foo" );
  PATH_CHECK( path("") / "foo", "foo" );
  PATH_CHECK( path("foo") / "", "foo" );
  PATH_CHECK( path( "/" ), "/" );
  PATH_CHECK( path( "/" ) / "", "/" );
  PATH_CHECK( path( "/f" ), "/f" );

  PATH_CHECK( "/foo", "/foo" );
  PATH_CHECK( path("") / "/foo", "/foo" );
  PATH_CHECK( path("/foo") / "", "/foo" );

  PATH_CHECK( "foo/", "foo" );
  PATH_CHECK( path("") / "foo/", "foo" );
  PATH_CHECK( path("foo") / "/", "foo" );

  PATH_CHECK( "foo/bar", "foo/bar" );
  PATH_CHECK( path("foo") / "bar", "foo/bar" );
  PATH_CHECK( path("foo") / path("bar"), "foo/bar" );
  PATH_CHECK( "foo" / path("bar"), "foo/bar" );

  PATH_CHECK( "a/b", "a/b" );  // probe for length effects
  PATH_CHECK( path("a") / "b", "a/b" );

  PATH_CHECK( "..", ".." );
  PATH_CHECK( path("..") / "", ".." );
  PATH_CHECK( path("") / "..", ".." );

  PATH_CHECK( "../..", "../.." );
  PATH_CHECK( path("..") / ".." , "../.." );

  PATH_CHECK( "../foo", "../foo" );
  PATH_CHECK( path("..") / "foo" , "../foo" );

  PATH_CHECK( "foo/..", "" );
  PATH_CHECK( path("foo") / ".." , "" );

  PATH_CHECK( "../f", "../f" );
  PATH_CHECK( path("..") / "f" , "../f" );

  PATH_CHECK( "f/..", "" );
  PATH_CHECK( path("f") / ".." , "" );

  PATH_CHECK( "foo/../..", ".." );
  PATH_CHECK( path("foo") / ".." / ".." , ".." );

  PATH_CHECK( "foo/../../..", "../.." );
  PATH_CHECK( path("foo") / ".." / ".." / ".." , "../.." );

  PATH_CHECK( "foo/../bar", "bar" );
  PATH_CHECK( path("foo") / ".." / "bar" , "bar" );

  PATH_CHECK( "foo/bar/..", "foo" );
  PATH_CHECK( path("foo") / "bar" / ".." , "foo" );

  PATH_CHECK( "foo/bar/../..", "" );
  PATH_CHECK( path("foo") / "bar" / ".." / "..", "" );

  PATH_CHECK( "foo/bar/../blah", "foo/blah" );
  PATH_CHECK( path("foo") / "bar" / ".." / "blah", "foo/blah" );

  PATH_CHECK( "f/../b", "b" );
  PATH_CHECK( path("f") / ".." / "b" , "b" );

  PATH_CHECK( "f/b/..", "f" );
  PATH_CHECK( path("f") / "b" / ".." , "f" );

  PATH_CHECK( "f/b/../a", "f/a" );
  PATH_CHECK( path("f") / "b" / ".." / "a", "f/a" );

  PATH_CHECK( "foo/bar/blah/../..", "foo" );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / "..", "foo" );

  PATH_CHECK( "foo/bar/blah/../../bletch", "foo/bletch" );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / ".." / "bletch", "foo/bletch" );
  
  BOOST_TEST( fs::posix_name("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") );
  BOOST_TEST( !fs::posix_name("F$O") );
  BOOST_TEST( !fs::posix_name(".") );
  BOOST_TEST( !fs::boost_file_name("ABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  BOOST_TEST( fs::boost_file_name("abcdefghijklmnopqrstuvwxyz") );
  BOOST_TEST( fs::boost_file_name("0123456789.-_") );
  BOOST_TEST( fs::boost_file_name("1234567890123456789012345678901") );
  BOOST_TEST( !fs::boost_file_name("12345678901234567890123456789012") );
  BOOST_TEST( !fs::boost_file_name("F$O") );
  BOOST_TEST( !fs::boost_file_name(".") );
  BOOST_TEST( !fs::boost_directory_name("ABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  BOOST_TEST( fs::boost_directory_name("abcdefghijklmnopqrstuvwxyz") );
  BOOST_TEST( fs::boost_directory_name("0123456789-_") );
  BOOST_TEST( fs::boost_directory_name("1234567890123456789012345678901") );
  BOOST_TEST( !fs::boost_directory_name("12345678901234567890123456789012") );
  BOOST_TEST( !fs::boost_directory_name("F$O") );

  check_throw( "...." );
  check_throw( "foo/...." );
  check_throw( "foo//bar" );
  check_throw( "foo\\bar" );
  check_throw( " " );
  check_throw( " foo" );
  check_throw( "foo " );
  check_throw( ">" );
  check_throw( "<" );
  check_throw( ":" );
  check_throw( "." );
  check_throw( "\"" );
  check_throw( "|" );

  check_throw( "c:" );
  check_throw( "c:/" );
  check_throw( "//share" );
  check_throw( "prn:" );

  path itr_ck( "/foo/bar" );
  path::iterator itr( itr_ck.begin() );
  BOOST_TEST( *itr == std::string( "/" ) );
  BOOST_TEST( *++itr == std::string( "foo" ) );
  BOOST_TEST( *++itr == std::string( "bar" ) );
  BOOST_TEST( ++itr == itr_ck.end() );
  BOOST_TEST( *--itr == std::string( "bar" ) );
  BOOST_TEST( *--itr == std::string( "foo" ) );
  BOOST_TEST( *--itr == std::string( "/" ) );

  itr_ck = "";
  BOOST_TEST( itr_ck.begin() == itr_ck.end() );

  itr_ck = path( "/" );
  BOOST_TEST( *itr_ck.begin() == std::string( "/" ) );
  BOOST_TEST( next(itr_ck.begin()) == itr_ck.end() );
  BOOST_TEST( *prior(itr_ck.end()) == std::string( "/" ) );
  BOOST_TEST( prior(itr_ck.end()) == itr_ck.begin() );

  itr_ck = path( "/foo" );
  BOOST_TEST( *itr_ck.begin() == std::string( "/" ) );
  BOOST_TEST( *next( itr_ck.begin() ) == std::string( "foo" ) );
  BOOST_TEST( next(next( itr_ck.begin() )) == itr_ck.end() );
  BOOST_TEST( next( itr_ck.begin() ) == prior( itr_ck.end() ) );
  BOOST_TEST( *prior( itr_ck.end() ) == std::string( "foo" ) );
  BOOST_TEST( *prior(prior( itr_ck.end() )) == std::string( "/" ) );
  BOOST_TEST( prior(prior( itr_ck.end() )) == itr_ck.begin() );

  itr_ck = "foo";
  BOOST_TEST( *itr_ck.begin() == std::string( "foo" ) );
  BOOST_TEST( next( itr_ck.begin() ) == itr_ck.end() );
  BOOST_TEST( *prior( itr_ck.end() ) == std::string( "foo" ) );
  BOOST_TEST( prior( itr_ck.end() ) == itr_ck.begin() );

  path p;

  p = "";
  BOOST_TEST( p.relative_path().string() == "" );
  BOOST_TEST( p.branch_path().string() == "" );
  BOOST_TEST( p.leaf() == "" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "" );
  BOOST_TEST( p.root_path().string() == "" );
  BOOST_TEST( !p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( !p.has_root_directory() );
  BOOST_TEST( !p.has_relative_path() );
  BOOST_TEST( !p.has_leaf() );
  BOOST_TEST( !p.has_branch_path() );
  BOOST_TEST( !p.is_complete() );

  p = "/";
  BOOST_TEST( p.relative_path().string() == "" );
  BOOST_TEST( p.branch_path().string() == "" );
  BOOST_TEST( p.leaf() == "/" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "/" );
  BOOST_TEST( p.root_path().string() == "/" );
  BOOST_TEST( p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( p.has_root_directory() );
  BOOST_TEST( !p.has_relative_path() );
  BOOST_TEST( p.has_leaf() );
  BOOST_TEST( !p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_TEST( p.is_complete() );
  else
    BOOST_TEST( !p.is_complete() );

  p = "foo";
  BOOST_TEST( p.relative_path().string() == "foo" );
  BOOST_TEST( p.branch_path().string() == "" );
  BOOST_TEST( p.leaf() == "foo" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "" );
  BOOST_TEST( p.root_path().string() == "" );
  BOOST_TEST( !p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( !p.has_root_directory() );
  BOOST_TEST( p.has_relative_path() );
  BOOST_TEST( p.has_leaf() );
  BOOST_TEST( !p.has_branch_path() );
  BOOST_TEST( !p.is_complete() );

  p = "/foo";
  BOOST_TEST( p.relative_path().string() == "foo" );
  BOOST_TEST( p.branch_path().string() == "/" );
  BOOST_TEST( p.leaf() == "foo" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "/" );
  BOOST_TEST( p.root_path().string() == "/" );
  BOOST_TEST( p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( p.has_root_directory() );
  BOOST_TEST( p.has_relative_path() );
  BOOST_TEST( p.has_leaf() );
  BOOST_TEST( p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_TEST( p.is_complete() );
  else
    BOOST_TEST( !p.is_complete() );

  p = "foo/bar";
  BOOST_TEST( p.relative_path().string() == "foo/bar" );
  BOOST_TEST( p.branch_path().string() == "foo" );
  BOOST_TEST( p.leaf() == "bar" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "" );
  BOOST_TEST( p.root_path().string() == "" );
  BOOST_TEST( !p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( !p.has_root_directory() );
  BOOST_TEST( p.has_relative_path() );
  BOOST_TEST( p.has_leaf() );
  BOOST_TEST( p.has_branch_path() );
  BOOST_TEST( !p.is_complete() );

  p = "/foo/bar";
  BOOST_TEST( p.relative_path().string() == "foo/bar" );
  BOOST_TEST( p.branch_path().string() == "/foo" );
  BOOST_TEST( p.leaf() == "bar" );
  BOOST_TEST( p.root_name() == "" );
  BOOST_TEST( p.root_directory() == "/" );
  BOOST_TEST( p.root_path().string() == "/" );
  BOOST_TEST( p.has_root_path() );
  BOOST_TEST( !p.has_root_name() );
  BOOST_TEST( p.has_root_directory() );
  BOOST_TEST( p.has_relative_path() );
  BOOST_TEST( p.has_leaf() );
  BOOST_TEST( p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_TEST( p.is_complete() );
  else
    BOOST_TEST( !p.is_complete() );

  if ( platform == "Windows" )
  {
    PATH_CHECK( path( "\\", fs::native ), "/" );
    PATH_CHECK( path( "\\f", fs::native ), "/f" );
    PATH_CHECK( path( "\\foo", fs::native ), "/foo" );
    PATH_CHECK( path( "foo\\bar", fs::native ), "foo/bar" );
    PATH_CHECK( path( "foo bar", fs::native ), "foo bar" );
    PATH_CHECK( path( "c:", fs::native ), "c:" );
    PATH_CHECK( path( "c:/", fs::native ), "c:/" );
    PATH_CHECK( path( "c:foo", fs::native ), "c:foo" );
    PATH_CHECK( path( "c:/foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "//share", fs::native ), "//share" );
    PATH_CHECK( path( "//share/", fs::native ), "//share/" );
    PATH_CHECK( path( "//share/foo", fs::native ), "//share/foo" );
    PATH_CHECK( path( "\\\\share", fs::native ), "//share" );
    PATH_CHECK( path( "\\\\share\\", fs::native ), "//share/" );
    PATH_CHECK( path( "\\\\share\\foo", fs::native ), "//share/foo" );
    PATH_CHECK( path( "c:/foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "prn:", fs::native ), "prn:" );

    p = path( "c:", fs::native );
    BOOST_TEST( p.relative_path().string() == "" );
    BOOST_TEST( p.branch_path().string() == "" );
    BOOST_TEST( p.leaf() == "c:" );
    BOOST_TEST( p.root_name() == "c:" );
    BOOST_TEST( p.root_directory() == "" );
    BOOST_TEST( p.root_path().string() == "c:" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( !p.has_root_directory() );
    BOOST_TEST( !p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( !p.has_branch_path() );
    BOOST_TEST( !p.is_complete() );

    p = path( "c:foo", fs::native );
    BOOST_TEST( p.relative_path().string() == "foo" );
    BOOST_TEST( p.branch_path().string() == "c:" );
    BOOST_TEST( p.leaf() == "foo" );
    BOOST_TEST( p.root_name() == "c:" );
    BOOST_TEST( p.root_directory() == "" );
    BOOST_TEST( p.root_path().string() == "c:" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( !p.has_root_directory() );
    BOOST_TEST( p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( p.has_branch_path() );
    BOOST_TEST( !p.is_complete() );
 
    p = path( "c:/", fs::native );
    BOOST_TEST( p.relative_path().string() == "" );
    BOOST_TEST( p.branch_path().string() == "c:" );
    BOOST_TEST( p.leaf() == "/" );
    BOOST_TEST( p.root_name() == "c:" );
    BOOST_TEST( p.root_directory() == "/" );
    BOOST_TEST( p.root_path().string() == "c:/" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( p.has_root_directory() );
    BOOST_TEST( !p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( p.has_branch_path() );
    BOOST_TEST( p.is_complete() );

    p = path( "c:/foo", fs::native );
    BOOST_TEST( p.relative_path().string() == "foo" );
    BOOST_TEST( p.branch_path().string() == "c:/" );
    BOOST_TEST( p.leaf() == "foo" );
    BOOST_TEST( p.root_name() == "c:" );
    BOOST_TEST( p.root_directory() == "/" );
    BOOST_TEST( p.root_path().string() == "c:/" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( p.has_root_directory() );
    BOOST_TEST( p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( p.has_branch_path() );
    BOOST_TEST( p.is_complete() );

/* Commented out until the semantics of //share are clearer.

    p = path( "//share", fs::native );
    BOOST_TEST( p.string() == "//share" );
    BOOST_TEST( p.relative_path().string() == "" );
    BOOST_TEST( p.branch_path().string() == "" );
    BOOST_TEST( p.leaf() == "//share" );
    BOOST_TEST( p.root_name() == "//share" );
    BOOST_TEST( p.root_directory() == "/" );
    BOOST_TEST( p.root_path().string() == "//share/" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( !p.has_root_directory() );
    BOOST_TEST( !p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( !p.has_branch_path() );
    BOOST_TEST( !p.is_complete() );
*/
    p = path( "//share/", fs::native );
    BOOST_TEST( p.relative_path().string() == "" );
    BOOST_TEST( p.branch_path().string() == "//share" );
    BOOST_TEST( p.leaf() == "/" );
    BOOST_TEST( p.root_name() == "//share" );
    BOOST_TEST( p.root_directory() == "/" );
    BOOST_TEST( p.root_path().string() == "//share/" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( p.has_root_directory() );
    BOOST_TEST( !p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( p.has_branch_path() );
    BOOST_TEST( p.is_complete() );

    p = path( "//share/foo", fs::native );
    BOOST_TEST( p.relative_path().string() == "foo" );
    BOOST_TEST( p.branch_path().string() == "//share/" );
    BOOST_TEST( p.leaf() == "foo" );
    BOOST_TEST( p.root_name() == "//share" );
    BOOST_TEST( p.root_directory() == "/" );
    BOOST_TEST( p.root_path().string() == "//share/" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( p.has_root_directory() );
    BOOST_TEST( p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( p.has_branch_path() );
    BOOST_TEST( p.is_complete() );

    p = path( "prn:", fs::native );
    BOOST_TEST( p.relative_path().string() == "" );
    BOOST_TEST( p.branch_path().string() == "" );
    BOOST_TEST( p.leaf() == "prn:" );
    BOOST_TEST( p.root_name() == "prn:" );
    BOOST_TEST( p.root_directory() == "" );
    BOOST_TEST( p.root_path().string() == "prn:" );
    BOOST_TEST( p.has_root_path() );
    BOOST_TEST( p.has_root_name() );
    BOOST_TEST( !p.has_root_directory() );
    BOOST_TEST( !p.has_relative_path() );
    BOOST_TEST( p.has_leaf() );
    BOOST_TEST( !p.has_branch_path() );
    BOOST_TEST( p.is_complete() );

    itr_ck = path( "c:", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_TEST( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_TEST( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "c:" ) );

    itr_ck = path( "c:/", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_TEST( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_TEST( next( next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_TEST( prior( prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_TEST( *prior( prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:foo", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_TEST( *next( itr_ck.begin() ) == std::string( "foo" ) );
    BOOST_TEST( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_TEST( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_TEST( *prior(prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:/foo", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_TEST( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_TEST( *next( next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_TEST( next( next( next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_TEST( prior( prior( prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_TEST( *prior( prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_TEST( *prior( prior( prior( itr_ck.end() ))) == std::string( "c:" ) );

    itr_ck = path( "//share", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_TEST( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_TEST( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "//share" ) );

    itr_ck = path( "//share/", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_TEST( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_TEST( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_TEST( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_TEST( *prior(prior( itr_ck.end() )) == std::string( "//share" ) );

    itr_ck = path( "//share/foo", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_TEST( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_TEST( *next(next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_TEST( next(next(next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_TEST( prior(prior(prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_TEST( *prior(prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_TEST( *prior(prior(prior( itr_ck.end() ))) == std::string( "//share" ) );

    itr_ck = path( "prn:", fs::native );
    BOOST_TEST( *itr_ck.begin() == std::string( "prn:" ) );
    BOOST_TEST( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_TEST( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_TEST( *prior( itr_ck.end() ) == std::string( "prn:" ) );
  }
//  std::cout << errors << " errors detected\n";
  
  return errors;
}
