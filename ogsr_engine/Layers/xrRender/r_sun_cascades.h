#pragma once

namespace sun
{

struct ray
{
    ray() = default;
    ray(Fvector3 const& _P, Fvector3 const& _D) : P(_P), D(_D) {}

    Fvector3 D;
    Fvector3 P;
};

struct cascade
{
    cascade() : reset_chain(false) {}

    u32 cascade_ind;

    xr_vector<ray> rays;

    float size;
    float bias;
    bool reset_chain;

    CSector* cull_sector{};

    CFrustum cull_frustum{};
    Fmatrix cull_xform{};
    Fvector3 cull_COP{};
};

} // namespace sun