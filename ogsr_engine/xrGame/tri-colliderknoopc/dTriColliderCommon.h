#pragma once

#include "../Level.h"
#include "../ode_include.h"
#include "../ExtendedGeom.h"
#include "dTriColliderMath.h"

extern xr_vector<Flags8> gl_cl_tries_state;
// extern xr_vector<int>::iterator		I,E,B				;

// NOTE: stride is sizeof(dContact) * N, where N is [0, ...)

inline dContactGeom* CONTACT(dContactGeom* ptr, const int stride)
{
    const size_t count = stride / sizeof(dContact);
    dContact* contact = (dContact*)(uintptr_t(ptr) - uintptr_t(offsetof(dContact, geom)));
    return &(contact[count]).geom;
}

inline dSurfaceParameters* SURFACE(dContactGeom* ptr, const int stride)
{
    const size_t count = stride / sizeof(dContact);
    dContact* contact = (dContact*)(uintptr_t(ptr) - uintptr_t(offsetof(dContact, geom)));
    return &(contact[count]).surface;
}

#define NUMC_MASK (0xffff)

constexpr float M_SIN_PI_3 = REAL(0.8660254037844386467637231707529362);
constexpr float M_COS_PI_3 = REAL(0.5000000000000000000000000000000000);
