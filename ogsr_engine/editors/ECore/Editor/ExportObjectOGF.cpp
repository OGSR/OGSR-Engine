//----------------------------------------------------
// file: ExportSkeleton.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ExportObjectOGF.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
#include "bone.h"
#include "motion.h"
#include "ui_main.h"
#include "nvMeshMender.h"

CObjectOGFCollectorPacked::CObjectOGFCollectorPacked(const Fbox &bb, int apx_vertices, int apx_faces)
{
    // Params
    m_VMscale.set	(bb.max.x-bb.min.x+EPS, bb.max.y-bb.min.y+EPS, bb.max.z-bb.min.z+EPS);
    m_VMmin.set		(bb.min).sub(EPS);
    m_VMeps.set		(m_VMscale.x/clpOGFMX/2,m_VMscale.y/clpOGFMY/2,m_VMscale.z/clpOGFMZ/2);
    m_VMeps.x		= (m_VMeps.x<EPS_L)?m_VMeps.x:EPS_L;
    m_VMeps.y		= (m_VMeps.y<EPS_L)?m_VMeps.y:EPS_L;
    m_VMeps.z		= (m_VMeps.z<EPS_L)?m_VMeps.z:EPS_L;

    // Preallocate memory
    m_Verts.reserve	(apx_vertices);
    m_Faces.reserve	(apx_faces);

    int		_size	= (clpOGFMX+1)*(clpOGFMY+1)*(clpOGFMZ+1);
    int		_average= (apx_vertices/_size)/2;
    for (int ix=0; ix<clpOGFMX+1; ix++)
        for (int iy=0; iy<clpOGFMY+1; iy++)
            for (int iz=0; iz<clpOGFMZ+1; iz++)
                m_VM[ix][iy][iz].reserve	(_average);
}
//----------------------------------------------------

u16 CObjectOGFCollectorPacked::VPack(SOGFVert& V)
{
    u32 P 	= 0xffffffff;

    u32 ix,iy,iz;
    ix = iFloor(float(V.P.x-m_VMmin.x)/m_VMscale.x*clpOGFMX);
    iy = iFloor(float(V.P.y-m_VMmin.y)/m_VMscale.y*clpOGFMY);
    iz = iFloor(float(V.P.z-m_VMmin.z)/m_VMscale.z*clpOGFMZ);
    R_ASSERT(ix<=clpOGFMX && iy<=clpOGFMY && iz<=clpOGFMZ);

	int similar_pos=-1;
    {
        U32Vec& vl=m_VM[ix][iy][iz];
        for(U32It it=vl.begin();it!=vl.end(); it++){
        	SOGFVert& src=m_Verts[*it];
        	if(src.similar_pos(V)){
	            if(src.similar(V)){
                    P = *it;
                    break;
            	}
                similar_pos=*it;
            }
        }
    }
    if (0xffffffff==P)
    {
    	if (similar_pos>=0) V.P.set(m_Verts[similar_pos].P);
        P = m_Verts.size();
    	if (P>=0xFFFF) return 0xffffffff;
        m_Verts.push_back(V);

        m_VM[ix][iy][iz].push_back(P);

        u32 ixE,iyE,izE;
        ixE = iFloor(float(V.P.x+m_VMeps.x-m_VMmin.x)/m_VMscale.x*clpOGFMX);
        iyE = iFloor(float(V.P.y+m_VMeps.y-m_VMmin.y)/m_VMscale.y*clpOGFMY);
        izE = iFloor(float(V.P.z+m_VMeps.z-m_VMmin.z)/m_VMscale.z*clpOGFMZ);

        R_ASSERT(ixE<=clpOGFMX && iyE<=clpOGFMY && izE<=clpOGFMZ);

        if (ixE!=ix)							m_VM[ixE][iy][iz].push_back	(P);
        if (iyE!=iy)							m_VM[ix][iyE][iz].push_back	(P);
        if (izE!=iz)							m_VM[ix][iy][izE].push_back	(P);
        if ((ixE!=ix)&&(iyE!=iy))				m_VM[ixE][iyE][iz].push_back(P);
        if ((ixE!=ix)&&(izE!=iz))				m_VM[ixE][iy][izE].push_back(P);
        if ((iyE!=iy)&&(izE!=iz))				m_VM[ix][iyE][izE].push_back(P);
        if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	m_VM[ixE][iyE][izE].push_back(P);
    }
    VERIFY(P<u16(-1));
    return (u16)P;
}
//----------------------------------------------------
void CObjectOGFCollectorPacked::ComputeBounding()
{
	m_Box.invalidate		();
//	FvectorVec				split_verts;
//    split_verts.resize		(m_Verts.size());
//    FvectorIt r_it=split_verts.begin();
    for (OGFVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++/*,r_it++*/){
//        r_it->set			(v_it->P);
        m_Box.modify		(v_it->P);
    }
}
//----------------------------------------------------

CExportObjectOGF::SSplit::SSplit(CSurface* surf, const Fbox& bb)
{
    apx_box				= bb;
	m_Surf 				= surf;
    m_CurrentPart		= NULL;
//    I_Current=V_Minimal=-1;
}
//----------------------------------------------------

CExportObjectOGF::SSplit::~SSplit()
{
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++)
    	xr_delete(*it);
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::AppendPart(int apx_vertices, int apx_faces)
{
	m_Parts.push_back	(new CObjectOGFCollectorPacked(apx_box,apx_vertices, apx_faces));
    m_CurrentPart		= m_Parts.back();
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::SavePart(IWriter& F, CObjectOGFCollectorPacked* part)
{
    // Header
    F.open_chunk		(OGF_HEADER);
    ogf_header			H;
    H.format_version	= xrOGF_FormatVersion;
    H.type				= (part->m_SWR.size())?MT_PROGRESSIVE:MT_NORMAL;
    H.shader_id			= 0;
    H.bb.min			= part->m_Box.min;
    H.bb.max			= part->m_Box.max;
    part->m_Box.getsphere(H.bs.c,H.bs.r);
    F.w					(&H,sizeof(H));
    F.close_chunk		();

    // Texture
    F.open_chunk		(OGF_TEXTURE);
    F.w_stringZ			(m_Surf->_Texture());
    F.w_stringZ			(m_Surf->_ShaderName());
    F.close_chunk		();

    // Vertices
    u32 dwFVF			= D3DFVF_XYZ|D3DFVF_NORMAL|(1<<D3DFVF_TEXCOUNT_SHIFT);
    F.open_chunk		(OGF_VERTICES);
    F.w_u32				(dwFVF);
    F.w_u32				(part->m_Verts.size());
    for (OGFVertIt v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
        SOGFVert& pV 	= *v_it;
        F.w				(&(pV.P),sizeof(float)*3);		// position (offset)
        F.w				(&(pV.N),sizeof(float)*3);		// normal
        F.w_float		(pV.UV.x); F.w_float(pV.UV.y);	// tu,tv
    }
    F.close_chunk		();

    // Faces
    F.open_chunk(OGF_INDICES);
    F.w_u32(part->m_Faces.size()*3);
    F.w(part->m_Faces.begin(),part->m_Faces.size()*3*sizeof(u16));
    F.close_chunk();

    // PMap
    if (part->m_SWR.size()) {
        F.open_chunk(OGF_SWIDATA);
        F.w_u32			(0);			// reserved space 16 bytes
        F.w_u32			(0);
        F.w_u32			(0);
        F.w_u32			(0);
        F.w_u32			(part->m_SWR.size()); // num collapses
        for (u32 swr_idx=0; swr_idx<part->m_SWR.size(); swr_idx++)
            F.w			(&part->m_SWR[swr_idx],sizeof(VIPM_SWR));
        F.close_chunk();
    }
/*
    AnsiString r=AnsiString("x:\\import\\test")+chunk_id+".smf";
    IWriter* W 	= FS._w_open(r.c_str());
// optimize
/*
    FvectorVec 			pts;
    U32Vec 				remap_idx	(part->m_Verts.size(),0);
    for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
        SOGFVert& pV 	= *v_it;
        BOOL bFound		= FALSE;
        for (FvectorIt vert_it=pts.begin(); vert_it!=pts.end(); vert_it++){
            if (vert_it->similar(pV.P,EPS_L)){
                remap_idx[v_it-part->m_Verts.begin()]=vert_it-pts.begin();
                bFound	= TRUE;
                break;
            }
        }
        if (!bFound){
            remap_idx[v_it-part->m_Verts.begin()]=pts.size();
            pts.push_back(pV.P);
        }
    }
			
    for (FvectorIt vert_it=pts.begin(); vert_it!=pts.end(); vert_it++){
        AnsiString 		tmp;
        tmp.sprintf		("v %f %f %f",vert_it->x,vert_it->y,-vert_it->z);
        W->w_string		(tmp.c_str());
    }
    for (OGFFaceIt f_it=part->m_Faces.begin(); f_it!=part->m_Faces.end(); f_it++){
        SOGFFace& pF 	= *f_it;
        AnsiString 		tmp;
        tmp.sprintf		("f %d %d %d",remap_idx[pF.v[0]]+1,remap_idx[pF.v[2]]+1,remap_idx[pF.v[1]]+1);
        W->w_string		(tmp.c_str());
    }
*/
/*
    // vertices
    for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
        SOGFVert& pV 	= *v_it;
        AnsiString 		tmp;
        tmp.sprintf		("v %f %f %f",pV.P.x,pV.P.y,-pV.P.z);
        W->w_string		(tmp.c_str());
    }
    // face
    for (OGFFaceIt f_it=part->m_Faces.begin(); f_it!=part->m_Faces.end(); f_it++){
        SOGFFace& pF 	= *f_it;
        AnsiString 		tmp;
        tmp.sprintf		("f %d %d %d",pF.v[0]+1,pF.v[2]+1,pF.v[1]+1);
        W->w_string		(tmp.c_str());
    }
    // normals
    W->w_string			("bind n vertex");
    for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
        SOGFVert& pV 	= *v_it;
        AnsiString 		tmp;
        tmp.sprintf		("n %f %f %f",pV.N.x,pV.N.y,-pV.N.z);
        W->w_string		(tmp.c_str());
    }
    FS.w_close	(W);
//*/            
}

void CExportObjectOGF::SSplit::Save(IWriter& F, int& chunk_id)
{
    for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++){
        CObjectOGFCollectorPacked* part = *it;
        F.open_chunk			(chunk_id);
        SavePart				(F, part);
        F.close_chunk			();
        chunk_id++;
    }
}
//----------------------------------------------------

void CObjectOGFCollectorPacked::MakeProgressive()
{
	VIPM_Init	();
    for (OGFVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++)
    	VIPM_AppendVertex(vert_it->P,vert_it->UV);
    for (OGFFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
    	VIPM_AppendFace(f_it->v[0],f_it->v[1],f_it->v[2]);

    VIPM_Result* R = VIPM_Convert(u32(-1),1.f,1);

    if (R){
        // Permute vertices
        OGFVertVec temp_list = m_Verts;
        for(u32 i=0; i<temp_list.size(); i++)
            m_Verts[R->permute_verts[i]]=temp_list[i];
    
        // Fill indices
        m_Faces.resize	(R->indices.size()/3);
        for (u32 f_idx=0; f_idx<m_Faces.size(); f_idx++){
            SOGFFace& F		= m_Faces[f_idx];
            F.v[0]			= R->indices[f_idx*3+0];
            F.v[1]			= R->indices[f_idx*3+1];
            F.v[2]			= R->indices[f_idx*3+2];
        }

        // Fill SWR
        m_SWR.resize		(R->swr_records.size());
        for (u32 swr_idx=0; swr_idx!=m_SWR.size(); swr_idx++)
            m_SWR[swr_idx]	= R->swr_records[swr_idx];
	}else{
    	Log("!..Can't make progressive.");
    }
    
    // cleanup
    VIPM_Destroy		();
}
//----------------------------------------------------

void CObjectOGFCollectorPacked::CalculateTB()
{
	u32 v_count_reserve			= 3*iFloor(float(m_Verts.size())*1.33f);
	u32 i_count_reserve			= 3*m_Faces.size();

	// Declare inputs
	xr_vector<NVMeshMender::VertexAttribute> 			input;
	input.push_back	(NVMeshMender::VertexAttribute());	// pos
	input.push_back	(NVMeshMender::VertexAttribute());	// norm
	input.push_back	(NVMeshMender::VertexAttribute());	// tex0
	input.push_back	(NVMeshMender::VertexAttribute());	// *** faces

	input[0].Name_= "position";	xr_vector<float>&	i_position	= input[0].floatVector_;	i_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	xr_vector<float>&	i_normal	= input[1].floatVector_;	i_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		xr_vector<float>&	i_tc		= input[2].floatVector_;	i_tc.reserve		(v_count_reserve);
	input[3].Name_= "indices";	xr_vector<int>&		i_indices	= input[3].intVector_;		i_indices.reserve	(i_count_reserve);

	// Declare outputs
	xr_vector<NVMeshMender::VertexAttribute> 			output;
	output.push_back(NVMeshMender::VertexAttribute());	// position, needed for mender
	output.push_back(NVMeshMender::VertexAttribute());	// normal
	output.push_back(NVMeshMender::VertexAttribute());	// tangent
	output.push_back(NVMeshMender::VertexAttribute());	// binormal
	output.push_back(NVMeshMender::VertexAttribute());	// tex0
	output.push_back(NVMeshMender::VertexAttribute());	// *** faces

	output[0].Name_= "position";
	output[1].Name_= "normal";
	output[2].Name_= "tangent";	
	output[3].Name_= "binormal";
	output[4].Name_= "tex0";	
	output[5].Name_= "indices";	

    // fill inputs (verts&indices)
    for (OGFVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++){
        SOGFVert	&iV = *vert_it;
        i_position.push_back(iV.P.x);	i_position.push_back(iV.P.y);	i_position.push_back(iV.P.z);
        i_normal.push_back	(iV.N.x);  	i_normal.push_back	(iV.N.y);	i_normal.push_back	(iV.N.z);
        i_tc.push_back		(iV.UV.x);	i_tc.push_back		(iV.UV.y);	i_tc.push_back		(0);
    }
    for (OGFFaceIt face_it=m_Faces.begin(); face_it!=m_Faces.end(); face_it++){
        SOGFFace	&iF = *face_it;
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
	// bind
	output[0].Name_= "position";
	output[1].Name_= "normal";
	output[2].Name_= "tangent";	
	output[3].Name_= "binormal";
	output[4].Name_= "tex0";	
	output[5].Name_= "indices";	

	xr_vector<float>&	o_position	= output[0].floatVector_;	R_ASSERT(output[0].Name_=="position");
	xr_vector<float>&	o_normal	= output[1].floatVector_;	R_ASSERT(output[1].Name_=="normal");
	xr_vector<float>&	o_tangent	= output[2].floatVector_;	R_ASSERT(output[2].Name_=="tangent");
	xr_vector<float>&	o_binormal	= output[3].floatVector_;	R_ASSERT(output[3].Name_=="binormal");
	xr_vector<float>&	o_tc		= output[4].floatVector_;	R_ASSERT(output[4].Name_=="tex0");
	xr_vector<int>&		o_indices	= output[5].intVector_;		R_ASSERT(output[5].Name_=="indices");

	// verify
	R_ASSERT		(3*m_Faces.size()	== o_indices.size());
    u32 v_cnt		= o_position.size();
    R_ASSERT		(0==v_cnt%3);
    R_ASSERT		(v_cnt == o_normal.size());
    R_ASSERT		(v_cnt == o_tangent.size());
    R_ASSERT		(v_cnt == o_binormal.size());
    R_ASSERT		(v_cnt == o_tc.size());
    v_cnt			/= 3;

    // retriving data
    u32 o_idx		= 0;
    for (face_it=m_Faces.begin(); face_it!=m_Faces.end(); face_it++){
        SOGFFace	&iF = *face_it;
        iF.v[0]		= (u16)o_indices[o_idx++];
        iF.v[1]		= (u16)o_indices[o_idx++];
        iF.v[2]		= (u16)o_indices[o_idx++];
    }
    m_Verts.clear	(); m_Verts.resize(v_cnt);
    for (u32 v_idx=0; v_idx!=v_cnt; v_idx++){
        SOGFVert	&iV = m_Verts[v_idx];
        iV.P.set	(o_position[v_idx*3+0],	o_position[v_idx*3+1],	o_position[v_idx*3+2]);
        iV.N.set	(o_normal[v_idx*3+0],	o_normal[v_idx*3+1],	o_normal[v_idx*3+2]);
        iV.T.set	(o_tangent[v_idx*3+0],	o_tangent[v_idx*3+1],	o_tangent[v_idx*3+2]);
        iV.B.set	(o_binormal[v_idx*3+0],	o_binormal[v_idx*3+1],	o_binormal[v_idx*3+2]);
        iV.UV.set	(o_tc[v_idx*3+0],		o_tc[v_idx*3+1]);
    }

    // Optimize texture coordinates
    // 1. Calc bounds
    Fvector2 	Tdelta;
    Fvector2 	Tmin,Tmax;
    Tmin.set	(flt_max,flt_max);
    Tmax.set	(flt_min,flt_min);
    for (v_idx=0; v_idx!=v_cnt; v_idx++){
        SOGFVert	&iV = m_Verts[v_idx];
        Tmin.min	(iV.UV);
        Tmax.max	(iV.UV);
    }
    Tdelta.x 	= floorf((Tmax.x-Tmin.x)/2+Tmin.x);
    Tdelta.y 	= floorf((Tmax.y-Tmin.y)/2+Tmin.y);

    Fvector2	Tsize;
    Tsize.sub	(Tmax,Tmin);
//	if ((Tsize.x>32)||(Tsize.y>32))
//    	Msg		("#!Surface [T:'%s', S:'%s'] has UV tiled more than 32 times.",m_Texture.c_str(),m_Shader.c_str());
    
    // 2. Recalc UV mapping
    for (v_idx=0; v_idx!=v_cnt; v_idx++){
        SOGFVert	&iV = m_Verts[v_idx];
        iV.UV.sub	(Tdelta);
    }
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::MakeProgressive()
{
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++)
    	(*it)->MakeProgressive();
}

CExportObjectOGF::CExportObjectOGF(CEditableObject* object)
{
	m_Source	= object;
}
//----------------------------------------------------

CExportObjectOGF::~CExportObjectOGF()
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++)
    	xr_delete(*it);
}
//----------------------------------------------------

CExportObjectOGF::SSplit* CExportObjectOGF::FindSplit(CSurface* surf)
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++)
    	if ((*it)->m_Surf==surf) return *it;
    return 0;
}
//----------------------------------------------------

bool CExportObjectOGF::PrepareMESH(CEditableMesh* MESH)
{
//        // generate normals
	BOOL bResult = TRUE;
    MESH->GenerateVNormals();
    // fill faces
    for (SurfFacesPairIt sp_it=MESH->m_SurfFaces.begin(); sp_it!=MESH->m_SurfFaces.end(); sp_it++){
        IntVec& face_lst= sp_it->second;
        CSurface* surf 	= sp_it->first;
        u32 dwTexCnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);	R_ASSERT(dwTexCnt==1);
        SSplit* split	= FindSplit(surf);
        if (0==split){
            SGameMtl* M = GMLib.GetMaterialByID(surf->_GameMtl());
            if (0==M){
                ELog.DlgMsg		(mtError,"Surface: '%s' contains undefined game material.",surf->_Name());
                bResult 		= FALSE; 
                break; 
            }
//                if (!M->Flags.is(SGameMtl::flDynamic)){
//                    ELog.DlgMsg		(mtError,"Surface: '%s' contains non-dynamic game material.",surf->_Name());
//                    bResult 		= FALSE; 
//                    break; 
//                }
            m_Splits.push_back(new SSplit(surf,m_Source->GetBox()));
            split		= m_Splits.back();
        }
        int 	elapsed_faces 	= surf->m_Flags.is(CSurface::sf2Sided)?face_lst.size()*2:face_lst.size();
        const 	bool b2sided	= surf->m_Flags.is(CSurface::sf2Sided);
            
        if (0==split->m_CurrentPart) split->AppendPart(elapsed_faces>0xffff?0xffff:elapsed_faces,elapsed_faces>0xffff?0xffff:elapsed_faces);
            
        do{
            for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
                bool bNewPart	= false;
                st_Face& face 	= MESH->m_Faces[*f_it];
                {
                    SOGFVert v[3];
                    for (int k=0; k<3; k++){
                        st_FaceVert& 	fv = face.pv[k];
                        int offs 		= 0;
                        Fvector2* 		uv = 0;
                        for (u32 t=0; t<dwTexCnt; t++){
                            st_VMapPt& vm_pt 	= MESH->m_VMRefs[fv.vmref].pts[t+offs];
                            st_VMap& vmap		= *MESH->m_VMaps[vm_pt.vmap_index];
                            if (vmap.type!=vmtUV){ offs++; t--; continue; }
                            uv					= &vmap.getUV(vm_pt.index);
                        }
                        R_ASSERT2		(uv,"uv empty");
                        u32 norm_id 	= (*f_it)*3+k;	R_ASSERT2(norm_id<MESH->GetFCount()*3,"Normal index out of range.");
                        v[k].set(MESH->m_Verts[fv.pindex],MESH->m_VNormals[norm_id],*uv);
                    }   
                    elapsed_faces--;
                    if (!split->m_CurrentPart->add_face(v[0], v[1], v[2])) 		bNewPart	= true;
                    if (b2sided){
                        v[0].N.invert(); v[1].N.invert(); v[2].N.invert();
                        if (!split->m_CurrentPart->add_face(v[2], v[1], v[0]))	bNewPart	= true;
                    }
                }
                if (bNewPart && (elapsed_faces>0)) split->AppendPart(elapsed_faces>0xffff?0xffff:elapsed_faces,elapsed_faces>0xffff?0xffff:elapsed_faces);
            }
        }while(elapsed_faces>0);
    }
    // mesh fin
    MESH->UnloadVNormals();
    return bResult;
}

bool CExportObjectOGF::Prepare(bool gen_tb, CEditableMesh* mesh)
{
    if( (m_Source->MeshCount() == 0) ) return false;

    CTimer T;
    T.Start();

    BOOL bResult 		= TRUE;
    if (mesh) bResult 	= PrepareMESH(mesh);
    else{
        for(EditMeshIt mesh_it=m_Source->FirstMesh();mesh_it!=m_Source->LastMesh();mesh_it++)
        	if (!PrepareMESH(*mesh_it)) { bResult=FALSE; break; }
    }
    Log				("Time A: ",T.GetElapsed_sec());

    if (!bResult)		return FALSE;
    
    // calculate TB
    if (gen_tb){
        for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++)
            (*split_it)->CalculateTB();
        Log				("Time B: ",T.GetElapsed_sec());
    }

    // fill per bone vertices
	if (m_Source->m_Flags.is(CEditableObject::eoProgressive)){
        for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++)
            (*split_it)->MakeProgressive();
    }

	// Compute bounding...
    ComputeBounding		();
    Log				("Time C: ",T.GetElapsed_sec());
    return bResult;
}
bool CExportObjectOGF::Export(IWriter& F, bool gen_tb, CEditableMesh* mesh)
{
    if( !Prepare(gen_tb,mesh) ) return false;

	// Saving geometry...
    if ((m_Splits.size()==1)&&(m_Splits[0]->m_Parts.size()==1)){
    	// export as single mesh
        m_Splits[0]->SavePart(F,m_Splits[0]->m_Parts[0]);
    }else{
    	// export as hierrarhy mesh
        // Header
        ogf_header 		H;
        H.format_version= xrOGF_FormatVersion;
        H.type			= MT_HIERRARHY;
        H.shader_id		= 0;
        H.bb.min		= m_Box.min;
        H.bb.max		= m_Box.max;
        m_Box.getsphere(H.bs.c,H.bs.r);
        F.w_chunk		(OGF_HEADER,&H,sizeof(H));

        // Desc
        ogf_desc		desc;
        m_Source->PrepareOGFDesc(desc);
        F.open_chunk	(OGF_S_DESC);
        desc.Save		(F);
        F.close_chunk	();

        // OGF_CHILDREN
        F.open_chunk	(OGF_CHILDREN);
        int chunk=0;
        for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++)
            (*split_it)->Save(F,chunk);
        F.close_chunk	();
    }
    

    return true;
}
//------------------------------------------------------------------------------

bool CExportObjectOGF::ExportAsSimple(IWriter& F)
{
    if( Prepare(true,NULL) )
    {
        // Saving geometry...
        if ((m_Splits.size()==1)&&(m_Splits[0]->m_Parts.size()==1)){
            // export as single mesh
            m_Splits[0]->SavePart(F,m_Splits[0]->m_Parts[0]);
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------

bool CExportObjectOGF::ExportAsWavefrontOBJ(IWriter& F, LPCSTR fn)
{
	if (!Prepare(false,NULL)) return false;

    string256 	name,ext; 
    _splitpath	(fn, 0, 0, name, ext );
    strcat		(name,ext);

	// writ comment
    F.w_string				("# This file uses meters as units for non-parametric coordinates.");

    string512 	tmp,tex_path,tex_name;
    // write mtl
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
	    _splitpath			((*split_it)->m_Surf->_Texture(), 0, 0, tex_name, 0 );
    	sprintf				(tmp,"newmtl %s",tex_name); 	F.w_string	(tmp);
	    _splitpath			((*split_it)->m_Surf->_Texture(), 0, tex_path, tex_name, 0 );
        strconcat			(sizeof(tex_path),tex_path,tex_path,"\\",tex_name,".tga");
    	sprintf				(tmp,"map_Kd %s",tex_path);		F.w_string	(tmp);
    }
    sprintf					(tmp,"mtllib %s",name);				 				F.w_string	(tmp);
    // write mtl
    u32 v_offs				= 0;
    for (split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
	    _splitpath			((*split_it)->m_Surf->_Texture(), 0, 0, tex_name, 0 );
        sprintf				(tmp,"g %d",split_it-m_Splits.begin());				F.w_string	(tmp);
        sprintf				(tmp,"usemtl %s",tex_name);							F.w_string	(tmp);
        Fvector 			mV;
        Fmatrix 			mZ;
        mZ.mirrorZ			();
        for (COGFCPIt it=(*split_it)->m_Parts.begin(); it!=(*split_it)->m_Parts.end(); it++){
            CObjectOGFCollectorPacked* part = *it;
            // vertices
            OGFVertVec& VERTS	= part->getV_Verts();
            OGFVertIt 			v_it;
            for (v_it=VERTS.begin(); v_it!=VERTS.end(); v_it++){
            	mZ.transform_tiny(mV,v_it->P);
                sprintf			(tmp,"v %f %f %f",mV.x,mV.y,mV.z); 		F.w_string	(tmp);
            }
            for (v_it=VERTS.begin(); v_it!=VERTS.end(); v_it++){
                sprintf			(tmp,"vt %f %f",v_it->UV.x,_abs(1.f-v_it->UV.y));		F.w_string	(tmp);
            }
            for (v_it=VERTS.begin(); v_it!=VERTS.end(); v_it++){
            	mZ.transform_dir(mV,v_it->N);
                sprintf			(tmp,"vn %f %f %f",mV.x,mV.y,mV.z);		F.w_string	(tmp);
            }
            for (v_it=VERTS.begin(); v_it!=VERTS.end(); v_it++){
            	mZ.transform_dir(mV,v_it->T);
                sprintf			(tmp,"vg %f %f %f",mV.x,mV.y,mV.z);		F.w_string	(tmp);
            }
            for (v_it=VERTS.begin(); v_it!=VERTS.end(); v_it++){
            	mZ.transform_dir(mV,v_it->B);
                sprintf			(tmp,"vb %f %f %f",mV.x,mV.y,mV.z);		F.w_string	(tmp);
            }
            // faces
            OGFFaceVec& FACES	= part->getV_Faces();
            OGFFaceIt 			f_it;
            for (f_it=FACES.begin(); f_it!=FACES.end(); f_it++){
                sprintf			(tmp,"f %d/%d/%d %d/%d/%d %d/%d/%d",v_offs+f_it->v[2]+1,v_offs+f_it->v[2]+1,v_offs+f_it->v[2]+1,
                                                                    v_offs+f_it->v[1]+1,v_offs+f_it->v[1]+1,v_offs+f_it->v[1]+1,
                                                                    v_offs+f_it->v[0]+1,v_offs+f_it->v[0]+1,v_offs+f_it->v[0]+1); 	F.w_string	(tmp);
            }
	        v_offs  			+= VERTS.size();
        }
    }
	return true;
}
//----------------------------------------------------


