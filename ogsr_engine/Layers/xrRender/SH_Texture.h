#pragma once

#include "../../xrCore/xr_resource.h"
#include "../../xr_3da/Render.h"

class ENGINE_API CAviPlayerCustom;
class CTheoraSurface;

class ECORE_API CTexture : public ITexture, public xr_resource_named
{
public:
    //	Since DX10 allows up to 128 unique textures,
    //	distance between enum values should be at leas 128
    enum ResourceShaderType //	Don't change this since it's hardware-dependent
    {
        rstPixel = 0, //	Default texture offset
        rstVertex = D3DVERTEXTEXTURESAMPLER0,
        rstGeometry = rstVertex + 256,
        rstHull = rstGeometry + 256,
        rstDomain = rstHull + 256,
        rstCompute = rstDomain + 256,
        rstInvalid = rstCompute + 256
    };

public:
    void apply_load(u32 stage);
    void apply_theora(u32 stage);
    void apply_avi(u32 stage);
    void apply_seq(u32 stage);
    void apply_normal(u32 stage);

    const char* GetName() const override { return cName.c_str(); }

    void Preload();
    void Preload(const char* Name);
    void Load();
    void Load(const char* Name) override;
    void PostLoad();
    void Unload() override;

    //	void								Apply			(u32 dwStage);

    void surface_set(ID3DBaseTexture* surf);
    ID3DBaseTexture* surface_get();

    IC BOOL isUser() { return flags.bUser; }
    IC u32 get_Width()
    {
        desc_enshure();
        return desc.Width;
    }
    IC u32 get_Height()
    {
        desc_enshure();
        return desc.Height;
    }

    void video_Sync(u32 _time) { m_play_time = _time; }
    void video_Play(BOOL looped, u32 _time = 0xFFFFFFFF);
    void video_Pause(BOOL state);
    void video_Stop();
    BOOL video_IsPlaying();

    CTexture();
    virtual ~CTexture();

#if defined(USE_DX10) || defined(USE_DX11)
    ID3DShaderResourceView* get_SRView() { return m_pSRView; }
#endif //	USE_DX10

private:
    IC BOOL desc_valid() { return pSurface == desc_cache; }
    IC void desc_enshure()
    {
        if (!desc_valid())
            desc_update();
    }
    void desc_update();
#if defined(USE_DX10) || defined(USE_DX11)
    void Apply(u32 dwStage);
    void ProcessStaging();
    D3D_USAGE GetUsage();
#endif //	USE_DX10

    //	Class data
public: //	Public class members (must be encapsulated furthur)
    struct
    {
        u32 bLoaded : 1;
        u32 bUser : 1;
        u32 seqCycles : 1;
        u32 MemoryUsage : 28;
#if defined(USE_DX10) || defined(USE_DX11)
        u32 bLoadedAsStaging : 1;
#endif //	USE_DX10
    } flags;
    fastdelegate::FastDelegate<void(u32)> bind;

    CAviPlayerCustom* pAVI;
    CTheoraSurface* pTheora;
    float m_material;
    shared_str m_bumpmap;

    union
    {
        u32 m_play_time; // sync theora time
        u32 seqMSPF; // Sequence data milliseconds per frame
    };

private:
    ID3DBaseTexture* pSurface;
    // Sequence data
    xr_vector<ID3DBaseTexture*> seqDATA;

    // Description
    ID3DBaseTexture* desc_cache;
    D3D_TEXTURE2D_DESC desc;

#if defined(USE_DX10) || defined(USE_DX11)
    ID3DShaderResourceView* m_pSRView;
    // Sequence view data
    xr_vector<ID3DShaderResourceView*> m_seqSRView;
#endif //	USE_DX10
};
struct resptrcode_texture : public resptr_base<CTexture>
{
    void create(LPCSTR _name);
    void destroy() { _set(NULL); }
    shared_str bump_get() { return _get()->m_bumpmap; }
    bool bump_exist() { return 0 != bump_get().size(); }
};
typedef resptr_core<CTexture, resptrcode_texture> ref_texture;
