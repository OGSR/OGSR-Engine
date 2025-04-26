#include "stdafx.h"

#include "blender_fluid.h"

#ifdef DX10_FLUID_ENABLE

#include "dx103DFluidEmitters.h"
#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"

namespace
{
shared_str strImpulseSize;
shared_str strImpulseCenter;
shared_str strSplatColor;
} // namespace

dx103DFluidEmitters::dx103DFluidEmitters(int gridWidth, int gridHeight, int gridDepth, dx103DFluidGrid* pGrid) : m_pGrid(pGrid)
{
    strImpulseSize = "size";
    strImpulseCenter = "center";
    strSplatColor = "splatColor";

    m_vGridDim[0] = float(gridWidth);
    m_vGridDim[1] = float(gridHeight);
    m_vGridDim[2] = float(gridDepth);

    InitShaders();
}

dx103DFluidEmitters::~dx103DFluidEmitters()
{
    DestroyShaders();

    m_pGrid = nullptr;
}

void dx103DFluidEmitters::InitShaders()
{
    {
        CBlender_fluid_emitter Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 1; ++i)
            m_EmitterTechnique[ET_SimpleGausian + i] = shader->E[i];
    }
}

void dx103DFluidEmitters::DestroyShaders()
{
    for (auto& i : m_EmitterTechnique)
    {
        //	Release shader's element.
        i = nullptr;
    }
}

void dx103DFluidEmitters::RenderDensity(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    const xr_vector<CEmitter>& Emitters = FluidData.GetEmittersList();
    const u32 iNumEmitters = Emitters.size();

    for (u32 i = 0; i < iNumEmitters; ++i)
    {
        const CEmitter& Emitter = Emitters[i];
        if (Emitter.m_bApplyDensity)
            ApplyDensity(cmd_list, Emitter);
    }
}

void dx103DFluidEmitters::RenderVelocity(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    const xr_vector<CEmitter>& Emitters = FluidData.GetEmittersList();
    const u32 iNumEmitters = Emitters.size();

    for (u32 i = 0; i < iNumEmitters; ++i)
    {
        const CEmitter& Emitter = Emitters[i];
        if (Emitter.m_bApplyImpulse)
            ApplyVelocity(cmd_list, Emitter);
    }
}

void dx103DFluidEmitters::ApplyDensity(CBackend& cmd_list, const CEmitter& Emitter)
{
    // Draw gaussian ball of color
    cmd_list.set_Element(m_EmitterTechnique[ET_SimpleGausian]);

    const float t = Device.fTimeGlobal;

    const float fRadius = Emitter.m_fRadius;

    switch (Emitter.m_eType)
    {
    case ET_SimpleDraught:
        // fRadius += (0.1f - fRadius) * (1.0f + 0.5f * _sin( (1.0f/30.0f) * t * (2.0f * float(PI))) );
        // float fFactor = 1.0f + 0.5f * _sin(t * (2.0f * float(PI)) / 30 );
        // FlowVelocity.mul( fFactor );
        break;
    }

    // Color is the density of the smoke. We use a sinusoidal function of 't' to make it more interesting.
    // static float t = 0.0f;
    // t += 0.05f;
    const float fMiddleIntencity = 1;
    const float saturation = Emitter.m_fSaturation;
    FLOAT density = 1.5f * (((_sin(t * 1.5f + 2.0f * float(PI) / 3.0f) * 0.5f + 0.5f)) * saturation + fMiddleIntencity * (1.0f - saturation));
    density *= Emitter.m_fDensity;
    Fvector4 color;
    color.set(density, density, density, 1.0f);

    Fvector4 center;
    center.set(Emitter.m_vPosition.x, Emitter.m_vPosition.y, Emitter.m_vPosition.z, 0);

    cmd_list.set_c(strImpulseSize, fRadius);
    cmd_list.set_c(strSplatColor, color);
    cmd_list.set_c(strImpulseCenter, center);

    m_pGrid->DrawSlices(cmd_list);
}

//	TODO: DX10: Remove this hack
static float lilrand() { return (rand() / float(RAND_MAX) - 0.5f) * 5.0f; }

void dx103DFluidEmitters::ApplyVelocity(CBackend& cmd_list, const CEmitter& Emitter)
{
    // Draw gaussian ball of velocity
    cmd_list.set_Element(m_EmitterTechnique[ET_SimpleGausian]);

    const float fRadius = Emitter.m_fRadius;
    Fvector FlowVelocity = Emitter.m_vFlowVelocity;

    const float t = Device.fTimeGlobal;

    switch (Emitter.m_eType)
    {
    case ET_SimpleDraught:
        // fRadius += (0.1f - fRadius) * (1.0f + 0.5f * _sin( (1.0f/30.0f) * t * (2.0f * float(PI))) );
        // float fFactor = 1.0f + 0.5f * _sin(t * (2.0f * float(PI)) / 10 );
        float fPeriod = Emitter.m_DraughtParams.m_fPeriod;
        if (fPeriod < 0.0001f)
            fPeriod = 0.0001f;
        const float fFactor = 1.0f + Emitter.m_DraughtParams.m_fAmp * _sin((t + Emitter.m_DraughtParams.m_fPhase) * (2.0f * float(PI)) / fPeriod);
        FlowVelocity.mul(fFactor);
        break;
    }

    cmd_list.set_c(strImpulseSize, fRadius);

    // Color in this case is the initial velocity given to the emitted smoke
    Fvector4 color;
    color.set(FlowVelocity.x, FlowVelocity.y, FlowVelocity.z, 0);
    cmd_list.set_c(strSplatColor, color);

    Fvector4 center;
    center.set(Emitter.m_vPosition.x + lilrand(), Emitter.m_vPosition.y + lilrand(), Emitter.m_vPosition.z + lilrand(), 0);
    cmd_list.set_c(strImpulseCenter, center);

    m_pGrid->DrawSlices(cmd_list);
}

#endif
