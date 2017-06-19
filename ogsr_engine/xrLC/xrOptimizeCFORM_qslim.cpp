#include "stdafx.h"
#include "build.h"
#include <MxStdModel.h>
#include <MxQSlim.h>

#pragma comment (lib,"xrQSlim.lib")

#define MAX_DECIMATE_ERROR 0.0005f
#define COMPACTNESS_RATIO  0.001f

void SaveAsSMF			(LPCSTR fname, CDB::CollectorPacked& CL)
{
	IWriter* W			= FS.w_open(fname);
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<CL.getVS(); v_idx++){
		Fvector* v		= CL.getV()+v_idx;
		sprintf			(tmp,"v %f %f %f",v->x,v->y,-v->z);
		W->w_string		(tmp);
	}
	// transfer faces
	for (u32 f_idx=0; f_idx<CL.getTS(); f_idx++){
		CDB::TRI* t		= CL.getT()+f_idx;
		sprintf			(tmp,"f %d %d %d",t->verts[0]+1,t->verts[2]+1,t->verts[1]+1);
		W->w_string		(tmp);
	}
	FS.w_close	(W);
}

struct face_props	{
	u16		material;
	u16		sector;
	Fvector	norm;
	void	set		(u16 mtl, u16 sect, const Fvector& n){material=mtl;sector=sect;norm.set(n);}
};
DEFINE_VECTOR(face_props,FPVec,FPVecIt);

void SimplifyCFORM		(CDB::CollectorPacked& CL)
{
	FPVec FPs;

	u32 base_verts_cnt		= u32(CL.getVS());
	u32 base_faces_cnt		= u32(CL.getTS());

	// save source SMF
#ifdef PRIQUEL
	bool					keep_temp_files = !!strstr(Core.Params,"-keep_temp_files");
	if (keep_temp_files)
#endif // PRIQUEL
	{
		string_path			fn;
		SaveAsSMF			(strconcat(sizeof(fn),fn,pBuild->path,"cform_source.smf"),CL);
	}

	// prepare model
	MxStdModel* mdl			= xr_new<MxStdModel>(base_verts_cnt,base_faces_cnt);

	// transfer vertices
	for (u32 v_idx=0; v_idx<base_verts_cnt; v_idx++){
		Fvector* v			= CL.getV()+v_idx;
		mdl->add_vertex		(v->x,v->y,v->z);
	}
	// transfer faces
	FPs.resize				(base_faces_cnt);
	for (u32 f_idx=0; f_idx<base_faces_cnt; f_idx++){
		CDB::TRI* t			= CL.getT()+f_idx;
		mdl->add_face		(t->verts[0],t->verts[1],t->verts[2]);
		FPs[f_idx].set		(t->material,t->sector,Fvector().mknormal(*(CL.getV()+t->verts[0]),*(CL.getV()+t->verts[1]),*(CL.getV()+t->verts[2])));
	}
	CL.clear				();

	// create and initialize qslim
	MxEdgeQSlim* slim		= xr_new<MxEdgeQSlim>(mdl);
	slim->boundary_weight	= 1000000.f;
	slim->compactness_ratio	= COMPACTNESS_RATIO;
	slim->meshing_penalty	= 1000000.f;
	slim->placement_policy	= MX_PLACE_ENDPOINTS;//MX_PLACE_ENDPOINTS;//MX_PLACE_ENDORMID;//MX_PLACE_OPTIMAL;
	slim->weighting_policy	= MX_WEIGHT_UNIFORM;//MX_WEIGHT_UNIFORM;//MX_WEIGHT_AREA;
	slim->initialize		();

	// constraint material&sector vertex
	Ivector2 f_rm[3]={{0,1}, {1,2}, {2,0}};
	for (f_idx=0; f_idx<slim->valid_faces; f_idx++){
		if (mdl->face_is_valid(f_idx)){
			MxFace& base_f				= mdl->face(f_idx);
			for (u32 edge_idx=0; edge_idx<3; edge_idx++){
				int K;
				u32 I					= f_rm[edge_idx].x;
				u32 J					= f_rm[edge_idx].y;
				const MxFaceList& N0	= mdl->neighbors(base_f[I]);
				const MxFaceList& N1	= mdl->neighbors(base_f[J]);
				for(K=0; K<N1.length(); K++) mdl->face_mark(N1[K], 0);
				for(K=0; K<N0.length(); K++) mdl->face_mark(N0[K], 1);
				for(K=0; K<N1.length(); K++) mdl->face_mark(N1[K], mdl->face_mark(N1[K])+1);
				const MxFaceList& N		= (N0.size()<N1.size())?N0:N1;
				face_props& base_t		= FPs[f_idx];
				if (N.size()){
					u32 cnt_pos=0, cnt_neg=0;
					bool need_constraint= false;
					for(K=0; K<N.length(); K++){
						u32 fff			= N[K];
						MxFace& cur_f	= mdl->face(fff);
						unsigned char mk= mdl->face_mark(fff);
						if((f_idx!=N[K])&&(mdl->face_mark(N[K])==2)){
							face_props& cur_t	= FPs[N[K]];
							if ((cur_t.material!=base_t.material)||(cur_t.sector!=base_t.sector)){
								need_constraint	= true;
								break;
							}
							float dot=base_t.norm.dotproduct(cur_t.norm);
							if		(fsimilar(dot,-1.f,EPS))	cnt_neg++;
							else if (fsimilar(dot,1.f,EPS))		cnt_pos++;
						}
					}
					if (need_constraint||((0==cnt_pos)&&(1==cnt_neg))){
						slim->constraint_manual	(base_f[I],base_f[J],f_idx);
					}
				}
			}
		}
	}
	// collect edges
	slim->collect_edges		();

	// decimate
	slim->decimate			(0,MAX_DECIMATE_ERROR);
	mdl->compact_vertices	();

	// rebuild CDB
	for (f_idx=0; f_idx<mdl->face_count(); f_idx++){
		if (mdl->face_is_valid(f_idx)){
			MxFace& F		= mdl->face(f_idx);
			face_props& FP	= FPs[f_idx];
			CL.add_face		(*((Fvector*)&mdl->vertex(F[0])),
							*((Fvector*)&mdl->vertex(F[1])),
							*((Fvector*)&mdl->vertex(F[2])),
							FP.material, FP.sector);
		}
	}


	// save source CDB
#ifdef PRIQUEL
	if (keep_temp_files)
#endif // PRIQUEL
	{
		string_path			fn;
		SaveAsSMF			(strconcat(sizeof(fn),fn,pBuild->path,"cform_optimized.smf"),CL);
	}

 	xr_delete				(slim);
	xr_delete				(mdl);
}

