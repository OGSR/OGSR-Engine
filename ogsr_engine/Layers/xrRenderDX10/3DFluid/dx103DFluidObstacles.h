#pragma once

#ifdef DX10_FLUID_ENABLE

class dx103DFluidGrid;
class dx103DFluidData;

class IPhysicsShell;
class IPhysicsElement;
class IPhysicsGeometry;

class dx103DFluidObstacles
{
public:
    dx103DFluidObstacles(int gridWidth, int gridHeight, int gridDepth, dx103DFluidGrid* pGrid);
    ~dx103DFluidObstacles();

    void ProcessObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, float timestep);

private:
    enum ObstacleShader
    {
        OS_OOBB = 0,
        OS_DynamicOOBB,
        OS_NumShaders
    };

private:
    void InitShaders();
    void DestroyShaders();

    void ProcessStaticObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid);
    void ProcessDynamicObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid, float timestep);

    //	This technique renders several objects.
    void RenderPhysicsShell(CBackend& cmd_list, IPhysicsShell* pShell, const Fmatrix& WorldToFluid, float timestep);
    void RenderPhysicsElement(CBackend& cmd_list, IPhysicsElement& Element, const Fmatrix& WorldToFluid, float timestep);

    void RenderStaticOOBB(CBackend& cmd_list, const Fmatrix& Transform) const;
    void RenderDynamicOOBB(CBackend& cmd_list, IPhysicsGeometry& Geometry, const Fmatrix& WorldToFluid, float timestep);

private:
    Fvector3 m_vGridDim;

    ref_selement m_ObstacleTechnique[OS_NumShaders];

    dx103DFluidGrid* m_pGrid;

    //	Cache vectors to avoid memory reallocations
    //	TODO: DX10: Reserve memory on object creation
    xr_vector<ISpatial*> m_lstRenderables;
    xr_vector<IPhysicsShell*> m_lstShells;
    xr_vector<IPhysicsElement*> m_lstElements;
};

#endif //	dx103DFluidObstacles_included
