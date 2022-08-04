#include "stdafx.h"
#include "game_base.h"
#include "ai_space.h"
#include "script_engine.h"
#include "level.h"
#include "xrMessages.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"

u64 g_qwStartGameTime = 12 * 60 * 60 * 1000;
float g_fTimeFactor;
u64 g_qwEStartGameTime = 12 * 60 * 60 * 1000;

xr_token game_types[];

game_PlayerState::game_PlayerState()
{
    skin = 0;
    m_online_time = 0;
    team = 0;
    money_for_round = 0;
    flags__ = 0;
    m_bCurrentVoteAgreed = 2;
    RespawnTime = 0;
    m_bPayForSpawn = false;

    clear();
}

void game_PlayerState::clear()
{
    name[0] = 0;
    m_iRivalKills = 0;
    m_iSelfKills = 0;
    m_iTeamKills = 0;
    m_iKillsInRowCurr = 0;
    m_iKillsInRowMax = 0;
    m_iDeaths = 0;
    lasthitter = 0;
    lasthitweapon = 0;
    experience_D = 0;
    experience_Real = 0;
    rank = 0;
    af_count = 0;
    experience_New = 0;
    pItemList.clear();
    pSpawnPointsList.clear();
    m_s16LastSRoint = -1;
    LastBuyAcount = 0;
    m_bClearRun = false;
    DeathTime = 0;
    mOldIDs.clear();
    money_added = 0;
    m_aBonusMoney.clear();
}

game_PlayerState::~game_PlayerState()
{
    pItemList.clear();
    pSpawnPointsList.clear();
};

bool game_PlayerState::testFlag(u16 f) const { return !!(flags__ & f); }

void game_PlayerState::setFlag(u16 f) { flags__ |= f; }

void game_PlayerState::resetFlag(u16 f) { flags__ &= ~(f); }

void game_PlayerState::net_Export(NET_Packet& P,
                                  BOOL Full) // KRodin: НЕ ИЗМЕНЯТЬ! Иначе будет несовместимо по сейвам! Единственный нужный нет_экспорт с нетпакетами на данный момент
{
    P.w_u8(Full ? 1 : 0);
    if (Full)
        P.w_stringZ(name);

    P.w_u8(team);
    P.w_s16(m_iRivalKills);
    P.w_s16(m_iSelfKills);
    P.w_s16(m_iTeamKills);
    P.w_s16(m_iDeaths);
    P.w_s32(money_for_round);
    P.w_float_q8(experience_D, -1.0f, 2.0f);
    P.w_u8(rank);
    P.w_u8(af_count);
    P.w_u16(flags__);
    P.w_u16(ping);

    P.w_u16(GameID);
    P.w_s8(skin);
    P.w_u8(m_bCurrentVoteAgreed);

    P.w_u32(Device.dwTimeGlobal - DeathTime);
};

void game_PlayerState::net_Import(NET_Packet& P) // KRodin: НЕ ИЗМЕНЯТЬ! Иначе будет несовместимо по сейвам! Это походу единственный нет_импорт, который нужен.
{
    BOOL bFullUpdate = !!P.r_u8();

    if (bFullUpdate)
        P.r_stringZ(name);

    P.r_u8(team);

    P.r_s16(m_iRivalKills);
    P.r_s16(m_iSelfKills);
    P.r_s16(m_iTeamKills);
    P.r_s16(m_iDeaths);

    P.r_s32(money_for_round);
    P.r_float_q8(experience_D, -1.0f, 2.0f);
    P.r_u8(rank);
    P.r_u8(af_count);
    P.r_u16(flags__);
    P.r_u16(ping);

    P.r_u16(GameID);
    P.r_s8(skin);
    P.r_u8(m_bCurrentVoteAgreed);

    DeathTime = P.r_u32();
};

void game_PlayerState::SetGameID(u16 NewID)
{
    if (mOldIDs.size() >= 10)
    {
        mOldIDs.pop_front();
    };
    mOldIDs.push_back(GameID);
    GameID = NewID;
}
bool game_PlayerState::HasOldID(u16 ID)
{
    OLD_GAME_ID_it ID_i = std::find(mOldIDs.begin(), mOldIDs.end(), ID);
    if (ID_i != mOldIDs.end() && *(ID_i) == ID)
        return true;
    return false;
}

game_TeamState::game_TeamState()
{
    score = 0;
    num_targets = 0;
}

game_GameState::game_GameState()
{
    m_type = -1;
    m_phase = GAME_PHASE_NONE;
    m_round = -1;
    m_round_start_time_str[0] = 0;

    VERIFY(g_pGameLevel);
    m_qwStartProcessorTime = Level().timeServer_Async();
    m_qwStartGameTime = g_qwStartGameTime;
    m_fTimeFactor = g_fTimeFactor;
    m_qwEStartProcessorTime = m_qwStartProcessorTime;
    m_qwEStartGameTime = g_qwEStartGameTime;
    m_fETimeFactor = m_fTimeFactor;
}

CLASS_ID game_GameState::getCLASS_ID(LPCSTR, bool isServer) { return isServer ? TEXT2CLSID("SV_SINGL") : TEXT2CLSID("CL_SINGL"); }

void game_GameState::switch_Phase(u32 new_phase)
{
    OnSwitchPhase(m_phase, new_phase);

    m_phase = u16(new_phase);
    m_start_time = Level().timeServer();
}

ALife::_TIME_ID game_GameState::GetGameTime()
{
    if (ai().get_alife() && ai().alife().initialized())
        return (ai().alife().time_manager().game_time());
    return (m_qwStartGameTime + ALife::_TIME_ID(m_fTimeFactor * float(Level().timeServer_Async() - m_qwStartProcessorTime)));
}

float game_GameState::GetGameTimeFactor()
{
    if (ai().get_alife() && ai().alife().initialized())
        return (ai().alife().time_manager().time_factor());
    return (m_fTimeFactor);
}

void game_GameState::SetGameTimeFactor(const float fTimeFactor)
{
    m_qwStartGameTime = GetGameTime();
    m_qwStartProcessorTime = Level().timeServer_Async();
    m_fTimeFactor = fTimeFactor;
}

void game_GameState::SetGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor)
{
    m_qwStartGameTime = GameTime;
    m_qwStartProcessorTime = Level().timeServer_Async();
    m_fTimeFactor = fTimeFactor;
}

ALife::_TIME_ID game_GameState::GetEnvironmentGameTime()
{
    return GetGameTime(); //(m_qwEStartGameTime + ALife::_TIME_ID(m_fETimeFactor*float(Level().timeServer_Async() - m_qwEStartProcessorTime)));
}

float game_GameState::GetEnvironmentGameTimeFactor()
{
    return GetGameTimeFactor(); //(m_fETimeFactor);
}

void game_GameState::SetEnvironmentGameTimeFactor(const float fTimeFactor)
{
    m_qwEStartGameTime = GetEnvironmentGameTime();
    m_qwEStartProcessorTime = Level().timeServer_Async();
    m_fETimeFactor = fTimeFactor;
}

void game_GameState::SetEnvironmentGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor)
{
    m_qwEStartGameTime = GameTime;
    m_qwEStartProcessorTime = Level().timeServer_Async();
    m_fETimeFactor = fTimeFactor;
}
