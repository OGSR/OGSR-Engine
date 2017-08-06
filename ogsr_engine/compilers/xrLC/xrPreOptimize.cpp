#include "stdafx.h"
#include "build.h"

const int	 HDIM_X = 56;
const int	 HDIM_Y = 24;
const int	 HDIM_Z = 56;

extern bool			g_bUnregister;
extern volatile u32	dwInvalidFaces;

IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void CBuild::PreOptimize()
{
	// We use overlapping hash table to avoid boundary conflicts
	vecVertex*			HASH	[HDIM_X+1][HDIM_Y+1][HDIM_Z+1];
	Fvector				VMmin,	VMscale, VMeps, scale;
	
	// Calculate offset,scale,epsilon
	Fbox				bb = scene_bb;
	VMscale.set			(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
	VMmin.set			(bb.min);
	VMeps.set			(VMscale.x/HDIM_X/2,VMscale.y/HDIM_Y/2,VMscale.z/HDIM_Z/2);
	VMeps.x				= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y				= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z				= (VMeps.z<EPS_L)?VMeps.z:EPS_L;
	scale.set			(float(HDIM_X),float(HDIM_Y),float(HDIM_Z));
	scale.div			(VMscale);
	
	u32	Vcount		= g_vertices.size(),	Vremoved=0;
	u32	Fcount		= g_faces.size(),		Fremoved=0;
	
	// Pre-alloc memory
	int		_size	= (HDIM_X+1)*(HDIM_Y+1)*(HDIM_Z+1);
	int		_average= (Vcount/_size)/2;	if (_average<2)	_average = 2;
	{
		for (int ix=0; ix<HDIM_X+1; ix++)
			for (int iy=0; iy<HDIM_Y+1; iy++)
				for (int iz=0; iz<HDIM_Z+1; iz++)
				{
					HASH[ix][iy][iz] = xr_new<vecVertex> ();
					HASH[ix][iy][iz]->reserve	(_average);
				}
	}
	
	// 
	Status("Processing...");
	g_bUnregister		= false;
	for (int it = 0; it<(int)g_vertices.size(); it++)
	{
		if (0==(it%100000)) {
			Progress(_sqrt(float(it)/float(g_vertices.size())));
			Status	("Processing... (%d verts removed)",Vremoved);
		}

		if (it>=(int)g_vertices.size()) break;

		Vertex	*pTest	= g_vertices[it];
		Fvector	&V		= pTest->P;

		// Hash
		u32 ix,iy,iz;
		ix = iFloor		((V.x-VMmin.x)*scale.x);
		iy = iFloor		((V.y-VMmin.y)*scale.y);
		iz = iFloor		((V.z-VMmin.z)*scale.z);
		R_ASSERT		(ix<=HDIM_X && iy<=HDIM_Y && iz<=HDIM_Z);
		vecVertex &H	= *(HASH[ix][iy][iz]);

		// Search similar vertices in hash table
		for (vecVertexIt T=H.begin(); T!=H.end(); T++)
		{
			Vertex *pBase = *T;
			if (pBase->similar(*pTest,g_params.m_weld_distance)) 
			{
				while				(pTest->adjacent.size())	pTest->adjacent.front()->VReplace(pTest, pBase);
				VertexPool.destroy	(g_vertices[it]);
				Vremoved		+=	1;
				pTest			=	NULL;
				break;
			}
		}
		
		// If we get here - there is no similar vertices - register in hash tables
		if (pTest) 
		{
			H.push_back	(pTest);

			u32 ixE,iyE,izE;
			ixE = iFloor((V.x+VMeps.x-VMmin.x)*scale.x);
			iyE = iFloor((V.y+VMeps.y-VMmin.y)*scale.y);
			izE = iFloor((V.z+VMeps.z-VMmin.z)*scale.z);
			R_ASSERT(ixE<=HDIM_X && iyE<=HDIM_Y && izE<=HDIM_Z);

			if (ixE!=ix)							HASH[ixE][iy][iz]->push_back		(pTest);
			if (iyE!=iy)							HASH[ix][iyE][iz]->push_back		(pTest);
			if (izE!=iz)							HASH[ix][iy][izE]->push_back		(pTest);
			if ((ixE!=ix)&&(iyE!=iy))				HASH[ixE][iyE][iz]->push_back		(pTest);
			if ((ixE!=ix)&&(izE!=iz))				HASH[ixE][iy][izE]->push_back		(pTest);
			if ((iyE!=iy)&&(izE!=iz))				HASH[ix][iyE][izE]->push_back		(pTest);
			if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	HASH[ixE][iyE][izE]->push_back		(pTest);
		}
	}
	
	Status("Removing degenerated/duplicated faces...");
	g_bUnregister	= false;
	for (u32 it=0; it<g_faces.size(); it++)
	{
		R_ASSERT		(it>=0 && it<(int)g_faces.size());
		Face* F			= g_faces[it];
		if ( F->isDegenerated()) {
			FacePool.destroy	(g_faces[it]);
			Fremoved			++;
		} else {
			// Check validity
			F->Verify			( );
		}
		Progress	(float(it)/float(g_faces.size()));
	}
	if (dwInvalidFaces)	
	{
		err_save		();
		if (!b_skipinvalid)
			Debug.fatal		(DEBUG_INFO,"* FATAL: %d invalid faces. Compilation aborted",dwInvalidFaces);
		else
			clMsg		("* Total %d invalid faces. Do something.",dwInvalidFaces);
	}

	Status("Adjacency check...");
	g_bUnregister = false;
	for (u32 it = 0; it<(int)g_vertices.size(); it++)
	{
		if (g_vertices[it] && (0==g_vertices[it]->adjacent.size()))
		{
			VertexPool.destroy	(g_vertices[it]);
			Vremoved			++;
		}
	}
	
	Status	("Cleanup...");
	g_vertices.erase	(std::remove(g_vertices.begin(),g_vertices.end(),(Vertex*)0),g_vertices.end());
	g_faces.erase		(std::remove(g_faces.begin(),g_faces.end(),(Face*)0),g_faces.end());
	{
		for (int ix=0; ix<HDIM_X+1; ix++)
			for (int iy=0; iy<HDIM_Y+1; iy++)
				for (int iz=0; iz<HDIM_Z+1; iz++)
				{
					xr_delete(HASH[ix][iy][iz]);
				}
	}
	mem_Compact			();
	clMsg("%d vertices removed. (%d left)",Vcount-g_vertices.size(),g_vertices.size());
	clMsg("%d faces removed. (%d left)",   Fcount-g_faces.size(),   g_faces.size());
	
	// -------------------------------------------------------------
	/*
	int		err_count	=0 ;
	for (int _1=0; _1<g_faces.size(); _1++)
	{
		Progress(float(_1)/float(g_faces.size()));
		for (int _2=0; _2<g_faces.size(); _2++)
		{
			if (_1==_2)		continue;
			if (FaceEqual(*g_faces[_1],*g_faces[_2]))	{
				err_count	++;
			}
		}
	}
	clMsg		("! duplicate/same faces found:%d",err_count);
	*/
	// -------------------------------------------------------------
}

void CBuild::IsolateVertices	(BOOL bProgress)
{
	if (bProgress)		Status		("Isolating vertices...");
	g_bUnregister		= false;
	u32 verts_old		= g_vertices.size();
	for (int it=0; it<int(g_vertices.size()); it++)	{
		if (bProgress)	Progress	(float(it)/float(g_vertices.size()));
		if (g_vertices[it] && g_vertices[it]->adjacent.empty())	VertexPool.destroy(g_vertices[it]);
	}
	vecVertexIt	_end	= std::remove	(g_vertices.begin(),g_vertices.end(),(Vertex*)0);
	g_vertices.erase	(_end,g_vertices.end());
	g_bUnregister		= true;
	mem_Compact			();
	u32	_count			= verts_old-g_vertices.size();
	if	(_count)		clMsg	("::compact:: %d verts removed",_count);
}
