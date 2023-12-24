#ifndef common_defines_h_included
#define common_defines_h_included

#if defined(SM_5) || defined(SM_4_1)
#define TEXTURE2DMS(a, b) Texture2DMS<a>
#else
#define TEXTURE2DMS(a, b) Texture2DMS<a, b>
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//
#define def_gloss float(2.f / 255.f)
#define def_aref float(200.f / 255.f)
#define def_dbumph float(0.333f)
#define def_virtualh float(0.05f) // 5cm
#define def_distort float(0.05f) // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr float(9.h) // hight luminance range float(3.h)
#define def_hdr_clip float(0.75h) //

#define LUMINANCE_VECTOR float3(0.2125, 0.7154, 0.0721)

//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size 1024
#endif
//////////////////////////////////////////////////////////////////////////////////////////

//#define USE_GRASS_WAVE								// enable grass wave
#define GRASS_WAVE_FREQ float(0.7) // frequency of waves
#define GRASS_WAVE_POWER float(3.0) // brightness of waves

#endif //	common_defines_h_included