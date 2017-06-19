//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonMotions.h"
#include 	"SkeletonAnimated.h"
#include	"fmesh.h"
#include	"motion.h"

motions_container*	g_pMotionsContainer	= 0;

u16 find_bone_id(vecBones* bones, shared_str nm)
{
	for (u16 i=0; i<(u16)bones->size(); i++)
		if (bones->at(i)->name==nm) return i;
	return BI_NONE;
}

//-----------------------------------------------------------------------
BOOL motions_value::load		(LPCSTR N, IReader *data, vecBones* bones)
{

	m_id						= N;

	bool bRes					= true;
	// Load definitions
	U16Vec rm_bones				(bones->size(),BI_NONE);
	IReader* MP 				= data->open_chunk(OGF_S_SMPARAMS);

	if (MP)
	{
		u16 vers 				= MP->r_u16();
		u16 part_bone_cnt		= 0;
		string128 				buf;
		R_ASSERT3				(vers<=xrOGF_SMParamsVersion,"Invalid OGF/OMF version:",N);
		
		// partitions
		u16						part_count;
		part_count 				= MP->r_u16();

		for (u16 part_i=0; part_i<part_count; part_i++)
		{
			CPartDef& PART		= m_partition[part_i];
			MP->r_stringZ		(buf,sizeof(buf));
#ifdef _EDITOR
			_strlwr(buf);
			PART.Name			= buf;
#else
			PART.Name			= _strlwr(buf);
#endif
			PART.bones.resize	(MP->r_u16());

			for (xr_vector<u32>::iterator b_it=PART.bones.begin(); b_it<PART.bones.end(); b_it++)
			{
				MP->r_stringZ	(buf,sizeof(buf));
				u16 m_idx 		= u16			(MP->r_u32());
				*b_it			= find_bone_id	(bones,buf); 
#ifdef _EDITOR
				if (*b_it==BI_NONE){
					bRes		= false;
					Msg			("!Can't find bone: '%s'",buf);
				}
#else
				VERIFY3			(*b_it!=BI_NONE,"Can't find bone:",buf);
#endif
				if (bRes)		rm_bones[m_idx] = u16(*b_it);
			}
			part_bone_cnt		= u16(part_bone_cnt + (u16)PART.bones.size());
		}

#ifdef _EDITOR
		if (part_bone_cnt!=(u16)bones->size()){
			bRes = false;
			Msg("!Different bone count [Object: '%d' <-> Motions: '%d']",bones->size(),part_bone_cnt);
		}
#else
		VERIFY3(part_bone_cnt==(u16)bones->size(),"Different bone count '%s'",N);
#endif
		if (bRes)
		{
			// motion defs (cycle&fx)
			u16 mot_count			= MP->r_u16();
            m_mdefs.resize			(mot_count);

			for (u16 mot_i=0; mot_i<mot_count; mot_i++)
			{
				MP->r_stringZ		(buf,sizeof(buf));
#ifdef _EDITOR
				_strlwr		(buf);
				shared_str nm		= buf;
#else
				shared_str nm		= _strlwr		(buf);
#endif
				u32 dwFlags			= MP->r_u32		();
				CMotionDef&	D		= m_mdefs[mot_i];
                D.Load				(MP,dwFlags,vers);
//.             m_mdefs.push_back	(D);
				
				if (dwFlags&esmFX)	
					m_fx.insert		(mk_pair(nm,mot_i));
				else				
					m_cycle.insert	(mk_pair(nm,mot_i));

                m_motion_map.insert	(mk_pair(nm,mot_i));
			}
		}
		MP->close();
	}else
	{
		Debug.fatal	(DEBUG_INFO,"Old skinned model version unsupported! (%s)",N);
	}
	if (!bRes)	return false;

	// Load animation
	IReader*	MS		= data->open_chunk(OGF_S_MOTIONS);
	if (!MS) 			return false;

	u32			dwCNT	= 0;
	MS->r_chunk_safe	(0,&dwCNT,sizeof(dwCNT));
    VERIFY		(dwCNT<0x3FFF); // MotionID 2 bit - slot, 14 bit - motion index

	// set per bone motion size
	for (u32 i=0; i<bones->size(); i++)
		m_motions[bones->at(i)->name].resize(dwCNT);

	// load motions
	for (u16 m_idx=0; m_idx<(u16)dwCNT; m_idx++){
		string128			mname;
		R_ASSERT			(MS->find_chunk(m_idx+1));             
		MS->r_stringZ		(mname,sizeof(mname));
#ifdef _DEBUG        
		// sanity check
		xr_strlwr			(mname);
        accel_map::iterator I= m_motion_map.find(mname); 
        VERIFY3				(I!=m_motion_map.end(),"Can't find motion:",mname);
        VERIFY3				(I->second==m_idx,"Invalid motion index:",mname);
#endif
		u32 dwLen			= MS->r_u32();
		for (u32 i=0; i<bones->size(); i++){
			u16 bone_id		= rm_bones[i];
			VERIFY2			(bone_id!=BI_NONE,"Invalid remap index.");
			CMotion&		M	= m_motions[bones->at(bone_id)->name][m_idx];
			M.set_count			(dwLen);
			M.set_flags			(MS->r_u8());
            
            if (M.test_flag(flRKeyAbsent))	{
                CKeyQR* r 		= (CKeyQR*)MS->pointer();
				u32 crc_q		= crc32(r,sizeof(CKeyQR));
				M._keysR.create	(crc_q,1,r);
                MS->advance		(1 * sizeof(CKeyQR));
            }else{
                u32 crc_q		= MS->r_u32	();
                M._keysR.create	(crc_q,dwLen,(CKeyQR*)MS->pointer());
                MS->advance		(dwLen * sizeof(CKeyQR));
            }
            if (M.test_flag(flTKeyPresent))	{
                u32 crc_t		= MS->r_u32	();
                M._keysT.create	(crc_t,dwLen,(CKeyQT*)MS->pointer());
                MS->advance		(dwLen * sizeof(CKeyQT));
                MS->r_fvector3	(M._sizeT);
                MS->r_fvector3	(M._initT);
            }else{
                MS->r_fvector3	(M._initT);
            }
		}
	}
//	Msg("Motions %d/%d %4d/%4d/%d, %s",p_cnt,m_cnt, m_load,m_total,m_r,N);
	MS->close();

	return bRes;
}

MotionVec* motions_value::bone_motions(shared_str bone_name)
{
	BoneMotionMapIt it			= m_motions.find(bone_name); VERIFY(it!=m_motions.end());
	return &it->second;
}
//-----------------------------------
motions_container::motions_container()
{
}
extern shared_str s_bones_array_const;
motions_container::~motions_container()
{
//	clean	(false);
//	clean	(true);
//	dump	();
	VERIFY	(container.empty());
	s_bones_array_const = 0;
}

bool motions_container::has(shared_str key)
{
	return (container.find(key)!=container.end());
}

motions_value* motions_container::dock(shared_str key, IReader *data, vecBones* bones)
{
	motions_value*	result		= 0	;
	SharedMotionsMapIt	I		= container.find	(key);
	if (I!=container.end())		result = I->second;
	if (0==result)				{
		// loading motions
		VERIFY					(data);
		result					= xr_new<motions_value>();
		result->m_dwReference	= 0;
		BOOL bres				= result->load	(key.c_str(),data,bones);
		if (bres)				container.insert(mk_pair(key,result));
		else					xr_delete		(result);
	}
	return result;
}
void motions_container::clean(bool force_destroy)
{
	SharedMotionsMapIt it	= container.begin();
	SharedMotionsMapIt _E	= container.end();
	if (force_destroy){
		for (; it!=_E; it++){
			motions_value*	sv = it->second;
			xr_delete		(sv);
		}
		container.clear		();
	}else{
		for (; it!=_E; )	{
			motions_value*	sv = it->second;
			if (0==sv->m_dwReference)	
			{
				SharedMotionsMapIt	i_current	= it;
				SharedMotionsMapIt	i_next		= ++it;
				xr_delete			(sv);
				container.erase		(i_current);
				it					= i_next;
			} else {
				it++;
			}
		}
	}
}
void motions_container::dump()
{
	SharedMotionsMapIt it	= container.begin();
	SharedMotionsMapIt _E	= container.end();
	Log	("--- motion container --- begin:");
	u32 sz					= sizeof(*this);
	for (u32 k=0; it!=_E; k++,it++){
		sz					+= it->second->mem_usage();
		Msg("#%3d: [%3d/%5d Kb] - %s",k,it->second->m_dwReference,it->second->mem_usage()/1024,it->first.c_str());
	}
	Msg ("--- items: %d, mem usage: %d Kb ",container.size(),sz/1024);
	Log	("--- motion container --- end.");
}

//////////////////////////////////////////////////////////////////////////
// High level control
void CMotionDef::Load(IReader* MP, u32 fl, u16 version)
{
	// params
	bone_or_part= MP->r_u16(); // bCycle?part_id:bone_id;
	motion		= MP->r_u16(); // motion_id
	speed		= Quantize(MP->r_float());
	power		= Quantize(MP->r_float());
	accrue		= Quantize(MP->r_float());
	falloff		= Quantize(MP->r_float());
	flags		= (u16)fl;
	if (!(flags&esmFX) && (falloff>=accrue)) falloff = u16(accrue-1);

	if(version>=4)
	{
		u32	cnt				= MP->r_u32();
        if(cnt>0)
        {
            marks.resize		(cnt);

            for(u32 i=0; i<cnt; ++i)
                marks[i].Load	(MP);
        }
	}
}

bool CMotionDef::StopAtEnd()
{
	return !!(flags&esmStopAtEnd);
}

bool motion_marks::pick_mark(const float& t) const
{
	C_ITERATOR	it		= intervals.begin();
	C_ITERATOR	it_e	= intervals.end();

	for( ;it!=it_e; ++it)
	{
		if( (*it).first<=t && (*it).second>=t )
			return true;
		
		if((*it).first>t)
			break;
	}
	return false;
}

void motion_marks::Load(IReader* R)
{
	xr_string 			tmp;
	R->r_string			(tmp);
    name				= tmp.c_str();
	u32 cnt				= R->r_u32();
	intervals.resize	(cnt);
	for(u32 i=0; i<cnt; ++i)
	{
		interval& item		= intervals[i];
		item.first			= R->r_float();
		item.second			= R->r_float();
	}
}
#ifdef _EDITOR
void motion_marks::Save(IWriter* W)
{
	W->w_string			(name.c_str());
	u32 cnt				= intervals.size();
    W->w_u32			(cnt);
	for(u32 i=0; i<cnt; ++i)
	{
		interval& item		= intervals[i];
		W->w_float			(item.first);
		W->w_float			(item.second);
	}
}
#endif
