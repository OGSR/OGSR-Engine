#include "common.h"

v2p_TL main(v_TL_positiont v)
{
    v2p_TL O;

    // Transform to screen space (in d3d9 it was done automatically)
    O.HPos.x = (v.P.x * screen_res.z * 2 - 1);
    O.HPos.y = -(v.P.y * screen_res.w * 2 - 1);
    O.HPos.zw = v.P.zw;

    O.Tex0 = v.Tex0;

    O.Color = v.Color;

    return O;
}
