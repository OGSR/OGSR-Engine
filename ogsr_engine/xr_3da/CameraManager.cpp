// CameraManager.cpp: implementation of the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "Environment.h"
#include "CameraBase.h"
#include "CameraManager.h"
#include "Effector.h"
#include "EffectorPP.h"

#include "x_ray.h"
#include "gamefont.h"
#include "render.h"

#define effCustomEffectorStartID 10000

float psCamInert = 0.f; // 0.7f;
float psSprintCamInert = 0.0f; // 0.7f;

float psCamSlideInert = 0.25f; // for 3rd person cam ?

SPPInfo pp_identity;
SPPInfo pp_zero;

SPPInfo& SPPInfo::add(const SPPInfo& ppi)
{
    blur += ppi.blur;
    gray += ppi.gray;
    duality.h += ppi.duality.h;
    duality.v += ppi.duality.v;

    noise.intensity = std::max(noise.intensity, ppi.noise.intensity);
    noise.grain = std::max(noise.grain, ppi.noise.grain);
    noise.fps = std::max(noise.fps, ppi.noise.fps);
    color_base += ppi.color_base;
    color_gray += ppi.color_gray;
    color_add += ppi.color_add;

    if (ppi.cm_tex1.size())
    {
        if (cm_tex1.size())
        {
            cm_tex2 = ppi.cm_tex1;
            cm_interpolate = 1.0f - cm_influence / (cm_influence + ppi.cm_influence);
        }
        else
        {
            cm_tex1 = ppi.cm_tex1;
            cm_influence = ppi.cm_influence;
            cm_interpolate = 0.0f;
        }
        cm_influence = std::max(cm_influence, ppi.cm_influence);
    }
    return *this;
}

SPPInfo& SPPInfo::sub(const SPPInfo& ppi)
{
    blur -= ppi.blur;
    gray -= ppi.gray;
    duality.h -= ppi.duality.h;
    duality.v -= ppi.duality.v;
    color_base -= ppi.color_base;
    color_gray -= ppi.color_gray;
    color_add -= ppi.color_add;
    return *this;
}

SPPInfo::SPPInfo()
{
    blur = gray = duality.h = duality.v = 0;
    noise.intensity = 0;
    noise.grain = 1;
    noise.fps = 10;
    color_base.set(.5f, .5f, .5f);
    color_gray.set(.333f, .333f, .333f);
    color_add.set(0.f, 0.f, 0.f);
    cm_influence = 0.0f;
    cm_interpolate = 0.0f;
}
void SPPInfo::normalize() {}
void SPPInfo::validate(LPCSTR str)
{
    VERIFY(_valid(duality.h), str);
    VERIFY(_valid(duality.v), str);
    VERIFY(_valid(blur), str);
    VERIFY(_valid(gray), str);
    VERIFY(_valid(noise.intensity), str);
    VERIFY(_valid(noise.grain), str);
    VERIFY(_valid(noise.fps), str);
    VERIFY(_valid(color_base.r), str);
    VERIFY(_valid(color_base.g), str);
    VERIFY(_valid(color_base.b), str);
    VERIFY(_valid(color_gray.r), str);
    VERIFY(_valid(color_gray.g), str);
    VERIFY(_valid(color_gray.b), str);
    VERIFY(_valid(color_add.r), str);
    VERIFY(_valid(color_add.g), str);
    VERIFY(_valid(color_add.b), str);
}

SPPInfo& SPPInfo::lerp(const SPPInfo& def, const SPPInfo& to, float factor)
{
    VERIFY(_valid(factor));
    SPPInfo& pp = *this;
    clamp(factor, 0.0f, 1.0f);

    pp.duality.h += def.duality.h + (to.duality.h - def.duality.h) * factor;
    pp.duality.v += def.duality.v + (to.duality.v - def.duality.v) * factor;
    pp.gray += def.gray + (to.gray - def.gray) * factor;
    pp.blur += def.blur + (to.blur - def.blur) * factor;
    pp.noise.intensity = to.noise.intensity; // + (to.noise.intensity - def.noise.intensity) * factor;
    pp.noise.grain = to.noise.grain; // + (to.noise.grain - def.noise.grain) * factor;
    pp.noise.fps = to.noise.fps; // + (to.noise.fps - def.noise.fps) * factor;

    pp.color_base.set(def.color_base.r + (to.color_base.r - def.color_base.r) * factor, def.color_base.g + (to.color_base.g - def.color_base.g) * factor,
                      def.color_base.b + (to.color_base.b - def.color_base.b) * factor);

    pp.color_gray.set(def.color_gray.r + (to.color_gray.r - def.color_gray.r) * factor, def.color_gray.g + (to.color_gray.g - def.color_gray.g) * factor,
                      def.color_gray.b + (to.color_gray.b - def.color_gray.b) * factor);

    pp.color_add.set(def.color_add.r + (to.color_add.r - def.color_add.r) * factor, def.color_add.g + (to.color_add.g - def.color_add.g) * factor,
                     def.color_add.b + (to.color_add.b - def.color_add.b) * factor);

    pp.cm_tex1 = to.cm_tex1;
    pp.cm_tex2 = to.cm_tex2;
    pp.cm_influence += def.cm_influence + (to.cm_influence - def.cm_influence) * factor;
    pp.cm_interpolate += def.cm_interpolate + (to.cm_interpolate - def.cm_interpolate) * factor;

    return *this;
}

CCameraManager::CCameraManager(bool bApplyOnUpdate)
{
#ifdef DEBUG
    dbg_upd_frame = 0;
#endif

    m_bAutoApply = bApplyOnUpdate;

    pp_identity.blur = 0;
    pp_identity.gray = 0;
    pp_identity.duality.h = 0;
    pp_identity.duality.v = 0;
    pp_identity.noise.intensity = 0;
    pp_identity.noise.grain = 1.0f;
    pp_identity.noise.fps = 30;
    pp_identity.color_base.set(.5f, .5f, .5f);
    pp_identity.color_gray.set(.333f, .333f, .333f);
    pp_identity.color_add.set(0, 0, 0);

    pp_zero.blur = pp_zero.gray = pp_zero.duality.h = pp_zero.duality.v = 0.0f;
    pp_zero.noise.intensity = 0;
    pp_zero.noise.grain = 0.0f;
    pp_zero.noise.fps = 0.0f;
    pp_zero.color_base.set(0, 0, 0);
    pp_zero.color_gray.set(0, 0, 0);
    pp_zero.color_add.set(0, 0, 0);

    pp_affected = pp_identity;
}

CCameraManager::~CCameraManager()
{
    for (auto& it : m_EffectorsCam)
        xr_delete(it);

    for (auto& it : m_EffectorsPP)
        xr_delete(it);
}

CEffectorCam* CCameraManager::GetCamEffector(ECamEffectorType type)
{
    for (auto& it : m_EffectorsCam)
        if (it->eType == type)
        {
            return it;
        }
    return nullptr;
}

CEffectorCam* CCameraManager::AddCamEffector(CEffectorCam* ef)
{
    m_EffectorsCam_added_deffered.push_back(ef);
    return m_EffectorsCam_added_deffered.back();
}

void CCameraManager::UpdateDeffered()
{
    auto it = m_EffectorsCam_added_deffered.begin();
    auto it_e = m_EffectorsCam_added_deffered.end();
    for (; it != it_e; ++it)
    {
        RemoveCamEffector((*it)->eType);

        if ((*it)->AbsolutePositioning())
            m_EffectorsCam.push_front(*it);
        else
            m_EffectorsCam.push_back(*it);

        OnEffectorAdded(*it);
    }

    m_EffectorsCam_added_deffered.clear();
}

void CCameraManager::RemoveCamEffector(ECamEffectorType type)
{
    for (auto it = m_EffectorsCam.begin(); it != m_EffectorsCam.end(); ++it)
        if ((*it)->eType == type)
        {
            OnEffectorReleased(*it);
            xr_delete(*it);
            m_EffectorsCam.erase(it);
            return;
        }
}

void CCameraManager::RemoveAllCamEffector()
{
    while (!m_EffectorsCam.empty())
    {
        RemoveCamEffector(m_EffectorsCam.back()->eType);
    }
}

CEffectorPP* CCameraManager::GetPPEffector(EEffectorPPType type)
{
    for (auto& it : m_EffectorsPP)
        if (it->Type() == type)
            return it;
    return nullptr;
}

ECamEffectorType CCameraManager::RequestCamEffectorId()
{
    ECamEffectorType index = (ECamEffectorType)effCustomEffectorStartID;
    for (; GetCamEffector(index); index = (ECamEffectorType)(index + 1))
    {
        ;
    }
    return index;
}

EEffectorPPType CCameraManager::RequestPPEffectorId()
{
    EEffectorPPType index = (EEffectorPPType)effCustomEffectorStartID;
    for (; GetPPEffector(index); index = (EEffectorPPType)(index + 1))
    {
        ;
    }
    return index;
}

CEffectorPP* CCameraManager::AddPPEffector(CEffectorPP* ef)
{
    RemovePPEffector(ef->Type());
    m_EffectorsPP.push_back(ef);
    return m_EffectorsPP.back();
}

void CCameraManager::RemovePPEffector(EEffectorPPType type)
{
    for (auto it = m_EffectorsPP.begin(); it != m_EffectorsPP.end(); ++it)
        if ((*it)->Type() == type)
        {
            OnEffectorReleased(*it);
            if ((*it)->FreeOnRemove())
            {
                xr_delete(*it);
            }
            m_EffectorsPP.erase(it);
            return;
        }
}

void CCameraManager::RemoveAllPPEffector()
{
    while (!m_EffectorsPP.empty())
    {
        RemovePPEffector(m_EffectorsPP.back()->Type());
    }
}

void CCameraManager::OnEffectorAdded(SBaseEffector* e) { }

void CCameraManager::OnEffectorReleased(SBaseEffector* e) { }

void CCameraManager::UpdateFromCamera(const CCameraBase* C)
{
    Update(C->vPosition, C->vDirection, C->vNormal, C->f_fov, C->f_aspect, g_pGamePersistent->Environment().CurrentEnv->far_plane, C->m_Flags.flags);
}

void CCameraManager::Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags)
{
    ZoneScoped;

#ifdef DEBUG
    VERIFY(dbg_upd_frame != Device.dwFrame); // already updated !!!
    dbg_upd_frame = Device.dwFrame;
#endif
    float v = psCamInert;

    // camera
    if (flags & CCameraBase::flPositionRigid)
        m_cam_info.p.set(P);
    else
        m_cam_info.p.inertion(P, v);

    if (flags & CCameraBase::flDirectionRigid)
    {
        m_cam_info.d.set(D);
        m_cam_info.n.set(N);
    }
    else
    {
        m_cam_info.d.inertion(D, v);
        m_cam_info.n.inertion(N, v);
    }

    // Normalize
    m_cam_info.d.normalize();
    m_cam_info.n.normalize();
    m_cam_info.r.crossproduct(m_cam_info.n, m_cam_info.d);
    m_cam_info.n.crossproduct(m_cam_info.d, m_cam_info.r);

    float aspect = Device.fHeight_2 / Device.fWidth_2;
    float src = 10 * Device.fTimeDelta;
    clamp(src, 0.f, 1.f);
    float dst = 1 - src;
    m_cam_info.fFov = m_cam_info.fFov * dst + fFOV_Dest * src;
    m_cam_info.fNear = VIEWPORT_NEAR;
    m_cam_info.fFar = m_cam_info.fFar * dst + fFAR_Dest * src;
    m_cam_info.fAspect = m_cam_info.fAspect * dst + (fASPECT_Dest * aspect) * src;
    m_cam_info.dont_apply = false;

    UpdateCamEffectors();

    UpdatePPEffectors();

    if (!m_cam_info.dont_apply && m_bAutoApply)
        ApplyDevice();

    UpdateDeffered();
}

bool CCameraManager::ProcessCameraEffector(CEffectorCam* eff)
{
    // Do NOT delete effector here! It's unsafe because:
    // 1. Leads to failed iterators in UpdateCamEffectors
    // 2. Child classes with overrided ProcessCameraEffector would be surprised if eff becames invalid pointer
    // The best way - return 'false' when the effector should be deleted, and delete it in ProcessCameraEffector

    bool res = false;
    if (eff->Valid() && eff->ProcessCam(m_cam_info))
    {
        res = true;
    }
    else if (eff->AllowProcessingIfInvalid())
    {
        eff->ProcessIfInvalid(m_cam_info);
    }
    return res;
}

void CCameraManager::UpdateCamEffectors()
{
    if (m_EffectorsCam.empty())
        return;

    auto r_it = m_EffectorsCam.rbegin();
    while (r_it != m_EffectorsCam.rend())
    {
        if (ProcessCameraEffector(*r_it))
            ++r_it;
        else
        {
            OnEffectorReleased(*r_it);

            xr_delete(*r_it);

            // Dereferencing reverse iterator returns previous element of the list, r_it.base() returns current element
            // So, we should use base()-1 iterator to delete just processed element. 'Previous' element would be
            // automatically changed after deletion, so r_it would dereferencing to another value, no need to change it

            auto r_to_del = r_it.base();
            m_EffectorsCam.erase(--r_to_del);
        }
    }

    m_cam_info.d.normalize();
    m_cam_info.n.normalize();
    m_cam_info.r.crossproduct(m_cam_info.n, m_cam_info.d);
    m_cam_info.n.crossproduct(m_cam_info.d, m_cam_info.r);
}

void CCameraManager::UpdatePPEffectors()
{
    pp_affected.validate("before applying pp");

    if (!m_EffectorsPP.empty())
    {
        int _count = 0;
        bool b = false;
        pp_affected = pp_identity;
        for (int i = m_EffectorsPP.size() - 1; i >= 0; --i)
        {
            CEffectorPP* eff = m_EffectorsPP[i];
            SPPInfo l_PPInf = pp_zero;
            if (eff->Valid() && eff->Process(l_PPInf))
            {
                ++_count;
                if (!b)
                {
                    pp_affected.add(l_PPInf);
                    pp_affected.sub(pp_identity);
                    pp_affected.validate("in cycle");
                }
                if (!eff->bOverlap)
                {
                    b = true;
                    pp_affected = l_PPInf;
                }
            }
            else
                RemovePPEffector(eff->Type());
        }
        if (0 == _count)
            pp_affected = pp_identity;
        else
            pp_affected.normalize();
    }
    else
    {
        pp_affected = pp_identity;
    }

    if (!positive(pp_affected.noise.grain))
        pp_affected.noise.grain = pp_identity.noise.grain;

    pp_affected.validate("after applying pp");
}

void CCameraManager::ApplyDevice(bool effectOnly)
{
    ZoneScoped;

    if (!effectOnly)
    {
        // Device params
        Device.mView.build_camera_dir(m_cam_info.p, m_cam_info.d, m_cam_info.n);

        Device.vCameraPosition.set(m_cam_info.p);
        Device.vCameraDirection.set(m_cam_info.d);
        Device.vCameraTop.set(m_cam_info.n);
        Device.vCameraRight.set(m_cam_info.r);
    }

    // projection
    Device.fFOV = m_cam_info.fFov;
    Device.fASPECT = m_cam_info.fAspect;

    Device.mProject.build_projection(deg2rad(Device.fFOV), m_cam_info.fAspect, m_cam_info.fNear, m_cam_info.fFar);

    if (IsMainMenuActive())
    {
        ResetPP();
    }
    else
    {
        pp_affected.validate("apply device");
        // postprocess
        IRender_Target* T = ::Render->getTarget();
        T->set_duality_h(pp_affected.duality.h);
        T->set_duality_v(pp_affected.duality.v);
        T->set_blur(pp_affected.blur);
        T->set_gray(pp_affected.gray);
        T->set_noise(pp_affected.noise.intensity);

        clamp(pp_affected.noise.grain, EPS_L, 1000.0f);

        T->set_noise_scale(pp_affected.noise.grain);

        T->set_noise_fps(pp_affected.noise.fps);
        T->set_color_base(pp_affected.color_base);
        T->set_color_gray(pp_affected.color_gray);
        T->set_color_add(pp_affected.color_add);

        T->set_cm_imfluence(pp_affected.cm_influence);
        T->set_cm_interpolate(pp_affected.cm_interpolate);
        T->set_cm_textures(pp_affected.cm_tex1, pp_affected.cm_tex2);
    }
}

void CCameraManager::ResetPP()
{
    IRender_Target* T = ::Render->getTarget();
    T->set_duality_h(pp_identity.duality.h);
    T->set_duality_v(pp_identity.duality.v);
    T->set_blur(pp_identity.blur);
    T->set_gray(pp_identity.gray);
    T->set_noise(pp_identity.noise.intensity);
    T->set_noise_scale(pp_identity.noise.grain);
    T->set_noise_fps(pp_identity.noise.fps);
    T->set_color_base(pp_identity.color_base);
    T->set_color_gray(pp_identity.color_gray);
    T->set_color_add(pp_identity.color_add);
    T->set_cm_imfluence(0.0f);
    T->set_cm_interpolate(1.0f);
    T->set_cm_textures("", "");
}

void CCameraManager::Dump()
{
    Fmatrix mInvCamera;
    Fvector _R, _U, _T, _P;

    mInvCamera.invert(Device.mView);
    _R.set(mInvCamera._11, mInvCamera._12, mInvCamera._13);
    _U.set(mInvCamera._21, mInvCamera._22, mInvCamera._23);
    _T.set(mInvCamera._31, mInvCamera._32, mInvCamera._33);
    _P.set(mInvCamera._41, mInvCamera._42, mInvCamera._43);
    Log("CCameraManager::Dump::vPosition  = ", _P);
    Log("CCameraManager::Dump::vDirection = ", _T);
    Log("CCameraManager::Dump::vNormal    = ", _U);
    Log("CCameraManager::Dump::vRight     = ", _R);
}


// demonized: removecameffector by pointer
void CCameraManager::RemoveCamEffector(CEffectorCam* ef)
{
    for (auto it = m_EffectorsCam.begin(); it != m_EffectorsCam.end(); ++it)
    {
        CEffectorCam* cam = (*it);
        if (cam == ef)
        {
            OnEffectorReleased(cam);
            m_EffectorsCam.erase(it);
            return;
        }
    }
}