#ifndef        COMMON_CONSTANTS_H
#define        COMMON_CONSTANTS_H

#define PI  3.14159265f
#define float_PI 1.570796325f
#define DOUBLE_PI  6.2831853f

#define USE_SUNMASK                		// enable sun mask
#ifndef SMAP_size
	#define SMAP_size        2048
#endif
#define parallax		float2(0.02, -0.01)				// shift for default stalker parallax calculating
#define lumcoeff		float3(0.299,0.587,0.114)		// luminance vector fo bokeh blur and SMAA
#define xmaterial		float(L_material.w)				// just shortcut for material
#define def_gloss       float(2.f /255.f)				// default gloss value
#define def_aref        float(200.f/255.f)				// default alpha-test value
#define def_virtualh    float(.05f)						// virtual position shift for bump look-a-like
#define def_distort     float(0.05f)					// distortion shift
#define def_hdr         float(6.h)						// hdr divider
#define def_sky_hdr     float(7.h)         				// hdr divider for sky
#define	LUMINANCE_VECTOR	float3(0.3f, 0.48f, 0.22f)  // luminance vector fo luminance estimating

#endif