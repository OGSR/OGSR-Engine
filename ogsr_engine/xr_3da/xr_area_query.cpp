#include "stdafx.h"
#include "xr_area.h"
#include "xr_object.h"
#include "../xrCore/_vector3d_ext.h"

using namespace collide;

bool CObjectSpace::BoxQuery(Fvector const& box_center, Fvector const& box_z_axis, Fvector const& box_y_axis, Fvector const& box_sizes, xr_vector<Fvector>* out_tris)
{
    Fvector z_axis = box_z_axis;
    z_axis.normalize();
    Fvector y_axis = box_y_axis;
    y_axis.normalize();
    Fvector x_axis;
    x_axis.crossproduct(box_y_axis, box_z_axis).normalize();

    Fplane planes[6];
    enum
    {
        left_plane,
        right_plane,
        top_plane,
        bottom_plane,
        front_plane,
        near_plane
    };

    planes[left_plane].build(box_center - (x_axis * (box_sizes.x * 0.5f)), -x_axis);
    planes[right_plane].build(box_center + (x_axis * (box_sizes.x * 0.5f)), x_axis);
    planes[top_plane].build(box_center + (y_axis * (box_sizes.y * 0.5f)), y_axis);
    planes[bottom_plane].build(box_center - (y_axis * (box_sizes.y * 0.5f)), -y_axis);
    planes[front_plane].build(box_center - (z_axis * (box_sizes.z * 0.5f)), -z_axis);
    planes[near_plane].build(box_center + (z_axis * (box_sizes.z * 0.5f)), z_axis);

    CFrustum frustum;
    frustum.CreateFromPlanes(planes, sizeof(planes) / sizeof(planes[0]));

    xrXRC xrc;
    xrc.frustum_query(CDB::OPT_FULL_TEST, &Static, frustum);

    if (out_tris)
    {
        for (CDB::RESULT* result = xrc.r_begin(); result != xrc.r_end(); ++result)
        {
            out_tris->push_back(result->verts[0]);
            out_tris->push_back(result->verts[1]);
            out_tris->push_back(result->verts[2]);
        }
    }

    return !!xrc.r_count();
}
