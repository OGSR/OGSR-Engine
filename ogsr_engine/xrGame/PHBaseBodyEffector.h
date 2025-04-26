#pragma once

#include "ode_include.h"

class CPHBaseBodyEffector
{
protected:
    dBodyID m_body;

public:
    void Init(dBodyID body) { m_body = body; }
};