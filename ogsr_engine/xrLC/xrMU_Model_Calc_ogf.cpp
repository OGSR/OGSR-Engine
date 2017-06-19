#include "stdafx.h"
#include "xrMU_Model.h"

#define	TRY(a) try { a; } catch (...) { clMsg("* E: %s", #a); }

void xrMU_Model::calc_ogf()
{
	// Build OGFs
	for (xrMU_Model::v_subdivs_it it=m_subdivs.begin(); it!=m_subdivs.end(); it++)
	{
		OGF*		pOGF	= xr_new<OGF> ();
		b_material*	M		= &(pBuild->materials[it->material]);	// and it's material
		R_ASSERT	(M);

		try {
			// Common data
			pOGF->Sector		= 0;
			pOGF->material		= it->material;

			// Collect textures
			OGF_Texture			T;
			TRY					(T.name		= pBuild->textures[M->surfidx].name);
			TRY					(T.pSurface = &(pBuild->textures[M->surfidx]));
			TRY					(pOGF->textures.push_back(T));

			// Collect faces & vertices
			try {
				xrMU_Model::v_faces_it	_beg	= m_faces.begin() + it->start;
				xrMU_Model::v_faces_it	_end	= _beg + it->count;
				for (xrMU_Model::v_faces_it Fit =_beg; Fit!=_end; Fit++)
				{
					OGF_Vertex			V[3];
					xrMU_Model::_face*	FF		= *Fit;
					R_ASSERT			(FF);
					for (u32 k=0; k<3; k++){
						xrMU_Model::_vertex*	_V		= FF->v[k];	
						u32 id			= (u32)(std::find(m_vertices.begin(),m_vertices.end(),_V)-m_vertices.begin());
						V[k].P			= _V->P;
						V[k].N			= _V->N; 
						V[k].Color		= color[id];
						V[k].T.set		(0,0,0);	//.
						V[k].B.set		(0,0,0);	//.
						V[k].UV.push_back(FF->tc[k]);
					}
					// build face
					TRY				(pOGF->_BuildFace(V[0],V[1],V[2]));
					V[0].UV.clear();	V[1].UV.clear();	V[2].UV.clear();
				}
			} catch (...) {  clMsg("* ERROR: MU2OGF, model %s, *faces*",*m_name); }
		} catch (...)
		{
			clMsg("* ERROR: MU2OGF, 1st part, model %s",*m_name);
		}

		try {
			pOGF->Optimize			();
		} catch (...)	{ clMsg	("* ERROR: MU2OGF, [optimize], model %s",*m_name); }
		try {
			pOGF->CalcBounds		();
		} catch (...)	{ clMsg	("* ERROR: MU2OGF, [bounds], model %s",*m_name); }
		try {
			pOGF->CalculateTB		();
		} catch (...)	{ clMsg	("* ERROR: MU2OGF, [calc_tb], model %s",*m_name); }
		try {
			pOGF->MakeProgressive	(c_PM_MetricLimit_mu);
		} catch (...)	{ clMsg	("* ERROR: MU2OGF, [progressive], model %s",*m_name); }
		try {
			pOGF->Stripify			();
		} catch (...)	{ clMsg	("* ERROR: MU2OGF, [stripify], model %s",*m_name); }

		it->ogf		=	pOGF;
	}
}
