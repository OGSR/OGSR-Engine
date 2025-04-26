#pragma once

class R_lod
{
public:
    R_constant* c_lod;

public:
    explicit R_lod(CBackend& cmd_list_in);

    void unmap() { c_lod = nullptr; }
    void set_lod(R_constant* C) { c_lod = C; }

    void set_lod(float lod) const;

    CBackend& cmd_list;
};
