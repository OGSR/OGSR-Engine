#pragma once
#include "../xr_3da/GameMtlLib.h"

class CPhysicsShellHolder;

class CPHSoundPlayer
{
    std::unordered_map<SGameMtlPair*, ref_sound> m_sound;
    CPhysicsShellHolder* m_object;
    std::unordered_map<SGameMtlPair*, u32> m_next_snd_time;

public:
    void Init(CPhysicsShellHolder* m_object);

    void Play(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel = true, float* vol = nullptr);
    void PlayNext(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel = true, float* vol = nullptr);

    CPHSoundPlayer() : m_object(nullptr){};
    CPHSoundPlayer(CPhysicsShellHolder* m_object);
    virtual ~CPHSoundPlayer();
};
