// occRasterizer.h: interface for the occRasterizer class.
//////////////////////////////////////////////////////////////////////
#pragma once

const int occ_dim_0 = 64;
const int occ_dim = occ_dim_0 + 4; // 2 pixel border around frame

class occTri
{
public:
    occTri* adjacent[3];
    Fvector raster[3];
    Fplane plane;
    float area;
    u32 flags;
    u32 skip;
    Fvector center;
};

const float occQ_s32 = float(0x40000000); // [-2..2]
typedef s32 occD;

class occRasterizer
{
private:
    occTri* bufFrame[occ_dim][occ_dim];
    float bufDepth[occ_dim][occ_dim];

    occD bufDepth_0[occ_dim_0][occ_dim_0];

public:
    IC int df_2_s32(float d) { return iFloor(d * occQ_s32); }
    IC int df_2_s32up(float d) { return iCeil(d * occQ_s32); }

    void clear();
    void propagade();
    u32 rasterize(occTri* T);
    BOOL test(float x0, float y0, float x1, float y1, float z);

    occTri** get_frame() { return &(bufFrame[0][0]); }
    float* get_depth() { return &(bufDepth[0][0]); }
    occD* get_depth_level() { return &(bufDepth_0[0][0]); }

    void on_dbg_render();

#if DEBUG
    struct pixel_box
    {
        Fvector center;
        Fvector radius;
        float z;
    } dbg_pixel_boxes[occ_dim_0 * occ_dim_0];
    bool dbg_HOM_draw_initialized;

#endif

    occRasterizer();
    ~occRasterizer() = default;
};

extern occRasterizer Raster;
