#pragma once

#include "../xrCore/xrPool.h"

#include "xr_collide_defs.h"

constexpr Fvector c_spatial_offset[8] = {{-1, -1, -1}, {1, -1, -1}, {-1, 1, -1}, {1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1}, {1, 1, 1}};

#pragma pack(push, 4)

/*
Requirements:
0. Generic
    * O(1) insertion
        - radius completely determines	"level"
        - position completely detemines "node"
    * O(1) removal
    *
1. Rendering
    * Should live inside spatial DB
    * Should have at least "bounding-sphere" or "bounding-box"
    * Should have pointer to "sector" it lives in
    * Approximate traversal order relative to point ("camera")
2. Spatial queries
    * Should live inside spatial DB
    * Should have at least "bounding-sphere" or "bounding-box"
*/

const float c_spatial_min = 8.f;
//////////////////////////////////////////////////////////////////////////
enum
{
    STYPE_RENDERABLE = (1 << 0),
    STYPE_LIGHTSOURCE = (1 << 1),
    STYPE_COLLIDEABLE = (1 << 2),
    STYPE_VISIBLEFORAI = (1 << 3),
    STYPE_REACTTOSOUND = (1 << 4),
    STYPE_PHYSIC = (1 << 5),
    STYPE_OBSTACLE = (1 << 6),
    STYPE_SHAPE = (1 << 7),
    STYPE_LIGHTSOURCEHEMI = (1 << 8),
    STYPE_PARTICLE = (1 << 9),

    STYPEFLAG_INVALIDSECTOR = (1 << 16)
};
//////////////////////////////////////////////////////////////////////////
// Comment:
//		ordinal objects			- renderable?, collideable?, visibleforAI?
//		physical-decorations	- renderable, collideable
//		lights					- lightsource
//		particles(temp-objects)	- renderable
//		sound					- ???
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// definition (Portal)
class IRender_Portal
{
public:
    virtual ~IRender_Portal() = default;
};

//////////////////////////////////////////////////////////////////////////
// definition (Sector)
class IRender_Sector
{
public:
    using sector_id_t = size_t;
    static auto constexpr INVALID_SECTOR_ID = static_cast<sector_id_t>(-1);

    virtual ~IRender_Sector() = default;
    sector_id_t unique_id{INVALID_SECTOR_ID};
};

class ISpatial_NODE;
class ISpatial_DB;

namespace Feel
{
class Sound;
}

class IRenderable;
class IRender_Light;
class CGameObject;

class XRCDB_API ISpatial
{
public:
    struct _spatial
    {
        u32 type;
        Fsphere sphere{};
        Fvector node_center{}; // Cached node center for TBV optimization
        float node_radius{}; // Cached node bounds for TBV optimization
        ISpatial_NODE* node_ptr{}; // Cached parent node for "empty-members" optimization
        IRender_Sector::sector_id_t sector_id{IRender_Sector::INVALID_SECTOR_ID};
        Fvector last_sector_check_pos{};
        ISpatial_DB* space{}; // allow different spaces
        std::string dbg_name{};

        _spatial() : type(0) {} // safe way to enhure type is zero before any contstructors takes place
    } spatial;

public:
    BOOL spatial_inside() const;
    void spatial_updatesector_internal(IRender_Sector::sector_id_t sector_id);

public:
    virtual void spatial_register();
    void spatial_unregister();
    virtual void spatial_move();
    virtual const Fvector& spatial_sector_point() const { return spatial.sphere.P; }
    virtual void spatial_updatesector(IRender_Sector::sector_id_t sector_id) 
    {
        spatial_updatesector_internal(sector_id);
    }

    virtual CObject* dcast_CObject() { return nullptr; }
    virtual Feel::Sound* dcast_FeelSound() { return nullptr; }
    virtual IRenderable* dcast_Renderable() { return nullptr; }
    virtual IRender_Light* dcast_Light() { return nullptr; }
    virtual CGameObject* dcast_GameObject() { return nullptr; }

    ISpatial(ISpatial_DB* space);
    virtual ~ISpatial();
};

//////////////////////////////////////////////////////////////////////////
class ISpatial_NODE
{
public:
    typedef _W64 unsigned ptrt;

public:
    ISpatial_NODE* parent; // parent node for "empty-members" optimization
    ISpatial_NODE* children[8]; // children nodes
    xr_vector<ISpatial*> items; // own items
public:
    void _init(ISpatial_NODE* _parent);
    void _remove(ISpatial* _S);
    void _insert(ISpatial* _S);
    BOOL _empty()
    {
        return items.empty() && children[0] == nullptr && children[1] == nullptr && children[2] == nullptr && children[3] == nullptr && children[4] == nullptr &&
            children[5] == nullptr && children[6] == nullptr && children[7] == nullptr;
    }
};

//////////////////////////////////////////////////////////////////////////
class XRCDB_API ISpatial_DB
{
private:
    xrCriticalSection cs;

    poolSS<ISpatial_NODE, 128> allocator;

    xr_vector<ISpatial_NODE*> allocator_pool;
    ISpatial* rt_insert_object{};

    ISpatial_NODE* m_root;
    Fvector m_center{};
    float m_bounds{};

public:

    xr_vector<ISpatial*>* q_result{};

    // stats
    u32 stat_nodes;
    u32 stat_objects;
    CStatTimer stat_insert;
    CStatTimer stat_remove;

    string_unordered_map<std::string, u32> object_track;

private:
    IC u32 _octant(u32 x, u32 y, u32 z) { return z * 4 + y * 2 + x; }
    IC u32 _octant(Fvector& base, Fvector& rel)
    {
        u32 o = 0;
        if (rel.x > base.x)
            o += 1;
        if (rel.y > base.y)
            o += 2;
        if (rel.z > base.z)
            o += 4;
        return o;
    }

    ISpatial_NODE* _node_create();
    void _node_destroy(ISpatial_NODE*& P);

    void _insert(ISpatial_NODE* N, Fvector& n_center, float n_radius);
    void _remove(ISpatial_NODE* N, ISpatial_NODE* N_sub);

public:
    ISpatial_DB();
    ~ISpatial_DB();

    // managing
    void initialize(Fbox& BB);
    void clear();

    void insert(ISpatial* S);
    void remove(ISpatial* S);

    void update(bool validate);
    BOOL verify();

public:
    enum
    {
        O_ONLYFIRST = (1 << 0),
        O_ONLYNEAREST = (1 << 1),

        O_force_u32 = u32(-1)
    };

    // query
    void q_ray(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const Fvector& _start, const Fvector& _dir, float _range);
    void q_box(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const Fvector& _center, const Fvector& _size);
    void q_sphere(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const Fvector& _center, float _radius);
    void q_frustum(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const CFrustum& _frustum);
};

XRCDB_API extern ISpatial_DB* g_SpatialSpace;
XRCDB_API extern ISpatial_DB* g_SpatialSpacePhysic;

#pragma pack(pop)
