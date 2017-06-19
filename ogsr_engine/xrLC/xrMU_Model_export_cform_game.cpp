#include "stdafx.h"
#include "xrMU_model.h"
/*
int getCFormVID		(xrMU_Model::v_vertices& V,xrMU_Model::_vertex *F)
{
	xrMU_Model::v_vertices_it it	= std::std::lower_bound(V.begin(),V.end(),F);
	return it-V.begin();
}
*/

extern int bCriticalErrCnt;
int getTriByEdge	(xrMU_Model::_vertex *V1, xrMU_Model::_vertex *V2, xrMU_Model::_face* parent, xrMU_Model::v_faces &ids)
{
	xrMU_Model::_face*	found	= 0;
	int		f_count				= 0;

	for (xrMU_Model::v_faces_it I=V1->adjacent.begin(); I!=V1->adjacent.end(); I++)
	{
		xrMU_Model::_face* test = *I;
		if (test == parent) continue;
		if (test->VContains(V2)) {
			f_count++;
			found = test;
		}
	}
	if (f_count>1) {
		bCriticalErrCnt	++;
		pBuild->err_multiedge.w_fvector3(V1->P);
		pBuild->err_multiedge.w_fvector3(V2->P);
	}
	if (found) {
		xrMU_Model::v_faces_it F = std::lower_bound(ids.begin(),ids.end(),found);
		if (found == *F) return (u32)(F-ids.begin());
		else return -1;
	} else {
		return -1;
	}
}

void xrMU_Reference::export_cform_game(CDB::CollectorPacked& CL)
{
	// Collecting data
	xrMU_Model::v_faces*	cfFaces		= xr_new<xrMU_Model::v_faces>		();
	xrMU_Model::v_vertices*	cfVertices	= xr_new<xrMU_Model::v_vertices>	();
	{
		xr_vector<bool>	cfVertexMarks;
		cfVertexMarks.assign(model->m_vertices.size(),false);

		std::sort			(model->m_vertices.begin(),model->m_vertices.end());

		// faces and mark vertices
		cfFaces->reserve	(model->m_faces.size());
		for (xrMU_Model::v_faces_it I=model->m_faces.begin(); I!=model->m_faces.end(); I++)
		{
			xrMU_Model::_face* F = *I;
			if (F->Shader().flags.bCollision) 
			{
				cfFaces->push_back	(F);

				for (u32 vit=0; vit<3; vit++)
				{
					u32 g_id		=  u32(std::lower_bound
						(
							model->m_vertices.begin(),model->m_vertices.end(),F->v[vit]
						) 
						- model->m_vertices.begin	());
					cfVertexMarks	[g_id] = true;
				}
			}
		}

		// verts
		cfVertices->reserve	(model->m_vertices.size());
		std::sort			(cfFaces->begin(),cfFaces->end());
		for (u32 V=0; V<model->m_vertices.size(); V++)
			if (cfVertexMarks[V]) cfVertices->push_back(model->m_vertices[V]);
	}

	// Collect faces
	u32	Offset			= (u32)CL.getTS();
	for (xrMU_Model::v_faces_it F = cfFaces->begin(); F!=cfFaces->end(); F++)
	{
		xrMU_Model::_face*	T = *F;
		
		// xform
		Fvector					P[3];
		xform.transform_tiny	(P[0],T->v[0]->P);
		xform.transform_tiny	(P[1],T->v[1]->P);
		xform.transform_tiny	(P[2],T->v[2]->P);

		CL.add_face				( P[0], P[1], P[2], T->dwMaterialGame, sector);
	}

	xr_delete		(cfFaces);
	xr_delete		(cfVertices);
}
