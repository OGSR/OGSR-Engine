//  inspector header  --------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#ifndef BOOST_INSPECTOR_HPP
#define BOOST_INSPECTOR_HPP

#include <string>
#include <iostream>
#include <set>
#include <boost/filesystem/path.hpp>

using std::string;
using boost::filesystem::path;

namespace boost
{
  namespace inspect
  {
    typedef std::set< string > string_set;
        
    class inspector
    {
    public:
      virtual ~inspector() {}

      virtual const char * name() const = 0; // example: "tab-check"
      virtual const char * desc() const = 0; // example: "verify no tabs"

      // always called:
      virtual void inspect(
        const string & library_name, // "filesystem"
        const path & full_path ) {}  // "c:/foo/boost/filesystem/path.hpp"

      // called only for registered leaf() signatures:
      virtual void inspect(
        const string & library_name, // "filesystem"
        const path & full_path,      // "c:/foo/boost/filesystem/path.hpp"
        const string & contents ) {} // contents of file

      // called after all paths visited, but still in time to call error():
      virtual void close() {}

      // callback used by constructor to register leaf() signature.
      // Signature can be a full file name (Jamfile) or partial (.cpp)
      void register_signature( const string & signature );
      const string_set & signatures() const { return m_signatures; }

      // report error callback (from inspect(), close() ):
      void error(
        const string & library_name,
        const path & full_path,
        const string & msg );

    private:
      string_set m_signatures;
    };

    inline string relative_to( const path & src, const path & base )
    {
      string::size_type pos( base.string().size() );
      return src.string().substr(
        pos + ( pos < src.string().size() ? 1 : 0 ) );
    }

    string impute_library( const path & full_dir_path );
 
  }
}

#endif // BOOST_INSPECTOR_HPP

