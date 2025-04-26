#include "stdafx.h"
#include "envelope.h"

CEnvelope::~CEnvelope() { Clear(); }

CEnvelope::CEnvelope(CEnvelope* source)
{
    *this = *source;
    for (u32 i = 0; i < source->keys.size(); i++)
        keys[i] = xr_new<st_Key>(*source->keys[i]);
}

void CEnvelope::Clear()
{
    for (auto& key : keys)
        xr_delete(key);
}

float CEnvelope::GetLength(float* mn, float* mx)
{
    if (!keys.empty())
    {
        if (mn)
            *mn = keys.front()->time;
        if (mx)
            *mx = keys.back()->time;
        return keys.back()->time - keys.front()->time;
    }
    if (mn)
        *mn = 0.f;
    if (mx)
        *mx = 0.f;
    return 0.f;
}

extern float evalEnvelope(CEnvelope* env, float time);
float CEnvelope::Evaluate(float time) { return evalEnvelope(this, time); }

void CEnvelope::Save(IWriter& F)
{
    F.w_u8((u8)behavior[0]);
    F.w_u8((u8)behavior[1]);
    F.w_u16((u16)keys.size());
    for (auto& key : keys)
        key->Save(F);
}

void CEnvelope::Load_1(IReader& F)
{
    Clear();
    F.r(behavior, sizeof(int) * 2);
    int y = F.r_u32();
    keys.resize(y);
    for (auto& key : keys)
    {
        key = xr_new<st_Key>();
        key->Load_1(F);
    }
}

void CEnvelope::Load_2(IReader& F)
{
    Clear();
    behavior[0] = F.r_u8();
    behavior[1] = F.r_u8();
    keys.resize(F.r_u16());
    for (auto& key : keys)
    {
        key = xr_new<st_Key>();
        key->Load_2(F);
    }
}
