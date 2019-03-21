#ifndef        COMMON_H
#define        COMMON_H

#include "common_samplers.h"
#include "common_uniforms.h"
#include "common_constants.h"
#include "ogse_functions.h"
#include "ogse_config.h"

uniform float4x4 mVPTexgen;

///////////////////////////////////// SWM /////////////////////////////////////
uniform	float4 m_blender_mode; // x = [0 - default, 1 - night vision, 2 - thermal vision]; y = [0.0f / 1.0f - происходит ли в данный момент рендеринг картинки для прицела]; z = [0.0f / 1.0f - выключен или включён двойной рендер]; w - зарезервировано на будущее.

// Активен-ли двойной рендер
inline bool isSecondVPActive()
{
	return (m_blender_mode.z == 1.f);
}

inline bool IsSVPFrame()
{
	return (m_blender_mode.y == 1.f);
}
///////////////////////////////////////////////////////////////////////////////

float3 	unpack_normal	(float3 v)	{ return 2*v-1;			}
float3 	unpack_bx2	(float3 v)	{ return 2*v-1; 		}
float3 	unpack_bx4	(float3 v)	{ return 4*v-2; 		}

float2 	unpack_tc_base	(float2 tc, float du, float dv)		{
		return (tc.xy + float2	(du,dv))*(32.f/32768.f);
}

float2 	unpack_tc_lmap	(float2 tc)	{ return tc*(1.f/32768.f);	} // [-1  .. +1 ]

float 	calc_cyclic 	(float x)				{
	float 	phase 	= 1/(2*3.141592653589f);
	float 	sqrt2	= 1.4142136f;
	float 	sqrt2m2	= 2.8284271f;
	float 	f 	= sqrt2m2*frac(x)-sqrt2;	// [-sqrt2 .. +sqrt2]
	return 	f*f - 1.f;				// [-1     .. +1]
}
float2 	calc_xz_wave 	(float2 dir2D, float frac)		{
	// Beizer
	float2  ctrl_A	= float2(0.f,		0.f	);
	float2 	ctrl_B	= float2(dir2D.x,	dir2D.y	);
	return  lerp	(ctrl_A, ctrl_B, frac);
}

//////////////////////////////////////////////////////////////////////////////////////////

float          calc_fogging               (float4 w_pos)      { return dot(w_pos,fog_plane);         }
float3         calc_reflection     (float3 pos_w, float3 norm_w)
{
    return reflect(normalize(pos_w-eye_position), norm_w);
}
float3        calc_sun_r1                (float3 norm_w)    { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));                 }
float3        calc_model_hemi_r1         (float3 norm_w)    { return max(0,norm_w.y)*L_hemi_color;                                         }
float3        calc_model_lq_lighting     (float3 norm_w)    { return L_material.x*calc_model_hemi_r1(norm_w) + L_ambient + L_material.y*calc_sun_r1(norm_w);         }

//////////////////////////////////////////////////////////////////////////////////////////
struct         v_static                {
        float4      P                	: POSITION;                        // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz,hemi occlusion)
        float4      T                 	: TANGENT;                // tangent
        float4      B                 	: BINORMAL;                // binormal
        float2      tc                	: TEXCOORD0;        // (u,v)
        float2      lmh                	: TEXCOORD1;        // (lmu,lmv)
        float4      color               : COLOR0;                        // (r,g,b,dir-occlusion)
};

struct         v_tree               	{
        float4      P                	: POSITION;                // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz)
        float3      T                 	: TANGENT;                // tangent
        float3      B                 	: BINORMAL;                // binormal
        float4      tc                	: TEXCOORD0;        // (u,v,frac,???)
};

struct         v_model                 	{
        float4      P                   : POSITION;                // (float,float,float,1)
        float3      N                	: NORMAL;                // (nx,ny,nz)
        float3      T                	: TANGENT;                // (nx,ny,nz)
        float3      B                	: BINORMAL;                // (nx,ny,nz)
        float2      tc                	: TEXCOORD0;        // (u,v)
};

struct        v_detail                    {
        float4      pos                : POSITION;                // (float,float,float,1)
        int4        misc        : TEXCOORD0;        // (u(Q),v(Q),frac,matrix-id)
#ifdef USE_HWI
		float4		m0			: TEXCOORD1;
		float4		m1			: TEXCOORD2;
#endif
};

#if defined(USE_HWSMAP) && !defined(USE_PCSS)
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
};
#else
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
};
#endif

//////////////////////////////////////////////////////////////////////////////////////////
struct         p_bumped        {
        float4      hpos        : POSITION;
        float2      tcdh        : TEXCOORD0;        // Texture coordinates
        float4      position    : TEXCOORD1;        // position + hemi
        float3       M1         : TEXCOORD2;        // nmap 2 eye - 1
        float3       M2         : TEXCOORD3;        // nmap 2 eye - 2
        float3       M3         : TEXCOORD4;        // nmap 2 eye - 3
#ifdef USE_TDETAIL
        float2      tcdbump          : TEXCOORD5;        // d-bump
    #ifdef USE_LM_HEMI
        float2      lmh                    : TEXCOORD6;        // lm-hemi
    #endif
#else
    #ifdef USE_LM_HEMI
        float2      lmh                   : TEXCOORD5;        // lm-hemi
    #endif
#endif
};
//////////////////////////////////////////////////////////////////////////////////////////
struct         p_flat                  {
        float4                hpos        : POSITION;
		float4                tcdh        : TEXCOORD0;        // Texture coordinates
        float4                position    : TEXCOORD1;        // position + hemi
        float3                N           : TEXCOORD2;        // Eye-space normal        (for lighting)
  #ifdef USE_TDETAIL
        float2                tcdbump                : TEXCOORD3;        // d-bump
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD4;        // lm-hemi
    #endif
  #else
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD3;        // lm-hemi
    #endif
  #endif
};
//////////////////////////////////////////////////////////////////////////////////////////
struct                  f_deffer        		{
        float4           position        		: COLOR0;        // px,py,pz, m-id
        float4           Ne                		: COLOR1;        // nx,ny,nz, hemi
        float4       	C                		: COLOR2;        // r, g, b,  gloss
};
//////////////////////////////////////////////////////////////////////////////////////////
struct  				p_screen                {
        float4          hpos               		: POSITION;
        float2          tc0                		: TEXCOORD0;        // Texture coordinates         (for sampling maps)
};
//////////////////////////////////////////////////////////////////////////////////////////

struct combine {
	float4 tc0:TEXCOORD0;
	float4 tc1:TEXCOORD1;
	float4 tc2:TEXCOORD2;
	float4 tc3:TEXCOORD3;
	float4 tc4:TEXCOORD4;
	float4 tc5:TEXCOORD5;
	float4 tc6:TEXCOORD6;
};

//////////////////////////////////////////////////////////////////////////////////////////

void        tonemap              (out float4 low, out float4 high, float3 rgb, float scale)
{
        rgb     =      	rgb*scale       ;
#ifdef	USE_BRANCHING		// ps_3_0
        low		=       rgb.xyzz		;
        high	=		low/def_hdr		;        // 8x dynamic range
#else
        low		=       float4           (rgb,           0 )	;
        high	=       float4       	(rgb/def_hdr,   0 )	;		// 8x dynamic range
#endif
}
void        tonemap_sky              (out float4 low, out float4 high, float3 rgb, float scale)
{
        rgb     =      	rgb*scale       ;
#ifdef	USE_BRANCHING		// ps_3_0
        low		=       rgb.xyzz		;
        high	=		low/def_sky_hdr		;        // 8x dynamic range
#else
        low		=       float4           (rgb,           0 )	;
        high	=       float4       	(rgb/def_sky_hdr,   0 )	;		// 8x dynamic range
#endif
}
float4		combine_bloom        (float3  low, float4 high)	{
        return        float4(low + high.xyz*high.a, 1.f);
}

float3	v_hemi        	(float3 n)                        	{        return L_hemi_color.xyz*(.5f + .5f*n.y);                   }
float3	v_sun           (float3 n)                        	{        return L_sun_color*dot(n,-L_sun_dir_w);                }
float3   p_hemi          (float2 tc)                         {
        float3        	t_lmh         = tex2D             	(s_hemi, tc).xyz;
        return  dot     (t_lmh,1.h/3.h);
}
//	contrast function
float Contrast(float Input, float ContrastPower)
{
     //piecewise contrast function
     bool IsAbovefloat = Input > 0.5 ;
     float ToRaise = saturate(2*(IsAbovefloat ? 1-Input : Input));
     float Output = 0.5*pow(ToRaise, ContrastPower);
     Output = IsAbovefloat ? 1-Output : Output;
     return Output;
}

#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#include "ogse_gbuffer.h"
#endif