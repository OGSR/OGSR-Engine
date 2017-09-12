//  tab_check header  --------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#ifndef BOOST_TAB_CHECK_HPP
#define BOOST_TAB_CHECK_HPP

#include "inspector.hpp"

namespace boost
{
  namespace inspect
  {
    class tab_check : public inspector
    {
      long m_files_with_errors;
    public:

      tab_check();
      virtual const char * name() const { return "tab-check"; }
      virtual const char * desc() const { return "tabs in file"; }

      virtual void inspect(
        const std::string & library_name,
        const path & full_path,
        const std::string & contents );

      virtual ~tab_check()
        { std::cout << "  " << m_files_with_errors << " files with tabs\n"; }
    };
  }
}

#endif // BOOST_TAB_CHECK_HPP
