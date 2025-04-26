#include "stdafx.h"


#include "r_backend_hemi.h"

R_hemi::R_hemi(CBackend& cmd_list_in) : cmd_list(cmd_list_in) { unmap(); }

void R_hemi::unmap()
{
    c_pos_faces = nullptr;
    c_neg_faces = nullptr;
    c_material = nullptr;
    c_hotness = nullptr;
    c_scale = nullptr;
}

void R_hemi::set_pos_faces(float posx, float posy, float posz) const
{
    if (c_pos_faces)
        cmd_list.set_c(c_pos_faces, posx, posy, posz, 0);
}
void R_hemi::set_neg_faces(float negx, float negy, float negz)
{
    if (c_neg_faces)
        cmd_list.set_c(c_neg_faces, negx, negy, negz, 0);
}

void R_hemi::set_material(float x, float y, float z, float w)
{
    if (c_material)
        cmd_list.set_c(c_material, x, y, z, w);
}

void R_hemi::set_hotness(float x, float y, float z, float w)
{
    if (c_hotness)
        cmd_list.set_c(c_hotness, x, y, z, w);
}

void R_hemi::set_scale(float x, float y, float z, float w)
{
    if (c_scale)
        cmd_list.set_c(c_scale, x, y, z, w);
}