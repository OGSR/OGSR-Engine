#pragma once

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

    void ProcessObstacles(const dx103DFluidData& FluidData, float timestep);

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

    void ProcessStaticObstacles(const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid);
    void ProcessDynamicObstacles(const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid, float timestep);

    //	This technique renders several objects.
    void RenderPhysicsShell(IPhysicsShell* pShell, const Fmatrix& WorldToFluid, float timestep);
    void RenderPhysicsElement(IPhysicsElement& Element, const Fmatrix& WorldToFluid, float timestep);

    void RenderStaticOOBB(const Fmatrix& Transform);
    void RenderDynamicOOBB(IPhysicsGeometry& Geometry, const Fmatrix& WorldToFluid, float timestep);

private:
    Fvector3 m_vGridDim{};

    ref_selement m_ObstacleTechnique[OS_NumShaders];

    dx103DFluidGrid* m_pGrid{};

    //	Cache vectors to avoid memory reallocations
    //	TODO: DX10: Reserve memory on object creation
    xr_vector<ISpatial*> m_lstRenderables;
    xr_vector<IPhysicsShell*> m_lstShells;
    xr_vector<IPhysicsElement*> m_lstElements;
};
