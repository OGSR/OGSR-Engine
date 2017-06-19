// Build.h: interface for the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_)
#define AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_
#pragma once

#include "communicate.h"
#include "b_globals.h"
#include "fs.h"
#include "xrLevel.h"
#include "Etextureparams.h"
#include "shader_xrlc.h"
#include "xrMU_Model.h"

#pragma comment(lib,"dxt.lib")
extern "C" bool __declspec(dllimport) __stdcall DXTCompress(LPCSTR out_name, u8* raw_data, u8* normal_map, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth);

struct b_BuildTexture : public b_texture
{
	STextureParams	THM;
	
	u32&	Texel	(u32 x, u32 y)
	{
		return pSurface[y*dwWidth+x];
	}
	void	Vflip		()
	{
		R_ASSERT(pSurface);
		for (u32 y=0; y<dwHeight/2; y++)
		{
			u32 y2 = dwHeight-y-1;
			for (u32 x=0; x<dwWidth; x++) 
			{
				u32		t	= Texel(x,y);
				Texel	(x,y)	= Texel(x,y2);
				Texel	(x,y2)	= t;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// tesselator callbacks
typedef	int		tesscb_estimator	(Face*		F);	// -1 = none, 0,1,2 = edge-number
typedef void	tesscb_face			(Face*		F);	// new face
typedef void	tesscb_vertex		(Vertex*	V);	// new vertex

//////////////////////////////////////////////////////////////////////////
class CBuild  
{
public:
	CMemoryWriter					err_invalid;
	CMemoryWriter					err_tjunction;
	CMemoryWriter					err_multiedge;
	void							err_save	();

	Fbox							scene_bb;
	
	xr_vector<b_material>			materials;
	xr_vector<b_shader>				shader_render;
	xr_vector<b_shader>				shader_compile;
	xr_vector<b_BuildTexture>		textures;

	xr_vector<b_glow>				glows;
	xr_vector<b_portal>				portals;
	xr_vector<b_lod>				lods;

	base_lighting					L_static;
	xr_vector<b_light_dynamic>		L_dynamic;
	
	xr_vector<xrMU_Model*>			mu_models;
	xr_vector<xrMU_Reference*>		mu_refs;

	Shader_xrLC_LIB					shaders;
	string_path						path;

	void	mem_Compact				();
	void	mem_CompactSubdivs		();
public:
	void	Load					(const b_params& P, const IReader&  fs);
	void	Run						(LPCSTR path);

	void	Tesselate				();
	void	PreOptimize				();
	void	CorrectTJunctions		();

	void	xrPhase_AdaptiveHT		();
	void	u_Tesselate				(tesscb_estimator* E, tesscb_face* F, tesscb_vertex* V);
	void	u_SmoothVertColors		(int count);

	void	CalcNormals				();
	void	xrPhase_TangentBasis	();

	void	BuildCForm				();
	void	BuildPortals			(IWriter &fs);
	void	BuildRapid				(BOOL bSave);
	void	xrPhase_Radiosity		();
		
	void	IsolateVertices			(BOOL bProgress);
	void	xrPhase_ResolveMaterials();
	void	xrPhase_UVmap			();
	void	xrPhase_Subdivide		();
	void	ImplicitLighting		();
	void	Light_prepare			();
	void	Light					();
	void	Light_R2				();
	void	LightVertex				();
	void	xrPhase_MergeLM			();
	void	xrPhase_MergeGeometry	();

	void	Flex2OGF				();
	void	BuildSectors			();

	void	SaveLights				(IWriter &fs);
	void	SaveTREE				(IWriter &fs);
	void	SaveSectors				(IWriter &fs);

	void	validate_splits			();

	CBuild	();
	~CBuild	();
};

extern CBuild* pBuild;

#endif // !defined(AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_)
