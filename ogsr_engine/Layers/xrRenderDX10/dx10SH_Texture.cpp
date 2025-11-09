#include "stdafx.h"


#include "../xrRender/ResourceManager.h"

#include "../../xr_3da/render.h"

#include "../../xr_3da/tntQAVI.h"
#include "../../xr_3da/xrTheora_Surface.h"

#include "../xrRender/dxRenderDeviceRender.h"

#include "StateManager/dx10ShaderResourceStateCache.h"

#define PRIORITY_HIGH 12
#define PRIORITY_NORMAL 8
#define PRIORITY_LOW 4

void resptrcode_texture::create(LPCSTR _name) { _set(DEV->_CreateTexture(_name)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
    pAVI = nullptr;
    pTheora = nullptr;
    seqMSPF = 0;
    flags.memUsage = 0;
    flags.bLoaded = false;
    flags.seqCycles = 0;
    m_material = 1.0f;
    bind = fastdelegate::MakeDelegate(this, &CTexture::apply_load);
}

CTexture::~CTexture()
{
    // Msg("! destroy texture name [%s]", loadedName.c_str());
    UnloadImpl();

    // release external reference
    DEV->_DeleteTexture(this);
}

void CTexture::surface_set(ID3DBaseTexture* surf)
{
    if (pSurface == surf)
        return;

    if (surf)
        surf->AddRef();

    _RELEASE(pSurface); 
    //_RELEASE(m_pSRView); // по идеи тут тоже надо релиз делать так как ниже вызывается CreateShaderResourceView но оно все ломает. потому оставил так
    m_pSRView = nullptr;

    _RELEASE(srv_all);

    for (auto& srv : srv_per_slice)
    {
        _RELEASE(srv);
    }

    pSurface = surf;

    if (pSurface)
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
        {
            D3D_TEXTURE2D_DESC desc;

            ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
            T->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;

            D3D_SHADER_RESOURCE_VIEW_DESC ViewDesc{};

            if (desc.MiscFlags & D3D_RESOURCE_MISC_TEXTURECUBE)
            {
                ViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
                ViewDesc.TextureCube.MostDetailedMip = 0;
                ViewDesc.TextureCube.MipLevels = desc.MipLevels;
            }
            else
            {
                const bool isArray = desc.ArraySize > 1;

                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.ViewDimension = isArray ? D3D_SRV_DIMENSION_TEXTURE2DARRAY : D3D_SRV_DIMENSION_TEXTURE2D;
                    if (isArray)
                    {
                        ViewDesc.Texture2DArray.MipLevels = desc.MipLevels;
                        ViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
                    }
                    else
                    {
                        ViewDesc.Texture2D.MostDetailedMip = 0;
                        ViewDesc.Texture2D.MipLevels = desc.MipLevels;
                    }
                }
                else
                {
                    ViewDesc.ViewDimension = isArray ? D3D_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D_SRV_DIMENSION_TEXTURE2DMS;
                    if (isArray)
                    {
                        ViewDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
                    }
                }
            }

            switch (desc.Format)
            {
            case DXGI_FORMAT_R32G8X24_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break;

            case DXGI_FORMAT_R24G8_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;

            case DXGI_FORMAT_R32_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R32_FLOAT; break;

            case DXGI_FORMAT_R16_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R16_FLOAT; break;
            }

            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_all));

            srv_per_slice.resize(desc.ArraySize);
            for (u32 id = 0; id < desc.ArraySize; ++id)
            {
                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.Texture2DArray.ArraySize = 1;
                    ViewDesc.Texture2DArray.FirstArraySlice = id;
                }
                else
                {
                    ViewDesc.Texture2DMSArray.ArraySize = 1;
                    ViewDesc.Texture2DMSArray.FirstArraySlice = id;
                }

                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_per_slice[id]));
            }
            set_slice(-1);
        }
        else
        {
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, NULL, &m_pSRView));
        }
    }
}

ID3DBaseTexture* CTexture::surface_get() const
{
    //if (pSurface)
    //    pSurface->AddRef();

    return pSurface;
}

void CTexture::PostLoad()
{
    if (pTheora)
        bind = fastdelegate::MakeDelegate(this, &CTexture::apply_theora);
    else if (pAVI)
        bind = fastdelegate::MakeDelegate(this, &CTexture::apply_avi);
    else if (!m_seqSRView.empty())
        bind = fastdelegate::MakeDelegate(this, &CTexture::apply_seq);
    else
        bind = fastdelegate::MakeDelegate(this, &CTexture::apply_normal);
}

void CTexture::apply_load(CBackend& cmd_list, u32 dwStage)
{
    if (!flags.bLoaded)
        Load();
    else
        PostLoad();
    bind(cmd_list, dwStage);
};

void CTexture::Apply(CBackend& cmd_list, u32 dwStage) const
{
    if (dwStage < rstVertex) //	Pixel shader stage resources
    {
        cmd_list.SRVSManager.SetPSResource(dwStage, m_pSRView);
    }
    else if (dwStage < rstGeometry) //	Vertex shader stage resources
    {
        cmd_list.SRVSManager.SetVSResource(dwStage - rstVertex, m_pSRView);
    }
    else if (dwStage < rstHull) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetGSResource(dwStage - rstGeometry, m_pSRView);
    }
    else if (dwStage < rstDomain) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetHSResource(dwStage - rstHull, m_pSRView);
    }
    else if (dwStage < rstCompute) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetDSResource(dwStage - rstDomain, m_pSRView);
    }
    else if (dwStage < rstInvalid) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetCSResource(dwStage - rstCompute, m_pSRView);
    }
    else
        VERIFY("Invalid stage");
}

void CTexture::apply_theora(CBackend& cmd_list, u32 dwStage)
{
    if (pTheora->Update(m_play_time != 0xFFFFFFFF ? m_play_time : Device.dwTimeContinual))
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        const u32 _w = pTheora->Width(false);

        // R_CHK(T2D->LockRect(0,&R,&rect,0));

        R_CHK(HW.get_context(cmd_list.context_id)->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == int(_w * 4));
        int _pos = 0;
        pTheora->DecompressFrame((u32*)mapData.pData, _w - pTheora->Width(true), _pos);
        VERIFY(u32(_pos) == pTheora->Height(true) * _w);
        // R_CHK				(T2D->UnlockRect(0));
        HW.get_context(cmd_list.context_id)->Unmap(T2D, 0);
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(cmd_list, dwStage);
}

void CTexture::apply_avi(CBackend& cmd_list, u32 dwStage)
{
    if (pAVI->NeedUpdate())
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        // R_CHK	(T2D->LockRect(0,&R,NULL,0));

        R_CHK(HW.get_context(cmd_list.context_id)->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == int(pAVI->m_dwWidth * 4));
        BYTE* ptr;
        pAVI->GetFrame(&ptr);
        CopyMemory(mapData.pData, ptr, pAVI->m_dwWidth * pAVI->m_dwHeight * 4);
        // R_CHK	(T2D->UnlockRect(0));
        HW.get_context(cmd_list.context_id)->Unmap(T2D, 0);
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(cmd_list, dwStage);
}

void CTexture::apply_seq(CBackend& cmd_list, u32 dwStage)
{
    // SEQ
    const u32 frame = Device.dwTimeContinual / seqMSPF; // Device.dwTimeGlobal
    const u32 frame_data = m_seqSRView.size();
    if (flags.seqCycles)
    {
        u32 frame_id = frame % (frame_data * 2);
        if (frame_id >= frame_data)
            frame_id = (frame_data - 1) - (frame_id % frame_data);
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }
    else
    {
        const u32 frame_id = frame % frame_data;
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(cmd_list, dwStage);
}

void CTexture::apply_normal(CBackend& cmd_list, u32 dwStage)
{
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(cmd_list, dwStage);
}

void CTexture::set_slice(int slice)
{
    m_pSRView = (slice < 0) ? srv_all : srv_per_slice[slice];
    curr_slice = slice;
}

void CTexture::Preload() { Preload(cName.c_str()); }

void CTexture::Preload(const char* Name)
{
    m_bumpmap = DEV->m_textures_description.GetBumpName(Name);
    m_material = DEV->m_textures_description.GetMaterial(Name);
    m_detail_scale = DEV->m_textures_description.GetScale(Name);
}

void CTexture::Load() { Load(cName.c_str()); }

void CTexture::Load(const char* Name)
{
    if (flags.bLoaded)
        return;

    flags.bLoaded = true;

    loadedName = Name;

    flags.memUsage = 0;

    if (0 == _stricmp(Name, "$null"))
    {
        return;
    }

    if (nullptr != strstr(Name, "$user$"))
    {
        return;
    }

    ZoneScoped;

    Preload(Name);

    // Check for OGM
    string_path fn;
    if (FS.exist(fn, fsgame::game_textures, Name, ".ogm"))
    {
        // AVI
        pTheora = xr_new<CTheoraSurface>();
        m_play_time = 0xFFFFFFFF;

        if (!pTheora->Load(fn))
        {
            xr_delete(pTheora);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.memUsage = pTheora->Width(true) * pTheora->Height(true) * 4;
            pTheora->Play(TRUE, Device.dwTimeContinual);

            // Now create texture
            ID3DTexture2D* pTexture = nullptr;

            D3D_TEXTURE2D_DESC desc;
            desc.Width = pTheora->Width(false);
            desc.Height = pTheora->Height(false);
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D_USAGE_DYNAMIC;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            const HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pTheora);
                pSurface = nullptr;
                m_pSRView = nullptr;
            }
            else
            {
                pSurface = pTexture;
                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, fsgame::game_textures, Name, ".avi"))
    {
        // AVI
        pAVI = xr_new<CAviPlayerCustom>();

        if (!pAVI->Load(fn))
        {
            xr_delete(pAVI);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.memUsage = pAVI->m_dwWidth * pAVI->m_dwHeight * 4;

            ID3DTexture2D* pTexture = nullptr;

            D3D_TEXTURE2D_DESC desc;
            desc.Width = pAVI->m_dwWidth;
            desc.Height = pAVI->m_dwHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D_USAGE_DYNAMIC;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            const HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pAVI);
                pSurface = nullptr;
                m_pSRView = nullptr;
            }
            else
            {
                pSurface = pTexture;
                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, fsgame::game_textures, Name, ".seq"))
    {
        // Sequence
        string256 buffer;
        IReader* _fs = FS.r_open(fn);

        flags.seqCycles = FALSE;
        _fs->r_string(buffer, sizeof(buffer));
        if (0 == _stricmp(buffer, "cycled"))
        {
            flags.seqCycles = TRUE;
            _fs->r_string(buffer, sizeof(buffer));
        }
        const u32 fps = atoi(buffer);
        seqMSPF = 1000 / fps;

        while (!_fs->eof())
        {
            _fs->r_string(buffer, sizeof(buffer));
            _Trim(buffer);

            if (buffer[0])
            {
                // Load another texture
                u32 mem = 0;
                pSurface = CRender::texture_load(buffer, mem);

                if (pSurface)
                {
                    seqDATA.push_back(pSurface);

                    m_seqSRView.push_back(nullptr);
                    HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_seqSRView.back());

                    flags.memUsage += mem;
                }
            }
        }

        FS.r_close(_fs);

        if (pSurface)
        {
            D3D_RESOURCE_DIMENSION type;
            pSurface->GetType(&type);

            if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
            {
                D3D_TEXTURE2D_DESC desc;

                ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
                T->GetDesc(&desc);

                desc_Width = desc.Width;
                desc_Height = desc.Height;
            }
        }

        pSurface = nullptr;
    }
    else
    {
        _RELEASE(pSurface);

        // Normal texture
        u32 mem = 0;
        pSurface = CRender::texture_load(Name, mem);

        // Calc memory usage and preload into vid-mem
        if (pSurface)
        {
            flags.memUsage = mem;
            
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
        }
    }

    if (pSurface)
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
        {
            D3D_TEXTURE2D_DESC desc;

            ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
            T->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;
        }
    }

    if (pSurface)
    {
        DXUT_SetDebugName(pSurface, cName.c_str());
    }

    PostLoad();
}

void CTexture::UnloadImpl()
{
#ifdef DEBUG
    string_path msg_buff;
    xr_sprintf(msg_buff, sizeof(msg_buff), "* Unloading texture [%s] pSurface RefCount=", cName.c_str());
#endif // DEBUG

    // if (flags.bLoaded) Msg("* Unloaded: %s",cName.c_str());

    flags.bLoaded = false;

    if (!seqDATA.empty())
    {
        for (u32 I = 0; I < seqDATA.size(); I++)
        {
            _RELEASE(seqDATA[I]);
            _RELEASE(m_seqSRView[I]);
        }
        seqDATA.clear();
        m_seqSRView.clear();

        pSurface = nullptr;
        m_pSRView = nullptr;
    }

#ifdef DEBUG
    _SHOW_REF(msg_buff, pSurface);
#endif // DEBUG

    _RELEASE(pSurface);

    if (!srv_per_slice.empty())
    {
        _RELEASE(srv_all);

        for (auto& srv : srv_per_slice)
        {
            _RELEASE(srv);
        }

        m_pSRView = nullptr;
    }
    else
    {
        _RELEASE(m_pSRView); // normal texture
    }

    xr_delete(pAVI);
    xr_delete(pTheora);

    bind = fastdelegate::MakeDelegate(this, &CTexture::apply_load);
}

void CTexture::Unload() { UnloadImpl(); }

void CTexture::video_Play(BOOL looped, u32 _time)
{
    if (pTheora)
        pTheora->Play(looped, (_time != 0xFFFFFFFF) ? (m_play_time = _time) : Device.dwTimeContinual);
}

void CTexture::video_Pause(BOOL state)
{
    if (pTheora)
        pTheora->Pause(state);
}

void CTexture::video_Stop()
{
    if (pTheora)
        pTheora->Stop();
}

BOOL CTexture::video_IsPlaying() { return (pTheora) ? pTheora->IsPlaying() : FALSE; }
