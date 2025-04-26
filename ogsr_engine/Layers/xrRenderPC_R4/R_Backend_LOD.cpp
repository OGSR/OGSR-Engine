#include "stdafx.h"
#include "r_backend_lod.h"

R_lod::R_lod(CBackend& cmd_list_in) : cmd_list(cmd_list_in) { unmap(); }

void R_lod::set_lod(float lod) const
{
    if (c_lod)
    {
        const float factor = clampr<float>(ceil(lod * lod * lod * lod * lod * 8.0f), 1, 7);
        cmd_list.set_c(c_lod, factor);
    }
}