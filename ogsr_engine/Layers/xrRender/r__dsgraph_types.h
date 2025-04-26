#pragma once

#include "xr_fixed_map.h"

class dxRender_Visual;

namespace R_dsgraph
{

//static SpinLock static_lock;

// Elementary types
struct _NormalItem
{
    float ssa{};
    dxRender_Visual* pVisual{};
};

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
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
    // _MatrixItem end

    ShaderElement* se;
};

struct _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

struct _TreeItem
{
    dxRender_Visual* pVisual;
    xr_vector<FloraVertData*> data;
};

// NORMAL
using mapNormalDirect = xr_vector<_NormalItem>;

class mapNormalItems
{
public:
    //static u32 instance_cnt;

    float ssa{};
    mapNormalDirect* items{};

    xr_unordered_map<u32, _TreeItem>* trees{};

    mapNormalItems()
    {
        items = xr_new<mapNormalDirect>();
        trees = xr_new<xr_unordered_map<u32, _TreeItem>>();

        //static_lock.lock();
        //instance_cnt++;
        //static_lock.unlock();
    }

    ~mapNormalItems()
    {
        xr_delete(items);
        xr_delete(trees);

        //static_lock.lock();
        //instance_cnt--;
        //static_lock.unlock();
    }
};

using mapNormal_T = xr_fixed_map<SPass*, mapNormalItems>;
using mapNormalPasses_T = mapNormal_T[SHADER_PASSES_MAX];

// MATRIX
using mapMatrixDirect = xr_vector<_MatrixItem>;

class mapMatrixItems
{
public:
    //static u32 instance_cnt;

    float ssa{};
    mapMatrixDirect* items{};

    mapMatrixItems()
    {
        items = xr_new<mapMatrixDirect>();

        //static_lock.lock();
        //instance_cnt++;
        //static_lock.unlock();
    }

    ~mapMatrixItems()
    {
        xr_delete(items);

        //static_lock.lock();
        //instance_cnt--;
        //static_lock.unlock();
    }
};

using mapMatrix_T = xr_fixed_map<SPass*, mapMatrixItems>;
using mapMatrixPasses_T = mapMatrix_T[SHADER_PASSES_MAX];

// Top level
using mapSorted_T = xr_fixed_map<float, _MatrixItemS>;
using mapLOD_T = xr_fixed_map<float, _LodItem>;
} // namespace R_dsgraph
