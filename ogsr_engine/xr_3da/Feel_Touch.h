#pragma once

#include "pure_relcase.h"

class ENGINE_API CObject;
namespace Feel
{
class ENGINE_API Touch : private pure_relcase
{
    friend class pure_relcase;

public:
    struct DenyTouch
    {
        CObject* O;
        DWORD Expire;
    };

protected:
    xr_vector<DenyTouch> feel_touch_disable;

public:
    xr_vector<CObject*> feel_touch;
    xr_vector<CObject*> q_nearest;

public:
    void feel_touch_relcase(CObject*);

public:
    Touch();
    virtual ~Touch();

    virtual BOOL feel_touch_contact(CObject* O);
    virtual void feel_touch_update(Fvector&, float, const std::function<void(CObject*, bool)>& = {}, const std::function<bool(CObject*)>& = {});
    virtual void feel_touch_deny(CObject* O, DWORD T);
    virtual void feel_touch_new(CObject* O){};
    virtual void feel_touch_delete(CObject* O){};
    virtual void feel_touch_relcase2(CObject*, const std::function<void(CObject*, bool)>& = {});
};
}; // namespace Feel
