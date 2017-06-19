#include "stdafx.h"
#pragma hdrstop
#include "TextureDescrManager.h"
#include "ETextureParams.h"

// eye-params
float					r__dtex_range	= 50;
class cl_dt_scaler		: public R_constant_setup {
public:
	float				scale;

	cl_dt_scaler		(float s) : scale(s)	{};
	virtual void setup	(R_constant* C)
	{
		RCache.set_c	(C,scale,scale,scale,1/r__dtex_range);
	}
};

void fix_texture_thm_name(LPSTR fn)
{
	LPSTR _ext = strext(fn);
	if(  _ext					&&
	  (0==stricmp(_ext,".tga")	||
		0==stricmp(_ext,".thm")	||
		0==stricmp(_ext,".dds")	||
		0==stricmp(_ext,".bmp")	||
		0==stricmp(_ext,".ogm")	) )
		*_ext = 0;
}
void CTextureDescrMngr::LoadLTX()
{
	string_path				fname;		
	FS.update_path			(fname,"$game_textures$","textures.ltx");

	if (FS.exist(fname))
	{
		CInifile			ini(fname);
		if (ini.section_exist("association"))
		{
			CInifile::Sect& data	= ini.r_section("association");
			CInifile::SectCIt I		= data.Data.begin();
			CInifile::SectCIt E		= data.Data.end();
			for ( ; I!=E; ++I)	
			{
				const CInifile::Item& item	= *I;

				texture_desc& desc		= m_texture_details[item.first];
				desc.m_assoc			= xr_new<texture_assoc>();

				string_path				T;
				float					s;

				int res = sscanf					(*item.second,"%[^,],%f",T,&s);
				R_ASSERT(res==2);
				desc.m_assoc->detail_name = T;
				desc.m_assoc->cs		= xr_new<cl_dt_scaler>(s);
				desc.m_assoc->usage		= 0;
				if(strstr(item.second.c_str(),"usage[diffuse_or_bump]"))
					desc.m_assoc->usage	= (1<<0)|(1<<1);
				else
				if(strstr(item.second.c_str(),"usage[bump]"))
					desc.m_assoc->usage	= (1<<1);
				else
				if(strstr(item.second.c_str(),"usage[diffuse]"))
					desc.m_assoc->usage	= (1<<0);
			}
		}//"association"

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& 	sect = ini.r_section("specification");
			for (CInifile::SectCIt I2=sect.Data.begin(); I2!=sect.Data.end(); ++I2)	
			{
				const CInifile::Item& item	= *I2;

				texture_desc& desc		= m_texture_details[item.first];
				desc.m_spec				= xr_new<texture_spec>();

				string_path				bmode, bparallax;
				int res = sscanf		(item.second.c_str(),"bump_mode[%[^]]], material[%f], parallax[%[^]]",bmode,&desc.m_spec->m_material,bparallax);
				R_ASSERT(res>=2);
				if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==':'))
				{
					// bump-map specified
					desc.m_spec->m_bump_name	=	bmode+4;
				}
				if (res == 3)
				{
					if ((bparallax[0]=='y')&&(bparallax[1]=='e')&&(bparallax[2]=='s'))
					{
						// parallax
						desc.m_spec->m_parallax = TRUE;
					} else {
						desc.m_spec->m_parallax = FALSE;
					}
				} else {
					desc.m_spec->m_parallax = FALSE;
				}
			}
		}//"specification"
#ifdef _EDITOR
		if (ini.section_exist("types"))
		{
			CInifile::Sect& 	data = ini.r_section("types");
			for (CInifile::SectCIt I=data.Data.begin(); I!=data.Data.end(); I++)	
			{
				const CInifile::Item& item	= *I;

				texture_desc& desc		= m_texture_details[item.first];
				desc.m_type				= (u16)atoi(item.second.c_str());
			}
		}//"types"
#endif
	}//file-exist
}

void CTextureDescrMngr::LoadTHM()
{
	FS_FileSet				flist;
	FS.file_list			(flist,"$game_textures$",FS_ListFiles,"*.thm");
	Msg						("count of .thm files=%d", flist.size());
	FS_FileSetIt It			= flist.begin();
	FS_FileSetIt It_e		= flist.end();
	STextureParams			tp;
	string_path				fn;
	for(;It!=It_e;++It)
	{
		
		FS.update_path		(fn,"$game_textures$", (*It).name.c_str());
		IReader* F			= FS.r_open(fn);
		strcpy_s				(fn,(*It).name.c_str());
		fix_texture_thm_name(fn);

		R_ASSERT			(F->find_chunk(THM_CHUNK_TYPE));
		F->r_u32			();
		tp.Clear			();
		tp.Load				(*F);
		FS.r_close			(F);
#ifdef _EDITOR
		texture_desc& desc		= m_texture_details[fn];
                desc.m_type                     = tp.type;
#endif
		if (STextureParams::ttImage		== tp.fmt ||
			STextureParams::ttTerrain	== tp.fmt ||
			STextureParams::ttNormalMap	== tp.fmt	)
		{
#ifndef _EDITOR
		texture_desc& desc		 = m_texture_details[fn];
#endif

			if( tp.detail_name.size() &&
				tp.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail) )
			{
				if(desc.m_assoc)
					xr_delete				(desc.m_assoc);

				desc.m_assoc				= xr_new<texture_assoc>();
				desc.m_assoc->detail_name	= tp.detail_name;
				desc.m_assoc->cs			= xr_new<cl_dt_scaler>(tp.detail_scale);
				desc.m_assoc->usage			= 0;
				
				if( tp.flags.is(STextureParams::flDiffuseDetail) )
					desc.m_assoc->usage		|= (1<<0);
				
				if( tp.flags.is(STextureParams::flBumpDetail) )
					desc.m_assoc->usage		|= (1<<1);

			}
			if(desc.m_spec)
				xr_delete				(desc.m_spec);

			desc.m_spec					= xr_new<texture_spec>();
			desc.m_spec->m_material		= tp.material+tp.material_weight;
			
			if(tp.bump_mode==STextureParams::tbmUse)
				desc.m_spec->m_bump_name	= tp.bump_name;
		}
	}
}

void CTextureDescrMngr::Load()
{
	CTimer					TT;
	TT.Start				();

	LoadLTX					();
	LoadTHM					();

	Msg("load time=%d ms",TT.GetElapsed_ms());
}

void CTextureDescrMngr::UnLoad()
{
	map_TD::iterator I = m_texture_details.begin();
	map_TD::iterator E = m_texture_details.end();
	for(;I!=E;++I)
	{
		xr_delete(I->second.m_assoc);
		xr_delete(I->second.m_spec);
	}
	m_texture_details.clear	();
}
/*
		LPCSTR		descr			=	Device.Resources->m_description->r_string("specification",*cName);
		string256	bmode;
		sscanf		(descr,"bump_mode[%[^]]], material[%f]",bmode,&m_material);
		if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==':'))
		{
			// bump-map specified
			m_bumpmap		=	bmode+4;
		}
*/
shared_str CTextureDescrMngr::GetBumpName(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_bump_name;
		}	
	}
	return "";
}

float CTextureDescrMngr::GetMaterial(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_material;
		}
	}
	return 1.0f;
}
/*
		LPCSTR		descr			=	Device.Resources->m_description->r_string("association",base);
		if (strstr(descr,"usage[diffuse_or_bump]"))	
		{ 
			bDetail_Diffuse	= TRUE; 
			bDetail_Bump = TRUE; 
		}
		
		if (strstr(descr,"usage[diffuse]"))			
		{ 
			bDetail_Diffuse	= TRUE; 
		}

		if (strstr(descr,"usage[bump]"))			
		{ 
			bDetail_Bump		= TRUE; 
		}
*/
void CTextureDescrMngr::GetTextureUsage	(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_assoc)
		{
			u8 usage	= I->second.m_assoc->usage;
			bDiffuse	= !!(usage & (1<<0));
			bBump		= !!(usage & (1<<1));
		}	
	}
}

BOOL CTextureDescrMngr::GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup* &CS) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_assoc)
		{
                        texture_assoc* TA = I->second.m_assoc;
			res	= TA->detail_name.c_str();
			CS	= TA->cs;
			return TRUE;
		}
	}
	return FALSE;
}
// KD
void CTextureDescrMngr::GetParallax(const shared_str& tex_name, BOOL& bParallax) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			bParallax = I->second.m_spec->m_parallax;
		}
	}
}
/*
	// Load detail textures association
	string256		fname;		
	FS.update_path	(fname,"$game_textures$","textures.ltx");
	LPCSTR	Iname	= fname;
	if (FS.exist(Iname))
	{
		xr_delete		(m_description);
		m_description	= xr_new<CInifile>	(Iname);
		CInifile&	ini	= *m_description;
		if (ini.section_exist("association"))
		{
			CInifile::Sect& 	data = ini.r_section("association");
			for (CInifile::SectIt I=data.begin(); I!=data.end(); I++)	
			{
				texture_detail			D;
				string256				T;
				float					s;

				CInifile::Item& item	= *I;
				sscanf					(*item.second,"%[^,],%f",T,&s);

				//
				D.T				= xr_strdup				(T);
				D.cs			= xr_new<cl_dt_scaler>	(s);
				LPSTR N			= xr_strdup				(*item.first);
				m_td.insert		(mk_pair(N,D));
			}
		}
	}
*/
#ifdef _EDITOR
u32 CTextureDescrMngr::GetTextureType(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		return I->second.m_type;
	}
	return u32(-1);
}
#endif
