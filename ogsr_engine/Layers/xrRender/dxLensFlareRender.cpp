#include "stdafx.h"
#include "dxLensFlareRender.h"
#include "../../xr_3da/xr_efflensflare.h"
#include "../../xr_3da/iGame_persistent.h"

#define MAX_Flares 24


void dxFlareRender::Copy(IFlareRender& _in) { *this = *(dxFlareRender*)&_in; }

void dxFlareRender::CreateShader(LPCSTR sh_name, LPCSTR tex_name)
{
    if (tex_name && tex_name[0])
        hShader.create(sh_name, tex_name);
}

void dxFlareRender::DestroyShader() { hShader.destroy(); }

void dxLensFlareRender::Copy(ILensFlareRender& _in) { *this = *(dxLensFlareRender*)&_in; }

void dxLensFlareRender::Render(CBackend& cmd_list, CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient)
{
    Fcolor dwLight;
    Fcolor color;
    Fvector vecSx, vecSy;

    dwLight.set(owner.LightColor);
    svector<ref_shader, MAX_Flares> _2render;

    u32 VS_Offset;
    FVF::LIT* pv = (FVF::LIT*)RImplementation.Vertex.Lock(MAX_Flares * 4, hGeom.stride(), VS_Offset);

    const float fDistance = g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.75f;

    if (bSun)
    {
        if (owner.m_Current->m_Flags.is(CLensFlareDescriptor::flSource))
        {
            vecSx.mul(owner.vecX, owner.m_Current->m_Source.fRadius * fDistance);
            vecSy.mul(owner.vecY, owner.m_Current->m_Source.fRadius * fDistance);
            if (owner.m_Current->m_Source.ignore_color)
                color.set(1.f, 1.f, 1.f, 1.f);
            else
                color.set(dwLight);
            color.a *= owner.m_StateBlend;
            const u32 c = color.get();
            pv->set(owner.vecLight.x + vecSx.x - vecSy.x, owner.vecLight.y + vecSx.y - vecSy.y, owner.vecLight.z + vecSx.z - vecSy.z, c, 0, 0);
            pv++;
            pv->set(owner.vecLight.x + vecSx.x + vecSy.x, owner.vecLight.y + vecSx.y + vecSy.y, owner.vecLight.z + vecSx.z + vecSy.z, c, 0, 1);
            pv++;
            pv->set(owner.vecLight.x - vecSx.x - vecSy.x, owner.vecLight.y - vecSx.y - vecSy.y, owner.vecLight.z - vecSx.z - vecSy.z, c, 1, 0);
            pv++;
            pv->set(owner.vecLight.x - vecSx.x + vecSy.x, owner.vecLight.y - vecSx.y + vecSy.y, owner.vecLight.z - vecSx.z + vecSy.z, c, 1, 1);
            pv++;
            _2render.push_back(((dxFlareRender*)&*owner.m_Current->m_Source.m_pRender)->hShader);
        }
    }
    if (owner.fBlend >= EPS_L)
    {
        if (owner.m_Current->m_Flags.is(CLensFlareDescriptor::flFlare))
        {
            if (bFlares)
            {
                Fvector vecDy;
                Fvector vecDx;
                vecDx.normalize(owner.vecAxis);
                vecDy.crossproduct(vecDx, owner.vecDir);

                Fvector vec;
                for (CLensFlareDescriptor::FlareIt it = owner.m_Current->m_Flares.begin(); it != owner.m_Current->m_Flares.end(); it++)
                {
                    const CLensFlareDescriptor::SFlare& F = *it;
                    vec.mul(owner.vecAxis, F.fPosition);
                    vec.add(owner.vecCenter);
                    vecSx.mul(vecDx, F.fRadius * fDistance);
                    vecSy.mul(vecDy, F.fRadius * fDistance);
                    const float cl = F.fOpacity * owner.fBlend * owner.m_StateBlend;
                    color.set(dwLight);
                    color.mul_rgba(cl);
                    const u32 c = color.get();
                    pv->set(vec.x + vecSx.x - vecSy.x, vec.y + vecSx.y - vecSy.y, vec.z + vecSx.z - vecSy.z, c, 0, 0);
                    pv++;
                    pv->set(vec.x + vecSx.x + vecSy.x, vec.y + vecSx.y + vecSy.y, vec.z + vecSx.z + vecSy.z, c, 0, 1);
                    pv++;
                    pv->set(vec.x - vecSx.x - vecSy.x, vec.y - vecSx.y - vecSy.y, vec.z - vecSx.z - vecSy.z, c, 1, 0);
                    pv++;
                    pv->set(vec.x - vecSx.x + vecSy.x, vec.y - vecSx.y + vecSy.y, vec.z - vecSx.z + vecSy.z, c, 1, 1);
                    pv++;
                    _2render.push_back(((dxFlareRender*)&*it->m_pRender)->hShader);
                }
            }
        }
        // gradient
        if (bGradient && (owner.fGradientValue >= EPS_L))
        {
            if (owner.m_Current->m_Flags.is(CLensFlareDescriptor::flGradient))
            {
                vecSx.mul(owner.vecX, owner.m_Current->m_Gradient.fRadius * owner.fGradientValue * fDistance);
                vecSy.mul(owner.vecY, owner.m_Current->m_Gradient.fRadius * owner.fGradientValue * fDistance);

                color.set(dwLight);
                color.mul_rgba(owner.fGradientValue * owner.m_StateBlend);

                const u32 c = color.get();
                pv->set(owner.vecLight.x + vecSx.x - vecSy.x, owner.vecLight.y + vecSx.y - vecSy.y, owner.vecLight.z + vecSx.z - vecSy.z, c, 0, 0);
                pv++;
                pv->set(owner.vecLight.x + vecSx.x + vecSy.x, owner.vecLight.y + vecSx.y + vecSy.y, owner.vecLight.z + vecSx.z + vecSy.z, c, 0, 1);
                pv++;
                pv->set(owner.vecLight.x - vecSx.x - vecSy.x, owner.vecLight.y - vecSx.y - vecSy.y, owner.vecLight.z - vecSx.z - vecSy.z, c, 1, 0);
                pv++;
                pv->set(owner.vecLight.x - vecSx.x + vecSy.x, owner.vecLight.y - vecSx.y + vecSy.y, owner.vecLight.z - vecSx.z + vecSy.z, c, 1, 1);
                pv++;
                _2render.push_back(((dxFlareRender*)&*owner.m_Current->m_Gradient.m_pRender)->hShader);
            }
        }
    }
    RImplementation.Vertex.Unlock(_2render.size() * 4, hGeom.stride());

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Geometry(hGeom);
    for (u32 i = 0; i < _2render.size(); i++)
    {
        if (_2render[i])
        {
            const u32 vBase = i * 4 + VS_Offset;
            cmd_list.set_Shader(_2render[i]);
            cmd_list.Render(D3DPT_TRIANGLELIST, vBase, 0, 4, 0, 2);
        }
    }
}

void dxLensFlareRender::OnDeviceCreate() { hGeom.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB); }

void dxLensFlareRender::OnDeviceDestroy() { hGeom.destroy(); }