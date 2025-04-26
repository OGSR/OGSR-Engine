#include "stdafx.h"

#include "blender_fluid.h"

#ifdef DX10_FLUID_ENABLE

#include "dx103DFluidManager.h"

#include "../../xrRender/dxRenderDeviceRender.h"

#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"
#include "dx103DFluidRenderer.h"
#include "dx103DFluidObstacles.h"
#include "dx103DFluidEmitters.h"

dx103DFluidManager FluidManager;

namespace
{
// For project, advect
shared_str strModulate;
shared_str strEpsilon;
// For confinement, advect
shared_str strTimeStep;
shared_str strForward;
shared_str strHalfVolumeDim;

shared_str strGravityBuoyancy;

} // namespace

LPCSTR dx103DFluidManager::m_pEngineTextureNames[NUM_RENDER_TARGETS] = {
    "$user$Texture_velocity1", //	RENDER_TARGET_VELOCITY1 = 0,
    "$user$Texture_color_out", //	RENDER_TARGET_COLOR,	//	Swap with object's
    "$user$Texture_obstacles", //	RENDER_TARGET_OBSTACLES,
    "$user$Texture_obstvelocity", //	RENDER_TARGET_OBSTVELOCITY,
    "$user$Texture_tempscalar", //	RENDER_TARGET_TEMPSCALAR,
    "$user$Texture_tempvector", //	 RENDER_TARGET_TEMPVECTOR,
    "$user$Texture_velocity0", //	RENDER_TARGET_VELOCITY0 = NUM_OWN_RENDER_TARGETS,	//	For textures generated from local data
    "$user$Texture_pressure", //	RENDER_TARGET_PRESSURE,
    "$user$Texture_color", //	RENDER_TARGET_COLOR_IN,
};

LPCSTR dx103DFluidManager::m_pShaderTextureNames[NUM_RENDER_TARGETS] = {
    "Texture_velocity1", //	RENDER_TARGET_VELOCITY1 = 0,
    "Texture_color_out", //	RENDER_TARGET_COLOR,	//	Swap with object's
    "Texture_obstacles", //	RENDER_TARGET_OBSTACLES,
    "Texture_obstvelocity", //	RENDER_TARGET_OBSTVELOCITY,
    "Texture_tempscalar", //	RENDER_TARGET_TEMPSCALAR,
    "Texture_tempvector", //	 RENDER_TARGET_TEMPVECTOR,
    "Texture_velocity0", //	RENDER_TARGET_VELOCITY0 = NUM_OWN_RENDER_TARGETS,	//	For textures generated from local data
    "Texture_pressure", //	RENDER_TARGET_PRESSURE,
    "Texture_color", //	RENDER_TARGET_COLOR_IN,
};

dx103DFluidManager::dx103DFluidManager()
    : m_bInited(false),
      m_nIterations(6), m_bUseBFECC(true),
      m_fSaturation(0.78f), m_bAddDensity(true),
      m_fImpulseSize(0.15f), m_fConfinementScale(0.0f),
      m_fDecay(1.0f),
        m_iTextureWidth(0),
        m_iTextureHeight(0),
        m_iTextureDepth(0),
        m_pGrid(nullptr),
        m_pRenderer(nullptr),
        m_pObstaclesHandler(nullptr),
        m_pEmittersHandler(nullptr)
{
    ZeroMemory(pRenderTargetViews, sizeof(pRenderTargetViews));

    RenderTargetFormats[RENDER_TARGET_VELOCITY1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    RenderTargetFormats[RENDER_TARGET_COLOR] = DXGI_FORMAT_R16_FLOAT;
    RenderTargetFormats[RENDER_TARGET_OBSTACLES] = DXGI_FORMAT_R8_UNORM;
    RenderTargetFormats[RENDER_TARGET_OBSTVELOCITY] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    RenderTargetFormats[RENDER_TARGET_TEMPSCALAR] = DXGI_FORMAT_R16_FLOAT;
    RenderTargetFormats[RENDER_TARGET_TEMPVECTOR] = DXGI_FORMAT_R16G16B16A16_FLOAT;
}

dx103DFluidManager::~dx103DFluidManager() { Destroy(); }

void dx103DFluidManager::Initialize(int width, int height, int depth)
{
    strModulate = "modulate";
    strEpsilon = "epsilon";
    strTimeStep = "timestep";
    strForward = "forward";
    strHalfVolumeDim = "halfVolumeDim";
    strGravityBuoyancy = "GravityBuoyancy";

    Destroy();

    m_iTextureWidth = width;
    m_iTextureHeight = height;
    m_iTextureDepth = depth;

    InitShaders();

    D3D_TEXTURE3D_DESC desc;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;

    D3D_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory(&SRVDesc, sizeof(SRVDesc));
    SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
    SRVDesc.Texture3D.MipLevels = 1;
    SRVDesc.Texture3D.MostDetailedMip = 0;

    for (int rtIndex = 0; rtIndex < NUM_RENDER_TARGETS; rtIndex++)
    {
        PrepareTexture(rtIndex);
        pRenderTargetViews[rtIndex] = nullptr;
    }

    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
    {
        desc.Format = RenderTargetFormats[rtIndex];
        SRVDesc.Format = RenderTargetFormats[rtIndex];
        CreateRTTextureAndViews(rtIndex, desc);
    }

    Reset();

    m_pGrid = xr_new<dx103DFluidGrid>();

    m_pGrid->Initialize(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth);

    m_pRenderer = xr_new<dx103DFluidRenderer>();
    m_pRenderer->Initialize(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth);

    m_pObstaclesHandler = xr_new<dx103DFluidObstacles>(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth, m_pGrid);

    m_pEmittersHandler = xr_new<dx103DFluidEmitters>(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth, m_pGrid);

    m_bInited = true;

    //	Create and grid and renderer here
    // grid = new Grid( m_pD3DDevice );
    // renderer = new VolumeRenderer( m_pD3DDevice );
}

void dx103DFluidManager::Destroy()
{
    if (!m_bInited)
        return;

    //	Destroy grid and renderer here
    xr_delete(m_pEmittersHandler);
    xr_delete(m_pObstaclesHandler);
    xr_delete(m_pRenderer);
    xr_delete(m_pGrid);
    // grid = new Grid( m_pD3DDevice );
    // renderer = new VolumeRenderer( m_pD3DDevice );

    // for(int rtIndex=0; rtIndex<NUM_OWN_RENDER_TARGETS; rtIndex++)
    for (int rtIndex = 0; rtIndex < NUM_RENDER_TARGETS; rtIndex++)
        DestroyRTTextureAndViews(rtIndex);

    DestroyShaders();

    m_bInited = false;
}

void dx103DFluidManager::InitShaders()
{
    {
        CBlender_fluid_advect Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 4; ++i)
            m_SimulationTechnique[SS_Advect + i] = shader->E[i];
    }

    {
        CBlender_fluid_advect_velocity Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 2; ++i)
            m_SimulationTechnique[SS_AdvectVel + i] = shader->E[i];
    }

    {
        CBlender_fluid_simulate Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 5; ++i)
            m_SimulationTechnique[SS_Vorticity + i] = shader->E[i];
    }
}

void dx103DFluidManager::DestroyShaders()
{
    for (auto& i : m_SimulationTechnique)
    {
        //	Release shader's element.
        i = nullptr;
    }
}

void dx103DFluidManager::PrepareTexture(int rtIndex) { pRTTextures[rtIndex] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(m_pEngineTextureNames[rtIndex]); }

void dx103DFluidManager::CreateRTTextureAndViews(int rtIndex, D3D_TEXTURE3D_DESC TexDesc)
{
    //	Resources must be already released by Destroy().

    ID3DTexture3D* pRT;

    // Create the texture
    CHK_DX(HW.pDevice->CreateTexture3D(&TexDesc, NULL, &pRT));
    // Create the render target view
    D3D_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = TexDesc.Format;
    DescRT.ViewDimension = D3D_RTV_DIMENSION_TEXTURE3D;
    DescRT.Texture3D.FirstWSlice = 0;
    DescRT.Texture3D.MipSlice = 0;
    DescRT.Texture3D.WSize = TexDesc.Depth;

    CHK_DX(HW.pDevice->CreateRenderTargetView(pRT, &DescRT, &pRenderTargetViews[rtIndex]));

    pRTTextures[rtIndex]->surface_set(pRT);

    //	CTexture owns ID3DxxTexture3D interface
    pRT->Release();
}

void dx103DFluidManager::DestroyRTTextureAndViews(int rtIndex)
{
    // pRTTextures[rtIndex]->surface_set(0);
    pRTTextures[rtIndex] = nullptr;
    _RELEASE(pRenderTargetViews[rtIndex]);
}

void dx103DFluidManager::Reset() const
{
    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
    {
        RCache.ClearRT(pRenderTargetViews[rtIndex], {}); // black
    }
}

void dx103DFluidManager::Update(CBackend& cmd_list, dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT_CTX(cmd_list, simulate_fluid);

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();
    const bool bSimulateFire = (VolumeSettings.m_SimulationType == dx103DFluidData::ST_FIRE);

    AttachFluidData(cmd_list, FluidData);

    // All drawing will take place to a viewport with the dimensions of a 3D texture slice
    D3D_VIEWPORT rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0.0f;
    rtViewport.MaxDepth = 1.0f;

    rtViewport.Width = (float)m_iTextureWidth;
    rtViewport.Height = (float)m_iTextureHeight;

    cmd_list.SetViewport(rtViewport);

    cmd_list.set_ZB(nullptr);

    UpdateObstacles(cmd_list, FluidData, timestep);

    // Set vorticity confinment and decay parameters
    if (m_bUseBFECC)
    {
        if (bSimulateFire)
        {
            m_fConfinementScale = 0.03f;
            m_fDecay = 0.9995f;
        }
        else
        {
            m_fConfinementScale = 0.06f;
            m_fDecay = 0.994f;
        }
    }
    else
    {
        m_fConfinementScale = 0.12f;
        m_fDecay = 0.9995f;
    }

    m_fConfinementScale = VolumeSettings.m_fConfinementScale;
    m_fDecay = VolumeSettings.m_fDecay;

    if (m_bUseBFECC)
        AdvectColorBFECC(cmd_list, timestep, bSimulateFire);
    else
        AdvectColor(cmd_list, timestep, bSimulateFire);

    AdvectVelocity(cmd_list, timestep, VolumeSettings.m_fGravityBuoyancy);

    ApplyVorticityConfinement(cmd_list, timestep);

    ApplyExternalForces(cmd_list, FluidData, timestep);

    ComputeVelocityDivergence(cmd_list, timestep);

    ComputePressure(cmd_list, timestep);

    ProjectVelocity(cmd_list, timestep);

    DetachAndSwapFluidData(cmd_list, FluidData);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0, nullptr, nullptr, pTarget->rt_Base_Depth->pZRT[cmd_list.context_id]); // LDR RT
    
    RImplementation.rmNormal(cmd_list);
}

void dx103DFluidManager::AttachFluidData(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, AttachFluidData);

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        ID3DTexture3D* pT = FluidData.GetTexture((dx103DFluidData::eVolumePrivateRT)i);
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(pT);
        _RELEASE(pT);

        VERIFY(!pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i]);
        pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i] = FluidData.GetView((dx103DFluidData::eVolumePrivateRT)i);
    }
}

void dx103DFluidManager::DetachAndSwapFluidData(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, DetachAndSwapFluidData);

    ID3DTexture3D* pTTarg = (ID3DTexture3D*)pRTTextures[RENDER_TARGET_COLOR]->surface_get();
    ID3DTexture3D* pTSrc = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    FluidData.SetTexture(dx103DFluidData::VP_COLOR, pTTarg);
    pRTTextures[RENDER_TARGET_COLOR]->surface_set(pTSrc);
    _RELEASE(pTTarg);
    _RELEASE(pTSrc);

    ID3DRenderTargetView* pV = FluidData.GetView(dx103DFluidData::VP_COLOR);
    FluidData.SetView(dx103DFluidData::VP_COLOR, pRenderTargetViews[RENDER_TARGET_COLOR]);
    _RELEASE(pRenderTargetViews[RENDER_TARGET_COLOR]);
    pRenderTargetViews[RENDER_TARGET_COLOR] = pV;

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(nullptr);
        _RELEASE(pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i]);
    }
}

void dx103DFluidManager::AdvectColorBFECC(CBackend& cmd_list, float timestep, bool bTeperature)
{
    PIX_EVENT_CTX(cmd_list, AdvectColorBFECC);

    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], {});
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], {});

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_Advect]);
    // Advect forward to get \phi^(n+1)
    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( NULL );
    // TimeStepShaderVariable->SetFloat(timestep);
    cmd_list.set_c(strTimeStep, timestep);
    // ModulateShaderVariable->SetFloat(1.0f);
    cmd_list.set_c(strModulate, 1.0f);
    // ForwardShaderVariable->SetFloat(1.0f);
    cmd_list.set_c(strForward, 1.0f);
    // SetRenderTarget( RENDER_TARGET_TEMPVECTOR );
    // TechniqueAdvect->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPVECTOR] );

    // Advect back to get \bar{\phi}
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
    ref_selement AdvectElement;
    if (bTeperature)
        AdvectElement = m_SimulationTechnique[SS_AdvectTemp];
    else
        AdvectElement = m_SimulationTechnique[SS_Advect];

    cmd_list.set_Element(AdvectElement);
    // pShaderResourceVariables[RENDER_TARGET_TEMPSCALAR]->SetResource( NULL );
    // pShaderResourceVariables[RENDER_TARGET_COLOR0]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPVECTOR] );
    //	Overwrite RENDER_TARGET_COLOR0 with RENDER_TARGET_TEMPVECTOR
    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strColorName(m_pEngineTextureNames[RENDER_TARGET_COLOR_IN]);
    const STextureList* _T = &*(AdvectElement->passes[0]->T);
    const u32 dwTextureStage = _T->find_texture_stage(strColorName);
    //	This will be overritten by the next technique.
    //	Otherwise we had to reset current texture list manually.
    pRTTextures[RENDER_TARGET_TEMPVECTOR]->bind(cmd_list, dwTextureStage);

    // TimeStepShaderVariable->SetFloat(timestep);
    cmd_list.set_c(strTimeStep, timestep);
    // ModulateShaderVariable->SetFloat(1.0f);
    cmd_list.set_c(strModulate, 1.0f);
    // ForwardShaderVariable->SetFloat(-1.0);
    cmd_list.set_c(strForward, -1.0f);
    // SetRenderTarget( RENDER_TARGET_TEMPSCALAR );
    // TechniqueAdvect->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_TEMPSCALAR]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPSCALAR] );

    // Advect forward but use the BFECC advection shader which
    //  uses both \phi and \bar{\phi} as source quantity
    //  (specifically, (3/2)\phi^n - (1/2)\bar{\phi})
    // if(ColorTextureNumber == 0)
    //{
    //	pShaderResourceVariables[RENDER_TARGET_COLOR1]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_COLOR1] );
    //	SetRenderTarget( RENDER_TARGET_COLOR0 );
    //}
    // else
    //{
    //	pShaderResourceVariables[RENDER_TARGET_COLOR0]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_COLOR0] );
    //	SetRenderTarget( RENDER_TARGET_COLOR1 );
    //}
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectBFECCTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectBFECC]);

    // D3DXVECTOR3 halfVol( grid->dim[0]/2.0f, grid->dim[1]/2.0f, grid->dim[2]/2.0f );
    // HalfVolumeDimShaderVariable->SetFloatVector( (float*)&halfVol);
    Fvector4 halfVol;
    halfVol.set((float)m_iTextureWidth / 2.0f, (float)m_iTextureHeight / 2.0f, (float)m_iTextureDepth / 2.0f, 0.0f);
    cmd_list.set_c(strHalfVolumeDim, halfVol);
    // ModulateShaderVariable->SetFloat(decay);
    cmd_list.set_c(strModulate, m_fDecay);
    // ForwardShaderVariable->SetFloat(1.0);
    cmd_list.set_c(strForward, 1.0f);
    // TechniqueAdvectBFECC->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_TEMPSCALAR]->SetResource( NULL );
    //  Apply the technique again so that the RENDER_TARGET_TEMPSCALAR shader resource is unbound
    // TechniqueAdvectBFECC->GetPassByIndex(0)->Apply(0);*/
}

void dx103DFluidManager::AdvectColor(CBackend& cmd_list, float timestep, bool bTeperature)
{
    PIX_EVENT_CTX(cmd_list, AdvectColor);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);

    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_Advect]);

    // TimeStepShaderVariable->SetFloat(timestep);
    cmd_list.set_c(strTimeStep, timestep);
    // ModulateShaderVariable->SetFloat(1.0f);
    cmd_list.set_c(strModulate, 1.0f);
    // ForwardShaderVariable->SetFloat(1.0);
    cmd_list.set_c(strForward, 1.0f);
    // ModulateShaderVariable->SetFloat(decay);
    cmd_list.set_c(strModulate, m_fDecay);
    // TechniqueAdvect->GetPassByIndex(0)->Apply(0);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::AdvectVelocity(CBackend& cmd_list, float timestep, float fGravity)
{
    PIX_EVENT_CTX(cmd_list, AdvectVelocity);

    // pShaderResourceVariables[RENDER_TARGET_VELOCITY1]->SetResource( NULL );
    // SetRenderTarget(RENDER_TARGET_VELOCITY1);
    //  Advect velocity by the fluid velocity
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);

    if (_abs(fGravity) < 0.000001)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectVel]);
    else
    {
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectVelGravity]);
        cmd_list.set_c(strGravityBuoyancy, fGravity);
    }

    // TimeStepShaderVariable->SetFloat(timestep);
    cmd_list.set_c(strTimeStep, timestep);
    // ModulateShaderVariable->SetFloat(1.0 );
    cmd_list.set_c(strModulate, 1.0f);
    // ForwardShaderVariable->SetFloat(1.0);
    cmd_list.set_c(strForward, 1.0f);
    // TechniqueAdvectVel->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_VELOCITY1]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_VELOCITY1] );
}

void dx103DFluidManager::ApplyVorticityConfinement(CBackend& cmd_list, float timestep)
{
    PIX_EVENT_CTX(cmd_list, ApplyVorticityConfinement);

    // Compute vorticity
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], {});

    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( NULL );
    // SetRenderTarget( RENDER_TARGET_TEMPVECTOR );
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    // TechniqueVorticity->GetPassByIndex(0)->Apply(0);
    cmd_list.set_Element(m_SimulationTechnique[SS_Vorticity]);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPVECTOR] );

    // Compute and apply vorticity confinement force
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Confinement]);
    // pShaderResourceVariables[RENDER_TARGET_VELOCITY1]->SetResource( NULL );
    // EpsilonShaderVariable->SetFloat(confinementScale);
    cmd_list.set_c(strEpsilon, m_fConfinementScale);
    // TimeStepShaderVariable->SetFloat(timestep);
    cmd_list.set_c(strTimeStep, timestep);
    // TechniqueConfinement->GetPassByIndex(0)->Apply(0);
    // SetRenderTarget( RENDER_TARGET_VELOCITY1 );
    //  Add the confinement force to the rest of the forces
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_VELOCITY1]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_VELOCITY1] );
}

void dx103DFluidManager::ApplyExternalForces(CBackend& cmd_list, const dx103DFluidData& FluidData, float timestep) const
{
    PIX_EVENT_CTX(cmd_list, ApplyExternalForces);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    m_pEmittersHandler->RenderDensity(cmd_list, FluidData);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    m_pEmittersHandler->RenderVelocity(cmd_list, FluidData);
}

void dx103DFluidManager::ComputeVelocityDivergence(CBackend& cmd_list, float timestep)
{
    PIX_EVENT_CTX(cmd_list, ComputeVelocityDivergence);

    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], {}); // black

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Divergence]);

    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( NULL );
    // SetRenderTarget( RENDER_TARGET_TEMPVECTOR );
    // TechniqueDivergence->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_TEMPVECTOR]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPVECTOR] );
}

void dx103DFluidManager::ComputePressure(CBackend& cmd_list, float timestep)
{
    PIX_EVENT_CTX(cmd_list, ComputePressure);

    RCache.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], {});

    // ID3DxxTexture3D	*pTemp = (ID3DxxTexture3D*) pRTTextures[RENDER_TARGET_TEMPSCALAR]->surface_get();
    // ID3DxxTexture3D	*pPressure = (ID3DxxTexture3D*) pRTTextures[RENDER_TARGET_PRESSURE]->surface_get();

    // unbind this variable from the other technique that may have used it
    // pShaderResourceVariables[RENDER_TARGET_TEMPSCALAR]->SetResource( NULL );
    // TechniqueAdvectBFECC->GetPassByIndex(0)->Apply(0);
    cmd_list.set_RT(nullptr);
    ref_selement CurrentTechnique = m_SimulationTechnique[SS_Jacobi];
    cmd_list.set_Element(CurrentTechnique);

    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strPressureName(m_pEngineTextureNames[RENDER_TARGET_PRESSURE]);
    const STextureList* _T = &*(CurrentTechnique->passes[0]->T);
    const u32 dwTextureStage = _T->find_texture_stage(strPressureName);

    for (int iteration = 0; iteration < m_nIterations / 2.0; iteration++)
    {
        // pShaderResourceVariables[RENDER_TARGET_PRESSURE]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_PRESSURE] );
        // TechniqueJacobi->GetPassByIndex(0)->Apply(0);
        // SetRenderTarget( RENDER_TARGET_TEMPSCALAR );
        cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
        pRTTextures[RENDER_TARGET_PRESSURE]->bind(cmd_list, dwTextureStage);
        m_pGrid->DrawSlices(cmd_list);
        // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
        // cmd_list.set_RT(0);

        // pShaderResourceVariables[RENDER_TARGET_PRESSURE]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_TEMPSCALAR] );
        // TechniqueJacobi->GetPassByIndex(0)->Apply(0);
        // SetRenderTarget( RENDER_TARGET_PRESSURE );
        cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_PRESSURE]);
        pRTTextures[RENDER_TARGET_TEMPSCALAR]->bind(cmd_list, dwTextureStage);
        m_pGrid->DrawSlices(cmd_list);
        // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
        // cmd_list.set_RT(0);
    }

    // pShaderResourceVariables[RENDER_TARGET_PRESSURE]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_PRESSURE] );
    // TechniqueJacobi->GetPassByIndex(0)->Apply(0);
}

void dx103DFluidManager::ProjectVelocity(CBackend& cmd_list, float timestep)
{
    PIX_EVENT_CTX(cmd_list, ProjectVelocity);

    // pShaderResourceVariables[RENDER_TARGET_VELOCITY0]->SetResource( NULL );
    // SetRenderTarget( RENDER_TARGET_VELOCITY0 );
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY0]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Project]);
    // ModulateShaderVariable->SetFloat(1.0f);
    cmd_list.set_c(strModulate, 1.0f);
    // TechniqueProject->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices(cmd_list);
    // m_pD3DDevice->OMSetRenderTargets(0, NULL, NULL);
    // pShaderResourceVariables[RENDER_TARGET_VELOCITY0]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_VELOCITY0] );
}

void dx103DFluidManager::RenderFluid(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    //	return;
    PIX_EVENT_CTX(cmd_list, render_fluid);

    //	Bind input texture
    ID3DTexture3D* pT = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(pT);
    _RELEASE(pT);

    //	Do rendering
    m_pRenderer->Draw(cmd_list, FluidData);

    //	Unbind input texture
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(nullptr);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0, nullptr, nullptr, pTarget->rt_Base_Depth->pZRT[cmd_list.context_id]); // LDR RT

    RImplementation.rmNormal(cmd_list);
}

void dx103DFluidManager::UpdateObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT_CTX(cmd_list, Fluid_update_obstacles);
    //	Reset data
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_OBSTACLES], {}); // black
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], {}); // black

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTACLES], 0);
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], 1);

    m_pObstaclesHandler->ProcessObstacles(cmd_list, FluidData, timestep);

    //	Just reset render targets:
    //	later only rt 0 will be reassigned so rt1
    //	would be bound all the time
    //	Reset to avoid confusion.
    cmd_list.set_RT(nullptr, 0);
    cmd_list.set_RT(nullptr, 1);
}

//	Allow real-time config reload
#ifdef DEBUG
void dx103DFluidManager::RegisterFluidData(dx103DFluidData* pData, const xr_string& SectionName)
{
    int iDataNum = m_lstFluidData.size();

    int i;

    for (i = 0; i < iDataNum; ++i)
    {
        if (m_lstFluidData[i] == pData)
            break;
    }

    if (iDataNum == i)
    {
        m_lstFluidData.push_back(pData);
        m_lstSectionNames.push_back(SectionName);
    }
    else
    {
        m_lstSectionNames[i] = SectionName;
    }
}

void dx103DFluidManager::DeregisterFluidData(dx103DFluidData* pData)
{
    int iDataNum = m_lstFluidData.size();

    int i;

    for (i = 0; i < iDataNum; ++i)
    {
        if (m_lstFluidData[i] == pData)
            break;
    }

    if (i != iDataNum)
    {
        xr_vector<xr_string>::iterator it1 = m_lstSectionNames.begin();
        xr_vector<dx103DFluidData*>::iterator it2 = m_lstFluidData.begin();
        // it1.advance(i);
        it1 += i;
        it2 += i;

        m_lstSectionNames.erase(it1);
        m_lstFluidData.erase(it2);
    }
}

void dx103DFluidManager::UpdateProfiles()
{
    int iDataNum = m_lstFluidData.size();

    int i;

    for (i = 0; i < iDataNum; ++i)
    {
        m_lstFluidData[i]->ReparseProfile(m_lstSectionNames[i]);
    }
}

#endif //	DEBUG

#endif
