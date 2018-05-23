#include "common.h"

p_bumped main 	(v_static I)
{

	float4	w_pos	= I.P				;
	float2 	tc		= unpack_tc_base	(I.tc,I.T.w,I.B.w);// + ogse_c_jitter.xy;	// copy tc
	float 	hemi 	= I.Nh.w			;

	// Eye-space pos/normal
	p_bumped 	O;
	float3	Pe	= mul		(m_V,  w_pos		);
	O.hpos 		= mul		(m_VP,	w_pos		);
	O.tcdh 		= float4	(tc.xyyy			);
	O.position	= float4	(Pe, hemi			);

	float3 	N 	= unpack_bx2(I.Nh);	// just scale (assume normal in the -.5f, .5f)
	float3 	T 	= unpack_bx2(I.T);	// 
	float3 	B 	= unpack_bx2(I.B);	// 
	float3x3 xform	= mul	((float3x3)m_V, float3x3(
						T.x,B.x,N.x,
						T.y,B.y,N.y,
						T.z,B.z,N.z
				));

	O.M1 			= xform[0]; 
	O.M2 			= xform[1]; 
	O.M3 			= xform[2]; 
	
#ifdef 	USE_TDETAIL
	O.tcdbump		= O.tcdh * dt_params;		// dt tc
#endif

#ifdef	USE_LM_HEMI
	O.lmh 			= unpack_tc_lmap	(I.lmh);
#endif
	return	O;
}

FXVS;
