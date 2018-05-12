#include "common.h"
#include "skin.h"

p_flat 	_main	( v_model I )
{
	// Eye-space pos/normal
	p_flat 		O;
	float3	Pe	= mul		(m_WV,  I.P					);
	O.hpos 		= mul		(m_WVP,	I.P					);
	O.N 		= mul		((float3x3)m_WV, (float3)I.N);

	O.tcdh 		= float4	(I.tc.xyyy					);
	O.position	= float4	(Pe, 		L_material.x	);

#ifdef USE_TDETAIL
	O.tcdbump	= O.tcdh*dt_params;					// dt tc
#endif

	return	O;
}

/////////////////////////////////////////////////////////////////////////
#ifdef 	SKIN_NONE
p_flat	main(v_model v) 		{ return _main(v); 		}
#endif

#ifdef 	SKIN_0
p_flat	main(v_model_skinned_0 v) 	{ return _main(skinning_0(v)); }
#endif

#ifdef	SKIN_1
p_flat	main(v_model_skinned_1 v) 	{ return _main(skinning_1(v)); }
#endif

#ifdef	SKIN_2
p_flat	main(v_model_skinned_2 v) 	{ return _main(skinning_2(v)); }
#endif

#ifdef	SKIN_3
p_flat	main(v_model_skinned_3 v) 	{ return _main(skinning_3(v)); }
#endif

#ifdef	SKIN_4
p_flat	main(v_model_skinned_4 v) 	{ return _main(skinning_4(v)); }
#endif

FXVS;
