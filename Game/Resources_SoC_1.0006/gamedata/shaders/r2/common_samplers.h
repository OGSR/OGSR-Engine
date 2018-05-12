#ifndef        COMMON_SAMPLERS_H
#define        COMMON_SAMPLERS_H

uniform sampler2D s_jitter_0;	// noise texture, 64x64
uniform sampler2D s_jitter_1;	// noise texture, 64x64
uniform sampler2D s_jitter_5;	// HD noise texture, screen resolution
uniform sampler2D s_sun_shafts; // current sunshafts texture
uniform sampler2D s_blur;		//simple blur

//////////////////////////////////////////////////////////////////////////////////////////
// Geometry phase / deferring               	//
uniform sampler2D       s_base;             	// base texture
uniform sampler2D       s_bump;             	// bump texture
uniform sampler2D       s_bumpX;                // bump# texture
uniform sampler2D       s_detail;               // detail texture
uniform sampler2D       s_hemi;             	// lmap_#2 texture

// Terrain samplers
uniform sampler2D       s_mask;                	// terrain mask

uniform sampler2D       s_dt_r;                	//
uniform sampler2D       s_dt_g;                	//
uniform sampler2D       s_dt_b;                	//
uniform sampler2D       s_dt_a;                	//

uniform sampler2D       s_dn_r;                	//
uniform sampler2D       s_dn_g;                	//
uniform sampler2D       s_dn_b;                	//
uniform sampler2D       s_dn_a;                	//

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting/shadowing phase                     //
uniform sampler2D       s_position;             // position G-buffer texture
uniform sampler2D       s_normal;               // normal G-buffer texture
uniform sampler2D       s_lmap;             	// 2D/cube projector lightmap
uniform sampler3D       s_material;             // lighting coeffs lookup texture
//////////////////////////////////////////////////////////////////////////////////////////
// Combine phase                                //
uniform sampler2D       s_diffuse;              // albedo G-buffer texture
uniform sampler2D       s_accumulator;      	// lighting texture
uniform sampler2D       s_bloom;                // bloom texture
uniform sampler2D       s_image;                // current image
uniform sampler2D       s_tonemap;              // tonemap texture
//////////////////////////////////////////////////////////////////////////////////////////

#endif