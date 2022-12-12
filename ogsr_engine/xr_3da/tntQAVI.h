#pragma once

#include "vfw.h"

class ENGINE_API CAviPlayerCustom
{
protected:
    CAviPlayerCustom* alpha{};

    AVIINDEXENTRY* m_pMovieIndex{};
    BYTE* m_pMovieData{};
    HIC m_aviIC{};
    BYTE* m_pDecompressedBuf{};

    BITMAPINFOHEADER m_biOutFormat{};
    BITMAPINFOHEADER m_biInFormat{};

    float m_fRate{}; // стандартная скорость, fps
    float m_fCurrentRate{}; // текущая скорость, fps

    DWORD m_dwFrameTotal{};
    DWORD m_dwFrameCurrent{};
    u32 m_dwFirstFrameOffset{};

    DWORD CalcFrame();

    BOOL DecompressFrame(DWORD dwFrameNum);
    VOID PreRoll(DWORD dwFrameNum);

public:
    CAviPlayerCustom();
    ~CAviPlayerCustom();

    DWORD m_dwWidth{}, m_dwHeight{};

    VOID GetSize(DWORD* dwWidth, DWORD* dwHeight);

    BOOL Load(char* fname);
    BOOL GetFrame(BYTE** pDest);

    BOOL NeedUpdate() { return CalcFrame() != m_dwFrameCurrent; }
    INT SetSpeed(INT nPercent);
};
