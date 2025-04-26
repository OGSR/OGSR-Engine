#pragma once

class IUIShader;

class IUIRender
{
public:
    enum ePrimitiveType
    {
        ptNone = -1,
        ptTriList,
        ptTriStrip,
        ptLineStrip,
        ptLineList
    };

    enum ePointType
    {
        pttNone = -1,
        pttTL,
        pttLIT
    };

    enum CullMode
    {
        cmNONE = 0,
        cmCW,
        cmCCW,
    };

public:
    // virtual ~IUIRender() {;}

    virtual void CreateUIGeom() = 0;
    virtual void DestroyUIGeom() = 0;

    virtual void SetShader(IUIShader& shader) = 0;
    virtual void SetAlphaRef(int aref) = 0;

    virtual void SetScissor(Irect* rect = nullptr) = 0;
    virtual void GetActiveTextureResolution(Fvector2& res) = 0;

    virtual void PushPoint(float x, float y, float z, u32 C, float u, float v) = 0;

    virtual void StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) = 0;
    virtual void FlushPrimitive() = 0;

    virtual LPCSTR UpdateShaderName(LPCSTR tex_name, LPCSTR sh_name) = 0;

    virtual void CacheSetXformWorld(const Fmatrix& M) = 0;
    virtual void CacheSetCullMode(CullMode) = 0;
};