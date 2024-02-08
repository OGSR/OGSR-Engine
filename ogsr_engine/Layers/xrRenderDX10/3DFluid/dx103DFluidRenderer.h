#pragma once

namespace dx103DFluidConsts
{
inline constexpr const char* m_pRTNames[]{"$user$rayDataTex", "$user$rayDataTexSmall", "$user$rayCastTex", "$user$edgeTex"};
inline constexpr const char* m_pResourceRTNames[]{"rayDataTex", "rayDataTexSmall", "rayCastTex", "edgeTex"};
inline constexpr D3DFORMAT RTFormats[]{D3DFMT_A32B32G32R32F, D3DFMT_A32B32G32R32F, D3DFMT_A32B32G32R32F, D3DFMT_R32F};

inline constexpr DXGI_FORMAT RenderTargetFormats[]{DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R8_UNORM,
                                                   DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT};

inline constexpr const char* m_pEngineTextureNames[]{
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
inline constexpr const char* m_pShaderTextureNames[]{
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

inline constexpr Fvector3 vertices[] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};
inline constexpr u32 m_iGridBoxVertNum = sizeof(vertices) / sizeof(vertices[0]);
inline constexpr u16 indices[] = {0, 4, 1, 1, 4, 5, 0, 1, 2, 2, 1, 3, 4, 6, 5, 6, 7, 5, 2, 3, 6, 3, 7, 6, 1, 5, 3, 3, 5, 7, 0, 2, 4, 2, 6, 4};
inline constexpr u32 m_iGridBoxFaceNum = (sizeof(indices) / sizeof(indices[0])) / 3;

inline constexpr DXGI_FORMAT m_VPRenderTargetFormats[]{
    DXGI_FORMAT_R16G16B16A16_FLOAT, // VP_VELOCITY0
    DXGI_FORMAT_R16_FLOAT, // VP_PRESSURE
    DXGI_FORMAT_R16_FLOAT // VP_COLOR
};
} // namespace dx103DFluidConsts

class dx103DFluidData;

class dx103DFluidRenderer
{
public:
    enum Renderer_RT
    {
        RRT_RayDataTex = 0,
        RRT_RayDataTexSmall,
        RRT_RayCastTex,
        RRT_EdgeTex,
        RRT_NumRT
    };

public:
    dx103DFluidRenderer();
    ~dx103DFluidRenderer();

    void Initialize(int gridWidth, int gridHeight, int gridDepth);
    void Destroy();

    void SetScreenSize(int width, int height);

    void Draw(const dx103DFluidData& FluidData);

private:
    enum RendererShader
    {
        RS_CompRayData_Back = 0,
        RS_CompRayData_Front,
        RS_QuadDownSampleRayDataTexture,

        RS_QuadEdgeDetect,
        RS_QuadRaycastFog,
        RS_QuadRaycastCopyFog,
        RS_QuadRaycastFire,
        RS_QuadRaycastCopyFire,

        RS_NumShaders
    };

    struct FogLighting
    {
        Fvector3 m_vLightIntencity{};

        void Reset() { m_vLightIntencity.set(0.f, 0.f, 0.f); }
    };

private:
    void InitShaders();
    void DestroyShaders();

    void CreateGridBox();
    void CreateScreenQuad();
    void CreateJitterTexture();
    void CreateHHGGTexture();

    void CalculateRenderTextureSize(int screenWidth, int screenHeight);
    void CreateRayDataResources(int width, int height);
    void PrepareCBuffer(const dx103DFluidData& FluidData, u32 RTWidth, u32 RTHeight);

    void ComputeRayData(const dx103DFluidData& FluidData);
    void ComputeEdgeTexture(const dx103DFluidData& FluidData);

    void DrawScreenQuad();
    void DrawBox();

    void CalculateLighting(const dx103DFluidData& FluidData, FogLighting& LightData);

private:
    bool m_bInited{};

    Fvector3 m_vGridDim{};
    float m_fMaxDim{};

    int m_iRenderTextureWidth{};
    int m_iRenderTextureHeight{};

    DirectX::XMMATRIX m_gridMatrix{};

    ref_rt RT[RRT_NumRT];

    ref_selement m_RendererTechnique[RS_NumShaders];

    ref_texture m_JitterTexture;
    ref_texture m_HHGGTexture;

    ref_geom m_GeomGridBox;
    ID3DVertexBuffer* m_pGridBoxVertexBuffer{};
    ID3DIndexBuffer* m_pGridBoxIndexBuffer{};

    ref_geom m_GeomQuadVertex;
    ID3DVertexBuffer* m_pQuadVertexBuffer{};

    //	Cache vectors to avoid memory reallocations
    //	TODO: DX10: Reserve memory on object creation
    xr_vector<ISpatial*> m_lstRenderables;
};
