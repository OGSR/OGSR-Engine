#pragma once

#define XRCDB_API

class CFrustum;
namespace Opcode
{
class Model;
};

namespace CDB
{
// Triangle
class alignas(16) XRCDB_API TRI //*** 16 bytes total (was 32 :)
{
public:
    u32 verts[3]; // 3*4 = 12b
    union
    {
        u32 dummy; // 4b
        struct
        {
            u32 material : 14; //
            u32 suppress_shadows : 1; //
            u32 suppress_wm : 1; //
            u32 sector : 16; //
        };
    };

public:
    TRI() = default;
};
static_assert(sizeof(TRI) == 16);

// Build callback
using build_callback = void(Fvector* V, const size_t Vcnt, TRI* T, const size_t Tcnt, void* params);

// Model definition
class XRCDB_API MODEL : Noncopyable
{
    friend class COLLIDER;
    enum : u32
    {
        S_INIT = 0,
        S_BUILD = 1,
        S_READY = 2
    };

private:
    Opcode::Model* tree{};
    std::atomic<u32> status{S_INIT}; // 0=init, 1=build, 2=ready

    // tris
    TRI* tris{};
    size_t tris_count{};
    Fvector* verts{};
    size_t verts_count{};

public:
    MODEL() = default;
    ~MODEL();

    IC Fvector* get_verts() { return verts; }
    IC const Fvector* get_verts() const { return verts; }
    IC size_t get_verts_count() const { return verts_count; }
    IC TRI* get_tris() { return tris; }
    IC const TRI* get_tris() const { return tris; }
    IC size_t get_tris_count() const { return tris_count; }
    IC void syncronize() const
    {
        if (S_READY != status)
            Log("! WARNING: syncronized CDB::query");

        while (S_READY != status)
            YieldProcessor();
    }

    void build(const Fvector* V, const size_t Vcnt, const TRI* T, const size_t Tcnt, build_callback* bc = nullptr, void* bcp = nullptr);
    u32 memory();
private:

    void build_internal(const Fvector* V, const size_t Vcnt, const TRI* T, const size_t Tcnt, build_callback* bc = nullptr, void* bcp = nullptr);
};

// Collider result
struct XRCDB_API alignas(16) RESULT
{
    Fvector verts[3];
    union
    {
        u32 dummy; // 4b
        struct
        {
            u32 material : 14; //
            u32 suppress_shadows : 1; //
            u32 suppress_wm : 1; //
            u32 sector : 16; //
        };
    };
    int id;
    float range;
    float u, v;
    u64 pad;
};
static_assert(sizeof(RESULT) == 64);

// Collider Options
enum
{
    OPT_CULL = (1 << 0),
    OPT_ONLYFIRST = (1 << 1),
    OPT_ONLYNEAREST = (1 << 2),
    OPT_FULL_TEST = (1 << 3) // for box & frustum queries - enable class III test(s)
};

// Collider itself
class XRCDB_API COLLIDER
{
    // Result management
    xr_vector<RESULT> rd;

public:
    COLLIDER() = default;
    ~COLLIDER() = default;

    void ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f);
    void box_query(u32 box_mode, const MODEL* m_def, const Fvector& b_center, const Fvector& b_dim);
    void frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F);

    ICF RESULT* r_begin() { return std::data(rd); }
    ICF RESULT* r_end() { return std::data(rd) + std::size(rd); }
    RESULT& r_add();
    ICF size_t r_count() { return rd.size(); };
    ICF void r_clear() { rd.clear(); };
};

class XRCDB_API Collector
{
    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    u32 VPack(const Fvector& V, float eps);

public:
    void add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy, float eps = EPS);

    void calc_adjacency(xr_vector<u32>& dest) const;

    const Fvector* getV() const { return verts.data(); }
    size_t getVS() const { return verts.size(); }
    TRI* getT() { return faces.data(); }
    size_t getTS() const { return faces.size(); }
    void clear()
    {
        verts.clear();
        faces.clear();
    }
};

}; // namespace CDB
