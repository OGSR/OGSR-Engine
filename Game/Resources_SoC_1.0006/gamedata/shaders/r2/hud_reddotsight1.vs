#include "common.h"
#include "skin.h"

// KRodin: TODO: Вынести в common?
float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	return screen;
}


struct vf
{
	float4 hpos : POSITION;
	float2 tc0 : TEXCOORD0; // base
	float4 tc1 : TEXCOORD1; // screen coords
};

vf _main (v_model v)
{
	vf o;
	o.hpos = mul(m_WVP, v.P); // xform, input in world coords
	o.tc0 = v.tc.xy; // copy tc
	o.tc1 = proj_to_screen(o.hpos);
	o.tc1.xy /= o.tc1.w;

	return o;
}

/////////////////////////////////////////////////////////////////////////
#ifdef SKIN_NONE
vf main(v_model v) { return _main(v); }
#endif

#ifdef SKIN_0
vf main(v_model_skinned_0 v) { return _main(skinning_0(v)); }
#endif

#ifdef SKIN_1
vf main(v_model_skinned_1 v) { return _main(skinning_1(v)); }
#endif

#ifdef SKIN_2
vf main(v_model_skinned_2 v) { return _main(skinning_2(v)); }
#endif

#ifdef	SKIN_3
vf main(v_model_skinned_3 v) 	{ return _main(skinning_3(v)); }
#endif

#ifdef	SKIN_4
vf main(v_model_skinned_4 v) 	{ return _main(skinning_4(v)); }
#endif
