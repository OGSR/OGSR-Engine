#ifndef        COMMON_UNIFORMS_H
#define        COMMON_UNIFORMS_H

// common matrices
uniform float3x4	m_W;			// model-to-world matrix
uniform float3x4	m_V;			// world-to-view matrix
uniform float4x4 	m_P;			// view-to-project matrix
uniform float3x4	m_WV;			// model-to-view matrix
uniform float4x4 	m_VP;			// world-to-project matrix
uniform float4x4 	m_WVP;			// model-to-project matrix
// shadow map matrices for transparent geometry
uniform float4x4 	m_shadow_near_ogse;	// transform for near shadow map
uniform float4x4 	m_shadow_far_ogse;	// transform for far shadow map
// motion blur matrices
uniform float4x4	m_current;		// current view-to-project matrix
uniform float4x4	m_previous;		// view-to-project matrix from previous frame
// constants
uniform float4		timers;			// timers: x = fTimeGlobal, y = fTimeGlobal*10, z = fTimeGlobal*0.1, w = _sin(fTimeGlobal)
uniform float4		fog_plane;		// just some planes for r1-like fog calculation
uniform float4		fog_params;		// params for r2 fog calculating: x = -near/(far-near), y,z,w = 1/(far-near)
uniform float4		fog_color;		// fog_color from weather config
uniform float3		L_sun_color;	// sun_color from weather config
uniform float4      env_color;      // sky_color from weather config
uniform float3		L_sun_dir_w;	// sun_dir from weather config in world space
uniform float3		L_sun_dir_e;	// sun_dir from weather config in view space
uniform float4		L_hemi_color;	// hemi_color from weather config
uniform float4		L_ambient;		// ambient  from weather config (w = skynbox lerp factor)
uniform float3 		eye_position;	// camera position
uniform float3		eye_direction;	// camera direction
uniform float3		eye_normal;		// camera top
uniform	float4 		dt_params;		// detail scale from textures.ltx or thm

uniform float2		m_blur;			// blur params: x = ps_r2_mblur/24, y = -ps_r2_mblur/24
uniform float4      L_material;     // some params for light calculating: x = hemi, y = sun, z = 0, w = material from textures.ltx
uniform float4      Ldynamic_color;		// dynamic light color
uniform float4      Ldynamic_pos;		// dynamic light position, w = 1/(light->range*light->range)
uniform float4      Ldynamic_dir;       // dynamic light direction

#endif