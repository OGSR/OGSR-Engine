#include "stdafx.h"
#include "cpuid.h"
#pragma warning(push)
#pragma warning(disable : 4995)
#include <xmmintrin.h>
#pragma warning(pop)

#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

struct alignas(16) vec_t : public Fvector3
{
    float pad;
};
struct alignas(16) aabb_t
{
    vec_t min;
    vec_t max;
};
struct alignas(16) ray_t
{
    vec_t pos;
    vec_t inv_dir;
    vec_t fwd_dir;
};
struct ray_segment_t
{
    float t_near, t_far;
};

// turn those verbose intrinsics into something readable.
#define loadps(mem) _mm_load_ps((const float* const)(mem))
#define storess(ss, mem) _mm_store_ss((float* const)(mem), (ss))
#define minss _mm_min_ss
#define maxss _mm_max_ss
#define minps _mm_min_ps
#define maxps _mm_max_ps
#define mulps _mm_mul_ps
#define subps _mm_sub_ps
#define rotatelps(ps) _mm_shuffle_ps((ps), (ps), 0x39) // a,b,c,d -> b,c,d,a
#define muxhps(low, high) _mm_movehl_ps((low), (high)) // low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}

static constexpr auto flt_plus_inf = std::numeric_limits<float>::infinity();
alignas(16) static constexpr float ps_cst_plus_inf[] = {flt_plus_inf, flt_plus_inf, flt_plus_inf, flt_plus_inf},
                                   ps_cst_minus_inf[] = {-flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf};

ICF BOOL isect_sse(const aabb_t& box, const ray_t& ray, float& dist)
{
    // you may already have those values hanging around somewhere
    const __m128 plus_inf = loadps(ps_cst_plus_inf), minus_inf = loadps(ps_cst_minus_inf);

    // use whatever's apropriate to load.
    const __m128 box_min = loadps(&box.min), box_max = loadps(&box.max), pos = loadps(&ray.pos), inv_dir = loadps(&ray.inv_dir);

    // use a div if inverted directions aren't available
    const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
    const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

    // the order we use for those min/max is vital to filter out
    // NaNs that happens when an inv_dir is +/- inf and
    // (box_min - pos) is 0. inf * 0 = NaN
    const __m128 filtered_l1a = minps(l1, plus_inf);
    const __m128 filtered_l2a = minps(l2, plus_inf);

    const __m128 filtered_l1b = maxps(l1, minus_inf);
    const __m128 filtered_l2b = maxps(l2, minus_inf);

    // now that we're back on our feet, test those slabs.
    __m128 lmax = maxps(filtered_l1a, filtered_l2a);
    __m128 lmin = minps(filtered_l1b, filtered_l2b);

    // unfold back. try to hide the latency of the shufps & co.
    const __m128 lmax0 = rotatelps(lmax);
    const __m128 lmin0 = rotatelps(lmin);
    lmax = minss(lmax, lmax0);
    lmin = maxss(lmin, lmin0);

    const __m128 lmax1 = muxhps(lmax, lmax);
    const __m128 lmin1 = muxhps(lmin, lmin);
    lmax = minss(lmax, lmax1);
    lmin = maxss(lmin, lmin1);

    const BOOL ret = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin);

    storess(lmin, &dist);
    // storess	(lmax, &rs.t_far);

    return ret;
}

template <bool bCull, bool bFirst, bool bNearest>
class alignas(16) ray_collider
{
public:
    COLLIDER* dest;
    TRI* tris;
    Fvector* verts;

    ray_t ray;
    float rRange;
    float rRange2;

    IC void _init(COLLIDER* CL, Fvector* V, TRI* T, const Fvector& C, const Fvector& D, float R)
    {
        dest = CL;
        tris = T;
        verts = V;
        ray.pos.set(C);
        ray.inv_dir.set(1.f, 1.f, 1.f).div(D);
        ray.fwd_dir.set(D);
        rRange = R;
        rRange2 = R * R;
    }

    // sse
    ICF BOOL _box_sse(const Fvector& bCenter, const Fvector& bExtents, float& dist)
    {
        aabb_t box;
        /*
            box.min.sub (bCenter,bExtents);	box.min.pad = 0;
            box.max.add	(bCenter,bExtents); box.max.pad = 0;
        */
        __m128 CN = _mm_unpacklo_ps(_mm_load_ss((float*)&bCenter.x), _mm_load_ss((float*)&bCenter.y));
        CN = _mm_movelh_ps(CN, _mm_load_ss((float*)&bCenter.z));
        __m128 EX = _mm_unpacklo_ps(_mm_load_ss((float*)&bExtents.x), _mm_load_ss((float*)&bExtents.y));
        EX = _mm_movelh_ps(EX, _mm_load_ss((float*)&bExtents.z));

        _mm_store_ps((float*)&box.min, _mm_sub_ps(CN, EX));
        _mm_store_ps((float*)&box.max, _mm_add_ps(CN, EX));

        return isect_sse(box, ray, dist);
    }

    IC bool _tri(u32* p, float& u, float& v, float& range)
    {
        Fvector edge1, edge2, tvec, pvec, qvec;
        float det, inv_det;

        // find vectors for two edges sharing vert0
        Fvector& p0 = verts[p[0]];
        Fvector& p1 = verts[p[1]];
        Fvector& p2 = verts[p[2]];
        edge1.sub(p1, p0);
        edge2.sub(p2, p0);
        // begin calculating determinant - also used to calculate U parameter
        // if determinant is near zero, ray lies in plane of triangle
        pvec.crossproduct(ray.fwd_dir, edge2);
        det = edge1.dotproduct(pvec);
        if (bCull)
        {
            if (det < EPS)
                return false;
            tvec.sub(ray.pos, p0); // calculate distance from vert0 to ray origin
            u = tvec.dotproduct(pvec); // calculate U parameter and test bounds
            if (u < 0.f || u > det)
                return false;
            qvec.crossproduct(tvec, edge1); // prepare to test V parameter
            v = ray.fwd_dir.dotproduct(qvec); // calculate V parameter and test bounds
            if (v < 0.f || u + v > det)
                return false;
            range = edge2.dotproduct(qvec); // calculate t, scale parameters, ray intersects triangle
            inv_det = 1.0f / det;
            range *= inv_det;
            u *= inv_det;
            v *= inv_det;
        }
        else
        {
            if (det > -EPS && det < EPS)
                return false;
            inv_det = 1.0f / det;
            tvec.sub(ray.pos, p0); // calculate distance from vert0 to ray origin
            u = tvec.dotproduct(pvec) * inv_det; // calculate U parameter and test bounds
            if (u < 0.0f || u > 1.0f)
                return false;
            qvec.crossproduct(tvec, edge1); // prepare to test V parameter
            v = ray.fwd_dir.dotproduct(qvec) * inv_det; // calculate V parameter and test bounds
            if (v < 0.0f || u + v > 1.0f)
                return false;
            range = edge2.dotproduct(qvec) * inv_det; // calculate t, ray intersects triangle
        }
        return true;
    }

    void _prim(DWORD prim)
    {
        float u, v, r;
        if (!_tri(tris[prim].verts, u, v, r))
            return;
        if (r <= 0 || r > rRange)
            return;

        if (bNearest)
        {
            if (dest->r_count())
            {
                RESULT& R = *dest->r_begin();
                if (r < R.range)
                {
                    R.id = prim;
                    R.range = r;
                    R.u = u;
                    R.v = v;
                    R.verts[0] = verts[tris[prim].verts[0]];
                    R.verts[1] = verts[tris[prim].verts[1]];
                    R.verts[2] = verts[tris[prim].verts[2]];
                    R.dummy = tris[prim].dummy;
                    rRange = r;
                    rRange2 = r * r;
                }
            }
            else
            {
                RESULT& R = dest->r_add();
                R.id = prim;
                R.range = r;
                R.u = u;
                R.v = v;
                R.verts[0] = verts[tris[prim].verts[0]];
                R.verts[1] = verts[tris[prim].verts[1]];
                R.verts[2] = verts[tris[prim].verts[2]];
                R.dummy = tris[prim].dummy;
                rRange = r;
                rRange2 = r * r;
            }
        }
        else
        {
            RESULT& R = dest->r_add();
            R.id = prim;
            R.range = r;
            R.u = u;
            R.v = v;
            R.verts[0] = verts[tris[prim].verts[0]];
            R.verts[1] = verts[tris[prim].verts[1]];
            R.verts[2] = verts[tris[prim].verts[2]];
            R.dummy = tris[prim].dummy;
        }
    }
    void _stab(const AABBNoLeafNode* node)
    {
        // Should help
        _mm_prefetch((char*)node->GetNeg(), _MM_HINT_NTA);

        // Actual ray/aabb test
        // use SSE
        float d;
        if (!_box_sse((Fvector&)node->mAABB.mCenter, (Fvector&)node->mAABB.mExtents, d))
            return;
        if (d > rRange)
            return;

        // 1st chield
        if (node->HasLeaf())
            _prim(node->GetPrimitive());
        else
            _stab(node->GetPos());

        // Early exit for "only first"
        if (bFirst && dest->r_count())
            return;

        // 2nd chield
        if (node->HasLeaf2())
            _prim(node->GetPrimitive2());
        else
            _stab(node->GetNeg());
    }
};

void COLLIDER::ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range)
{
    ZoneScoped;

    m_def->syncronize();

    // Get nodes
    const AABBNoLeafTree* T = (const AABBNoLeafTree*)m_def->tree->GetTree();
    const AABBNoLeafNode* N = T->GetNodes();
    r_clear();

    // SSE
    // Binary dispatcher
    if (ray_mode & OPT_CULL)
    {
        if (ray_mode & OPT_ONLYFIRST)
        {
            if (ray_mode & OPT_ONLYNEAREST)
            {
                ray_collider<true, true, true> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
            else
            {
                ray_collider<true, true, false> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
        }
        else
        {
            if (ray_mode & OPT_ONLYNEAREST)
            {
                ray_collider<true, false, true> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
            else
            {
                ray_collider<true, false, false> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
        }
    }
    else
    {
        if (ray_mode & OPT_ONLYFIRST)
        {
            if (ray_mode & OPT_ONLYNEAREST)
            {
                ray_collider<false, true, true> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
            else
            {
                ray_collider<false, true, false> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
        }
        else
        {
            if (ray_mode & OPT_ONLYNEAREST)
            {
                ray_collider<false, false, true> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
            else
            {
                ray_collider<false, false, false> RC;
                RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                RC._stab(N);
            }
        }
    }
}
