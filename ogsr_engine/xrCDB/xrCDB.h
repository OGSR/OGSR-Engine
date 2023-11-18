#pragma once

#ifdef XRCDB_STATIC
#define XRCDB_API
#elif defined XRCDB_EXPORTS
#define XRCDB_API __declspec(dllexport)
#else
#define XRCDB_API __declspec(dllimport)
#endif

// forward declarations
class CFrustum;
namespace Opcode
{
class OPCODE_Model;
class AABBNoLeafNode;
}; // namespace Opcode

#pragma pack(push, 8)
namespace CDB
{
#ifdef _M_X64
#pragma pack(push, 1)
// Triangle for x86
class XRCDB_API TRI_DEPRECATED //*** 16 bytes total (was 32 :)
{
public:
    u32 verts[3]; // 3*4 = 12b
    union
    {
        u32 dummy; // 4b
        struct
        {
            u32 material : 14;
            u32 suppress_shadows : 1;
            u32 suppress_wm : 1;
            u32 sector : 16;
        };
    };

public:
    IC u32 IDvert(u32 ID) { return verts[ID]; }
};
#pragma pack(pop)
#endif
// Triangle
class XRCDB_API TRI //*** 24 bytes total
{
public:
    u32 verts[3]; // 3*4 = 12b
    union
    {
        size_t dummy;
        struct
        {
            size_t material : 14;
            size_t suppress_shadows : 1;
            size_t suppress_wm : 1;
            size_t sector : 16;
#ifdef _M_X64
            size_t dumb : 32;
#endif
        };
#ifdef _M_X64
        struct
        {
            u32 dummy_low;
            u32 dummy_high;
        };
#endif
    };

#ifdef _M_X64
    TRI()
    {
        verts[0] = 0;
        verts[1] = 0;
        verts[2] = 0;
        dummy = 0;
    }

    TRI& operator=(const TRI_DEPRECATED& oldTri)
    {
        verts[0] = oldTri.verts[0];
        verts[1] = oldTri.verts[1];
        verts[2] = oldTri.verts[2];
        dummy = oldTri.dummy;
        dumb = 0;
        return *this;
    }
#endif
public:
    IC u32 IDvert(u32 ID) { return verts[ID]; }
};

// Build callback
typedef void __stdcall build_callback(Fvector* V, int Vcnt, TRI* T, int Tcnt, void* params);

// Model definition
class XRCDB_API MODEL
{
    friend class COLLIDER;
    enum
    {
        S_READY = 0,
        S_INIT = 1,
        S_BUILD = 2,
        S_forcedword = u32(-1)
    };

private:
    xrCriticalSection cs;
    Opcode::OPCODE_Model* tree;
    u32 status; // 0=ready, 1=init, 2=building

    // tris
    TRI* tris;
    int tris_count;
    Fvector* verts;
    int verts_count;

public:
    MODEL();
    ~MODEL();

    IC Fvector* get_verts() { return verts; }
    IC int get_verts_count() const { return verts_count; }
    IC TRI* get_tris() { return tris; }
    IC int get_tris_count() const { return tris_count; }
    IC void syncronize() const
    {
        if (S_READY != status)
        {
            Log("! WARNING: syncronized CDB::query");
            xrCriticalSection* C = (xrCriticalSection*)&cs;
            C->Enter();
            C->Leave();
        }
    }

    void build_internal(Fvector* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc = nullptr, void* bcp = nullptr, const bool rebuildTrisRequired = true);
    void build(Fvector* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc = nullptr, void* bcp = nullptr, const bool rebuildTrisRequired = true);
    u32 memory();
};

// Collider result
struct XRCDB_API RESULT
{
    Fvector verts[3];

    union
    {
        size_t dummy; // 4b
        struct
        {
            size_t material : 14;
            size_t suppress_shadows : 1;
            size_t suppress_wm : 1;
            size_t sector : 16;
#ifdef _M_X64
            u64 stub : 32;
#endif
        };
#ifdef _M_X64
        struct
        {
            u32 dummy_h;
            u32 dummy_l;
        };
#endif
    };

    int id;
    float range;
    float u, v;
};

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
    ~COLLIDER();

    void ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f);
    void box_query(u32 box_mode, const MODEL* m_def, const Fvector& b_center, const Fvector& b_dim);
    void frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F);

    ICF RESULT* r_begin() { return std::data(rd); }
    ICF RESULT* r_end() { return std::data(rd) + std::size(rd); }
    RESULT& r_add();
    void r_free();
    ICF size_t r_count() { return rd.size(); };
    ICF void r_clear() { rd.clear(); };
    ICF void r_clear_compact() { rd.clear(); };
};

//
class XRCDB_API Collector
{
    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    u32 VPack(const Fvector& V, float eps);

public:
    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);

    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, size_t dummy);
    void add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, size_t dummy, float eps = EPS);

    void add_face_packed(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector, float eps = EPS);
    void remove_duplicate_T();
    void calc_adjacency(xr_vector<u32>& dest);

    Fvector* getV() { return &*verts.begin(); }
    size_t getVS() { return verts.size(); }
    TRI* getT() { return &*faces.begin(); }
    size_t getTS() { return faces.size(); }
    void clear()
    {
        verts.clear();
        faces.clear();
    }
};

class Noncopyable
{
public:
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

#pragma warning(push)
#pragma warning(disable : 4275)
const u32 clpMX = 24, clpMY = 16, clpMZ = 24;
class XRCDB_API CollectorPacked : private Noncopyable
{
    typedef xr_vector<u32> DWORDList;
    typedef DWORDList::iterator DWORDIt;

    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    Fvector VMmin, VMscale;
    DWORDList VM[clpMX + 1][clpMY + 1][clpMZ + 1];
    Fvector VMeps;

    u32 VPack(const Fvector& V);

public:
    CollectorPacked(const Fbox& bb, int apx_vertices = 5000, int apx_faces = 5000);

    //		__declspec(noinline) CollectorPacked &operator=	(const CollectorPacked &object)
    //		{
    //			verts
    //		}

    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);
    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, size_t dummy);

    xr_vector<Fvector>& getV_Vec() { return verts; }
    Fvector* getV() { return &*verts.begin(); }
    size_t getVS() { return verts.size(); }
    TRI* getT() { return &*faces.begin(); }
    size_t getTS() { return faces.size(); }
    void clear();
};
#pragma warning(pop)
}; // namespace CDB

#pragma pack(pop)
