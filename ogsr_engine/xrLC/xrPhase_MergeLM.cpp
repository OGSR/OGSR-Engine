#include "stdafx.h"
#include "build.h"
#include "lightmap.h"
#include "xrPhase_MergeLM_Rect.h"

// Surface access
extern void _InitSurface	();
extern BOOL _rect_place		(L_rect &r, lm_layer*		D);

IC int	compare_defl		(CDeflector* D1, CDeflector* D2)
{
	// First  - by material
	u16 M1		= D1->GetBaseMaterial();
	u16 M2		= D2->GetBaseMaterial();
	if (M1<M2)	return	1;  // less
	if (M1>M2)	return	0;	// more
	return				2;	// equal
}

// should define LESS(D1<D2) behaviour
// sorting - in increasing order
IC int	sort_defl_analyze	(CDeflector* D1, CDeflector* D2)
{
	// first  - get material index
	u16 M1		= D1->GetBaseMaterial();
	u16 M2		= D2->GetBaseMaterial();

	// 1. material area
	u32	 A1		= pBuild->materials[M1].internal_max_area;
	u32	 A2		= pBuild->materials[M2].internal_max_area;
	if (A1<A2)	return	2;	// A2 better
	if (A1>A2)	return	1;	// A1 better

	// 2. material sector (geom - locality)
	u32	 s1		= pBuild->materials[M1].sector;
	u32	 s2		= pBuild->materials[M2].sector;
	if (s1<s2)	return	2;	// s2 better
	if (s1>s2)	return	1;	// s1 better

	// 3. just material index
	if (M1<M2)	return	2;	// s2 better
	if (M1>M2)	return	1;	// s1 better

	// 4. deflector area
	u32 da1		= D1->layer.Area();
	u32 da2		= D2->layer.Area();
	if (da1<da2)return	2;	// s2 better
	if (da1>da2)return	1;	// s1 better

	// 5. they are EQUAL
	return				0;	// equal
}

// should define LESS(D1<D2) behaviour
// sorting - in increasing order
IC bool	sort_defl_complex	(CDeflector* D1, CDeflector* D2)
{
	switch (sort_defl_analyze(D1,D2))	
	{
	case 1:		return true;	// 1st is better 
	case 2:		return false;	// 2nd is better
	case 0:		return false;	// none is better
	default:	return false;
	}
}

class	pred_remove { public: IC bool	operator() (CDeflector* D) { { if (0==D) return TRUE;}; if (D->bMerged) {D->bMerged=FALSE; return TRUE; } else return FALSE;  }; };

void CBuild::xrPhase_MergeLM()
{
	vecDefl			Layer;

	// **** Select all deflectors, which contain this light-layer
	Layer.clear	();
	for (u32 it=0; it<g_deflectors.size(); it++)
	{
		CDeflector*	D		= g_deflectors[it];
		if (D->bMerged)		continue;
		Layer.push_back		(D);
	}

	// Merge this layer (which left unmerged)
	while (Layer.size()) 
	{
		string512	phase_name;
		sprintf		(phase_name,"Building lightmap %d...",g_lightmaps.size());
		Phase		(phase_name);

		// Sort layer by similarity (state changes)
		// + calc material area
		Status		("Selection...");
		for (u32 it=0; it<materials.size(); it++) materials[it].internal_max_area	= 0;
		for (u32 it=0; it<Layer.size(); it++)	{
			CDeflector*	D		= Layer[it];
			materials[D->GetBaseMaterial()].internal_max_area	= _max(D->layer.Area(),materials[D->GetBaseMaterial()].internal_max_area);
		}
		std::stable_sort(Layer.begin(),Layer.end(),sort_defl_complex);

		// Select first deflectors which can fit
		Status		("Selection...");
		u32 maxarea		= c_LMAP_size*c_LMAP_size*8;	// Max up to 8 lm selected
		u32 curarea		= 0;
		u32 merge_count	= 0;
		for (u32 it=0; it<(int)Layer.size(); it++)	{
			int		defl_area	= Layer[it]->layer.Area();
			if (curarea + defl_area > maxarea) break;
			curarea		+=	defl_area;
			merge_count ++;
		}

		// Startup
		Status		("Processing...");
		_InitSurface			();
		CLightmap*	lmap		= xr_new<CLightmap> ();
		g_lightmaps.push_back	(lmap);

		// Process 
		for (u32 it=0; it<merge_count; it++) 
		{
			if (0==(it%1024))	Status	("Process [%d/%d]...",it,merge_count);
			lm_layer&	L		= Layer[it]->layer;
			L_rect		rT,rS; 
			rS.a.set	(0,0);
			rS.b.set	(L.width+2*BORDER-1, L.height+2*BORDER-1);
			rS.iArea	= L.Area();
			rT			= rS;
			if (_rect_place(rT,&L)) 
			{
				BOOL		bRotated;
				if (rT.SizeX() == rS.SizeX()) {
					R_ASSERT(rT.SizeY() == rS.SizeY());
					bRotated = FALSE;
				} else {
					R_ASSERT(rT.SizeX() == rS.SizeY());
					R_ASSERT(rT.SizeY() == rS.SizeX());
					bRotated = TRUE;
				}
				lmap->Capture		(Layer[it],rT.a.x,rT.a.y,rT.SizeX(),rT.SizeY(),bRotated);
				Layer[it]->bMerged	= TRUE;
			}
			Progress(_sqrt(float(it)/float(merge_count)));
		}
		Progress	(1.f);

		// Remove merged lightmaps
		Status			("Cleanup...");
		vecDeflIt last	= std::remove_if	(Layer.begin(),Layer.end(),pred_remove());
		Layer.erase		(last,Layer.end());

		// Save
		Status			("Saving...");
		lmap->Save		();
	}
	clMsg		("%d lightmaps builded",g_lightmaps.size());

	// Cleanup deflectors
	Progress	(1.f);
	Status		("Destroying deflectors...");
	for (u32 it=0; it<g_deflectors.size(); it++)
		xr_delete(g_deflectors[it]);
	g_deflectors.clear_and_free	();
}
