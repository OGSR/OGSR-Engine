#pragma once

// Fog volumes
const u32 FV_HEADER = 0;
const u32 FV_MAIN = 1;

struct CFogVolume
{
	shared_str					m_name;
	Fmatrix						m_xform;
	Fmatrix						m_inv_xform;

	Fbox						m_bbox;

	u32							num_vertices;
	u32							num_indices;

	IDirect3DVertexBuffer9*		g_vb;
	IDirect3DIndexBuffer9*		g_ib;
	ref_geom					hGeom;

	Fcolor						m_color;			// fog color
	float						fMaxDensity;		// on what distance will be full fogging
};
// fog volumes container class
class CLevelFog
{
public:
	CLevelFog();
	~CLevelFog();
public:
	bool					m_bLoaded;
	u16						m_version;
	xr_vector<CFogVolume>	m_volumes;

	IBlender*				b_fvolumes;
	ref_shader				s_fvolumes;

	ref_rt					rt_FV_faces;	// render target containing front and back faces of volumes
/*	ref_rt					rt_FV_color;	// final target with volumes in color
	ref_rt					rt_FV_tc;		// target with texcoords for 3D sampling*/

public:

	void					Load			();
	void					Unload			();
};

extern CLevelFog *FV;