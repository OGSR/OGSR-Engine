// Copyright (C) 2001
// Mac Murrett
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Mac Murrett makes no representations
// about the suitability of this software for any purpose.  It is
// provided "as is" without express or implied warranty.
//
// See http://www.boost.org for most recent version including documentation.

#ifndef BOOST_PACKAGE_MJM012402_HPP
#define BOOST_PACKAGE_MJM012402_HPP


namespace boost {

namespace threads {

namespace mac {

namespace detail {


class base_package: private noncopyable
{
  public:
    virtual void accept() = 0;
};

template<class R>
class package: public base_package
{
  public:
    inline package(function<R> &rFunctor):
        m_rFunctor(rFunctor)
        {    /* no-op */                }
    inline ~package()
        {    /* no-op */                }

    virtual void accept()
        {    m_oR = m_rFunctor();    }
    inline R return_value()
        {    return(m_oR);            }

  private:
    function<R> &m_rFunctor;
    R m_oR;
};

template<>
class package<void>: public base_package
{
  public:
    inline package(function<void> &rFunctor):
        m_rFunctor(rFunctor)
        {    /* no-op */                }
    inline ~package()
        {    /* no-op */                }

    virtual void accept()
        {    m_rFunctor();            }
    inline void return_value()
        {    return;                    }

  private:
    function<void> &m_rFunctor;
};


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_PACKAGE_MJM012402_HPP
