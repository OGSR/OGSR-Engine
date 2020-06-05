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
	FS_FileSet flist;
	FS.file_list(flist, "$game_textures$", FS_ListFiles | FS_RootOnly, "*textures*.ltx");
	Msg("[%s] count of *textures*.ltx files: [%u]", __FUNCTION__, flist.size());

	for (const auto& file : flist)
	{
		string_path fn;
		FS.update_path(fn, "$game_textures$", file.name.c_str());
		CInifile ini(fn);

		if (ini.section_exist("association"))
		{
			for (const auto& [key, value] : ini.r_section("association").Data)
			{
				texture_desc&  desc = m_texture_details[key];
				if (desc.m_assoc)
					xr_delete(desc.m_assoc);
				desc.m_assoc = xr_new<texture_assoc>();

				string_path T;
				float s;
				int res = sscanf(value.c_str(), "%[^,],%f", T, &s);
				R_ASSERT(res == 2);
				desc.m_assoc->detail_name = T;

				cl_dt_scaler*& dts = m_detail_scalers[key];
				if (dts)
					dts->scale = s;
				else
					dts = xr_new<cl_dt_scaler>(s);

				desc.m_assoc->usage = 0;
				if (strstr(value.c_str(), "usage[diffuse_or_bump]"))
					desc.m_assoc->usage = (1 << 0) | (1 << 1);
				else if (strstr(value.c_str(), "usage[bump]"))
					desc.m_assoc->usage = (1 << 1);
				else if (strstr(value.c_str(), "usage[diffuse]"))
					desc.m_assoc->usage = (1 << 0);
			}
		}

		if (ini.section_exist("specification"))
		{
			for (const auto& [key, value] : ini.r_section("specification").Data)
			{
				texture_desc& desc = m_texture_details[key];
				if (desc.m_spec)
					xr_delete(desc.m_spec);
				desc.m_spec = xr_new<texture_spec>();

				string_path bmode, bparallax;
				int res = sscanf(value.c_str(), "bump_mode[%[^]]], material[%f], parallax[%[^]]", bmode, &desc.m_spec->m_material, bparallax);
				R_ASSERT(res >= 2);

				if ((bmode[0] == 'u') && (bmode[1] == 's') && (bmode[2] == 'e') && (bmode[3] == ':')) // bump-map specified
					desc.m_spec->m_bump_name = bmode + 4;
	
				if (res == 3) // parallax
					desc.m_spec->m_use_steep_parallax = (bparallax[0] == 'y') && (bparallax[1] == 'e') && (bparallax[2] == 's');
				else
					desc.m_spec->m_use_steep_parallax = false;
			}
		}
	}
}

void CTextureDescrMngr::LoadTHM(LPCSTR initial)
{
	FS_FileSet				flist;
	FS.file_list			(flist, initial, FS_ListFiles, "*.thm");
#ifdef DEBUG
	Msg						("count of .thm files=%d", flist.size());
#endif // #ifdef DEBUG
	FS_FileSetIt It			= flist.begin();
	FS_FileSetIt It_e		= flist.end();
	STextureParams			tp;
	string_path				fn;
	for(;It!=It_e;++It)
	{
		
		FS.update_path		(fn, initial, (*It).name.c_str());
		IReader* F			= FS.r_open(fn);
		xr_strcpy			(fn,(*It).name.c_str());
		fix_texture_thm_name(fn);

		R_ASSERT			(F->find_chunk(THM_CHUNK_TYPE));
		F->r_u32			();
		tp.Clear			();
		tp.Load				(*F);
		FS.r_close			(F);
		if (
#ifdef USE_SHOC_THM_FORMAT
			STextureParams::ttImage == tp.fmt
			|| STextureParams::ttTerrain == tp.fmt
			|| STextureParams::ttNormalMap == tp.fmt
#else
			STextureParams::ttImage == tp.type
			|| STextureParams::ttTerrain == tp.type
			|| STextureParams::ttNormalMap == tp.type
#endif
		)
		{
			texture_desc&	desc	= m_texture_details[fn];
			cl_dt_scaler*&	dts		= m_detail_scalers[fn];

			if( tp.detail_name.size() &&
				tp.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail) )
			{
				if(desc.m_assoc)
					xr_delete				(desc.m_assoc);

				desc.m_assoc				= xr_new<texture_assoc>();
				desc.m_assoc->detail_name	= tp.detail_name;
				if (dts)
					dts->scale = tp.detail_scale;
				else
					/*desc.m_assoc->cs*/dts	= xr_new<cl_dt_scaler>(tp.detail_scale);

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
			desc.m_spec->m_use_steep_parallax = false;
			
			if(tp.bump_mode==STextureParams::tbmUse)
			{
				desc.m_spec->m_bump_name	= tp.bump_name;
			}
			else if (tp.bump_mode==STextureParams::tbmUseParallax)
			{
				desc.m_spec->m_bump_name	= tp.bump_name;
				desc.m_spec->m_use_steep_parallax = true;
			}

		}
	}
}

void CTextureDescrMngr::Load()
{
#ifdef DEBUG
	CTimer					TT;
	TT.Start				();
#endif // #ifdef DEBUG

#ifdef USE_TEXTURES_LTX
	LoadLTX();
#endif
	LoadTHM					("$game_textures$");
	LoadTHM					("$level$");

#ifdef DEBUG
	Msg("load time=%d ms",TT.GetElapsed_ms());
#endif // #ifdef DEBUG
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

CTextureDescrMngr::~CTextureDescrMngr()
{
	map_CS::iterator I = m_detail_scalers.begin();
	map_CS::iterator E = m_detail_scalers.end();

	for(;I!=E;++I)
		xr_delete(I->second);

	m_detail_scalers.clear	();
}

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

BOOL CTextureDescrMngr::UseSteepParallax(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_use_steep_parallax;
		}	
	}
	return FALSE;
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
			map_CS::const_iterator It2 = m_detail_scalers.find(tex_name);
			CS	= It2==m_detail_scalers.end()?0:It2->second;//TA->cs;
			return TRUE;
		}
	}
	return FALSE;
}
