#pragma once

#include "xr_fixed_map.h"

class dxRender_Visual;

namespace R_dsgraph
{

// Elementary types
struct _NormalItem
{
    float ssa{};
    dxRender_Visual* pVisual{};
};

struct _TreeItem
{
    dxRender_Visual* pVisual;
    xr_vector<FloraVertData*> data;
};

using vs_type = SVS*;
using gs_type = SGS*;
using ps_type = SPS*;
using hs_type = SHS*;
using ds_type = SDS*;

struct pass_ptr_less
{
    bool operator()(const SPass* lhs, const SPass* rhs) const
    {
        if (lhs->vs != rhs->vs)
            return lhs->vs < rhs->vs;

        if (lhs->gs != rhs->gs)
            return lhs->gs < rhs->gs;

        if (lhs->ps != rhs->ps)
            return lhs->ps < rhs->ps;

        if (lhs->hs != rhs->hs)
            return lhs->hs < rhs->hs;

        if (lhs->ds != rhs->ds)
            return lhs->ds < rhs->ds;

        if (lhs->constants != rhs->constants)
            return lhs->constants < rhs->constants;

        if (lhs->state != rhs->state)
            return lhs->state < rhs->state;

        return lhs->T < rhs->T;     
    }
};

// NORMAL
using mapNormalDirect = xr_vector<_NormalItem>;
class mapNormalItems
{
public:
    mapNormalDirect* items{};
    xr_unordered_map<u32, _TreeItem>* trees{};

    mapNormalItems()
    {
        items = xr_new<mapNormalDirect>();
        trees = xr_new<xr_unordered_map<u32, _TreeItem>>();
    }

    ~mapNormalItems()
    {
        xr_delete(items);
        xr_delete(trees);
    }
};

using mapNormal_T = xr_map<SPass*, mapNormalItems, pass_ptr_less>;
using mapNormalPasses_T = mapNormal_T[SHADER_PASSES_MAX];

struct _MatrixItem
{
    float ssa{};
    IRenderable* pObject{};
    dxRender_Visual* pVisual{};
    Fmatrix Matrix{}; // matrix (copy)
};

struct _MatrixItemS
{
    // Хак для использования списков инициализации
    // Не используем наследование

    // _MatrixItem begin
    float ssa{};
    IRenderable* pObject{};
    dxRender_Visual* pVisual{};
    Fmatrix Matrix{}; // matrix (copy)
    // _MatrixItem end

    ShaderElement* se;
};

// MATRIX
using mapMatrixDirect = xr_vector<_MatrixItem>;
class mapMatrixItems
{
public:
    mapMatrixDirect* items{};

    mapMatrixItems() { items = xr_new<mapMatrixDirect>(); }

    ~mapMatrixItems() { xr_delete(items); }
};

using mapMatrix_T = xr_map<SPass*, mapMatrixItems, pass_ptr_less>;
using mapMatrixPasses_T = mapMatrix_T[SHADER_PASSES_MAX];

// Top level
using mapSorted_T = xr_fixed_map<float, _MatrixItemS>;
using mapSortedLarge_T = xr_map<SPass*, mapMatrixItems, pass_ptr_less>;

// LOD
struct _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

} // namespace R_dsgraph
