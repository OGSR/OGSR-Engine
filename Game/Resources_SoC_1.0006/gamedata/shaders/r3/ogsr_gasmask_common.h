#ifndef GASMASK_COMMON_H
#define GASMASK_COMMON_H

/*
    Gasmask overlay shader

    Credits: ZHC (visor reflection)
    /////////////////
    Anomaly Team 2020
    /////////////////
*/

// Main gasmask textures
Texture2D s_mask_nm_1;
Texture2D s_mask_nm_2;
Texture2D s_mask_nm_3;
Texture2D s_mask_nm_4;
Texture2D s_mask_nm_5;
Texture2D s_mask_nm_6;
Texture2D s_mask_nm_7;
Texture2D s_mask_nm_8;
Texture2D s_mask_nm_9;
Texture2D s_mask_nm_10;

// Vignette masks
Texture2D s_mask_v_1;
Texture2D s_mask_v_2;
Texture2D s_mask_v_3;

// Breath noise texture
Texture2D s_breath_noise;

// RT including drops
Texture2D s_image_blurred;

// Constants from engine
uniform float4 mask_control;

uniform float2 breath_size;
uniform float2 breath_idx;

uniform float3 gasmask_inertia;
uniform float3 device_inertia;

uniform float4 m_cam_inertia_smooth;

// Gasmask settings
//#define GM_DIST_INT 0.05 //Refraction intensity
//#define GM_DIFF_INT 0.25 //Diffuse cracks intensity
//#define GM_VIG_INT 0.5 //Vignette intensity

#define GM_DIST_INT 0.02f // Refraction intensity
#define GM_DIFF_INT 0.2f // Diffuse cracks intensity
#define GM_VIG_INT 0.5f // Vignette intensity

// Raindrops settings
#define GM_DROPS_TURBSIZE 15.0 // Turbulence power
#define GM_DROPS_TURBSHIFT float4(0.35, 1.0, 0.0, 1.0) // Turbulence offset
#define GM_DROPS_TURBTIME sin(0.1 / 3.0)
#define GM_DROPS_TURBCOF 0.33 // Turbulence intensity

// Glass reflections settings
#define GM_VIS_NUM 16 // Reflection quality
#define GM_VIS_RADIUS 0.45 // Reflection radius
#define GM_VIS_INTENSITY 0.5 // Reflection intensity

float resize(float input, float factor, float offset) { return (input - 0.5f + offset) / factor + 0.5f - offset; }

float2 rotate(float2 v, float2 o, float a)
{
    float s = sin(a);
    float c = cos(a);
    float2x2 m = float2x2(c, -s, s, c);
    return mul(m, (v - o)) + o;
}

float3 rotateZ(float3 v, float3 o, float a)
{
    float s = sin(a);
    float c = cos(a);
    float2x2 m = float2x2(c, -s, s, c);
    return float3(mul(m, (v.xy - o.xy) + o.xy), v.z);
}

float2 TransformPlane(float2 uv, float3 center, float XRot, float YRot, float ZRot)
{
    // First Rotate around Y axis
    float2 RayDirection = float2(uv.x, 0.0);
    float2 A1 = float2(0.0, -1.0);
    float2 B1 = RayDirection - A1;
    float2 C1 = rotate(float2(-1.0, 0.0), float2(center.x, 0.0), YRot);
    float2 D1 = rotate(float2(1.0, 0.0), float2(center.x, 0.0), YRot) - C1;

    // calculate intersection point
    float u = ((C1.y + 1.0) * D1.x - C1.x * D1.y) / (D1.x * B1.y - D1.y * B1.x);

    // position on the XY plane after Y-axis rotation
    float sx = u * B1.x;
    float sy = u * uv.y;

    // Now Rotate around X axis
    RayDirection = float2(sy, 0.0);
    float2 B2 = RayDirection - A1;
    float2 C2 = rotate(float2(-1.0, 0.0), float2(center.y, 0.0), XRot);
    float2 D2 = rotate(float2(1.0, 0.0), float2(center.y, 0.0), XRot) - C2;

    // calculate intersection point
    float v = ((C2.y + 1.0) * D2.x - C2.x * D2.y) / (D2.x * B2.y - D2.y * B2.x);

    // the position after x and y rotations
    float3 pos = float3(v * sx, v * B2.x, 0.0);

    // Now rotate the position around Z axis
    float3 finalPos = rotateZ(pos, center, ZRot);

    // final position on the 3D plane after Z-axis rotation
    return finalPos.xy;
}

float2 applyInertia(float2 texcoord)
{
    float2 uv = (-1.0 + 2.0 * texcoord);
    // uv.x = resize(uv.x, 1.1, 0);
    // uv.y = resize(uv.y, 1.1, 0);
    float3 pivotPoint = float3(0.0, 0.0, 0.0);
    float R_X = m_cam_inertia_smooth.y * gasmask_inertia.y;
    float R_Y = m_cam_inertia_smooth.x * gasmask_inertia.x;
    float R_Z = m_cam_inertia_smooth.z * gasmask_inertia.z; // Adjust the value for Z-axis rotation
    float3 MyCoords = float3(TransformPlane(uv, pivotPoint, R_X, R_Y, R_Z), 0.0);
    float2 MyTexCoord = (MyCoords.xy + float2(1.0, 1.0)) / 2.0;
    texcoord = MyTexCoord;
    return texcoord;
}

float2 applyInertiaDev(float2 texcoord)
{
    float2 uv = (-1.0 + 2.0 * texcoord);
    // uv.x = resize(uv.x, 1.1, 0);
    // uv.y = resize(uv.y, 1.1, 0);
    float3 pivotPoint = float3(0.0, 0.0, 0.0);
    float R_X = m_cam_inertia_smooth.y * device_inertia.y;
    float R_Y = m_cam_inertia_smooth.x * device_inertia.x;
    float R_Z = m_cam_inertia_smooth.z * device_inertia.z; // Adjust the value for Z-axis rotation
    float3 MyCoords = float3(TransformPlane(uv, pivotPoint, R_X, R_Y, R_Z), 0.0);
    float2 MyTexCoord = (MyCoords.xy + float2(1.0, 1.0)) / 2.0;
    texcoord = MyTexCoord;
    return texcoord;
}

#endif