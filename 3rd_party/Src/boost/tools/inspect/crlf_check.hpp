//  crfl_check header  --------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  Contributed by Joerg Walter

#ifndef BOOST_CRLF_CHECK_HPP
#define BOOST_CRLF_CHECK_HPP

#include "inspector.hpp"

namespace boost
{
  namespace inspect
  {
    class crlf_check : public inspector
    {
      long m_files_with_errors;
    public:

      crlf_check();
      virtual const char * name() const { return "crlf-check"; }
      virtual const char * desc() const { return "invalid (cr only) line-ending"; }

      virtual void inspect(
        const std::string & library_name,
        const path & full_path,
        const std::string & contents );

      virtual ~crlf_check()
        { std::cout << "  " << m_files_with_errors << " files with invalid line endings\n"; }
    };
  }
}

#endif // BOOST_CRLF_CHECK_HPP
