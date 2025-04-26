#include "stdafx.h"



void CConstant::Calculate()
{
    if (dwFrame == Device.dwFrame)
        return;
    dwFrame = Device.dwFrame;

    if (modeProgrammable == dwMode)
        return;

    const float t = Device.fTimeGlobal;
    set_float(_R.Calculate(t), _G.Calculate(t), _B.Calculate(t), _A.Calculate(t));
}

void CConstant::Load(IReader* fs)
{
    dwMode = modeWaveForm;

    fs->r(&_R, sizeof(WaveForm));
    fs->r(&_G, sizeof(WaveForm));
    fs->r(&_B, sizeof(WaveForm));
    fs->r(&_A, sizeof(WaveForm));
}

void CConstant::Save(IWriter* fs) const
{
    fs->w(&_R, sizeof(WaveForm));
    fs->w(&_G, sizeof(WaveForm));
    fs->w(&_B, sizeof(WaveForm));
    fs->w(&_A, sizeof(WaveForm));
}
