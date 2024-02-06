#pragma once

struct Flight
{
public:
    u32 type; /* Type of light source */
    Fcolor diffuse; /* Diffuse color of light */
    Fcolor specular; /* Specular color of light */
    Fcolor ambient; /* Ambient color of light */
    Fvector position; /* Position in world space */
    Fvector direction; /* Direction in world space */
    float range; /* Cutoff range */
    float falloff; /* Falloff */
    float attenuation0; /* Constant attenuation */
    float attenuation1; /* Linear attenuation */
    float attenuation2; /* Quadratic attenuation */
    float theta; /* Inner angle of spotlight cone */
    float phi; /* Outer angle of spotlight cone */

    IC void set(u32 ltType, float x, float y, float z)
    {
        ZeroMemory(this, sizeof(Flight));
        type = ltType;
        diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
        specular.set(diffuse);
        position.set(x, y, z);
        direction.set(x, y, z);
        direction.normalize_safe();
        range = _sqrt(flt_max);
    }
    IC void mul(float brightness)
    {
        diffuse.mul_rgb(brightness);
        ambient.mul_rgb(brightness);
        specular.mul_rgb(brightness);
    }
};
