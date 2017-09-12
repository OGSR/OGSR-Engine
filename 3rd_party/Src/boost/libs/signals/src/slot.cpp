// Boost.Signals library
//
// Copyright (C) 2001 Doug Gregor (gregod@cs.rpi.edu)
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

#include <boost/signals/slot.hpp>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      void slot_base::create_connection()
      {
        // Create a new connection object
        basic_connection* con = new basic_connection();

        /* nothrow */ {
          // The signal portion isn't really necessary, except that we need a 
          // signal for the connection to be connected.
          con->signal = static_cast<void*>(this);
          con->signal_data = 0;
          con->signal_disconnect = &bound_object_destructed;
        }

        // This connection watches for destruction of bound objects. Note 
        // that the reset routine will delete con if an allocation throws
        watch_bound_objects.reset(con);

        // We create a scoped connection, so that exceptions thrown while
        // adding bound objects will cause a cleanup of the bound objects 
        // already connected.
        scoped_connection safe_connection(watch_bound_objects);

        // Now notify each of the bound objects that they are connected to this
        // slot.
        for(std::vector<const trackable*>::iterator i = bound_objects.begin();
            i != bound_objects.end(); ++i) {
          // Notify the object that the slot is connecting to it
          BOOST_SIGNALS_NAMESPACE::detail::bound_object binding;
          (*i)->signal_connected(watch_bound_objects, binding);
        
          // This will notify the bound object that the connection just made
          // should be disconnected if an exception is thrown before the
          // end of this iteration
          BOOST_SIGNALS_NAMESPACE::detail::auto_disconnect_bound_object disconnector(binding);

          // Add the binding to the list of bindings for the connection
          con->bound_objects.push_back(binding);

          // The connection object now knows about the bound object, so if an
          // exception is thrown later the connection object will notify the
          // bound object of the disconnection automatically
          disconnector.release();
        }

        // No exceptions will be thrown past this point.
        safe_connection.release();
      }
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost
