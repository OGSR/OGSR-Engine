#include "stdafx.h"
#include "build.h"

bool has_same_edge(Face* F1, Face* F2)
{
	for (int e=0; e<3; e++)
	{
		Vertex *v1_a, *v1_b;
		F1->EdgeVerts(e,&v1_a,&v1_b);	if (v1_a>v1_b) swap(v1_a,v1_b);
		for (int r=0; r<3; r++)
		{
			Vertex *v2_a, *v2_b;
			F2->EdgeVerts(r,&v2_a,&v2_b);	if (v2_a>v2_b) swap(v2_a,v2_b);
			if ((v1_a==v2_a)&&(v1_b==v2_b))	return true;
		}
	}
	return false;
}

void RecurseTri(Face* F, Vertex* V, vecFace& new_adj, float sm_cos)
{
	for (u32 a=0; a<V->adjacent.size(); a++)
	{
		Face* Fn					= V->adjacent[a];
		if (Fn->flags.bSplitted)	continue;
		if (has_same_edge(F,Fn)){
			float	cosa			= F->N.dotproduct(Fn->N);
			if (cosa>sm_cos){ 
				new_adj.push_back	(Fn); 
				Fn->flags.bSplitted	= true; 
				RecurseTri			(Fn,V,new_adj,sm_cos); 
			}
		}
	}
}

// Performs simple cross-smooth
void CBuild::CalcNormals()
{
	u32		Vcount	= g_vertices.size();
	float	p_total = 0;
	float	p_cost  = 1.f/(Vcount);

	// Clear temporary flag
	Status			("Processing...");
	float sm_cos	= _cos(deg2rad(g_params.m_sm_angle));

	for (vecFaceIt it = g_faces.begin(); it!=g_faces.end(); it++)
	{
		(*it)->flags.bSplitted	= true;
		(*it)->CalcNormal		();
	}

	// remark:
	//	we use Face's bSplitted value to indicate that face is processed
	//  so bSplitted means bUsed
	for (u32 I=0; I<Vcount; I++)
	{
		Vertex* V = g_vertices[I];

		for (vecFaceIt AFit = V->adjacent.begin(); AFit != V->adjacent.end(); AFit++)
		{
			Face*	F			= *AFit;
			F->flags.bSplitted	= false;
		}
		std::sort(V->adjacent.begin(), V->adjacent.end());

		while (V->adjacent.size())	{
			vecFace new_adj;
			for (u32 a=0; a<V->adjacent.size(); a++)
			{
				Face* Fn		= V->adjacent[a];
				if (Fn->flags.bSplitted) continue;
				RecurseTri		(Fn,V,new_adj,sm_cos);
				break			;
			}

			// 
			Vertex*	NV			= V->CreateCopy_NOADJ();
			for (u32 a=0; a<new_adj.size(); a++)
			{
				Face* test		= new_adj[a];
				test->VReplace	(V,NV);
			}
			NV->normalFromAdj	();
		}
/*
// old version
		for (u32 AF = 0; AF < V->adjacent.size(); AF++)
		{
			Face*	F			= V->adjacent[AF];
			if (F->flags.bSplitted)	continue;	// Face already used in calculation

			Vertex*	NV			= V->CreateCopy_NOADJ();
			
			// Calculate it's normal
			NV->N.set(0,0,0);
			for (u32 NF = 0; NF < V->adjacent.size(); NF++)
			{
				Face*	Fn		= V->adjacent[NF];

				float	cosa	= F->N.dotproduct(Fn->N);
				if (cosa>sm_cos) {
					NV->N.add		(Fn->N);
					if (!Fn->flags.bSplitted) {
						Fn->VReplace_not_remove	(V,NV);
						Fn->flags.bSplitted		= true;
					}
				}
			}
			if (NV->adjacent.empty()) VertexPool.destroy(NV);
			else NV->N.normalize_safe();
			int y=0;
		}
*/
		Progress(p_total+=p_cost);
	}
	Progress		(1.f);

	// Destroy unused vertices
	IsolateVertices	(FALSE);

	// Recalculate normals
	for (vecVertexIt it=g_vertices.begin(); it!=g_vertices.end(); it++)
		(*it)->normalFromAdj	();

	clMsg	("%d vertices was duplicated 'cause of SM groups",g_vertices.size()-Vcount);

	// Clear temporary flag
	for (vecFaceIt it = g_faces.begin(); it!=g_faces.end(); it++)
		(*it)->flags.bSplitted = false;

	// Models
	Status	("Models...");
	for		(u32 m=0; m<mu_models.size(); m++)
		mu_models[m]->calc_normals();
}
