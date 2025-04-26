#pragma once

// refs
struct FSlideWindowItem;

#include "FBasicVisual.h"

class FTreeVisual : public dxRender_Visual, public IRender_Mesh
{
public:
    virtual void Render(CBackend& cmd_list, float lod, bool use_fast_geo); // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void DoRenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data, u32 countV, u32 startI, u32 PC);
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

    FTreeVisual(void);
    virtual ~FTreeVisual(void);
};

class FTreeVisual_ST : public FTreeVisual
{
    typedef FTreeVisual inherited;

public:
    FTreeVisual_ST(void);
    virtual ~FTreeVisual_ST(void);

    void Render(CBackend& cmd_list, float lod, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void RenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data) override;
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

private:
    FTreeVisual_ST(const FTreeVisual_ST& other);
    void operator=(const FTreeVisual_ST& other) = delete;
};

class FTreeVisual_PM : public FTreeVisual
{
    typedef FTreeVisual inherited;

    FSlideWindowItem* pSWI{};
    u32 selected_lod_id[R__NUM_CONTEXTS]{};

public:
    FTreeVisual_PM(void);
    virtual ~FTreeVisual_PM(void);

    void select_lod_id(float lod, u32 context_id) override;

    void Render(CBackend& cmd_list, float lod, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void RenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data) override;
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

private:
    FTreeVisual_PM(const FTreeVisual_PM& other);
    void operator=(const FTreeVisual_PM& other) = delete;
};
