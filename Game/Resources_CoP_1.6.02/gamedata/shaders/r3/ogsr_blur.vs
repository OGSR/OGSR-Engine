#include "common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
p_screen main(v2p_screen I)
{
    p_screen O;

    // Fill the current viewport from UVs. Pixel positions * screen_res only
    // cover a corner when the viewport is the (smaller) destination RT.
    O.hpos.x = I.tc0.x * 2.0f - 1.0f;
    O.hpos.y = -(I.tc0.y * 2.0f - 1.0f);
    O.hpos.zw = I.HPos.zw;

    O.tc0 = I.tc0;

    return O;
}