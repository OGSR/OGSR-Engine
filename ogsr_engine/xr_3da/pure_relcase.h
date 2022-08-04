#pragma once

#include "IGame_Level.h"

class ENGINE_API pure_relcase
{
private:
    int m_ID;

public:
    template <typename class_type>
    pure_relcase(void (class_type::*function_to_bind)(CObject*))
    {
        R_ASSERT(g_pGameLevel);
        class_type* self = static_cast<class_type*>(this);
        g_pGameLevel->Objects.relcase_register(fastdelegate::MakeDelegate(self, function_to_bind), &m_ID);
    }
    virtual ~pure_relcase();
};
