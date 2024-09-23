#pragma once

#include "../xrcdb/xr_collide_defs.h"
#include "vismask.h"

// refs
class ENGINE_API CObject;
class ENGINE_API CInifile;

// t-defs
constexpr u32 clGET_TRIS = (1 << 0);
constexpr u32 clGET_BOXES = (1 << 1);
constexpr u32 clGET_SPHERES = (1 << 2);
constexpr u32 clQUERY_ONLYFIRST = (1 << 3); // stop if was any collision
constexpr u32 clQUERY_TOPLEVEL = (1 << 4); // get only top level of model box/sphere
constexpr u32 clQUERY_STATIC = (1 << 5); // static
constexpr u32 clQUERY_DYNAMIC = (1 << 6); // dynamic
constexpr u32 clCOARSE = (1 << 7); // coarse test (triangles vs obb)

struct clQueryTri
{
    Fvector p[3];
    const CDB::TRI* T;
};

struct clQueryCollision
{
    xr_vector<CObject*> objects; // affected objects
    xr_vector<clQueryTri> tris; // triangles		(if queried)
    xr_vector<Fobb> boxes; // boxes/ellipsoids	(if queried)
    xr_vector<Fvector4> spheres; // spheres			(if queried)

    IC void Clear()
    {
        objects.clear();
        tris.clear();
        boxes.clear();
        spheres.clear();
    }
    IC void AddTri(const Fmatrix& m, const CDB::TRI* one, const Fvector* verts)
    {
        clQueryTri T;
        m.transform_tiny(T.p[0], verts[one->verts[0]]);
        m.transform_tiny(T.p[1], verts[one->verts[1]]);
        m.transform_tiny(T.p[2], verts[one->verts[2]]);
        T.T = one;
        tris.push_back(T);
    }
    IC void AddTri(const CDB::TRI* one, const Fvector* verts)
    {
        clQueryTri T;
        T.p[0] = verts[one->verts[0]];
        T.p[1] = verts[one->verts[1]];
        T.p[2] = verts[one->verts[2]];
        T.T = one;
        tris.push_back(T);
    }
    IC void AddBox(const Fmatrix& M, const Fbox& B)
    {
        Fobb box;
        Fvector c;
        B.getcenter(c);
        B.getradius(box.m_halfsize);

        Fmatrix T, R;
        T.translate(c);
        R.mul_43(M, T);

        box.xform_set(R);
        boxes.push_back(box);
    }
    IC void AddBox(const Fobb& B) { boxes.push_back(B); }
};

enum /*ENGINE_API*/ ECollisionFormType
{
    cftObject,
    cftShape
};

class ENGINE_API ICollisionForm
{
    friend class CObjectSpace;

protected:
    CObject* owner; // владелец
    u32 dwQueryID{};

protected:
    Fbox bv_box; // (Local) BBox объекта
    Fsphere bv_sphere; // (Local) Sphere
private:
    ECollisionFormType m_type;

    virtual BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R) = 0;

public:
    ICollisionForm(CObject* _owner, ECollisionFormType tp);
    virtual ~ICollisionForm();

    BOOL RayQuery(collide::rq_results& dest, const collide::ray_defs& rq)
    {
        dest.r_clear();
        return _RayQuery(rq, dest);
    }

    IC CObject* Owner() const { return owner; }
    const Fbox& getBBox() const { return bv_box; }
    float getRadius() const { return bv_sphere.R; }
    const Fsphere& getSphere() const { return bv_sphere; }
    ECollisionFormType Type() const { return m_type; }
};

class ENGINE_API CCF_Skeleton : public ICollisionForm
{
public:
    struct ENGINE_API SElement
    {
        union
        {
            struct
            {
                Fmatrix b_IM; // world 2 bone xform
                Fvector b_hsize;
            };
            struct
            {
                Fsphere s_sphere;
            };
            struct
            {
                Fcylinder c_cylinder;
            };
        };
        u16 type;
        u16 elem_id;

    public:
        SElement() : elem_id(u16(-1)), type(0) {}
        SElement(u16 id, u16 t) : elem_id(id), type(t) {}
        BOOL valid() const { return (elem_id != (u16(-1))) && (type != 0); }
        void center(Fvector& center) const;
    };
    DEFINE_VECTOR(SElement, ElementVec, ElementVecIt);

private:
    VisMask vis_mask;
    SpinLock elements_lock;
    ElementVec elements;

    u32 dwFrame{}; // The model itself
    u32 dwFrameTL{}; // Top level

    void BuildState();
    void BuildTopLevel();

public:
    CCF_Skeleton(CObject* _owner);

    virtual BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R);

    bool _ElementCenter(u16 elem_id, Fvector& e_center);
    const ElementVec& _GetElements() { return elements; }

    void Calculate();

    void _dbg_refresh()
    {
        BuildTopLevel();
        BuildState();
    }
};

class ENGINE_API CCF_Shape : public ICollisionForm
{
public:
    union shape_data
    {
        Fsphere sphere;
        struct
        {
            Fmatrix box;
            Fmatrix ibox;
        };
    };
    struct shape_def
    {
        int type;
        shape_data data;
    };
    xr_vector<shape_def> shapes;

public:
    CCF_Shape(CObject* _owner);

    virtual BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R);

    void add_sphere(Fsphere& S);
    void add_box(Fmatrix& B);

    void ComputeBounds();

    BOOL Contact(CObject* O);
    xr_vector<shape_def>& Shapes() { return shapes; }
};
