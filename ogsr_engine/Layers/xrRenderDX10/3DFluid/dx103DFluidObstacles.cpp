#include "stdafx.h"

#include "blender_fluid.h"

#ifdef DX10_FLUID_ENABLE

#include "dx103DFluidObstacles.h"

#include "../../../xr_3da/xr_object.h"

#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"

#include "../xr_3da/IPhysicsDefinitions.h"

namespace
{
//	For OOBB
shared_str strOOBBClipPlane;

//	For velocity calculation
shared_str strWorldToLocal;
shared_str strLocalToWorld;
shared_str strMassCenter;
shared_str strOOBBWorldAngularVelocity;
shared_str strOOBBWorldTranslationVelocity;

Fvector4 UnitClipPlanes[] = {
    {-1.f, 0.0f, 0.0f, 0.5f}, //
    {1.f, 0.0f, 0.0f, 0.5f}, //
    {0.0f, -1.f, 0.0f, 0.5f}, //	Top
    {0.0f, 1.f, 0.0f, 0.5f}, //	Bottom
    {0.0f, 0.0f, -1.f, 0.5f}, //
    {0.0f, 0.0f, 1.f, 0.5f}, //
};

} // namespace

dx103DFluidObstacles::dx103DFluidObstacles(int gridWidth, int gridHeight, int gridDepth, dx103DFluidGrid* pGrid) : m_pGrid(pGrid)
{
    VERIFY(m_pGrid);

	strOOBBClipPlane = "OOBBClipPlane";
    strWorldToLocal = "WorldToLocal";
    strLocalToWorld = "LocalToWorld";
    strMassCenter = "MassCenter";
    strOOBBWorldAngularVelocity = "OOBBWorldAngularVelocity";
    strOOBBWorldTranslationVelocity = "OOBBWorldTranslationVelocity";

    m_vGridDim[0] = float(gridWidth);
    m_vGridDim[1] = float(gridHeight);
    m_vGridDim[2] = float(gridDepth);

    InitShaders();
}

dx103DFluidObstacles::~dx103DFluidObstacles()
{
    DestroyShaders();

    m_pGrid = nullptr;
}

void dx103DFluidObstacles::InitShaders()
{
    {
        CBlender_fluid_obst Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 2; ++i)
            m_ObstacleTechnique[OS_OOBB + i] = shader->E[i];
    }
}

void dx103DFluidObstacles::DestroyShaders()
{
    for (auto& i : m_ObstacleTechnique)
    {
        //	Release shader's element.
        i = nullptr;
    }
}

void dx103DFluidObstacles::ProcessObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT_CTX(cmd_list, ProcessObstacles);

    //	Prepare world-space to grid transform
    Fmatrix WorldToFluid;
    {
        Fmatrix InvFluidTranform;
        Fmatrix Scale;
        Fmatrix Translate;
        Fmatrix TranslateScale;

        Scale.scale(m_vGridDim.x, -(m_vGridDim.y), m_vGridDim.z);
        Translate.translate(0.5, -0.5, 0.5);
        //	Actually it is mul(Translate, Scale).
        //	Our matrix multiplication is not correct.
        TranslateScale.mul(Scale, Translate);
        InvFluidTranform.invert(FluidData.GetTransform());
        WorldToFluid.mul(TranslateScale, InvFluidTranform);
    }

    ProcessDynamicObstacles(cmd_list, FluidData, WorldToFluid, timestep);

    //	Render static obstacles last
    //	to override speed where bounding shapes of dynamic and
    //	static objects intersect.
    ProcessStaticObstacles(cmd_list, FluidData, WorldToFluid);
}

void dx103DFluidObstacles::RenderStaticOOBB(CBackend& cmd_list, const Fmatrix& Transform) const
{
    PIX_EVENT_CTX(cmd_list, RenderObstacle);

    //	Shader must be already set up!
    Fmatrix InvTransform;
    Fmatrix ClipTransform;
    InvTransform.invert(Transform);
    ClipTransform.transpose(InvTransform);

    for (int i = 0; i < 6; ++i)
    {
        Fvector4 TransformedPlane;
        ClipTransform.transform(TransformedPlane, UnitClipPlanes[i]);
        TransformedPlane.normalize_as_plane();
        cmd_list.set_ca(strOOBBClipPlane, i, TransformedPlane);
    }

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidObstacles::ProcessStaticObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid)
{
    cmd_list.set_Element(m_ObstacleTechnique[OS_OOBB]);

    const xr_vector<Fmatrix>& Obstacles = FluidData.GetObstaclesList();
    const int iObstNum = Obstacles.size();
    for (int i = 0; i < iObstNum; ++i)
    {
        Fmatrix Transform;
        Transform.mul(WorldToFluid, Obstacles[i]);

        RenderStaticOOBB(cmd_list, Transform);
    }
}

void dx103DFluidObstacles::ProcessDynamicObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, const Fmatrix& WorldToFluid, float timestep)
{
    m_lstRenderables.clear();
    m_lstShells.clear();
    m_lstElements.clear();

    Fbox box;
    box.min = Fvector3().set(-0.5f, -0.5f, -0.5f);
    box.max = Fvector3().set(0.5f, 0.5f, 0.5f);
    box.xform(FluidData.GetTransform());
    Fvector3 center;
    Fvector3 size;
    box.getcenter(center);
    box.getradius(size);

    // Traverse object database
    g_SpatialSpace->q_box(m_lstRenderables,
                          0, // ISpatial_DB::O_ORDERED,
                          STYPE_RENDERABLE, center, size);

    // Determine visibility for dynamic part of scene
    for (const auto& spatial : m_lstRenderables)
    {
        //	Can use to optimize invisible dynamic objects if necessary
        // CSector*	sector		= (CSector*)spatial->spatial.sector;
        // if	(0==sector)
        // continue;	// disassociated from S/P structure
        // if	(PortalTraverser.i_marker != sector->r_marker)
        // continue;	// inactive (untouched) sector

        // renderable
        // IRenderable*	renderable		= spatial->dcast_Renderable	();
        // if (0==renderable)				continue;					// unknown, but renderable object (r1_glow???)

        CObject* pObject = spatial->dcast_CObject();
        if (!pObject)
            continue;

        const IObjectPhysicsCollision* pCollision = pObject->physics_collision();
        if (!pCollision)
            continue;

        IPhysicsShell* pShell = pCollision->physics_shell();
        IPhysicsElement* pElement = pCollision->physics_character();
        if (pShell)
        {
            //	Push shell here
            m_lstShells.push_back(pShell);
        }
        else if (pElement)
        {
            m_lstElements.push_back(pElement);
        }
    }

    if (!(m_lstShells.size() || m_lstElements.size()))
        return;

    cmd_list.set_Element(m_ObstacleTechnique[OS_DynamicOOBB]);

    Fmatrix FluidToWorld;
    FluidToWorld.invert(WorldToFluid);

    cmd_list.set_c(strWorldToLocal, WorldToFluid);
    cmd_list.set_c(strLocalToWorld, FluidToWorld);

    const int iShellsNum = m_lstShells.size();
    for (int i = 0; i < iShellsNum; ++i)
    {
        RenderPhysicsShell(cmd_list, m_lstShells[i], WorldToFluid, timestep);
    }

    const int iElementsNum = m_lstElements.size();
    for (int i = 0; i < iElementsNum; ++i)
    {
        RenderPhysicsElement(cmd_list , * m_lstElements[i], WorldToFluid, timestep);
    }
}

//	TODO: DX10: Do it using instancing.
void dx103DFluidObstacles::RenderPhysicsShell(CBackend& cmd_list, IPhysicsShell* pShell, const Fmatrix& WorldToFluid, float timestep)
{
    const u16 iObstNum = pShell->get_ElementsNumber();
    for (u16 i = 0; i < iObstNum; ++i)
    {
        IPhysicsElement& Element = pShell->IElement(i);

        RenderPhysicsElement(cmd_list, Element, WorldToFluid, timestep);
    }
}

void dx103DFluidObstacles::RenderPhysicsElement(CBackend& cmd_list, IPhysicsElement& Element, const Fmatrix& WorldToFluid, float timestep)
{
    //	Shader must be already set up!
    const Fvector3& MassCenter3 = Element.mass_Center();
    Fvector3 AngularVelocity3;
    Fvector3 TranslationVelocity3;
    Element.get_AngularVel(AngularVelocity3);
    Element.get_LinearVel(TranslationVelocity3);

    Fvector4 MassCenter;
    Fvector4 AngularVelocity;
    Fvector4 TranslationVelocity;
    MassCenter.set(MassCenter3.x, MassCenter3.y, MassCenter3.z, 0.0f);
    AngularVelocity.set(AngularVelocity3.x, AngularVelocity3.y, AngularVelocity3.z, 0.0f);
    TranslationVelocity.set(TranslationVelocity3.x, TranslationVelocity3.y, TranslationVelocity3.z, 0.0f);

    float fVelocityScale;

    VERIFY(timestep != 0);

    fVelocityScale = 1 / timestep;

    //	Convert speed
    fVelocityScale /= 30.0f * 2.0f;

    //	Emphasize velocity influence on the fog
    // fVelocityScale *= 10;
    // fVelocityScale *= 4;	//	Good for the beginning
    fVelocityScale *= 6;

    AngularVelocity.mul(fVelocityScale);
    TranslationVelocity.mul(fVelocityScale);

    //	Emphasize velocity influence on the fog
    // TranslationVelocity.mul( 2.0f );

    cmd_list.set_c(strMassCenter, MassCenter);
    cmd_list.set_c(strOOBBWorldAngularVelocity, AngularVelocity);
    cmd_list.set_c(strOOBBWorldTranslationVelocity, TranslationVelocity);

    const int iShapeNum = Element.numberOfGeoms();

    for (u16 i = 0; i < iShapeNum; ++i)
    {
        if (Element.geometry(i)->collide_fluids())
            RenderDynamicOOBB(cmd_list, * Element.geometry(i), WorldToFluid, timestep);
    }
}

void dx103DFluidObstacles::RenderDynamicOOBB(CBackend& cmd_list, IPhysicsGeometry& Geometry, const Fmatrix& WorldToFluid, float timestep)
{
    PIX_EVENT_CTX(cmd_list, RenderDynamicObstacle);

    Fmatrix Transform;
    // Transform.mul(WorldToFluid, Element.XFORM());

    Fvector3 BoxSize;
    Fmatrix OOBBTransform;
    Geometry.get_Box(OOBBTransform, BoxSize);

    Transform.mul(WorldToFluid, OOBBTransform);

    //	Shader must be already set up!
    //	DynOOBBData must be already set up!
    Fmatrix InvTransform;
    Fmatrix ClipTransform;
    InvTransform.invert(Transform);
    ClipTransform.transpose(InvTransform);

    for (int i = 0; i < 6; ++i)
    {
        Fvector4 UpdatedPlane = UnitClipPlanes[i];
        UpdatedPlane.w *= BoxSize[i / 2];
        Fvector4 TransformedPlane;
        ClipTransform.transform(TransformedPlane, UpdatedPlane);
        TransformedPlane.normalize_as_plane();
        cmd_list.set_ca(strOOBBClipPlane, i, TransformedPlane);
    }

    m_pGrid->DrawSlices(cmd_list);
}

#endif
