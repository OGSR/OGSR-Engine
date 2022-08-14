#pragma once

#include "iinputreceiver.h"
#include "effector.h"

class ENGINE_API CDemoRecord : public CEffectorCam, public IInputReceiver, public pureRender
{
    //struct force_position
    //{
    //    bool set_position;
    //    Fvector p;
    //} g_position{};

    int iCount;
    IWriter* file{};



    Fmatrix m_Camera;
    u32 m_Stage;

    Fvector m_vT;
    Fvector m_vR;
    Fvector m_vVelocity;
    Fvector m_vAngularVelocity;

    bool m_bMakeCubeMap;
    bool m_bMakeScreenshot;
    int m_iLMScreenshotFragment;
    bool m_bMakeLevelMap;

    float m_fSpeed0;
    float m_fSpeed1;
    float m_fSpeed2;
    float m_fSpeed3;
    float m_fAngSpeed0;
    float m_fAngSpeed1;
    float m_fAngSpeed2;
    float m_fAngSpeed3;

    std::unique_ptr<CGameFont> pFontSystem;

    void MakeCubeMapFace(Fvector& D, Fvector& N);
    void MakeLevelMapProcess();
    void MakeScreenshotFace();
    void RecordKey();
    void MakeCubemap();
    void MakeScreenshot();
    void MakeLevelMapScreenshot(bool bHQ);

public:
    CDemoRecord(const char* name, float life_time = 60 * 60 * 1000);
    virtual ~CDemoRecord();

    void IR_OnKeyboardPress(int dik) override;
    void IR_OnKeyboardHold(int dik) override;
    void IR_OnKeyboardRelease(int dik) override;

    void IR_OnMousePress(int btn) override;
    void IR_OnMouseRelease(int btn) override;

    void IR_OnMouseMove(int dx, int dy) override;
    void IR_OnMouseHold(int btn) override;

    BOOL ProcessCam(SCamEffectorInfo& info) override;

    void OnRender() override
    {
        if (pFontSystem)
            pFontSystem->OnRender();
    }

    Fvector m_HPB;
    Fvector m_Position;
    bool m_b_redirect_input_to_level;

    // static void SetGlobalPosition(const Fvector& p) { g_position.p.set(p), g_position.set_position = true; }
    // static void GetGlobalPosition(Fvector& p) { p.set(g_position.p); }
};
