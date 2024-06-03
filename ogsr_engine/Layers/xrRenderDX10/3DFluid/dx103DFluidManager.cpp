#include "stdafx.h"
#include "dx103DFluidManager.h"
#include "../../xrRender/dxRenderDeviceRender.h"
#include "dx103DFluidBlenders.h"
#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"
#include "dx103DFluidRenderer.h"
#include "dx103DFluidObstacles.h"
#include "dx103DFluidEmitters.h"

dx103DFluidManager FluidManager;

// For render call
// DrawTextureShaderVariable = pEffect->GetVariableByName( "textureNumber")->AsScalar();
constexpr const char* strDrawTexture("textureNumber");
// For project, advect
// ModulateShaderVariable = pEffect->GetVariableByName( "modulate")->AsScalar();
constexpr const char* strModulate("modulate");
// For confinement
// EpsilonShaderVariable = pEffect->GetVariableByName( "epsilon")->AsScalar();
constexpr const char* strEpsilon("epsilon");
// For confinement, advect
constexpr const char* strTimeStep("timestep");
// For advect BFECC
// ForwardShaderVariable = pEffect->GetVariableByName( "forward")->AsScalar();
constexpr const char* strForward("forward");
// HalfVolumeDimShaderVariable = pEffect->GetVariableByName( "halfVolumeDim")->AsVector();
constexpr const char* strHalfVolumeDim("halfVolumeDim");

constexpr const char* strGravityBuoyancy("GravityBuoyancy");

dx103DFluidManager::dx103DFluidManager() {}

dx103DFluidManager::~dx103DFluidManager() { Destroy(); }

void dx103DFluidManager::Initialize(int width, int height, int depth)
{
    // if (strstr(Core.Params,"-no_volumetric_fog"))
    /*if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;*/

    Destroy();

    m_iTextureWidth = width;
    m_iTextureHeight = height;
    m_iTextureDepth = depth;

    InitShaders();

    D3D_TEXTURE3D_DESC desc{};
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;

    D3D_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
    SRVDesc.Texture3D.MipLevels = 1;
    SRVDesc.Texture3D.MostDetailedMip = 0;

    for (int rtIndex = 0; rtIndex < NUM_RENDER_TARGETS; rtIndex++)
    {
        PrepareTexture(rtIndex);
        pRenderTargetViews[rtIndex] = 0;
    }

    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
    {
        desc.Format = dx103DFluidConsts::RenderTargetFormats[rtIndex];
        SRVDesc.Format = dx103DFluidConsts::RenderTargetFormats[rtIndex];
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
    for (int i = 0; i < SS_NumShaders; ++i)
    {
        //	Release shader's element.
        m_SimulationTechnique[i] = 0;
    }
}

void dx103DFluidManager::PrepareTexture(int rtIndex)
{
    pRTTextures[rtIndex] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(dx103DFluidConsts::m_pEngineTextureNames[rtIndex]);
}

void dx103DFluidManager::CreateRTTextureAndViews(int rtIndex, D3D_TEXTURE3D_DESC TexDesc)
{
    //	Resources must be already released by Destroy().

    ID3DTexture3D* pRT;

    // Create the texture
    CHK_DX(HW.pDevice->CreateTexture3D(&TexDesc, NULL, &pRT));
    // Create the render target view
    D3D_RENDER_TARGET_VIEW_DESC DescRT{};
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
    pRTTextures[rtIndex] = 0;
    _RELEASE(pRenderTargetViews[rtIndex]);
}

void dx103DFluidManager::Reset()
{
    constexpr float color[4] = {0, 0, 0, 0};

    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
    {
        HW.pContext->ClearRenderTargetView(pRenderTargetViews[rtIndex], color);
    }
}

void dx103DFluidManager::Update(dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT(simulate_fluid);

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();
    const bool bSimulateFire = (VolumeSettings.m_SimulationType == dx103DFluidData::ST_FIRE);

    AttachFluidData(FluidData);

    // All drawing will take place to a viewport with the dimensions of a 3D texture slice
    D3D_VIEWPORT rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0.0f;
    rtViewport.MaxDepth = 1.0f;

    rtViewport.Width = (float)m_iTextureWidth;
    rtViewport.Height = (float)m_iTextureHeight;

    HW.pContext->RSSetViewports(1, &rtViewport);

    RCache.set_ZB(0);

    UpdateObstacles(FluidData, timestep);

    // Set vorticity confinment and decay parameters
    /*if (m_bUseBFECC)
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
    }*/

    m_fConfinementScale = VolumeSettings.m_fConfinementScale;
    m_fDecay = VolumeSettings.m_fDecay;

    if (m_bUseBFECC)
        AdvectColorBFECC(timestep, bSimulateFire);
    else
        AdvectColor(timestep, bSimulateFire);

    AdvectVelocity(timestep, VolumeSettings.m_fGravityBuoyancy);

    ApplyVorticityConfinement(timestep);

    ApplyExternalForces(FluidData, timestep);

    ComputeVelocityDivergence(timestep);

    ComputePressure(timestep);

    ProjectVelocity(timestep);

    DetachAndSwapFluidData(FluidData);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    if (!RImplementation.o.dx10_msaa)
        pTarget->u_setrt(pTarget->rt_Generic_0, 0, 0, HW.pBaseZB); // LDR RT
    else
        pTarget->u_setrt(pTarget->rt_Generic_0_r, 0, 0, pTarget->rt_MSAADepth->pZRT); // LDR RT

    RImplementation.rmNormal();
}

void dx103DFluidManager::AttachFluidData(dx103DFluidData& FluidData)
{
    PIX_EVENT(AttachFluidData);

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        ID3DTexture3D* pT = FluidData.GetTexture((dx103DFluidData::eVolumePrivateRT)i);
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(pT);
        _RELEASE(pT);

        VERIFY(!pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i]);
        pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i] = FluidData.GetView((dx103DFluidData::eVolumePrivateRT)i);
    }
}

void dx103DFluidManager::DetachAndSwapFluidData(dx103DFluidData& FluidData)
{
    PIX_EVENT(DetachAndSwapFluidData);

    ID3DTexture3D* pTTarg = (ID3DTexture3D*)pRTTextures[RENDER_TARGET_COLOR]->surface_get();
    ID3DTexture3D* pTSrc = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    FluidData.SetTexture(dx103DFluidData::VP_COLOR, pTTarg);
    pRTTextures[RENDER_TARGET_COLOR]->surface_set(pTSrc);
    _RELEASE(pTSrc);

    ID3DRenderTargetView* pV = FluidData.GetView(dx103DFluidData::VP_COLOR);
    FluidData.SetView(dx103DFluidData::VP_COLOR, pRenderTargetViews[RENDER_TARGET_COLOR]);
    _RELEASE(pRenderTargetViews[RENDER_TARGET_COLOR]);
    pRenderTargetViews[RENDER_TARGET_COLOR] = pV;

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(0);
        _RELEASE(pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i]);
    }
}

void dx103DFluidManager::AdvectColorBFECC(float timestep, bool bTeperature)
{
    PIX_EVENT(AdvectColorBFECC);

    constexpr float color[4] = {0, 0, 0, 0};

    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], color);
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], color);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    if (bTeperature)
        RCache.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        RCache.set_Element(m_SimulationTechnique[SS_Advect]);

    RCache.set_c(strTimeStep, timestep);
    RCache.set_c(strModulate, 1.0f);
    RCache.set_c(strForward, 1.0f);

    m_pGrid->DrawSlices();

    // Advect back to get \bar{\phi}
    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
    ref_selement AdvectElement;
    if (bTeperature)
        AdvectElement = m_SimulationTechnique[SS_AdvectTemp];
    else
        AdvectElement = m_SimulationTechnique[SS_Advect];

    RCache.set_Element(AdvectElement);

    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strColorName(dx103DFluidConsts::m_pEngineTextureNames[RENDER_TARGET_COLOR_IN]);
    STextureList* _T = &*(AdvectElement->passes[0]->T);
    u32 dwTextureStage = _T->find_texture_stage(strColorName);
    //	This will be overritten by the next technique.
    //	Otherwise we had to reset current texture list manually.
    pRTTextures[RENDER_TARGET_TEMPVECTOR]->bind(dwTextureStage);

    RCache.set_c(strTimeStep, timestep);
    RCache.set_c(strModulate, 1.0f);
    RCache.set_c(strForward, -1.0f);

    m_pGrid->DrawSlices();
 
    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    if (bTeperature)
        RCache.set_Element(m_SimulationTechnique[SS_AdvectBFECCTemp]);
    else
        RCache.set_Element(m_SimulationTechnique[SS_AdvectBFECC]);

    Fvector4 halfVol;
    halfVol.set((float)m_iTextureWidth / 2.0f, (float)m_iTextureHeight / 2.0f, (float)m_iTextureDepth / 2.0f, 0.0f);
    RCache.set_c(strHalfVolumeDim, halfVol);
    RCache.set_c(strModulate, m_fDecay);
    RCache.set_c(strForward, 1.0f);

    m_pGrid->DrawSlices();
}

void dx103DFluidManager::AdvectColor(float timestep, bool bTeperature)
{
    PIX_EVENT(AdvectColor);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);

    if (bTeperature)
        RCache.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        RCache.set_Element(m_SimulationTechnique[SS_Advect]);

    RCache.set_c(strTimeStep, timestep);
    RCache.set_c(strModulate, 1.0f);
    RCache.set_c(strForward, 1.0f);
    RCache.set_c(strModulate, m_fDecay);

    m_pGrid->DrawSlices();
}

void dx103DFluidManager::AdvectVelocity(float timestep, float fGravity)
{
    PIX_EVENT(AdvectVelocity);

   //  Advect velocity by the fluid velocity
    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);

    if (_abs(fGravity) < 0.000001)
        RCache.set_Element(m_SimulationTechnique[SS_AdvectVel]);
    else
    {
        RCache.set_Element(m_SimulationTechnique[SS_AdvectVelGravity]);
        RCache.set_c(strGravityBuoyancy, fGravity);
    }

    RCache.set_c(strTimeStep, timestep);
    RCache.set_c(strModulate, 1.0f);
    RCache.set_c(strForward, 1.0f);
    m_pGrid->DrawSlices();
}

void dx103DFluidManager::ApplyVorticityConfinement(float timestep)
{
    PIX_EVENT(ApplyVorticityConfinement);

    // Compute vorticity
    constexpr float color[4] = {0, 0, 0, 0};
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], color);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);

    RCache.set_Element(m_SimulationTechnique[SS_Vorticity]);
    m_pGrid->DrawSlices();

    // Compute and apply vorticity confinement force
    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    RCache.set_Element(m_SimulationTechnique[SS_Confinement]);

    RCache.set_c(strEpsilon, m_fConfinementScale);
    RCache.set_c(strTimeStep, timestep);

    m_pGrid->DrawSlices();
}

void dx103DFluidManager::ApplyExternalForces(const dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT(ApplyExternalForces);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    m_pEmittersHandler->RenderDensity(FluidData);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    m_pEmittersHandler->RenderVelocity(FluidData);
}

void dx103DFluidManager::ComputeVelocityDivergence(float timestep)
{
    PIX_EVENT(ComputeVelocityDivergence);

    constexpr float color[4] = {0, 0, 0, 0};
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], color);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    RCache.set_Element(m_SimulationTechnique[SS_Divergence]);

     m_pGrid->DrawSlices();
}

void dx103DFluidManager::ComputePressure(float timestep)
{
    PIX_EVENT(ComputePressure);

    constexpr float color[4] = {0, 0, 0, 0};
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], color);

    RCache.set_RT(0);
    ref_selement CurrentTechnique = m_SimulationTechnique[SS_Jacobi];
    RCache.set_Element(CurrentTechnique);

    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strPressureName(dx103DFluidConsts::m_pEngineTextureNames[RENDER_TARGET_PRESSURE]);
    STextureList* _T = &*(CurrentTechnique->passes[0]->T);
    u32 dwTextureStage = _T->find_texture_stage(strPressureName);

    for (int iteration = 0; iteration < m_nIterations / 2.0; iteration++)
    {
        RCache.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
        pRTTextures[RENDER_TARGET_PRESSURE]->bind(dwTextureStage);
        m_pGrid->DrawSlices();

        RCache.set_RT(pRenderTargetViews[RENDER_TARGET_PRESSURE]);
        pRTTextures[RENDER_TARGET_TEMPSCALAR]->bind(dwTextureStage);
        m_pGrid->DrawSlices();
    }
}

void dx103DFluidManager::ProjectVelocity(float timestep)
{
    PIX_EVENT(ProjectVelocity);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY0]);
    RCache.set_Element(m_SimulationTechnique[SS_Project]);

    RCache.set_c(strModulate, 1.0f);

    m_pGrid->DrawSlices();
}

void dx103DFluidManager::RenderFluid(dx103DFluidData& FluidData)
{
    //	return;
    PIX_EVENT(render_fluid);

    //	Bind input texture
    ID3DTexture3D* pT = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(pT);
    _RELEASE(pT);

    //	Do rendering
    m_pRenderer->Draw(FluidData);

    //	Unbind input texture
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(0);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    if (!RImplementation.o.dx10_msaa)
        pTarget->u_setrt(pTarget->rt_Generic_0, 0, 0, HW.pBaseZB); // LDR RT
    else
        pTarget->u_setrt(pTarget->rt_Generic_0_r, 0, 0, pTarget->rt_MSAADepth->pZRT); // LDR RT

    RImplementation.rmNormal();
}

void dx103DFluidManager::UpdateObstacles(const dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT(Fluid_update_obstacles);
    //	Reset data
    constexpr float color[4] = {0, 0, 0, 0};
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_OBSTACLES], color);
    HW.pContext->ClearRenderTargetView(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], color);

    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTACLES], 0);
    RCache.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], 1);

    m_pObstaclesHandler->ProcessObstacles(FluidData, timestep);

    //	Just reset render targets:
    //	later only rt 0 will be reassigned so rt1
    //	would be bound all the time
    //	Reset to avoid confusion.
    RCache.set_RT(0, 0);
    RCache.set_RT(0, 1);
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
