//-----------------------------------------------------------------------------
// boost fsm/transition.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2002
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#ifndef BOOST_FSM_TRANSITION_INCLUDED
#define BOOST_FSM_TRANSITION_INCLUDED

#include <cassert>

namespace fsm {
namespace aux {

// represent a signle transition between states |From| and |To|

template<
      typename T
    , typename From
    , typename Event
    , typename To
    , bool (T::* transition_func)(Event const&)
    >
struct transition
{
    typedef T       fsm_t;
    typedef From    from_state_t;
    typedef Event   event_t;
    typedef To      to_state_t;

    typedef typename Event::base_t  base_event_t;
    static bool do_transition(T& x, base_event_t const& e)
    {
        assert(dynamic_cast<event_t const*>(&e) == &e);
        return (x.*transition_func)(static_cast<event_t const &>(e));
    }
};

} // namespace aux
} // namespace fsm

#endif // BOOST_FSM_TRANSITION_INCLUDED
