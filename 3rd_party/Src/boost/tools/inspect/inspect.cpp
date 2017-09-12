//  inspect program  ---------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  This program recurses through sub-directories looking for various problems.
//  It contains some Boost specific features, like ignoring "CVS" and "bin",
//  and the code that identifies library names assumes the Boost directory
//  structure.

//  See http://www.boost.org/tools/inspect for more information.

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cassert>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>

#include "inspector.hpp" // includes <string>, <boost/filesystem/path.hpp>,
                         // <iostream>, <set>
                         // and gives using for string and path.
#include "copyright_check.hpp"
#include "crlf_check.hpp"
#include "link_check.hpp"
#include "long_name_check.hpp"
#include "tab_check.hpp"

namespace fs = boost::filesystem;

namespace
{
  typedef boost::shared_ptr< boost::inspect::inspector > inspector_ptr;

  struct inspector_element
  {
    inspector_ptr  inspector;

    inspector_element( boost::inspect::inspector * p ) : inspector(p) {}
  };

  typedef std::list< inspector_element > inspector_list;

  long file_count;
  long directory_count;
  long error_count;

  boost::inspect::string_set content_signatures;

  fs::directory_iterator end_itr;

  struct error_msg
  {
    string library;
    string rel_path;
    string msg;

    bool operator<( const error_msg & rhs ) const
    {
      if ( library < rhs.library ) return true;
      if ( library > rhs.library ) return false;
      if ( rel_path < rhs.rel_path ) return true;
      if ( rel_path > rhs.rel_path ) return false;
      return msg < rhs.msg;
    }
  };

  typedef std::vector< error_msg > error_msg_vector;
  error_msg_vector msgs;

//  visit_predicate (determines which directories are visited)  --------------//

  typedef bool(*pred_type)(const path&);

  bool visit_predicate( const path & pth )
  {
    string leaf( pth.leaf() );
    return
      leaf != "CVS"
      && leaf != "bin"
      && leaf != "jam_src" // this really out of our hands
      && leaf != "status"  // too many issues with generated HTML files
      ;
  }

//  library_from_content  ----------------------------------------------------//

  string library_from_content( const string & content )
  {
    string::size_type pos( content.find( "www.boost.org/libs/" ) );

    if ( pos == string::npos ) return "unknown";

    string lib;
    pos += 19;
    while ( content[pos] != ' '
      && content[pos] != '/'
      && content[pos] != '\n'
      && content[pos] != '\r'
      && content[pos] != '\t' ) lib += content[pos++];
    return lib;
  }

//  find_signature  ----------------------------------------------------------//

  bool find_signature( const path & file_path,
    const boost::inspect::string_set & signatures )
  {
    string name( file_path.leaf() );
    if ( signatures.find( name ) == signatures.end() )
    {
      string::size_type pos( name.rfind( '.' ) );
      if ( pos == string::npos
        || signatures.find( name.substr( pos ) )
          == signatures.end() ) return false;
    }
    return true;
  }

//  load_content  ------------------------------------------------------------//

  void load_content( const path & file_path, string & target )
  {
    target = "";

    if ( !find_signature( file_path, content_signatures ) ) return;

    fs::ifstream fin( file_path );
    if ( !fin )
      throw string( "could not open input file: " ) + file_path.string();
    std::getline( fin, target, '\0' ); // read the whole file
  }

//  check  -------------------------------------------------------------------//

  void check( const string & lib,
    const path & pth, const string & content, const inspector_list & insp_list )
  {
    // invoke each inspector
    for ( inspector_list::const_iterator itr = insp_list.begin();
      itr != insp_list.end(); ++itr )
    {
      itr->inspector->inspect( lib, pth ); // always call two-argument form
      if ( find_signature( pth, itr->inspector->signatures() ) )
      {
          itr->inspector->inspect( lib, pth, content );
      }
    }
  }

//  visit_all  ---------------------------------------------------------------//

  void visit_all( const string & lib,
    const path & dir_path, const inspector_list & insps )
  {
    ++directory_count;

    for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
    {

      if ( fs::is_directory( *itr ) )
      {
        if ( visit_predicate( *itr ) )
        {
          string cur_lib( boost::inspect::impute_library( *itr ) );
          check( cur_lib, *itr, "", insps );
          visit_all( cur_lib, *itr, insps );
        }
      }
      else
      {
        ++file_count;
        string content;
        load_content( *itr, content );
        check( lib == "unknown"
          ? library_from_content( content ) : lib, *itr, content, insps );
      }
    }
  }

//  display_errors  ----------------------------------------------------------//

  void display_errors()
  {

    std::sort( msgs.begin(), msgs.end() );

    // display error messages with group indication
    error_msg current;
    string sep;
    for ( error_msg_vector::iterator itr ( msgs.begin() );
      itr != msgs.end(); ++itr )
    {
      if ( current.library != itr->library )
      {
        std::cout << "\n\n" << itr->library;
      }
      if ( current.library != itr->library
        || current.rel_path != itr->rel_path )
      {
        std::cout << "\n   " << itr->rel_path;
        sep = ": ";
      }
      if ( current.library != itr->library
        || current.rel_path != itr->rel_path
        || current.msg != itr->msg )
      {
        std::cout << sep << itr->msg;
        sep = ", ";
      }
      current.library = itr->library;
      current.rel_path = itr->rel_path;
      current.msg = itr->msg;
   }
  }

} // unnamed namespace

namespace boost
{
  namespace inspect
  {

//  register_signature  ------------------------------------------------------//

    void inspector::register_signature( const string & signature )
    {
      m_signatures.insert( signature );
      content_signatures.insert( signature );
    }

//  error  -------------------------------------------------------------------//

    void inspector::error( const string & library_name,
      const path & full_path, const string & msg )
    {
      ++error_count;
      error_msg err_msg;
      err_msg.library = library_name;
      err_msg.rel_path = relative_to( full_path, fs::initial_path() );
      err_msg.msg = msg;
      msgs.push_back( err_msg );

//     std::cout << library_name << ": "
//        << full_path.string() << ": "
//        << msg << '\n';

    }

//  impute_library  ----------------------------------------------------------//

    string impute_library( const path & full_dir_path )
    {
      path relative( relative_to( full_dir_path, fs::initial_path() ) );
      if ( relative.empty() ) return "boost-root";
      string first( *relative.begin() );
      string second =  // borland 5.61 requires op=  
        ++relative.begin() == relative.end()
          ? string() : *++relative.begin();

      if ( first == "boost" )
        return second.empty() ? string( "unknown" ) : second;

      return (( first == "libs" || first == "tools" ) && !second.empty())
        ? second : first;
    }

  } // namespace inspect
} // namespace boost

//  cpp_main()  --------------------------------------------------------------//

int cpp_main( int argc, char * argv[] )
{
  fs::initial_path();

  inspector_list inspectors;

  inspectors.push_back( inspector_element( new boost::inspect::copyright_check ) );
  inspectors.push_back( inspector_element( new boost::inspect::crlf_check ) );
  inspectors.push_back( inspector_element( new boost::inspect::link_check ) );
  inspectors.push_back( inspector_element( new boost::inspect::long_name_check ) );
  inspectors.push_back( inspector_element( new boost::inspect::tab_check ) );

  visit_all( "boost-root", fs::initial_path(), inspectors );

  // close
  for ( inspector_list::iterator itr = inspectors.begin();
        itr != inspectors.end(); ++itr )
  {
    itr->inspector->close();
  }

  display_errors();

  std::cout << "\n\n";
  std::cout << file_count << " files scanned\n";
  std::cout << directory_count << " directories scanned\n";
  std::cout << error_count << " problems reported\n";
  std::cout << "\nproblem counts:\n";

  return 0;
}
