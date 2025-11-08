#pragma once

class CUIXml;

class CMMSound
{
public:
    CMMSound();
    ~CMMSound();
    void Init(CUIXml& xml_doc, LPCSTR path);
    void whell_Play();
    void whell_Stop();
    void whell_Click();
    void whell_UpdateMoving(float frequency);

    void music_Play();
    void music_Stop();
    void music_Update();

    void all_Stop();

protected:
    ref_sound m_music_sources[2];
    ref_sound m_whell;
    ref_sound m_whell_click;
    xr_vector<xr_string> m_play_list;
};
