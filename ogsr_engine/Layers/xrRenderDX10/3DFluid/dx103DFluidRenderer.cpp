#include "stdafx.h"

#include "blender_fluid.h"

#ifdef DX10_FLUID_ENABLE

#include "dx103DFluidRenderer.h"
#include "../dx10BufferUtils.h"
#include "../../xrRender/dxRenderDeviceRender.h"
#include "dx103DFluidData.h"
#include <DirectXPackedVector.h>

struct VsInput
{
    D3DXVECTOR3 pos;
};

namespace
{
// For render call
shared_str strZNear;
shared_str strZFar;
shared_str strGridScaleFactor;
shared_str strEyeOnGrid;
shared_str strWorldViewProjection;
shared_str strInvWorldViewProjection;
shared_str strRTWidth;
shared_str strRTHeight;

shared_str strDiffuseLight;
} // namespace

LPCSTR dx103DFluidRenderer::m_pRTNames[RRT_NumRT] = {"$user$rayDataTex", "$user$rayDataTexSmall", "$user$rayCastTex", "$user$edgeTex"};

LPCSTR dx103DFluidRenderer::m_pResourceRTNames[RRT_NumRT] = {"rayDataTex", "rayDataTexSmall", "rayCastTex", "edgeTex"};

dx103DFluidRenderer::dx103DFluidRenderer() : m_bInited(false)
{
    RTFormats[RRT_RayDataTex] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    RTFormats[RRT_RayDataTexSmall] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    RTFormats[RRT_RayCastTex] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    RTFormats[RRT_EdgeTex] = DXGI_FORMAT_R32_FLOAT;

    strZNear = "ZNear";
    strZFar = "ZFar";
    strGridScaleFactor = "gridScaleFactor";
    strEyeOnGrid = "eyeOnGrid";
    strWorldViewProjection = "WorldViewProjection";
    strInvWorldViewProjection = "InvWorldViewProjection";
    strRTWidth = "RTWidth";
    strRTHeight = "RTHeight";
    strDiffuseLight = "DiffuseLight";
}

dx103DFluidRenderer::~dx103DFluidRenderer() { Destroy(); }

void dx103DFluidRenderer::Initialize(int gridWidth, int gridHeight, int gridDepth)
{
    Destroy();

    m_vGridDim[0] = float(gridWidth);
    m_vGridDim[1] = float(gridHeight);
    m_vGridDim[2] = float(gridDepth);

    m_fMaxDim = _max(_max(m_vGridDim[0], m_vGridDim[1]), m_vGridDim[2]);

    // Initialize the grid offset matrix
    {
        // Make a scale matrix to scale the unit-sided box to be unit-length on the
        //  side/s with maximum dimension
        const auto scaleM = DirectX::XMMatrixScaling(m_vGridDim[0] / m_fMaxDim, m_vGridDim[1] / m_fMaxDim, m_vGridDim[2] / m_fMaxDim);

        // offset grid to be centered at origin
        const auto translationM = DirectX::XMMatrixTranslation(-0.5, -0.5, -0.5);

        m_gridMatrix = translationM * scaleM;
    }

    InitShaders();
    CreateGridBox();
    CreateScreenQuad();
    CreateJitterTexture();
    CreateHHGGTexture();

    m_bInited = true;
}

void dx103DFluidRenderer::Destroy()
{
    if (!m_bInited)
        return;

    // createJitterTexture();
    m_JitterTexture = nullptr;
    m_HHGGTexture = nullptr;

    // createScreenQuad();
    m_GeomQuadVertex = nullptr;
    _RELEASE(m_pQuadVertexBuffer);

    // createGridBox();
    m_GeomGridBox = nullptr;
    _RELEASE(m_pGridBoxVertexBuffer);
    _RELEASE(m_pGridBoxIndexBuffer);

    DestroyShaders();
}

void dx103DFluidRenderer::InitShaders()
{
    {
        CBlender_fluid_raydata Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 3; ++i)
            m_RendererTechnique[RS_CompRayData_Back + i] = shader->E[i];
    }

    {
        CBlender_fluid_raycast Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 5; ++i)
            m_RendererTechnique[RS_QuadEdgeDetect + i] = shader->E[i];
    }
}

void dx103DFluidRenderer::DestroyShaders()
{
    for (auto& i : m_RendererTechnique)
    {
        //	Release shader's element.
        i = nullptr;
    }
}

void dx103DFluidRenderer::CreateGridBox()
{
    const VsInput vertices[] = {
        {D3DXVECTOR3(0, 0, 0)}, {D3DXVECTOR3(0, 0, 1)}, {D3DXVECTOR3(0, 1, 0)}, {D3DXVECTOR3(0, 1, 1)},
        {D3DXVECTOR3(1, 0, 0)}, {D3DXVECTOR3(1, 0, 1)}, {D3DXVECTOR3(1, 1, 0)}, {D3DXVECTOR3(1, 1, 1)},
    };
    m_iGridBoxVertNum = sizeof(vertices) / sizeof(vertices[0]);

    R_CHK(dx10BufferUtils::CreateVertexBuffer(&m_pGridBoxVertexBuffer, vertices, sizeof(vertices)));

    // Create index buffer
    const u16 indices[] = {0, 4, 1, 1, 4, 5, 0, 1, 2, 2, 1, 3, 4, 6, 5, 6, 7, 5, 2, 3, 6, 3, 7, 6, 1, 5, 3, 3, 5, 7, 0, 2, 4, 2, 6, 4};
    m_iGridBoxFaceNum = (sizeof(indices) / sizeof(indices[0])) / 3;

    R_CHK(dx10BufferUtils::CreateIndexBuffer(&m_pGridBoxIndexBuffer, indices, sizeof(indices)));
    HW.stats_manager.increment_stats(sizeof(indices), enum_stats_buffer_type_index, D3DPOOL_MANAGED);

    constexpr D3DVERTEXELEMENT9 layout[]{{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};

    m_GeomGridBox.create(layout, m_pGridBoxVertexBuffer, m_pGridBoxIndexBuffer);
}

void dx103DFluidRenderer::CreateScreenQuad()
{
    constexpr D3DVERTEXELEMENT9 quadlayout[]{{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};

    // Create a screen quad for all render to texture operations
    VsInput svQuad[4];
    svQuad[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
    svQuad[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
    svQuad[2].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
    svQuad[3].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);

    R_CHK(dx10BufferUtils::CreateVertexBuffer(&m_pQuadVertexBuffer, svQuad, sizeof(svQuad)));
    m_GeomQuadVertex.create(quadlayout, m_pQuadVertexBuffer, nullptr);
}

void dx103DFluidRenderer::CreateJitterTexture()
{
    BYTE data[256 * 256];
    for (unsigned char& i : data)
    {
        i = (unsigned char)(rand() / float(RAND_MAX) * 256);
    }

    D3D_TEXTURE2D_DESC desc;
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    desc.Format = DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA dataDesc;
    dataDesc.pSysMem = data;
    dataDesc.SysMemPitch = 256;

    ID3DTexture2D* NoiseTexture = nullptr;

    CHK_DX(HW.pDevice->CreateTexture2D(&desc, &dataDesc, &NoiseTexture));

    m_JitterTexture = dxRenderDeviceRender::Instance().Resources->_CreateTexture("$user$NVjitterTex");
    m_JitterTexture->surface_set(NoiseTexture);

    _RELEASE(NoiseTexture);
}

template <size_t iNumSamples>
struct HHGG_Gen
{
    // cubic b-spline
    consteval float bsW0(float a) { return (1.0f / 6.0f * (-(a * a * a) + (3.0f * a * a) - (3.0f * a) + 1.0f)); }
    consteval float bsW1(float a) { return (1.0f / 6.0f * ((3.0f * a * a * a) - (6.0f * a * a) + 4.0f)); }
    consteval float bsW2(float a) { return (1.0f / 6.0f * (-(3.0f * a * a * a) + (3.0f * a * a) + (3.0f * a) + 1.0f)); }
    consteval float bsW3(float a) { return (1.0f / 6.0f * a * a * a); }
    consteval float g0(float a) { return (bsW0(a) + bsW1(a)); }
    consteval float g1(float a) { return (bsW2(a) + bsW3(a)); }
    consteval float h0texels(float a) { return (1.0f + a - (bsW1(a) / (bsW0(a) + bsW1(a)))); }
    consteval float h1texels(float a) { return (1.0f - a + (bsW3(a) / (bsW2(a) + bsW3(a)))); }

    consteval HHGG_Gen()
    {
        for (size_t i = 0; i < iNumSamples; i++)
        {
            float a = i / (float)(iNumSamples - 1);
            data[4 * i] = -h0texels(a);
            data[4 * i + 1] = h1texels(a);
            data[4 * i + 2] = 1.0f - g0(a);
            data[4 * i + 3] = g0(a);
        }
    }
    float data[4 * iNumSamples]{};
};

void dx103DFluidRenderer::CreateHHGGTexture()
{
    constexpr size_t iNumSamples = 16;
    constexpr auto hhgg_tex = HHGG_Gen<iNumSamples>();

    //	Min value is -1
    //	Max value is +1
    DirectX::PackedVector::HALF converted[std::size(hhgg_tex.data)];
    DirectX::PackedVector::XMConvertFloatToHalfStream(converted, sizeof(converted[0]), hhgg_tex.data, sizeof(hhgg_tex.data[0]), std::size(hhgg_tex.data));

    D3D_TEXTURE1D_DESC desc;
    desc.Width = iNumSamples;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA dataDesc;
    dataDesc.pSysMem = converted;
    dataDesc.SysMemPitch = sizeof(converted);

    ID3DTexture1D* HHGGTexture = nullptr;

    CHK_DX(HW.pDevice->CreateTexture1D(&desc, &dataDesc, &HHGGTexture));

    m_HHGGTexture = dxRenderDeviceRender::Instance().Resources->_CreateTexture("$user$NVHHGGTex");
    m_HHGGTexture->surface_set(HHGGTexture);

    _RELEASE(HHGGTexture);
}

void dx103DFluidRenderer::SetScreenSize(int width, int height) { CreateRayDataResources(width, height); }

void dx103DFluidRenderer::CalculateRenderTextureSize(int screenWidth, int screenHeight)
{
    const int maxProjectedSide = int(3.0 * _sqrt(3.0) * m_fMaxDim);
    const int maxScreenDim = _max(screenWidth, screenHeight);

    if (maxScreenDim > maxProjectedSide)
    {
        m_iRenderTextureWidth = screenWidth / 8;
        m_iRenderTextureHeight = screenHeight / 8;
    }
    else
    {
        m_iRenderTextureWidth = screenWidth;
        m_iRenderTextureHeight = screenHeight;
    }
}

void dx103DFluidRenderer::CreateRayDataResources(int width, int height)
{
    // find a good resolution for raycasting purposes
    CalculateRenderTextureSize(width, height);

    RT[0] = nullptr;
    RT[0].create(m_pRTNames[0], width, height, RTFormats[0]);

    for (int i = 1; i < RRT_NumRT; ++i)
    {
        RT[i] = nullptr;
        RT[i].create(m_pRTNames[i], m_iRenderTextureWidth, m_iRenderTextureHeight, RTFormats[i]);
    }
}

void dx103DFluidRenderer::Draw(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    //	We don't need ZB anyway
    cmd_list.set_ZB(nullptr);

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();
    const bool bRenderFire = (VolumeSettings.m_SimulationType == dx103DFluidData::ST_FIRE);

    FogLighting LightData;

    CalculateLighting(FluidData, LightData);

    //	Set shader element to set up all necessary constants to constant buffer
    //	If you change constant buffer layout make sure this hack works ok.
    cmd_list.set_Element(m_RendererTechnique[RS_CompRayData_Back]);

    float color[4] = {0, 0, 0, 0};

    // Ray cast and render to a temporary buffer
    //=========================================================================

    // Compute the ray data required by the raycasting pass below.
    //  This function will render to a buffer of float4 vectors, where
    //  xyz is starting position of the ray in grid space
    //  w is the length of the ray in view space
    ComputeRayData(cmd_list, FluidData);

    // Do edge detection on this image to find any
    //  problematic areas where we need to raycast at higher resolution
    ComputeEdgeTexture(cmd_list, FluidData);

    // Raycast into the temporary render target:
    //  raycasting is done at the smaller resolution, using a fullscreen quad

    cmd_list.ClearRT(RT[RRT_RayCastTex], {}); // black

    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(cmd_list, RT[RRT_RayCastTex], nullptr, nullptr, nullptr); // LDR RT

    RImplementation.rmNormal(cmd_list);

    if (bRenderFire)
        cmd_list.set_Element(m_RendererTechnique[RS_QuadRaycastFire]);
    else
        cmd_list.set_Element(m_RendererTechnique[RS_QuadRaycastFog]);

    PrepareCBuffer(cmd_list, FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);

    DrawScreenQuad(cmd_list);

    // Render to the back buffer sampling from the raycast texture that we just created
    //  If and edge was detected at the current pixel we will raycast again to avoid
    //  smoke aliasing artifacts at scene edges

    //	Restore render state
    pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0, nullptr, nullptr, pTarget->rt_Base_Depth->pZRT[cmd_list.context_id]); // LDR RT

    if (bRenderFire)
        cmd_list.set_Element(m_RendererTechnique[RS_QuadRaycastCopyFire]);
    else
        cmd_list.set_Element(m_RendererTechnique[RS_QuadRaycastCopyFog]);

    RImplementation.rmNormal(cmd_list);

    PrepareCBuffer(cmd_list, FluidData, Device.dwWidth, Device.dwHeight);

    cmd_list.set_c(strDiffuseLight, LightData.m_vLightIntencity.x, LightData.m_vLightIntencity.y, LightData.m_vLightIntencity.z, 1.0f);

    DrawScreenQuad(cmd_list);
}

void dx103DFluidRenderer::ComputeRayData(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    // Clear the color buffer to 0
    cmd_list.ClearRT(RT[RRT_RayDataTex], {});

    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(cmd_list, RT[RRT_RayDataTex], nullptr, nullptr, nullptr); // LDR RT

    cmd_list.set_Element(m_RendererTechnique[RS_CompRayData_Back]);

    PrepareCBuffer(cmd_list, FluidData, Device.dwWidth, Device.dwHeight);

    // Render volume back faces
    // We output xyz=(0,-1,0) and w=min(sceneDepth, boxDepth)
    DrawBox(cmd_list);

    // Render volume front faces using subtractive blending
    // We output xyz="position in grid space" and w=boxDepth,
    //  unless the pixel is occluded by the scene, in which case we output xyzw=(1,0,0,0)
    pTarget->u_setrt(cmd_list, RT[RRT_RayDataTex], nullptr, nullptr, nullptr); // LDR RT
    cmd_list.set_Element(m_RendererTechnique[RS_CompRayData_Front]);
    PrepareCBuffer(cmd_list, FluidData, Device.dwWidth, Device.dwHeight);
    DrawBox(cmd_list);
}

void dx103DFluidRenderer::ComputeEdgeTexture(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(cmd_list, RT[RRT_RayDataTexSmall], nullptr, nullptr, nullptr); // LDR RT
    cmd_list.set_Element(m_RendererTechnique[RS_QuadDownSampleRayDataTexture]);

    // First setup viewport to match the size of the destination low-res texture
    PrepareCBuffer(cmd_list, FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);

    // Downsample the rayDataTexture to a new small texture, simply using point sample (no filtering)
    DrawScreenQuad(cmd_list);

    // Create an edge texture, performing edge detection on 'rayDataTexSmall'
    pTarget->u_setrt(cmd_list, RT[RRT_EdgeTex], nullptr, nullptr, nullptr); // LDR RT
    cmd_list.set_Element(m_RendererTechnique[RS_QuadEdgeDetect]);
    PrepareCBuffer(cmd_list, FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);
    DrawScreenQuad(cmd_list);
}

void dx103DFluidRenderer::DrawScreenQuad(CBackend& cmd_list)
{
    cmd_list.set_Geometry(m_GeomQuadVertex);
    cmd_list.Render(D3DPT_TRIANGLESTRIP, 0, 2);
}

void dx103DFluidRenderer::DrawBox(CBackend& cmd_list)
{
    cmd_list.set_Geometry(m_GeomGridBox);
    cmd_list.Render(D3DPT_TRIANGLELIST, 0, 0, m_iGridBoxVertNum, 0, m_iGridBoxFaceNum);
}

void dx103DFluidRenderer::CalculateLighting(const dx103DFluidData& FluidData, FogLighting& LightData)
{
    m_lstRenderables.clear();

    LightData.Reset();

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();

    Fvector4 hemi_color = g_pGamePersistent->Environment().CurrentEnv->hemi_color;
    // hemi_color.mul(0.2f);
    hemi_color.mul(VolumeSettings.m_fHemi);
    LightData.m_vLightIntencity.set(hemi_color.x, hemi_color.y, hemi_color.z);
    LightData.m_vLightIntencity.add(g_pGamePersistent->Environment().CurrentEnv->ambient);

    const Fmatrix& Transform = FluidData.GetTransform();

    Fbox box;
    box.min = Fvector3().set(-0.5f, -0.5f, -0.5f);
    box.max = Fvector3().set(0.5f, 0.5f, 0.5f);
    box.xform(Transform);
    Fvector3 center;
    Fvector3 size;
    box.getcenter(center);
    box.getradius(size);

    // Traverse object database
    g_SpatialSpace->q_box(m_lstRenderables,
                          0, // ISpatial_DB::O_ORDERED,
                          STYPE_LIGHTSOURCE, center, size);

    // Determine visibility for dynamic part of scene
    for (ISpatial* spatial : m_lstRenderables)
    {
        // Light
        const light* pLight = smart_cast<light*>(spatial->dcast_Light());
        VERIFY(pLight);

        if (pLight->flags.bMoveable || pLight->flags.bHudMode /*|| pLight->flags.bStatic*/)
            continue;

        const float d = pLight->position.distance_to(Transform.c);

        const float R = pLight->range + _max(size.x, _max(size.y, size.z));
        if (d >= R)
            continue;

        Fvector3 LightIntencity;

        LightIntencity.set(pLight->color.r, pLight->color.g, pLight->color.b);

        const float r = pLight->range;
        const float a = clampr(1.f - d / (r + EPS), 0.f, 1.f) * (2.f);

        LightIntencity.mul(a);

        LightData.m_vLightIntencity.add(LightIntencity);
    }
}

void dx103DFluidRenderer::PrepareCBuffer(CBackend& cmd_list, const dx103DFluidData& FluidData, u32 RTWidth, u32 RTHeight) const
{
    using namespace DirectX;

    const Fmatrix& transform = FluidData.GetTransform();
    cmd_list.set_xform_world(transform);

    // The near and far planes are used to unproject the scene's z-buffer values
    cmd_list.set_c(strZNear, VIEWPORT_NEAR);
    cmd_list.set_c(strZFar, g_pGamePersistent->Environment().CurrentEnv->far_plane);

    const auto gridWorld = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&transform));
    const auto View = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&cmd_list.xforms.m_v));
    auto WorldView = gridWorld * View;

    // The length of one of the axis of the worldView matrix is the length of longest side of the box
    //  in view space. This is used to convert the length of a ray from view space to grid space.
    const float worldScale = XMVectorGetX(XMVector3Length(WorldView.r[0]));
    cmd_list.set_c(strGridScaleFactor, worldScale);

    // We prepend the current world matrix with this other matrix which adds an offset (-0.5, -0.5, -0.5)
    //  and scale factors to account for unequal number of voxels on different sides of the volume box.
    // This is because we want to preserve the aspect ratio of the original simulation grid when
    //  raytracing through it.
    WorldView = m_gridMatrix * WorldView;

    // worldViewProjection is used to transform the volume box to screen space
    const auto Projection = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&cmd_list.xforms.m_p));
    const auto WorldViewProjection = WorldView * Projection;

    Fmatrix tempM1{};
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&tempM1), WorldViewProjection);
    cmd_list.set_c(strWorldViewProjection, tempM1);

    // invWorldViewProjection is used to transform positions in the "near" plane into grid space
    Fmatrix tempM2{};
    const auto InvWorldViewProjection = XMMatrixInverse(nullptr, WorldViewProjection);
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&tempM2), InvWorldViewProjection);
    cmd_list.set_c(strInvWorldViewProjection, tempM2);

    // Compute the inverse of the worldView matrix
    const auto WorldViewInv = XMMatrixInverse(nullptr, WorldView);
    // Compute the eye's position in "grid space" (the 0-1 texture coordinate cube)
    const auto EyeInGridSpace = XMVector3Transform(XMVectorSet(0, 0, 0, 0), WorldViewInv);
    Fvector4 tempV{};
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&tempV), EyeInGridSpace);
    cmd_list.set_c(strEyeOnGrid, tempV);

    cmd_list.set_c(strRTWidth, (float)RTWidth);
    cmd_list.set_c(strRTHeight, (float)RTHeight);
}

#endif
