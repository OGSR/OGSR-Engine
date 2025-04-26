#pragma once

struct SOneDDOParams
{
    void Mul(float v);
    float velocity;
    float acceleration;
};

struct SAllDDOParams
{
    void Reset();
    void Load(CInifile* ini);
    SOneDDOParams translational;
    SOneDDOParams rotational;
    u16 L2frames;
};

struct SAllDDWParams
{
    SAllDDOParams objects_params;
    float reanable_factor;
};

extern SAllDDWParams worldDisablingParams;
