#include "stdafx.h"

#include "PHSoundPlayer.h"
#include "PhysicsShellHolder.h"

CPHSoundPlayer::CPHSoundPlayer(CPhysicsShellHolder* obj) { Init(obj); }

CPHSoundPlayer::~CPHSoundPlayer()
{
    for (auto& it : m_sound)
        it.second.stop();
    m_object = nullptr;
}

void CPHSoundPlayer::Init(CPhysicsShellHolder* obj) { m_object = obj; }

void CPHSoundPlayer::Play(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel, float* vol)
{
    if (auto found = m_sound.find(mtl_pair); found != m_sound.end() && found->second._feedback())
        return;

    if (check_vel)
    {
        Fvector vel;
        m_object->PHGetLinearVell(vel);
        if (vel.square_magnitude() <= 0.01f)
            return;
    }

    auto& snd = m_sound[mtl_pair];
    CLONE_MTL_SOUND(snd, mtl_pair, CollideSounds);
    snd.play_at_pos(smart_cast<CPhysicsShellHolder*>(m_object), *pos);
    if (vol)
        snd._feedback()->set_volume(*vol);

    Fvector2 dist = m_object->CollideSndDist();
    if (dist.x >= 0.f || dist.y >= 0.f)
    {
        if (dist.x < 0.f)
            dist.x = snd.get_params()->min_distance;
        if (dist.y < 0.f)
            dist.y = snd.get_params()->max_distance;
        snd._feedback()->set_range(dist.x, dist.y);
    }
}

void CPHSoundPlayer::PlayNext(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel, float* vol)
{
    if (check_vel)
    {
        Fvector vel;
        m_object->PHGetLinearVell(vel);
        if (vel.square_magnitude() <= 0.01f)
            return;
    }

    if (auto found = m_next_snd_time.find(mtl_pair); found != m_next_snd_time.end() && found->second > Device.dwTimeGlobal)
        return;

    auto& snd = GET_RANDOM(mtl_pair->CollideSounds);
    // Половина от времени звука, поэтому умножаем не на 1000, а на 500.
    m_next_snd_time[mtl_pair] = Device.dwTimeGlobal + iFloor(snd.get_length_sec() * 500.0f);

    Fvector2* range = nullptr;
    Fvector2 dist = m_object->CollideSndDist();
    if (dist.x >= 0.f || dist.y >= 0.f)
    {
        if (dist.x < 0.f)
            dist.x = snd.get_params()->min_distance;
        if (dist.y < 0.f)
            dist.y = snd.get_params()->max_distance;
        range = &dist;
    }

    snd.play_no_feedback(nullptr, 0, 0, pos, vol, nullptr, range);
}
