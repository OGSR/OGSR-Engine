#pragma once

#include "../xrcdb/xr_collide_defs.h"
#include "render.h"
#include "pure_relcase.h"
#include "xr_object.h"

class IRender_Sector;
class CObject;
class ISpatial;

namespace Feel
{
const float fuzzy_update_vis = 1000.f; // speed of fuzzy-logic desisions
const float fuzzy_update_novis = 1000.f; // speed of fuzzy-logic desisions
const float fuzzy_guaranteed = 0.001f; // distance which is supposed 100% visible
const float lr_granularity = 0.1f; // assume similar positions

class ENGINE_API Vision : private pure_relcase
{
    friend class pure_relcase;

private:
    xr_vector<CObject*> seen;
    xr_vector<CObject*> query;
    xr_vector<CObject*> diff;

    xr_vector<ISpatial*> r_spatial;
    collide::rq_results RQR;

    CFrustum Frustum;
    std::shared_mutex lock_query, lock_visible;

    void o_new(CObject* E);
    void o_delete(CObject* E);
    void o_trace(Fvector& P, float dt, float vis_threshold);

public:
    Vision();
    virtual ~Vision();

    struct feel_visible_Item
    {
        float fuzzy{}; // note range: (-1[no]..1[yes])
        CObject* O{};

        collide::ray_cache Cache{};
        float Cache_vis{};

        Fvector cp_LP{};
        Fvector cp_LR_src{};
        Fvector cp_LR_dst{};

        Fvector cp_LAST{}; // last point found to be visible

        float trans{};
    };
    xr_vector<feel_visible_Item> feel_visible;

public:
    void feel_vision_clear();
    void feel_vision_query(Fmatrix& mFull, Fvector& P);
    void feel_vision_update(CObject* parent, Fvector& P, float dt, float vis_threshold);
    void feel_vision_relcase(CObject* object);
    void feel_vision_get(xr_vector<CObject*>& R)
    {
        R.clear();

        if (feel_visible.empty())
            return;

        R.reserve(feel_visible.size());

        std::shared_lock lock{lock_visible};

        for (const auto& item : feel_visible)
        {
            if (item.O && !item.O->getDestroy() && positive(item.fuzzy))
                R.push_back(item.O);
        }
    }
    Fvector feel_vision_get_vispoint(const CObject* _O)
    {
        if (!_O || _O->getDestroy() || feel_visible.empty())
            return {};

        std::shared_lock lock{lock_visible};

        auto it = std::find_if(feel_visible.begin(), feel_visible.end(), [_O](const auto& item) { return _O == item.O && positive(item.fuzzy); });
        if (it != feel_visible.end())
        {
            return it->cp_LAST;
        }

        return {};
    }
    virtual BOOL feel_vision_isRelevant(CObject* O) = 0;
    virtual float feel_vision_mtl_transp(CObject* O, u32 element) = 0;

    float feel_vision_get_transparency(const CObject* _O) const;
};
}; // namespace Feel