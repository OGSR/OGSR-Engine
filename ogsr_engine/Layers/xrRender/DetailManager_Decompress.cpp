#include "stdafx.h"

#include "DetailManager.h"
#include "../xrCDB/cl_intersect.h"


//--------------------------------------------------- Decompression
IC float Interpolate(float* base, u32 x, u32 y, u32 size)
{
    const float f = float(size);
    const float fx = float(x) / f;
    const float ifx = 1.f - fx;
    const float fy = float(y) / f;
    const float ify = 1.f - fy;

    const float c01 = base[0] * ifx + base[1] * fx;
    const float c23 = base[2] * ifx + base[3] * fx;

    const float c02 = base[0] * ify + base[2] * fy;
    const float c13 = base[1] * ify + base[3] * fy;

    const float cx = ify * c01 + fy * c23;
    const float cy = ifx * c02 + fx * c13;
    return (cx + cy) / 2;
}

IC bool InterpolateAndDither(float* alpha255, u32 x, u32 y, u32 sx, u32 sy, u32 size, int dither[16][16])
{
    clamp(x, (u32)0, size - 1);
    clamp(y, (u32)0, size - 1);
    int c = iFloor(Interpolate(alpha255, x, y, size) + .5f);
    clamp(c, 0, 255);

    const u32 row = (y + sy) % 16;
    const u32 col = (x + sx) % 16;
    return c > dither[col][row];
}


#ifdef DEBUG
//#include "../../Include/xrRender/DebugRender.h"
#include "dxDebugRender.h"
static void draw_obb(const Fmatrix& matrix, const u32& color)
{
    Fvector aabb[8];
    matrix.transform_tiny(aabb[0], Fvector().set(-1, -1, -1)); // 0
    matrix.transform_tiny(aabb[1], Fvector().set(-1, +1, -1)); // 1
    matrix.transform_tiny(aabb[2], Fvector().set(+1, +1, -1)); // 2
    matrix.transform_tiny(aabb[3], Fvector().set(+1, -1, -1)); // 3
    matrix.transform_tiny(aabb[4], Fvector().set(-1, -1, +1)); // 4
    matrix.transform_tiny(aabb[5], Fvector().set(-1, +1, +1)); // 5
    matrix.transform_tiny(aabb[6], Fvector().set(+1, +1, +1)); // 6
    matrix.transform_tiny(aabb[7], Fvector().set(+1, -1, +1)); // 7

    u16 aabb_id[12 * 2] = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 1, 5, 2, 6, 3, 7, 0, 4};

    rdebug_render->add_lines(aabb, sizeof(aabb) / sizeof(Fvector), &aabb_id[0], sizeof(aabb_id) / (2 * sizeof(u16)), color);
}

bool det_render_debug = false;
#endif

#include "../../xr_3da/gamemtllib.h"

extern float ps_current_detail_scale;
void CDetailManager::cache_Decompress(Slot* S)
{
    ZoneScoped;

    VERIFY(S);
    Slot& D = *S;
    D.type = stReady;
    if (D.empty)
        return;

    DetailSlot& DS = QueryDB(D.sx, D.sz);

    // Select polygons
    Fvector bC, bD;
    D.vis.box.get_CD(bC, bD);

    xrc.box_query(CDB::OPT_FULL_TEST, g_pGameLevel->ObjectSpace.GetStaticModel(), bC, bD);
    u32 triCount = xrc.r_count();
    CDB::TRI* tris = g_pGameLevel->ObjectSpace.GetStaticTris();
    Fvector* verts = g_pGameLevel->ObjectSpace.GetStaticVerts();

    if (0 == triCount)
        return;

    // Build shading table
    float alpha255[dm_obj_in_slot][4];
    for (int i = 0; i < dm_obj_in_slot; i++)
    {
        alpha255[i][0] = 255.f * float(DS.palette[i].a0) / 15.f;
        alpha255[i][1] = 255.f * float(DS.palette[i].a1) / 15.f;
        alpha255[i][2] = 255.f * float(DS.palette[i].a2) / 15.f;
        alpha255[i][3] = 255.f * float(DS.palette[i].a3) / 15.f;
    }

    // Prepare to selection
    float density = ps_current_detail_density;
    float jitter = density / 1.7f;
    u32 d_size = iCeil(dm_slot_size / density);
    svector<int, dm_obj_in_slot> selected;

    // Prepare to actual-bounds-calculations
    Fbox Bounds;
    Bounds.invalidate();

    // Decompressing itself
    for (u32 z = 0; z <= d_size; z++)
    {
        for (u32 x = 0; x <= d_size; x++)
        {
            // shift
            const u32 shift_x = Random.randI(16);
            const u32 shift_z = Random.randI(16);

            // Iterpolate and dither palette
            selected.clear();

            if ((DS.id0 != DetailSlot::ID_Empty) && InterpolateAndDither(alpha255[0], x, z, shift_x, shift_z, d_size, dither))
                selected.push_back(0);
            if ((DS.id1 != DetailSlot::ID_Empty) && InterpolateAndDither(alpha255[1], x, z, shift_x, shift_z, d_size, dither))
                selected.push_back(1);
            if ((DS.id2 != DetailSlot::ID_Empty) && InterpolateAndDither(alpha255[2], x, z, shift_x, shift_z, d_size, dither))
                selected.push_back(2);
            if ((DS.id3 != DetailSlot::ID_Empty) && InterpolateAndDither(alpha255[3], x, z, shift_x, shift_z, d_size, dither))
                selected.push_back(3);

            // Select
            if (selected.empty())
                continue;

            u32 index;
            if (selected.size() == 1)
                index = selected[0];
            else
                index = selected[Random.randI(selected.size())];

            CDetail& Dobj = objects.at(DS.r_id(index));

            // Position (XZ)
            float rx = (float(x) / float(d_size)) * dm_slot_size + D.vis.box.min.x;
            float rz = (float(z) / float(d_size)) * dm_slot_size + D.vis.box.min.z;

            Fvector Item_P{rx + Random.randFs(jitter), D.vis.box.max.y, rz + Random.randFs(jitter)};

            // Position (Y)
            float y = D.vis.box.min.y - 5;
            constexpr Fvector dir{0.f, -1.f, 0.f};
            Fvector3 terrain_normal{};

            float r_u, r_v, r_range;
            for (u32 tid = 0; tid < triCount; tid++)
            {
                CDB::TRI& T = tris[xrc.r_begin()[tid].id];
                SGameMtl* mtl = GMLib.GetMaterialByIdx(T.material);
                if (mtl->Flags.test(SGameMtl::flPassable))
                    continue;

                Fvector Tv[3] = {verts[T.verts[0]], verts[T.verts[1]], verts[T.verts[2]]};
                if (CDB::TestRayTri(Item_P, dir, Tv, r_u, r_v, r_range, TRUE))
                {
                    if (r_range >= 0)
                    {
                        float y_test = Item_P.y - r_range;
                        if (y_test > y)
                            y = y_test;
                        terrain_normal.mknormal(Tv[0], Tv[1], Tv[2]);
                    }
                }
            }

            // Slope Limit
            const float DotP = terrain_normal.dotproduct(dir);
            if (DotP > -(1.0f - Random.randF(ps_ssfx_terrain_grass_slope * 0.8f, ps_ssfx_terrain_grass_slope)))
                continue;

            if (y < D.vis.box.min.y)
                continue;
            Item_P.y = y;

            auto& Item = D.G[index].items.emplace_back();

            // Angles and scale
            Item.scale = Random.randF(Dobj.m_fMinScale * 0.5f, Dobj.m_fMaxScale * 0.9f);

            Item.scale *= ps_current_detail_scale;
            // X-Form BBox
            Fmatrix mScale, mXform;
            Fbox ItemBB;

            Item.xform.rotateY(Random.randF(0, PI_MUL_2));

            // Terrain Alignment
            if (ps_ssfx_terrain_grass_align)
            {
                // Current matrix
                const Fmatrix CurrMatrix = Item.xform;
                // Align to terrain
                Item.xform.j.set(terrain_normal);
                Fvector::generate_orthonormal_basis(Item.xform.j, Item.xform.i, Item.xform.k);
                // Apply random rotation from old matrix
                Item.xform.mulB_43(CurrMatrix);
            }

            Item.xform.translate_over(Item_P);

            mScale.scale(Item.scale, Item.scale, Item.scale);
            mXform.mul_43(Item.xform, mScale);

            ItemBB.xform(Dobj.bv_bb, mXform);
            Bounds.merge(ItemBB);

#ifdef DEBUG
            if (det_render_debug)
                draw_obb(mXform, color_rgba(255, 0, 0, 255)); // Fmatrix().mul_43( mXform, Fmatrix().scale(5,5,5) )
#endif

            // clamp для hemi перенесен сюда из вершинного шейдера:
            // Some spots are bugged ( Full black ), better if we limit the value till a better solution.
            float c_hemi = std::clamp(DS.r_qclr(DS.c_hemi, 15), 0.05f, 1.0f);

            // init xform and terrain normal
            const Fmatrix& M = Item.xform;

            Item.data = {M._11,
                         M._21,
                         M._31,
                         M._41,
                         M._12,
                         M._22,
                         M._32,
                         M._42,
                         M._13,
                         M._23,
                         M._33,
                         M._43,
                         c_hemi,
                         Item.alpha,
                         static_cast<float>(Dobj.m_Flags.is(DO_NO_WAVING)),
                         0.f,
                         terrain_normal.x,
                         terrain_normal.y,
                         terrain_normal.z,
                         0.f};
        }
    }

    // Update bounds to more tight and real ones
    D.vis.clear();
    D.vis.box.set(Bounds);
    D.vis.box.getsphere(D.vis.sphere.P, D.vis.sphere.R);
}
