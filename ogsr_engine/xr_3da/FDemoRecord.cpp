#include "stdafx.h"

#include "igame_level.h"
#include "x_ray.h"
#include "gamefont.h"
#include "fDemoRecord.h"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "xr_object.h"
#include "CustomHUD.h"

static Flags32 s_hud_flag{}, s_dev_flags{};

static Fbox curr_lm_fbox{};

// +X, -X, +Y, -Y, +Z, -Z
constexpr Fvector cmNorm[]{{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}};
constexpr Fvector cmDir[]{{1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, -1.f}};

static void setup_lm_screenshot_matrices()
{
    psHUD_Flags.assign(0);

    // build camera matrix
    Fbox bb = curr_lm_fbox;
    bb.getcenter(Device.vCameraPosition);

    Device.vCameraDirection.set(0.f, -1.f, 0.f);
    Device.vCameraTop.set(0.f, 0.f, 1.f);
    Device.vCameraRight.set(1.f, 0.f, 0.f);
    Device.mView.build_camera_dir(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop);

    bb.xform(Device.mView);
    // build project matrix
    Device.mProject.build_projection_ortho(bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.min.z, bb.max.z);
}

static Fbox get_level_screenshot_bound()
{
    Fbox res = g_pGameLevel->ObjectSpace.GetBoundingVolume();
    if (pGameIni->line_exist(g_pGameLevel->name(), "bound_rect"))
    {
        Fvector4 res2d = pGameIni->r_fvector4(g_pGameLevel->name(), "bound_rect");
        res.min.x = res2d.x;
        res.min.z = res2d.y;

        res.max.x = res2d.z;
        res.max.z = res2d.w;
    }

    return res;
}

static void GetLM_BBox(Fbox& bb, int Step)
{
    float half_x = bb.min.x + (bb.max.x - bb.min.x) / 2;
    float half_z = bb.min.z + (bb.max.z - bb.min.z) / 2;
    switch (Step)
    {
    case 0: {
        bb.max.x = half_x;
        bb.min.z = half_z;
    }
    break;
    case 1: {
        bb.min.x = half_x;
        bb.min.z = half_z;
    }
    break;
    case 2: {
        bb.max.x = half_x;
        bb.max.z = half_z;
    }
    break;
    case 3: {
        bb.min.x = half_x;
        bb.max.z = half_z;
    }
    break;
    default: {
    }
    break;
    }
}

static void update_whith_timescale(Fvector& v, const Fvector& v_delta)
{
    VERIFY(!fis_zero(Device.time_factor()));
    float scale = 1.f / Device.time_factor();
    v.mad(v, v_delta, scale);
}

CDemoRecord::CDemoRecord(const char* name, float life_time) : CEffectorCam(cefDemo, life_time)
{
    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);

    pFontSystem = std::make_unique<CGameFont>("hud_font_di", CGameFont::fsDeviceIndependent);

    m_iLMScreenshotFragment = -1;

    m_b_redirect_input_to_level = false;

    if (strlen(name) > 0) // что б можно было demo_record без файла использовать
    {
        _unlink(name);
        file = FS.w_open(name);
    }
    //if (file)
    {
        //g_position.set_position = false;
        IR_Capture(); // capture input
        m_Camera.invert(Device.mView);

        // parse yaw
        Fvector& dir = m_Camera.k;
        Fvector DYaw;
        DYaw.set(dir.x, 0.f, dir.z);
        DYaw.normalize_safe();
        if (DYaw.x < 0)
            m_HPB.x = acosf(DYaw.z);
        else
            m_HPB.x = 2 * PI - acosf(DYaw.z);

        // parse pitch
        dir.normalize_safe();
        m_HPB.y = asinf(dir.y);
        m_HPB.z = 0;

        m_Position.set(m_Camera.c);

        m_vVelocity.set(0, 0, 0);
        m_vAngularVelocity.set(0, 0, 0);
        iCount = 0;

        m_vT.set(0, 0, 0);
        m_vR.set(0, 0, 0);
        m_bMakeCubeMap = false;
        m_bMakeScreenshot = false;
        m_bMakeLevelMap = false;

        m_fSpeed0 = pSettings->r_float("demo_record", "speed0");
        m_fSpeed1 = pSettings->r_float("demo_record", "speed1");
        m_fSpeed2 = pSettings->r_float("demo_record", "speed2");
        m_fSpeed3 = pSettings->r_float("demo_record", "speed3");
        m_fAngSpeed0 = pSettings->r_float("demo_record", "ang_speed0");
        m_fAngSpeed1 = pSettings->r_float("demo_record", "ang_speed1");
        m_fAngSpeed2 = pSettings->r_float("demo_record", "ang_speed2");
        m_fAngSpeed3 = pSettings->r_float("demo_record", "ang_speed3");
    }
    //else
    //{
    //    fLifeTime = -1;
    //}
}

CDemoRecord::~CDemoRecord()
{
    IR_Release(); // release input

    if (file)
    {
        FS.w_close(file);
    }

    Device.seqRender.Remove(this);
}

void CDemoRecord::MakeScreenshotFace()
{
    switch (m_Stage)
    {
    case 0:
        s_hud_flag.assign(psHUD_Flags);
        psHUD_Flags.assign(0);
        break;
    case 1:
        Render->Screenshot();
        psHUD_Flags.assign(s_hud_flag);
        m_bMakeScreenshot = false;
        break;
    }
    m_Stage++;
}

void CDemoRecord::MakeLevelMapProcess()
{
    switch (m_Stage)
    {
    case 0: {
        s_dev_flags = psDeviceFlags;
        s_hud_flag.assign(psHUD_Flags);
        psDeviceFlags.zero();
        psDeviceFlags.set(rsClearBB /*| rsFullscreen | rsDrawStatic*/, true);
    }
    break;

    case DEVICE_RESET_PRECACHE_FRAME_COUNT + 30: {
        setup_lm_screenshot_matrices();

        string_path tmp;
        if (m_iLMScreenshotFragment == -1)
            xr_sprintf(tmp, sizeof(tmp), "map_%s", *g_pGameLevel->name());
        else
            xr_sprintf(tmp, sizeof(tmp), "map_%s#%d", *g_pGameLevel->name(), m_iLMScreenshotFragment);

        if (m_iLMScreenshotFragment != -1)
        {
            ++m_iLMScreenshotFragment;

            if (m_iLMScreenshotFragment != 4)
            {
                curr_lm_fbox = get_level_screenshot_bound();
                GetLM_BBox(curr_lm_fbox, m_iLMScreenshotFragment);
                m_Stage -= 20;
            }
        }

        Render->Screenshot(IRender_interface::SM_FOR_LEVELMAP, tmp);

        if (m_iLMScreenshotFragment == -1 || m_iLMScreenshotFragment == 4)
        {
            psHUD_Flags.assign(s_hud_flag);

            psDeviceFlags = s_dev_flags;

            m_bMakeLevelMap = false;
            m_iLMScreenshotFragment = -1;
        }
    }
    break;
    default: {
        setup_lm_screenshot_matrices();
    }
    break;
    }
    m_Stage++;
}

void CDemoRecord::MakeCubeMapFace(Fvector& D, Fvector& N)
{
    switch (m_Stage)
    {
    case 0:
        N.set(cmNorm[m_Stage]);
        D.set(cmDir[m_Stage]);
        s_hud_flag.assign(psHUD_Flags);
        psHUD_Flags.assign(0);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: {
        N.set(cmNorm[m_Stage]);
        D.set(cmDir[m_Stage]);
        string32 buf;
        Render->Screenshot(IRender_interface::SM_FOR_CUBEMAP, itoa(m_Stage, buf, 10));
        break;
    }
    case 6: {
        string32 buf;
        Render->Screenshot(IRender_interface::SM_FOR_CUBEMAP, itoa(m_Stage, buf, 10));
        N.set(m_Camera.j);
        D.set(m_Camera.k);
        psHUD_Flags.assign(s_hud_flag);
        m_bMakeCubeMap = false;
        break;
    }
    }
    m_Stage++;
}

BOOL CDemoRecord::ProcessCam(SCamEffectorInfo& info)
{
    info.dont_apply = false;
    //if (!file)
    //    return TRUE;

    if (m_bMakeScreenshot)
    {
        MakeScreenshotFace();

        // update camera
        info.n.set(m_Camera.j);
        info.d.set(m_Camera.k);
        info.p.set(m_Camera.c);
    }
    else if (m_bMakeLevelMap)
    {
        MakeLevelMapProcess();

        info.dont_apply = true;
    }
    else if (m_bMakeCubeMap)
    {
        MakeCubeMapFace(info.d, info.n);

        info.p.set(m_Camera.c);
        info.fAspect = 1.f;
    }
    else
    {
        static const bool bShowInfo = READ_IF_EXISTS(pSettings, r_bool, "demo_record", "show_info", true);

        if (psHUD_Flags.test(HUD_DRAW) && bShowInfo && ((Device.dwTimeGlobal / 750) % 3 != 0))
        {
            pFontSystem->SetColor(color_rgba(255, 0, 0, 255));
            pFontSystem->SetAligment(CGameFont::alCenter);
            pFontSystem->OutSetI(0, -.05f);
            pFontSystem->OutNext("%s", "RECORDING");
            pFontSystem->OutNext("Key frames count: %d", iCount);
            pFontSystem->SetAligment(CGameFont::alLeft);
            pFontSystem->OutSetI(-0.2f, +.05f);
            pFontSystem->OutNext("SPACE");
            pFontSystem->OutNext("BACK");
            pFontSystem->OutNext("ESC");
            pFontSystem->OutNext("F11");
            pFontSystem->OutNext("LCONTROL+F11");
            pFontSystem->OutNext("F12");
            pFontSystem->SetAligment(CGameFont::alLeft);
            pFontSystem->OutSetI(0, +.05f);
            pFontSystem->OutNext("= Append Key");
            pFontSystem->OutNext("= Cube Map");
            pFontSystem->OutNext("= Quit");
            pFontSystem->OutNext("= Level Map ScreenShot");
            pFontSystem->OutNext("= Level Map ScreenShot(High Quality)");
            pFontSystem->OutNext("= ScreenShot");
        }

        m_vVelocity.lerp(m_vVelocity, m_vT, 0.3f);
        m_vAngularVelocity.lerp(m_vAngularVelocity, m_vR, 0.3f);

        float speed = m_fSpeed1, ang_speed = m_fAngSpeed1;

        if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
        {
            speed = m_fSpeed0;
            ang_speed = m_fAngSpeed0;
        }
        else if (pInput->iGetAsyncKeyState(DIK_LALT))
        {
            speed = m_fSpeed2;
            ang_speed = m_fAngSpeed2;
        }
        else if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
        {
            speed = m_fSpeed3;
            ang_speed = m_fAngSpeed3;
        }

        m_vT.mul(m_vVelocity, Device.fTimeDelta * speed);
        m_vR.mul(m_vAngularVelocity, Device.fTimeDelta * ang_speed);

        // m_vR.x Up\Down
        // m_vR.y Left\Right
        // m_vR.z Rotate Left\Rotate Right

        m_HPB.x -= m_vR.y;
        m_HPB.y -= m_vR.x;
        m_HPB.z += m_vR.z;

        //if (g_position.set_position)
        //{
        //    m_Position.set(g_position.p);
        //    g_position.set_position = false;
        //}
        //else
        //    g_position.p.set(m_Position);

        // move
        Fvector vmove;

        vmove.set(m_Camera.k);
        vmove.normalize_safe();
        vmove.mul(m_vT.z);
        m_Position.add(vmove);

        vmove.set(m_Camera.i);
        vmove.normalize_safe();
        vmove.mul(m_vT.x);
        m_Position.add(vmove);

        vmove.set(m_Camera.j);
        vmove.normalize_safe();
        vmove.mul(m_vT.y);
        m_Position.add(vmove);

        m_Camera.setHPB(m_HPB.x, m_HPB.y, m_HPB.z);
        m_Camera.translate_over(m_Position);

        // update camera
        info.n.set(m_Camera.j);
        info.d.set(m_Camera.k);
        info.p.set(m_Camera.c);

        fLifeTime -= Device.fTimeDelta;

        m_vT.set(0, 0, 0);
        m_vR.set(0, 0, 0);
    }
    return TRUE;
}

void CDemoRecord::IR_OnKeyboardPress(int dik)
{
    if (dik == DIK_MULTIPLY)
    {
        m_b_redirect_input_to_level = !m_b_redirect_input_to_level;
        return;
    }

    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnKeyboardPress(dik);
        return;
    }

    if (dik == DIK_GRAVE)
        Console->Show();
    if (dik == DIK_SPACE)
        RecordKey();
    if (dik == DIK_BACK)
        MakeCubemap();
    if (dik == DIK_F11)
        MakeLevelMapScreenshot(pInput->iGetAsyncKeyState(DIK_LCONTROL));
    if (dik == DIK_F12)
        MakeScreenshot();
    if (dik == DIK_ESCAPE)
        fLifeTime = -1;

    if (dik == DIK_RETURN)
    {
        if (auto entity = g_pGameLevel->CurrentEntity())
        {
            entity->ForceTransformAndDirection(m_Camera);
            fLifeTime = -1;
        }
    }

    if (dik == DIK_PAUSE)
        Device.Pause(!Device.Paused(), true, true, "demo_record");
}

void CDemoRecord::IR_OnKeyboardHold(int dik)
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnKeyboardHold(dik);
        return;
    }

    Fvector vT_delta{}, vR_delta{};

    switch (dik)
    {
    case DIK_A:
    case DIK_NUMPAD1:
    case DIK_LEFT: vT_delta.x -= 1.0f; break; // Slide Left
    case DIK_D:
    case DIK_NUMPAD3:
    case DIK_RIGHT: vT_delta.x += 1.0f; break; // Slide Right

    case DIK_S: vT_delta.y -= 1.0f; break; // Slide Up
    case DIK_W: vT_delta.y += 1.0f; break; // Slide Down

    // rotate
    case DIK_NUMPAD2: vR_delta.x -= 1.0f; break; // Pitch Down
    case DIK_NUMPAD8: vR_delta.x += 1.0f; break; // Pitch Up

    case DIK_E:
    case DIK_NUMPAD6: vR_delta.y += 1.0f; break; // Turn Left
    case DIK_Q:
    case DIK_NUMPAD4: vR_delta.y -= 1.0f; break; // Turn Right

    case DIK_NUMPAD9: vR_delta.z -= 2.0f; break; // Rotate Right
    case DIK_NUMPAD7: vR_delta.z += 2.0f; break; // Rotate Left
    }

    update_whith_timescale(m_vT, vT_delta);
    update_whith_timescale(m_vR, vR_delta);
}

void CDemoRecord::IR_OnKeyboardRelease(int dik) 
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnKeyboardRelease(dik);
        return;
    }
}

void CDemoRecord::IR_OnMouseMove(int dx, int dy)
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnMouseMove(dx, dy);
        return;
    }

    Fvector vR_delta{};

    float scale = .5f; // psMouseSens;
    if (dx || dy)
    {
        vR_delta.y += float(dx) * scale; // heading
        vR_delta.x += ((psMouseInvert.test(1)) ? -1 : 1) * float(dy) * scale * (3.f / 4.f); // pitch
    }
    update_whith_timescale(m_vR, vR_delta);
}

void CDemoRecord::IR_OnMouseHold(int btn)
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnMouseHold(btn);
        return;
    }

    Fvector vT_delta{};

    switch (btn)
    {
    case 0: vT_delta.z += 1.0f; break; // Move Forward
    case 1: vT_delta.z -= 1.0f; break; // Move Backward
    }
    update_whith_timescale(m_vT, vT_delta);
}

void CDemoRecord::IR_OnMousePress(int btn) 
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnMousePress(btn);
        return;
    }
}

void CDemoRecord::IR_OnMouseRelease(int btn) 
{
    if (m_b_redirect_input_to_level)
    {
        g_pGameLevel->IR_OnMouseRelease(btn);
        return;
    }
}


void CDemoRecord::RecordKey()
{
    Fmatrix g_matView;
    g_matView.invert(m_Camera);
    if (file)
        file->w(&g_matView, sizeof(Fmatrix));
    iCount++;
}

void CDemoRecord::MakeCubemap()
{
    m_bMakeCubeMap = true;
    m_Stage = 0;
}

void CDemoRecord::MakeScreenshot()
{
    m_bMakeScreenshot = true;
    m_Stage = 0;
}

void CDemoRecord::MakeLevelMapScreenshot(bool bHQ)
{
    // KRodin: луа ещё и в консоли мне точно не нужен.
    // Console->Execute("run_string level.set_weather(\"map\",true)");

    if (!bHQ)
        m_iLMScreenshotFragment = -1;
    else
        m_iLMScreenshotFragment = 0;

    curr_lm_fbox = get_level_screenshot_bound();
    GetLM_BBox(curr_lm_fbox, m_iLMScreenshotFragment);

    m_bMakeLevelMap = true;
    m_Stage = 0;
}
