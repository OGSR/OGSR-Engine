#include "common.h"

p_flat main 	(v_static I)
{

	float4	w_pos	= I.P				;
	float2 	tc		= unpack_tc_base	(I.tc,I.T.w,I.B.w);// + ogse_c_jitter.xy;	// copy tc
	float 	hemi 	= I.Nh.w			;

	// Eye-space pos/normal
	p_flat 	O;
	float3	Pe	= mul		(m_V,  w_pos		);
	O.hpos 		= mul		(m_VP,	w_pos		);
	O.tcdh 		= float4	(tc.xyyy			);
	O.position	= float4	(Pe, hemi			);
	O.N 		= mul		((float3x3)m_V,	unpack_bx2(I.Nh));
	
#ifdef 	USE_TDETAIL
	O.tcdbump		= O.tcdh * dt_params;		// dt tc
#endif

#ifdef	USE_LM_HEMI
	O.lmh 			= unpack_tc_lmap	(I.lmh);
#endif
	return	O;
}

FXVS;
