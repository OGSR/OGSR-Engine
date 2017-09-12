//  copyright_check implementation  ------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#include "copyright_check.hpp"

namespace boost
{
  namespace inspect
  {
   copyright_check::copyright_check() : m_files_with_errors(0)
   {
     register_signature( ".c" );
     register_signature( ".cpp" );
     register_signature( ".cxx" );
     register_signature( ".h" );
     register_signature( ".hpp" );
     register_signature( ".hxx" );
     register_signature( "Jamfile" );
     register_signature( ".jam" );
   }
     
   void copyright_check::inspect(
      const string & library_name,
      const path & full_path,   // example: c:/foo/boost/filesystem/path.hpp
      const string & contents )     // contents of file to be inspected
    {
      if ( contents.find( "Copyright" ) == string::npos
        && contents.find( "copyright" ) == string::npos )
      {
        ++m_files_with_errors;
        error( library_name, full_path, desc() );
      }
    }
  } // namespace inspect
} // namespace boost


