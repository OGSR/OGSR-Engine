#pragma once

#include "../../xrCore/fixedmap.h"

class dxRender_Visual;

// #define	USE_RESOURCE_DEBUGGER

namespace R_dsgraph
{
// Elementary types
struct _NormalItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

struct _MatrixItem
{
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
};

struct _MatrixItemS : public _MatrixItem
{
    ShaderElement* se;
};

struct _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

#ifdef USE_RESOURCE_DEBUGGER
typedef ref_vs vs_type;
typedef ref_ps ps_type;
typedef ref_gs gs_type;
typedef ref_hs hs_type;
typedef ref_ds ds_type;
#else
typedef SVS* vs_type;
typedef ID3DGeometryShader* gs_type;
typedef ID3D11HullShader* hs_type;
typedef ID3D11DomainShader* ds_type;
typedef ID3DPixelShader* ps_type;
#endif

// NORMAL
typedef xr_vector<_NormalItem> mapNormalDirect;
struct mapNormalItems : public mapNormalDirect
{
    float ssa;
};
struct mapNormalTextures : public FixedMAP<STextureList*, mapNormalItems>
{
    float ssa;
};
struct mapNormalStates : public FixedMAP<ID3DState*, mapNormalTextures>
{
    float ssa;
};
struct mapNormalCS : public FixedMAP<R_constant_table*, mapNormalStates>
{
    float ssa;
};
struct mapNormalAdvStages
{
    hs_type hs;
    ds_type ds;
    mapNormalCS mapCS;
};
struct mapNormalPS : public FixedMAP<ps_type, mapNormalAdvStages>
{
    float ssa;
};
struct mapNormalGS : public FixedMAP<gs_type, mapNormalPS>
{
    float ssa;
};
struct mapNormalVS : public FixedMAP<vs_type, mapNormalGS>
{};

typedef mapNormalVS mapNormal_T;
typedef mapNormal_T mapNormalPasses_T[SHADER_PASSES_MAX];

// MATRIX
typedef xr_vector<_MatrixItem> mapMatrixDirect;
struct mapMatrixItems : public mapMatrixDirect
{
    float ssa;
};
struct mapMatrixTextures : public FixedMAP<STextureList*, mapMatrixItems>
{
    float ssa;
};
struct mapMatrixStates : public FixedMAP<ID3DState*, mapMatrixTextures>
{
    float ssa;
};
struct mapMatrixCS : public FixedMAP<R_constant_table*, mapMatrixStates>
{
    float ssa;
};
struct mapMatrixAdvStages
{
    hs_type hs;
    ds_type ds;
    mapMatrixCS mapCS;
};
struct mapMatrixPS : public FixedMAP<ps_type, mapMatrixAdvStages>
{
    float ssa;
};
struct mapMatrixGS : public FixedMAP<gs_type, mapMatrixPS>
{
    float ssa;
};
struct mapMatrixVS : public FixedMAP<vs_type, mapMatrixGS>
{};
typedef mapMatrixVS mapMatrix_T;
typedef mapMatrix_T mapMatrixPasses_T[SHADER_PASSES_MAX];

// Top level
typedef FixedMAP<float, _MatrixItemS> mapSorted_T;
typedef mapSorted_T::TNode mapSorted_Node;

typedef FixedMAP<float, _MatrixItemS> mapHUD_T;
typedef mapHUD_T::TNode mapHUD_Node;

typedef FixedMAP<float, _LodItem> mapLOD_T;
typedef mapLOD_T::TNode mapLOD_Node;
}; // namespace R_dsgraph
