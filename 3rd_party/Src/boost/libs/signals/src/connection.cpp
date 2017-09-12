// Boost.Signals library
//
// Copyright (C) 2001, 2002 Doug Gregor (gregod@cs.rpi.edu)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
 
// For more information, see http://www.boost.org

#define BOOST_SIGNALS_IN_LIBRARY_SOURCE

#include <boost/signals/connection.hpp>
#include <cassert>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    void connection::disconnect() const
    {
      if (this->connected()) {
        // Make sure we have a reference to the basic_connection object,
        // because 'this' may disappear
        shared_ptr<detail::basic_connection> local_con = con;

        void (*signal_disconnect)(void*, void*) = local_con->signal_disconnect;

        // Note that this connection no longer exists
        // Order is important here: we could get into an infinite loop if this
        // isn't cleared before we try the disconnect.
        local_con->signal_disconnect = 0;

        // Disconnect signal
        signal_disconnect(local_con->signal, local_con->signal_data);
      
        // Disconnect all bound objects
        typedef std::list<BOOST_SIGNALS_NAMESPACE::detail::bound_object>::iterator iterator;
        for (iterator i = local_con->bound_objects.begin(); 
             i != local_con->bound_objects.end(); ++i) {
          assert(i->disconnect != 0);
          i->disconnect(i->obj, i->data);
        }
      }
    }
  } // end namespace boost 
} // end namespace boost

#ifndef BOOST_MSVC
// Explicit instantiations to keep everything in the library
template class std::list<boost::BOOST_SIGNALS_NAMESPACE::detail::bound_object>;
#endif
