//  long_name_check header  --------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#ifndef BOOST_FILE_NAME_CHECK_HPP
#define BOOST_FILE_NAME_CHECK_HPP

#include "inspector.hpp"

namespace boost
{
  namespace inspect
  {
    class long_name_check : public inspector
    {
      long m_long_name_errors;
    public:
      long_name_check();
      virtual ~long_name_check();

      virtual const char * name() const { return "long-name-check"; }
      virtual const char * desc() const { return "file names too long"; }

      virtual void inspect(
        const std::string & library_name,
        const path & full_path );
    };
  }
}

#endif // BOOST_FILE_NAME_CHECK_HPP
