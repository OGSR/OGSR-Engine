#ifndef _XR_COMM_
#define _XR_COMM_

#include "xrLevel.h"

#pragma pack(push,4)

const u32 XR_MAX_PORTAL_VERTS	= 6;

// internal use
struct b_rc_face
{
	u16					dwMaterial;
	u32					dwMaterialGame;
	Fvector2			t[3];				// TC
	u16					reserved;
};
// All types to interact with xrLC
typedef Fvector			b_vertex;

struct b_face
{
	u32					v[3];				// vertices
	Fvector2			t[3];				// TC
	u16					dwMaterial;			// index of material
	u32					dwMaterialGame;		// unique-ID of game material
};

struct b_material
{
	u16					surfidx;			// indices of texture surface
	u16					shader;				// index of shader that combine them
	u16					shader_xrlc;		// compiler options
	u16					sector;				// ***
	u16					reserved;			//
	u32					internal_max_area;	//
};

struct b_shader
{
	string128			name;
};
#ifdef _WIN64
struct help_b_texture
{
	string128			name;
	u32					dwWidth;
	u32					dwHeight;
	BOOL				bHasAlpha;
	DWORD				pSurface;
};
#endif
struct b_texture
{
	string128			name;
	u32					dwWidth;
	u32					dwHeight;
	BOOL				bHasAlpha;
	u32*				pSurface;
};

struct b_light_control						// controller or "layer", 30fps
{
	string64			name;				// empty for base layer
	u32					count;				// 0 for base layer
	// u32				data[];
};

struct b_light
{
	u32					controller_ID;		// 0 = base layer
	Flight				data;
};

struct b_light_static	: public b_light	// For static lighting
{
};

struct b_light_dynamic	: public b_light	// For dynamic models
{
	svector<u16,16>		sectors;
};

struct b_glow
{
	Fvector				P;
	float				size;
	u32					flags;				// 0x01 = non scalable
	u32					dwMaterial;			// index of material
};

struct b_portal
{
	u16					sector_front;
	u16					sector_back;
	svector<Fvector,XR_MAX_PORTAL_VERTS>	vertices;
};

struct b_lod_face
{
	Fvector				v		[4];
	Fvector2			t		[4];
};

struct b_lod
{
	b_lod_face			faces	[8];
	u32					dwMaterial;
};

/*
	u32 NUMBER-OF-OBJECTS

	stringZ		name
	u32			vert_count
	b_vertex	vertices[]
	u32			face_count
	b_faces		faces[]
	u16			lod_id;			// u16(-1) = no lod, just static geometry
*/
struct b_mu_model
{
	string128			name;
    int					vert_cnt;
    b_vertex*			verts;
    int					face_cnt;
    b_face*				faces;
	u16					lod_id;				// u16(-1) = no lod, just static geometry
};

/*
	self-describing
*/
struct b_mu_reference
{
	u32					model_index;
    Fmatrix				transform;
    Flags32				flags;
	u16					sector;
    u32					reserved	[8];
};

struct b_params
{
	// Normals & optimization
	float		m_sm_angle;				// normal smooth angle		- 89.0
	float		m_weld_distance;		// by default 0.005f		- 5mm

	// Light maps
	float		m_lm_pixels_per_meter;	// LM - by default: 4 ppm
	u32			m_lm_jitter_samples;	// 1/4/9 - by default		- 4
	u32			m_lm_rms_zero;			// RMS - after what the lightmap will be shrinked to ZERO pixels
	u32			m_lm_rms;				// RMS - shrink and recalc

    // build quality
	u16			m_quality;
	u16			u_reserved;

	// Progressive
	float		f_reserved[6];

    void        Init					()
	{
        // Normals & optimization
        m_sm_angle              = 75.f;
        m_weld_distance         = 0.005f;

        // Light maps
		m_lm_rms_zero			= 4;
		m_lm_rms				= 4;

        setHighQuality			();
	}
	void		setDraftQuality()
	{
    	m_quality				= ebqDraft;
		m_lm_pixels_per_meter	= 0.1f;
		m_lm_jitter_samples		= 1;
	}
	void		setHighQuality()
	{
    	m_quality				= ebqHigh;
		m_lm_pixels_per_meter	= 10;
		m_lm_jitter_samples		= 9;
	}
};
#pragma pack(pop)

enum EBUILD_CHUNKS
{
	EB_Version			= 0,	// XRCLC_CURRENT_VERSION
	EB_Parameters,
	EB_Vertices,
	EB_Faces,
	EB_Materials,
	EB_Shaders_Render,
	EB_Shaders_Compile,
	EB_Textures,
	EB_Glows,
	EB_Portals,
	EB_Light_control,
	EB_Light_static,
	EB_Light_dynamic,
	EB_LOD_models,
    EB_MU_models,
    EB_MU_refs,

	EB_FORCE_DWORD = u32(-1)
};

#endif
