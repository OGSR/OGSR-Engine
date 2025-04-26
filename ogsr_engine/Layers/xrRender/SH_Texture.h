#pragma once

#include "../../xrCore/xr_resource.h"
#include "../../xr_3da/Render.h"

class ENGINE_API CAviPlayerCustom;
class CTheoraSurface;

class ECORE_API CTexture : public ITexture, public xr_resource_named
{
public:
    enum MaxTextures
    {
        //	Actually these values are 128
        mtMaxPixelShaderTextures = 16,
        mtMaxVertexShaderTextures = 4,
        mtMaxGeometryShaderTextures = 16,
        mtMaxHullShaderTextures = 16,
        mtMaxDomainShaderTextures = 16,
        mtMaxComputeShaderTextures = 16,
    };

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
    CTexture();
    virtual ~CTexture();

    void set_slice(int slice);

    const char* GetName() const override { return cName.c_str(); }

    const char* GetLoadedName() const override { return loadedName.c_str(); }

    void Preload();
    void Preload(const char* Name);

    void Load();
    void Load(const char* Name) override;

    void PostLoad();
    void Unload() override;

    //	void								Apply			(u32 dwStage);

    void surface_set(ID3DBaseTexture* surf);
    ID3DBaseTexture* surface_get() const;

    virtual u32 get_Width()
    {
        return desc_Width;
    }
    virtual u32 get_Height()
    {
        return desc_Height;
    }

    void video_Sync(u32 _time) { m_play_time = _time; }
    void video_Play(BOOL looped, u32 _time = 0xFFFFFFFF);
    void video_Pause(BOOL state);
    void video_Stop();
    BOOL video_IsPlaying();

    //ID3DShaderResourceView* get_SRView() const { return m_pSRView; }

private:

    void Apply(CBackend& cmd_list, u32 dwStage) const;

    void UnloadImpl();
    
    void apply_load(CBackend& cmd_list, u32 stage);
    void apply_theora(CBackend& cmd_list, u32 stage);
    void apply_avi(CBackend& cmd_list, u32 stage);
    void apply_seq(CBackend& cmd_list, u32 stage);
    void apply_normal(CBackend& cmd_list, u32 stage);

    //	Class data
public: //	Public class members (must be encapsulated furthur)
    struct
    {
        u32 bLoaded : 1;
        u32 seqCycles : 1;
        u32 memUsage : 28;
    } flags;

    fastdelegate::FastDelegate<void(CBackend&, u32)> bind;

    CAviPlayerCustom* pAVI;
    CTheoraSurface* pTheora;

    float m_material{};
    float m_detail_scale{};

    shared_str m_bumpmap;

    bool m_is_hot = false;

    union
    {
        u32 m_play_time; // sync theora time
        u32 seqMSPF; // Sequence data milliseconds per frame
    };

    int curr_slice{ -1 };
    int last_slice{ -1 };

private:
    ID3DBaseTexture* pSurface{};
    // Sequence data
    xr_vector<ID3DBaseTexture*> seqDATA;

    ID3DShaderResourceView* m_pSRView{};
    ID3DShaderResourceView* srv_all{};
    xr_vector<ID3DShaderResourceView*> srv_per_slice;

    // Sequence view data
    xr_vector<ID3DShaderResourceView*> m_seqSRView;

    shared_str loadedName;

    u32 desc_Width{};
    u32 desc_Height{};
};

struct resptrcode_texture : public resptr_base<CTexture>
{
    void create(LPCSTR _name);
    void destroy() { _set(nullptr); }
    shared_str bump_get() { return _get()->m_bumpmap; }
    bool bump_exist() { return 0 != bump_get().size(); }
};
typedef resptr_core<CTexture, resptrcode_texture> ref_texture;
