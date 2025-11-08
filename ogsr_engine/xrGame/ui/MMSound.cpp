#include "StdAfx.h"

#include "MMSound.h"
#include "xrUIXmlParser.h"

CMMSound::CMMSound() {}

CMMSound::~CMMSound() { all_Stop(); }

void CMMSound::Init(CUIXml& xml_doc, LPCSTR path)
{
    string256 _path;

    int nodes_num = xml_doc.GetNodesNum(path, 0, "menu_music");

    XML_NODE* tab_node = xml_doc.NavigateToNode(path, 0);
    xml_doc.SetLocalRoot(tab_node);
    for (int i = 0; i < nodes_num; ++i)
        m_play_list.push_back(xml_doc.Read("menu_music", i, ""));
    xml_doc.SetLocalRoot(xml_doc.GetRoot());

    string_path buff{};

    strconcat(sizeof(_path), _path, path, ":whell_sound");
    if (FS.exist(buff, fsgame::game_sounds, xml_doc.Read(_path, 0, ""), ".ogg"))
        m_whell.create(xml_doc.Read(_path, 0, ""), st_Effect, sg_SourceType);

    strconcat(sizeof(_path), _path, path, ":whell_click");
    if (FS.exist(buff, fsgame::game_sounds, xml_doc.Read(_path, 0, ""), ".ogg"))
        m_whell_click.create(xml_doc.Read(_path, 0, ""), st_Effect, sg_SourceType);
}

void CMMSound::whell_Play()
{
    if (m_whell._handle() && !m_whell._feedback())
        m_whell.play(NULL, sm_Looped | sm_2D);
}

void CMMSound::whell_Stop()
{
    if (m_whell._feedback())
        m_whell.stop();
}

void CMMSound::whell_Click()
{
    if (m_whell_click._handle())
        m_whell_click.play(NULL, sm_2D);
}

void CMMSound::whell_UpdateMoving(float frequency) { m_whell.set_frequency(frequency); }

void CMMSound::music_Play()
{
    if (m_play_list.empty())
        return;

    const int i = Random.randI(m_play_list.size());

    bool stereo{};
    string_path buff{};

    if (FS.exist(buff, fsgame::game_sounds, m_play_list[i].c_str(), ".ogg"))
    {
        m_music_sources[0].create(m_play_list[i].c_str(), st_Music, sg_SourceType);
        m_music_sources[1].destroy();
        stereo = true;
    }
    else
    {
        string_path _path;
        string_path _path2;
        strconcat(sizeof(_path), _path, m_play_list[i].c_str(), "_l.ogg");
        strconcat(sizeof(_path2), _path2, m_play_list[i].c_str(), "_r.ogg");
        VERIFY(FS.exist(fsgame::game_sounds, _path));
        VERIFY(FS.exist(fsgame::game_sounds, _path2));

        m_music_sources[0].create(_path, st_Music, sg_SourceType);
        m_music_sources[1].create(_path2, st_Music, sg_SourceType);
        stereo = false;
    }

    if (!stereo)
    {
        m_music_sources[0].play_at_pos(0, Fvector().set(-0.5f, 0.f, 0.3f), sm_2D);
        m_music_sources[1].play_at_pos(0, Fvector().set(+0.5f, 0.f, 0.3f), sm_2D);
    }
    else
        m_music_sources[0].play_at_pos(0, Fvector().set(0.f, 0.f, 0.3f), sm_2D);
}

void CMMSound::music_Update()
{
    if (!m_music_sources[0]._feedback() || (m_music_sources[1]._handle() && !m_music_sources[1]._feedback()))
        music_Play();
}

void CMMSound::music_Stop()
{
    for (auto& src : m_music_sources)
        src.stop();
}

void CMMSound::all_Stop()
{
    music_Stop();
    m_whell.stop();
    m_whell_click.stop();
}