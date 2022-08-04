#pragma once

// refs
class CTheoraStream;

class ENGINE_API CTheoraSurface
{
    CTheoraStream* m_rgb;
    CTheoraStream* m_alpha;

    u32 tm_start{};
    u32 tm_play;
    u32 tm_total;
    BOOL ready;
    BOOL bShaderYUV2RGB;

public:
    BOOL playing;
    BOOL looped;

protected:
    void Reset();

public:
    CTheoraSurface();
    virtual ~CTheoraSurface();

    BOOL Valid();
    BOOL Load(const char* fname);

    BOOL Update(u32 _time);
    void DecompressFrame(u32* dst, u32 _width, int& count);

    void Play(BOOL _looped, u32 _time);
    void Pause(BOOL _pause) { playing = !_pause; }
    void Stop()
    {
        playing = FALSE;
        Reset();
    }
    BOOL IsPlaying() { return playing; }

    u32 Width(bool bRealSize);
    u32 Height(bool bRealSize);
};
