#pragma once

#include "light.h"
#include "light_package.h"

class CLight_DB
{
public:
    ref_light sun_adapted{};
    light_Package package{};

    xr_vector<ref_light> v_hemi;

public:
    void add_light(light* L);

    void Load(IReader* fs);
    void LoadHemi();

    void Unload();

    light* Create();
    void UpdateSun() const;

    CLight_DB();
    ~CLight_DB();
};
