//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#pragma once

#include "../../Include/xrRender/DrawUtils.h"
//----------------------------------------------------

struct SPrimitiveBuffer
{
    ref_geom pGeom;
    u32 v_cnt;
    u32 i_cnt;
    D3DPRIMITIVETYPE p_type;
    u32 p_cnt;
    fastdelegate::FastDelegate<void()> OnRender;
    void RenderDIP() const;
    void RenderDP();

public:
    SPrimitiveBuffer() : OnRender(nullptr), pGeom(nullptr) { ; }
    void CreateFromData(D3DPRIMITIVETYPE _pt, u32 _p_cnt, u32 FVF, LPVOID vertices, u32 _v_cnt, u16* indices = nullptr, u32 _i_cnt = 0);
    void Destroy();
    void Render() const { OnRender(); }
};

//----------------------------------------------------
// Utilities
//----------------------------------------------------
class ECORE_API CDrawUtilities : public CDUInterface, public pureRender
{
    SPrimitiveBuffer m_SolidCone;
    SPrimitiveBuffer m_WireCone;
    SPrimitiveBuffer m_SolidSphere;
    SPrimitiveBuffer m_WireSphere;
    SPrimitiveBuffer m_SolidSpherePart;
    SPrimitiveBuffer m_WireSpherePart;
    SPrimitiveBuffer m_SolidCylinder;
    SPrimitiveBuffer m_WireCylinder;
    SPrimitiveBuffer m_SolidBox;
    SPrimitiveBuffer m_WireBox;
    CGameFont* m_Font;

public:
    ref_geom vs_L;
    ref_geom vs_TL;
    ref_geom vs_LIT;

protected:
    FVF::L* m_DD_pv;
    FVF::L* m_DD_pv_start;
    u32 m_DD_base;
    BOOL m_DD_wire{};
    void DD_DrawFace_flush(BOOL try_again);

public:
    void DD_DrawFace_begin(BOOL bWire);
    void DD_DrawFace_push(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr);
    void DD_DrawFace_end();

public:
    CDrawUtilities()
    {
        vs_L = nullptr;
        vs_TL = nullptr;
        vs_LIT = nullptr;
        m_DD_pv = nullptr;
        m_DD_pv_start = nullptr;
        m_DD_base = 0;
        m_Font = nullptr;
    }

    void OnDeviceCreate();
    virtual void OnDeviceDestroy();

    void UpdateGrid(int number_of_cell, float square_size, int subdiv = 10);

    //----------------------------------------------------
    virtual void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, u32 clr, BOOL bRot45 = false);
    virtual void DrawCross(const Fvector& p, float sz, u32 clr, BOOL bRot45 = false) { DrawCross(p, sz, sz, sz, sz, sz, sz, clr, bRot45); }
    virtual void DrawEntity(u32 clr, ref_shader s);
    virtual void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, u32 clr, BOOL bDrawEntity);
    virtual void DrawRomboid(const Fvector& p, float radius, u32 clr);
    virtual void DrawJoint(const Fvector& p, float radius, u32 clr);

    virtual void DrawSpotLight(const Fvector& p, const Fvector& d, float range, float phi, u32 clr);
    virtual void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, u32 clr);
    virtual void DrawPointLight(const Fvector& p, float radius, u32 clr);

    virtual void DrawSound(const Fvector& p, float radius, u32 clr);
    virtual void DrawLineSphere(const Fvector& p, float radius, u32 clr, BOOL bCross);

    virtual void dbgDrawPlacement(const Fvector& p, int sz, u32 clr, LPCSTR caption = nullptr, u32 clr_font = 0xffffffff);
    virtual void dbgDrawVert(const Fvector& p0, u32 clr, LPCSTR caption = nullptr);
    virtual void dbgDrawEdge(const Fvector& p0, const Fvector& p1, u32 clr, LPCSTR caption = nullptr);
    virtual void dbgDrawFace(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr, LPCSTR caption = nullptr);

    virtual void DrawFace(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
    virtual void DrawLine(const Fvector& p0, const Fvector& p1, u32 clr);
    virtual void DrawLink(const Fvector& p0, const Fvector& p1, float sz, u32 clr);
    IC virtual void DrawFaceNormal(const Fvector& p0, const Fvector& p1, const Fvector& p2, float size, u32 clr)
    {
        Fvector N, C, P;
        N.mknormal(p0, p1, p2);
        C.set(p0);
        C.add(p1);
        C.add(p2);
        C.div(3);
        P.mad(C, N, size);
        DrawLine(C, P, clr);
    }
    IC virtual void DrawFaceNormal(const Fvector* p, float size, u32 clr) { DrawFaceNormal(p[0], p[1], p[2], size, clr); }
    IC virtual void DrawFaceNormal(const Fvector& C, const Fvector& N, float size, u32 clr)
    {
        Fvector P;
        P.mad(C, N, size);
        DrawLine(C, P, clr);
    }
    virtual void DrawSelectionBox(const Fvector& center, const Fvector& size, u32* c = nullptr);
    IC virtual void DrawSelectionBoxB(const Fbox& box, u32* c = nullptr)
    {
        Fvector S, C;
        box.getsize(S);
        box.getcenter(C);
        DrawSelectionBox(C, S, c);
    }
    virtual void DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
    virtual void DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
    virtual void DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
    virtual void DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
    virtual void DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);

    virtual void DrawBox(const Fvector& offs, const Fvector& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
    virtual void DrawAABB(const Fvector& p0, const Fvector& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
    virtual void DrawAABB(const Fmatrix& parent, const Fvector& center, const Fvector& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
    virtual void DrawOBB(const Fmatrix& parent, const Fobb& box, u32 clr_s, u32 clr_w);
    virtual void DrawSphere(const Fmatrix& parent, const Fvector& center, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
    virtual void DrawSphere(const Fmatrix& parent, const Fsphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
    {
        DrawSphere(parent, S.P, S.R, clr_s, clr_w, bSolid, bWire);
    }
    virtual void DrawCylinder(const Fmatrix& parent, const Fvector& center, const Fvector& dir, float height, float radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                                        BOOL bWire);
    virtual void DrawCone(const Fmatrix& parent, const Fvector& apex, const Fvector& dir, float height, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
    virtual void DrawPlane(const Fvector& center, const Fvector2& scale, const Fvector& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
    virtual void DrawPlane(const Fvector& p, const Fvector& n, const Fvector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
    virtual void DrawRectangle(const Fvector& o, const Fvector& u, const Fvector& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);

    virtual void DrawGrid();
    virtual void DrawPivot(const Fvector& pos, float sz = 5.f);
    virtual void DrawAxis(const Fmatrix& T);
    virtual void DrawObjectAxis(const Fmatrix& T, float sz, BOOL sel);
    virtual void DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd);

    virtual void DrawIndexedPrimitive(int prim_type, u32 pc, const Fvector& pos, const Fvector* vb, const u32& vb_size, const u32* ib, const u32& ib_size,
                                                const u32& clr_argb, float scale = 1.0f){};
    virtual void DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, Fvector* vertices, int vc, u32 color, BOOL bCull, BOOL bCycle);
    virtual void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, int vc, BOOL bCull, BOOL bCycle);
    virtual void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, int vc, BOOL bCull, BOOL bCycle);

    virtual void OutText(const Fvector& pos, LPCSTR text, u32 color = 0xFF000000, u32 shadow_color = 0xFF909090);

    virtual void OnRender();
};
extern ECORE_API CDrawUtilities DUImpl;
