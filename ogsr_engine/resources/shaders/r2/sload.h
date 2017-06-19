#ifndef SLOAD_H
#define SLOAD_H

#include "common.h"
#include "_shaders_config.h"
#include "effects\pom.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Bumped surface loader                //
//////////////////////////////////////////////////////////////////////////////////////////
struct surface_bumped
{
	float4	base;
	float3	normal;
	float	gloss;
	float	height;
};
float4 tbase(float2 tc) {return tex2D(s_base, tc);}
uniform sampler2D s_detailBump;
uniform sampler2D s_detailBumpX;
surface_bumped sload_i( p_bumped I)
{
	surface_bumped S;

	update_texcoords(I);

	float4 Nu 			=		tex2D(s_bump, I.tcdh);                        // IN:  normal.gloss
	float4 NuE           =		tex2D(s_bumpX,I.tcdh);                        // IN:         normal_error.height
	S.base              =		tex2D(s_base, I.tcdh);                         // IN:  rgb.a		
	S.normal            = 		Nu.wzy + (NuE.xyz - float3(1.f,1.f,1.f));							 // (Nu.wzyx - .5h) + (E-.5)
	S.gloss             =       Nu.x*Nu.x	;                                        //        S.gloss             =        Nu.x*Nu.x;
	S.height            =       NuE.z       ;
	
#ifdef        USE_TDETAIL
	float4 detail		= tex2D( s_detail, I.tcdbump);
	S.base.rgb			*= detail.rgb * 2;
	#ifdef        USE_TDETAIL_BUMP
		float4 NDetail		= tex2D( s_detailBump, I.tcdbump);
		float4 NDetailX		= tex2D( s_detailBumpX, I.tcdbump);
//		S.gloss				= NDetail.x * 2;
		S.normal			+= NDetail.wzy + NDetailX.xyz - 1.0; //	(Nu.wzyx - .5h) + (E-.5)
//	#else        //	USE_TDBUMP
//		S.gloss				*= detail.w * 2;
	#endif        //	USE_TDBUMP
#endif			
	
        return                S;
}
surface_bumped sload( p_bumped I)
{
	surface_bumped S = sload_i(I);
	S.normal.z *= 0.5;		// Удвоение контрастности бампа. Удалить по возможности.
	return S;
}

#endif