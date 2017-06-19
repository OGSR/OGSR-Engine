#pragma once

#define MAX_PUDDLES 30

struct SPuddle
{
	Fvector		P;
	float		max_depth;
	float		radius;
	Fmatrix		xform;

	SPuddle() {P.set(0,0,0); max_depth = 0; radius = 30; xform.identity();};
	void		make_xform();
};

class CPuddles
{
public:
	SPuddle		points[MAX_PUDDLES];
	u32			size;
	bool		m_bLoaded;
	IRender_Mesh *mesh;

	CPuddles():size(0),m_bLoaded(false){mesh = xr_new<IRender_Mesh>();};
//	~CPuddles();

	void	Load();
	void	Unload();
};

extern CPuddles *Puddles;