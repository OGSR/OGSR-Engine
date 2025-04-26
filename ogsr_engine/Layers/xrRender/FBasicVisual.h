#pragma once

#include "../../xr_3da/vis_common.h"

#include "../../Include/xrRender/RenderVisual.h"

#define VLOAD_NOVERTICES (1 << 0)

// The class itself
class CKinematicsAnimated;
class CKinematics;
class IParticleCustom;

struct IRender_Mesh
{
    // format
    ref_geom rm_geom;

    // verts
    ID3DVertexBuffer* p_rm_Vertices;
    u32 vBase;
    u32 vCount;

    // indices
    ID3DIndexBuffer* p_rm_Indices;
    u32 iBase;
    u32 iCount;

    u32 dwPrimitives;

    IRender_Mesh()
    {
        p_rm_Vertices = nullptr;
        p_rm_Indices = nullptr;
    }
    virtual ~IRender_Mesh();

private:
    IRender_Mesh(const IRender_Mesh& other);
    void operator=(const IRender_Mesh& other);
};

// The class itself
class ECORE_API dxRender_Visual : public IRenderVisual
{
public:
    virtual shared_str getDebugName() const { return dbg_name; }
    virtual shared_str getDebugInfo() const;

    // Common data for rendering
    u32 Type; // visual's type
    ref_shader shader; // pipe state, shared
    bool IsHudVisual{};

    bool is_level_static{};

    // Simp: параметры для рендера деревьев добавил сюда, чтобы не делать множество dynamic_cast
    u32 base_crc{};
    u32 crc[R__NUM_CONTEXTS]{};

    FloraVertData tree_data{};

    /************************* Add by Zander *******************************/
protected:
    bool _renderFlag{true}; // if false, don`t push this to render / add by Zander

    shared_str dbg_name;

    shared_str dbg_shader_name;
    shared_str dbg_texture_name;

    u16 dbg_shader_id; // should not be ZERO
    
    vis_data vis; // visibility-data

public:
    inline bool getRZFlag() const { return _renderFlag; }
    inline void setRZFlag(const bool f) { _renderFlag = f; }
    /************************* End add *************************************/

    virtual void Render(CBackend& cmd_list, float lod, bool use_fast_geo) {} // LOD - Level Of Detail  [0..1], Ignored
    virtual void RenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data) {}

    virtual void select_lod_id(float lod, u32 context_id) {};

    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Release(); // Shared memory release
    virtual void Copy(dxRender_Visual* from);
    virtual void Spawn(){};
    virtual void Depart(){};

    //	virtual	CKinematics*		dcast_PKinematics			()				{ return 0;	}
    //	virtual	CKinematicsAnimated*dcast_PKinematicsAnimated	()				{ return 0;	}
    //	virtual IParticleCustom*	dcast_ParticleCustom		()				{ return 0;	}

    virtual vis_data& getVisData() { return vis; }
    virtual u32 getType() { return Type; }

    dxRender_Visual();
    virtual ~dxRender_Visual();

    virtual void MarkAsHot(bool is_hot);
};
