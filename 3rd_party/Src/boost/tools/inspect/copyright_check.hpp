//  copyright_check header  --------------------------------------------------//

//  (C) Copyright Beman Dawes 2002, 2003. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#ifndef BOOST_COPYRIGHT_CHECK_HPP
#define BOOST_COPYRIGHT_CHECK_HPP

#include "inspector.hpp"

namespace boost
{
  namespace inspect
  {
    class copyright_check : public inspector
    {
      long m_files_with_errors;
    public:

      copyright_check();
      virtual const char * name() const { return "copyright-check"; }
      virtual const char * desc() const { return "missing copyright"; }

      virtual void inspect(
        const std::string & library_name,
        const path & full_path,
        const std::string & contents );

      virtual ~copyright_check()
        { std::cout << "  " << m_files_with_errors << " files missing copyrights\n"; }
    };
  }
}

#endif // BOOST_COPYRIGHT_CHECK_HPP
