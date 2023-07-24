#pragma once

// refs
class ENGINE_API CGameFont;
class ILoadingScreen;

// definition
class ENGINE_API CApplication : public pureFrame, public IEventReceiver
{
    // levels
    struct sLevelInfo
    {
        char* folder;
    };

    ILoadingScreen* loadingScreen;

    u32 ll_dwReference;

    EVENT eQuit;
    EVENT eStart;
    EVENT eStartLoad;
    EVENT eDisconnect;

    void Level_Append(LPCSTR lname);

    int load_stage;
    int max_load_stage;

public:
    // Levels
    xr_vector<sLevelInfo> Levels;
    u32 Level_Current;
    void Level_Scan();
    int Level_ID(LPCSTR name);
    void Level_Set(u32 ID);

    // Loading
    void LoadBegin();
    void LoadEnd();
    void LoadTitleInt(); // 100 советов по выживанию в Зоне
    void LoadStage();
    void LoadDraw();
    void LoadForceFinish();

    void SetLoadStageTitle(pcstr ls_title);

    virtual void OnEvent(EVENT E, u64 P1, u64 P2);

    // Other
    CApplication();
    ~CApplication();

    virtual void OnFrame();
    void load_draw_internal();
    void SetLoadingScreen(ILoadingScreen* newScreen);
    void DestroyLoadingScreen();
};

extern ENGINE_API CApplication* pApp;
extern ENGINE_API bool IS_OGSR_GA;
