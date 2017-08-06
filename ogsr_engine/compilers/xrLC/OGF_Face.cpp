#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "..\xr_3da\fmesh.h"
#include "xrOcclusion.h"
#include "nvMeshMender.h"

using namespace std;

void set_status(char* N, int id, int f, int v)
{
	string1024 status_str;

	sprintf	(status_str,"Model #%4d [F:%5d, V:%5d]: %s...",id,f,v,N);
	Status	(status_str);
	clMsg	(status_str);
}

BOOL OGF_Vertex::similar(OGF* ogf, OGF_Vertex& V)
{
	const float ntb		= _cos	(deg2rad(5.f));
	if (!P.similar(V.P)) 		return FALSE;
	if (!N.similar(V.N)) 		return FALSE;
	if (!T.similar(V.T)) 		return FALSE;
	if (!B.similar(V.B)) 		return FALSE;
	
	R_ASSERT(UV.size()==V.UV.size());
	for (u32 i=0; i<V.UV.size(); i++) {
		OGF_Texture *T = &*ogf->textures.begin()+i;
		b_texture	*B = T->pSurface;
		float		eu = 1.f/float(B->dwWidth );
		float		ev = 1.f/float(B->dwHeight);
		if (!UV[i].similar(V.UV[i],eu,ev)) return FALSE;
	}
	return TRUE;
}
void OGF_Vertex::dump	(u32 id)
{
//	Msg	("%d: ");
}
BOOL x_vertex::similar	(OGF* ogf, x_vertex& V)
{
	return P.similar(V.P);
}
u16 OGF::x_BuildVertex	(x_vertex& V1)
{
	for (itXV it=x_vertices.begin(); it!=x_vertices.end(); it++)
		if (it->similar(this,V1)) return u16(it-x_vertices.begin());
	x_vertices.push_back	(V1);
	return (u32)			x_vertices.size()-1;
}
u16 OGF::_BuildVertex	(OGF_Vertex& V1)
{
	try {
		for (itOGF_V it=vertices.begin(); it!=vertices.end(); it++)
		{
			if (it->similar(this,V1)) return u16(it-vertices.begin());
		}
	} catch (...) { clMsg("* ERROR: OGF::_BuildVertex");	}

	vertices.push_back	(V1);
	return (u32)vertices.size()-1;
}
void OGF::x_BuildFace	(OGF_Vertex& V1, OGF_Vertex& V2, OGF_Vertex& V3, bool _tc_)
{
	if (_tc_)	return	;	// make empty-list for stuff that has relevant TCs
	x_face	F;
	u32		VertCount	= (u32)x_vertices.size();
	F.v[0]	= x_BuildVertex(x_vertex(V1));
	F.v[1]	= x_BuildVertex(x_vertex(V2));
	F.v[2]	= x_BuildVertex(x_vertex(V3));
	if (!F.Degenerate()) {
		x_faces.push_back(F);
	} else {
		if (x_vertices.size()>VertCount) 
			x_vertices.erase(x_vertices.begin()+VertCount,x_vertices.end());
	}
}
void OGF::_BuildFace	(OGF_Vertex& V1, OGF_Vertex& V2, OGF_Vertex& V3, bool _tc_)
{
	OGF_Face			F;
	u32		VertCount	= (u32)vertices.size();
	F.v[0]	= _BuildVertex(V1);
	F.v[1]	= _BuildVertex(V2);
	F.v[2]	= _BuildVertex(V3);
	if (!F.Degenerate()) {
		for (itOGF_F I=faces.begin(); I!=faces.end(); I++)		if (I->Equal(F)) return;
		faces.push_back	(F);
		x_BuildFace		(V1,V2,V3,_tc_);
	} else {
		if (vertices.size()>VertCount) 
			vertices.erase(vertices.begin()+VertCount,vertices.end());
	}
}
BOOL OGF::dbg_SphereContainsVertex(Fvector& c, float R)
{
	Fsphere	S;	S.set(c,R);
	for (u32 it=0; it<vertices.size(); it++)
		if (S.contains(vertices[it].P))	return	TRUE;
	return FALSE	;
}

void OGF::adjacent_select	(xr_vector<u32>& dest, xr_vector<bool>& vmark, xr_vector<bool>& fmark)
{
	// 0. Search for the group
	for (u32 fit=0; fit<faces.size(); fit++)	{
		OGF_Face&	F		= faces	[fit];
		if (fmark[fit])		continue;			// already registered

		// new face - if empty - just put it in, else check connectivity
		if (dest.empty())	{
			fmark[fit]		= true	;
			dest.push_back	(F.v[0]);	vmark[F.v[0]]=true;
			dest.push_back	(F.v[1]);	vmark[F.v[1]]=true;
			dest.push_back	(F.v[2]);	vmark[F.v[2]]=true;
		} else {
			// check connectivity
			BOOL	bConnected	=	FALSE;
			for (u32 vid=0; vid<3; vid++)	{
				u32		id = F.v	[vid];	// search in already registered verts
				for (u32 sid=0; sid<dest.size(); sid++)
				{
					if (id==dest[sid])	{
						bConnected	= TRUE;	// this face shares at least one vertex with already selected faces
						break;
					}
				}
				if (bConnected)	break;
			}
			if (bConnected)		{
				// add this face's vertices
				fmark[fit]	= true	;
				if (!vmark[F.v[0]])	{ dest.push_back	(F.v[0]);	vmark[F.v[0]]=true; }
				if (!vmark[F.v[1]])	{ dest.push_back	(F.v[1]);	vmark[F.v[1]]=true; }
				if (!vmark[F.v[2]])	{ dest.push_back	(F.v[2]);	vmark[F.v[2]]=true; }
			}
		}
	}
}

void OGF::Optimize	()
{
	// Real optimization
	//////////////////////////////////////////////////////////////////////////
	// x-vertices
	try {
		if (x_vertices.size() && x_faces.size())
		{
			try {
				VERIFY	(x_vertices.size()	<= vertices.size()	);
				VERIFY	(x_faces.size()		== faces.size()		);
			} catch(...) {
				Msg	("* ERROR: optimize: x-geom : verify: failed");
			}

			// Optimize texture coordinates
			/*
			Fvector2 Tdelta;
			try {
				// 1. Calc bounds
				Fvector2 Tmin,Tmax;
				Tmin.set(flt_max,flt_max);
				Tmax.set(flt_min,flt_min);
				for (u32 j=0; j<x_vertices.size(); j++)			{
					x_vertex& V = x_vertices[j];
					//Tmin.min	(V.UV);
					//Tmax.max	(V.UV);
				}
				Tdelta.x = floorf((Tmax.x-Tmin.x)/2+Tmin.x);
				Tdelta.y = floorf((Tmax.y-Tmin.y)/2+Tmin.y);
			} catch(...) {
				Msg	("* ERROR: optimize: x-geom : bounds: failed");
			}

			// 2. Recalc UV mapping
			try {
				for (u32 i=0; i<x_vertices.size(); i++)
					x_vertices[i].UV.sub	(Tdelta);
			} catch(...) {
				Msg	("* ERROR: optimize: x-geom : recalc : failed");
			}
			*/
		}
	} catch(...) {
		Msg	("* ERROR: optimize: x-geom : failed");
	}

	//////////////////////////////////////////////////////////////////////////
	// Detect relevant number of UV pairs
	try {
		R_ASSERT			(vertices.size());
		dwRelevantUV		= vertices.front().UV.size();
		Shader_xrLC*	SH	= pBuild->shaders.Get(pBuild->materials[material].reserved);
		if (!SH->flags.bOptimizeUV)		return;
	} catch(...) {
		Msg	("* ERROR: optimize: std-geom : find relevant UV");
	}

	// Build p-rep
	/*
	typedef xr_vector<u32>	flist	;
	xr_vector<flist>		prep	;	prep.resize(vertices.size());
	for (u32 fit=0; fit<faces.size(); fit++)	{
		OGF_Face&	F		= faces	[fit];
		prep[F.v[0]].push_back		(fit);
		prep[F.v[1]].push_back		(fit);
		prep[F.v[2]].push_back		(fit);
	}
	*/

	// Optimize texture coordinates
	xr_vector<bool>	vmarker;	vmarker.assign	(vertices.size(),false);
	xr_vector<bool>	fmarker;	fmarker.assign	(faces.size(),false);

	for (;;)	{
		// 0. Search for the group
		xr_vector<u32>	selection		;
		for (;;)	{
			u32		_old	= selection.size();
			adjacent_select	(selection,vmarker,fmarker);
			u32		_new	= selection.size();
			if (_old==_new)	break;		// group selected !
		}
		if (selection.empty())		break;

		// 1. Calc bounds
		Fvector2 Tdelta;
		try {
			Fvector2 Tmin,Tmax;
			Tmin.set(flt_max,flt_max);
			Tmax.set(flt_min,flt_min);
			for (u32 j=0; j<selection.size(); j++)
			{
				OGF_Vertex& V = vertices[selection[j]];
				Tmin.min(V.UV[0]);
				Tmax.max(V.UV[0]);
			}
			Tdelta.x = floorf((Tmax.x-Tmin.x)/2+Tmin.x);
			Tdelta.y = floorf((Tmax.y-Tmin.y)/2+Tmin.y);
		} catch(...) {
			Msg	("* ERROR: optimize: std-geom : delta UV");
		}

		// 2. Recalc UV mapping
		try {
			for (u32 i=0; i<selection.size(); i++)
				vertices[selection[i]].UV[0].sub(Tdelta);
		} catch(...) {
			Msg	("* ERROR: optimize: std-geom : recalc UV");
		}
		selection.clear	();
	}
}

// Calculate T&B
void OGF::CalculateTB()
{
	u32 v_count_reserve			= 3*iFloor(float(vertices.size())*1.33f);
	u32 i_count_reserve			= 3*faces.size();

	// Declare inputs
	xr_vector<NVMeshMender::VertexAttribute> 			input;
	input.push_back	(NVMeshMender::VertexAttribute());	// pos
	input.push_back	(NVMeshMender::VertexAttribute());	// norm
	input.push_back	(NVMeshMender::VertexAttribute());	// tex0
	input.push_back	(NVMeshMender::VertexAttribute());	// color
	input.push_back	(NVMeshMender::VertexAttribute());	// *** faces

	input[0].Name_= "position";	xr_vector<float>&	i_position	= input[0].floatVector_;	i_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	xr_vector<float>&	i_normal	= input[1].floatVector_;	i_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		xr_vector<float>&	i_tc		= input[2].floatVector_;	i_tc.reserve		(v_count_reserve);
	input[3].Name_= "clr";		xr_vector<float>&	i_color		= input[3].floatVector_;	i_normal.reserve	(v_count_reserve);
	input[4].Name_= "indices";	xr_vector<int>&		i_indices	= input[4].intVector_;		i_indices.reserve	(i_count_reserve);

	// Declare outputs
	xr_vector<NVMeshMender::VertexAttribute> 			output;
	output.push_back(NVMeshMender::VertexAttribute());	// position, needed for mender
	output.push_back(NVMeshMender::VertexAttribute());	// normal
	output.push_back(NVMeshMender::VertexAttribute());	// tangent
	output.push_back(NVMeshMender::VertexAttribute());	// binormal
	output.push_back(NVMeshMender::VertexAttribute());	// tex0
	output.push_back(NVMeshMender::VertexAttribute());	// color
	output.push_back(NVMeshMender::VertexAttribute());	// *** faces

	output[0].Name_= "position";
	output[1].Name_= "normal";
	output[2].Name_= "tangent";	
	output[3].Name_= "binormal";
	output[4].Name_= "tex0";	
	output[5].Name_= "clr";	
	output[6].Name_= "indices";	

	// fill inputs (verts&indices)
	for (itOGF_V vert_it=vertices.begin(); vert_it!=vertices.end(); vert_it++){
		OGF_Vertex	&iV = *vert_it;
		i_position.push_back(iV.P.x);		i_position.push_back(iV.P.y);		i_position.push_back(iV.P.z);
		i_normal.push_back	(iV.N.x);  		i_normal.push_back	(iV.N.y);		i_normal.push_back	(iV.N.z);
		i_color.push_back	(iV.Color._x);	i_color.push_back	(iV.Color._y);	i_color.push_back	(iV.Color._z);
		i_tc.push_back		(iV.UV[0].x);	i_tc.push_back		(iV.UV[0].y);	i_tc.push_back		(0);
	}
	for (itOGF_F face_it=faces.begin(); face_it!=faces.end(); face_it++){
		OGF_Face	&iF = *face_it;
		i_indices.push_back	(iF.v[0]);
		i_indices.push_back	(iF.v[1]);
		i_indices.push_back	(iF.v[2]);
	}

	// Perform munge
	NVMeshMender mender;
	if (!mender.Munge(
		input,										// input attributes
		output,										// outputs attributes
		deg2rad(75.f),								// tangent space smooth angle
		0,											// no texture matrix applied to my texture coordinates
		NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
		NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
		NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
		))
	{
		Debug.fatal	(DEBUG_INFO,"NVMeshMender failed (%s)",mender.GetLastError().c_str());
	}

	// Bind declarators
	xr_vector<float>&	o_position	= output[0].floatVector_;	R_ASSERT(output[0].Name_=="position");
	xr_vector<float>&	o_normal	= output[1].floatVector_;	R_ASSERT(output[1].Name_=="normal");
	xr_vector<float>&	o_tangent	= output[2].floatVector_;	R_ASSERT(output[2].Name_=="tangent");
	xr_vector<float>&	o_binormal	= output[3].floatVector_;	R_ASSERT(output[3].Name_=="binormal");
	xr_vector<float>&	o_tc		= output[4].floatVector_;	R_ASSERT(output[4].Name_=="tex0");
	xr_vector<float>&	o_color		= output[5].floatVector_;	R_ASSERT(output[5].Name_=="clr");
	xr_vector<int>&		o_indices	= output[6].intVector_;		R_ASSERT(output[6].Name_=="indices");

	// verify
	R_ASSERT		(3*faces.size()	== o_indices.size());
	u32 v_cnt		= o_position.size();
	R_ASSERT		(0==v_cnt%3);
	R_ASSERT		(v_cnt == o_normal.size());
	R_ASSERT		(v_cnt == o_tangent.size());
	R_ASSERT		(v_cnt == o_binormal.size());
	R_ASSERT		(v_cnt == o_tc.size());
	R_ASSERT		(v_cnt == o_color.size());
	v_cnt			/= 3;

	// retriving data
	u32 o_idx		= 0;
	for (itOGF_F face_it=faces.begin(); face_it!=faces.end(); face_it++){
		OGF_Face	&iF = *face_it;
		iF.v[0]		= o_indices[o_idx++];
		iF.v[1]		= o_indices[o_idx++];
		iF.v[2]		= o_indices[o_idx++];
	}
	vertices.clear	(); vertices.resize(v_cnt);
	for (u32 v_idx=0; v_idx!=v_cnt; v_idx++){
		OGF_Vertex	&iV = vertices[v_idx];
		iV.P.set	(o_position[v_idx*3+0],	o_position[v_idx*3+1],	o_position[v_idx*3+2]);
		iV.N.set	(o_normal[v_idx*3+0],	o_normal[v_idx*3+1],	o_normal[v_idx*3+2]);
		iV.T.set	(o_tangent[v_idx*3+0],	o_tangent[v_idx*3+1],	o_tangent[v_idx*3+2]);
		iV.B.set	(o_binormal[v_idx*3+0],	o_binormal[v_idx*3+1],	o_binormal[v_idx*3+2]);
		iV.UV.resize(1);
		iV.UV[0].set(o_tc[v_idx*3+0],		o_tc[v_idx*3+1]);
		iV.Color._set(o_color[v_idx*3+0],	o_color[v_idx*3+1],		o_color[v_idx*3+2]);
	}
}

// Make Progressive
xrCriticalSection			progressive_cs
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(progressive_cs))
#endif // PROFILE_CRITICAL_SECTIONS
;
void OGF::MakeProgressive	(float metric_limit)
{
	// test
	// there is no-sense to simplify small models
	// for batch size 50,100,200 - we are CPU-limited anyway even on nv30
	// for nv40 and up the better guess will probably be around 500
	if (faces.size()<c_PM_FaceLimit)		return		;	

//. AlexMX added for draft build mode
	if (g_params.m_quality==ebqDraft)		return		;

	progressive_cs.Enter	();

	//////////////////////////////////////////////////////////////////////////
	// NORMAL
	vecOGF_V	_saved_vertices		=	vertices	;
	vecOGF_F	_saved_faces		=	faces		;

	{
		// prepare progressive geom
		VIPM_Init				();
		//clMsg("--- append v start .");
		for (u32 v_idx=0;  v_idx<vertices.size(); v_idx++)	
			VIPM_AppendVertex	(vertices[v_idx].P,	vertices[v_idx].UV[0]					);
		//clMsg("--- append f start .");
		for (u32 f_idx=0;  f_idx<faces.size();    f_idx++)	
			VIPM_AppendFace		(faces[f_idx].v[0],	faces[f_idx].v[1],	faces[f_idx].v[2]	);
		//clMsg("--- append end.");

		// Convert
		VIPM_Result*	VR		= 0;
		try						{
						VR		= VIPM_Convert			(u32(25),1.f,1);
		} catch (...)			{
			progressive_clear	()		;
			clMsg				("* mesh simplification failed: access violation");
		}
		if (0==VR)				{
			progressive_clear	()		;
			clMsg				("* mesh simplification failed");
		}
		while (VR && VR->swr_records.size()>0)	{
			// test metric
			u32		_full	=	vertices.size	()		;
			u32		_remove	=	VR->swr_records.size()	;
			u32		_simple	=	_full - _remove			;
			float	_metric	=	float(_remove)/float(_full);
			if		(_metric<metric_limit)		{
				progressive_clear				()		;
				clMsg	("* mesh simplified from [%4dv] to [%4dv], nf[%4d] ==> em[%0.2f]-discarded",_full,_simple,VR->indices.size()/3,metric_limit);
				break									;
			} else {
				clMsg	("* mesh simplified from [%4dv] to [%4dv], nf[%4d] ==> em[%0.2f]-accepted", _full,_simple,VR->indices.size()/3,metric_limit);
			}

			// OK
			// Permute vertices
			for(u32 i=0; i<vertices.size(); i++)
				vertices[VR->permute_verts[i]]=_saved_vertices[i];

			// Fill indices
			faces.resize			(VR->indices.size()/3);
			for (u32 f_idx=0; f_idx<faces.size(); f_idx++){
				faces[f_idx].v[0]	= VR->indices[f_idx*3+0];
				faces[f_idx].v[1]	= VR->indices[f_idx*3+1];
				faces[f_idx].v[2]	= VR->indices[f_idx*3+2];
			}
			// Fill SWR
			m_SWI.count				= VR->swr_records.size();
			m_SWI.sw				= xr_alloc<FSlideWindow>(m_SWI.count);
			for (u32 swr_idx=0; swr_idx!=m_SWI.count; swr_idx++){
				FSlideWindow& dst	= m_SWI.sw[swr_idx];
				VIPM_SWR& src		= VR->swr_records[swr_idx];
				dst.num_tris		= src.num_tris;
				dst.num_verts		= src.num_verts;
				dst.offset			= src.offset;
			}

			break	;
		}
		// cleanup
		VIPM_Destroy			();
	}

	//////////////////////////////////////////////////////////////////////////
	// FAST-PATH
	if (progressive_test() && x_vertices.size() && x_faces.size())
	{
		// prepare progressive geom
		VIPM_Init				();
		Fvector2				zero; zero.set		(0,0);
		for (u32 v_idx=0;  v_idx<x_vertices.size(); v_idx++)	VIPM_AppendVertex	(x_vertices[v_idx].P,	zero						);
		for (u32 f_idx=0;  f_idx<x_faces.size();    f_idx++)	VIPM_AppendFace		(x_faces[f_idx].v[0],	x_faces[f_idx].v[1],	x_faces[f_idx].v[2]	);

		VIPM_Result*	VR		= 0;
		try						{
			VR		= VIPM_Convert			(u32(25),1.f,1);
		} catch (...)			{
			faces				= _saved_faces		;
			vertices			= _saved_vertices	;
			progressive_clear	()		;
			clMsg				("* X-mesh simplification failed: access violation");
		}
		if (0==VR)				{
			faces				= _saved_faces		;
			vertices			= _saved_vertices	;
			progressive_clear	()		;
			clMsg				("* X-mesh simplification failed");
		} else {
			// Convert
			/*
			VIPM_Result*	VR		= VIPM_Convert		(u32(25),1.f,1);
			VERIFY			(VR->swr_records.size()>0)	;
			*/

			// test metric
			u32		_full	=	vertices.size	()		;
			u32		_remove	=	VR->swr_records.size()	;
			u32		_simple	=	_full - _remove			;
			float	_metric	=	float(_remove)/float(_full);
			clMsg	("X mesh simplified from [%4dv] to [%4dv], nf[%4d]",_full,_simple,VR ? VR->indices.size()/3 : 0);

			// OK
			vec_XV					vertices_saved;

			// Permute vertices
			vertices_saved			= x_vertices;
			for(u32 i=0; i<x_vertices.size(); i++)
				x_vertices[VR->permute_verts[i]]=vertices_saved[i];

			// Fill indices
			x_faces.resize			(VR->indices.size()/3);
			for (u32 f_idx=0; f_idx<x_faces.size(); f_idx++){
				x_faces[f_idx].v[0]	= VR->indices[f_idx*3+0];
				x_faces[f_idx].v[1]	= VR->indices[f_idx*3+1];
				x_faces[f_idx].v[2]	= VR->indices[f_idx*3+2];
			}

			// Fill SWR
			x_SWI.count				= VR->swr_records.size();
			x_SWI.sw				= xr_alloc<FSlideWindow>(x_SWI.count);
			for (u32 swr_idx=0; swr_idx!=x_SWI.count; swr_idx++){
				FSlideWindow& dst	= x_SWI.sw[swr_idx];
				VIPM_SWR& src		= VR->swr_records[swr_idx];
				dst.num_tris		= src.num_tris;
				dst.num_verts		= src.num_verts;
				dst.offset			= src.offset;
			}
		}

		// cleanup
		VIPM_Destroy			();
	}

	progressive_cs.Leave	();
}

void OGF_Base::Save	(IWriter &fs)
{
}

// Represent a node as HierrarhyVisual
void OGF_Node::Save	(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// Header
	fs.open_chunk		(OGF_HEADER);
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_HIERRARHY;
	H.shader_id			= 0;
	H.bb.min			= bbox.min;
	H.bb.max			= bbox.max;
	H.bs.c				= C;
	H.bs.r				= R;
	fs.w				(&H,sizeof(H));
	fs.close_chunk		();

	// Children
	fs.open_chunk		(OGF_CHILDREN_L);
	fs.w_u32			((u32)chields.size());
	fs.w				(&*chields.begin(),(u32)chields.size()*sizeof(u32));
	fs.close_chunk		();
}

extern u16	RegisterShader	(LPCSTR T);

void OGF_LOD::Save		(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// Header
	ogf_header			H;
	string1024			sid;
	strconcat			(sizeof(sid),sid,
		pBuild->shader_render[pBuild->materials[lod_Material].shader].name,
		"/",
		pBuild->textures[pBuild->materials[lod_Material].surfidx].name
		);
	fs.open_chunk		(OGF_HEADER);
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_LOD;
	H.shader_id			= RegisterShader(sid);
	H.bb.min			= bbox.min;
	H.bb.max			= bbox.max;
	H.bs.c				= C;
	H.bs.r				= R;
	fs.w				(&H,sizeof(H));
	fs.close_chunk		();

	// Chields
	fs.open_chunk		(OGF_CHILDREN_L);
	fs.w_u32			((u32)chields.size());
	fs.w				(&*chields.begin(),(u32)chields.size()*sizeof(u32));
	fs.close_chunk		();

	// Lod-def
	fs.open_chunk		(OGF_LODDEF2);
	fs.w				(lod_faces,sizeof(lod_faces));
	fs.close_chunk		();
}
