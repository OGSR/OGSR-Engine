#ifndef _TextureDescrManager_included_
#define _TextureDescrManager_included_

#pragma once
#include "ETextureParams.h"

class CTextureDescrMngr
{
	struct texture_assoc
	{
		shared_str			detail_name;
		R_constant_setup*	        cs;
		u8				usage;
                texture_assoc                   ():cs(NULL),usage(0){}
	};
	struct texture_spec
	{
		shared_str			m_bump_name;
		float				m_material;
		bool				m_parallax;
	};
	struct texture_desc{
		texture_assoc*		m_assoc;
		texture_spec*		m_spec;
#ifdef _EDITOR
		STextureParams::ETType	m_type;
#endif
                texture_desc            ():m_assoc(NULL),m_spec(NULL){}
	};
	DEFINE_MAP(shared_str, texture_desc,	map_TD,	map_TDIt);

	map_TD									m_texture_details;

	void		LoadLTX		();
	void		LoadTHM		();

public:
	void		Load		();
	void		UnLoad		();
public:
	shared_str	GetBumpName		(const shared_str& tex_name) const;
	float		GetMaterial		(const shared_str& tex_name) const;
// KD
	void		GetParallax		(const shared_str& tex_name, BOOL& bParallax) const;
// KD
	void		GetTextureUsage	(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const;
	BOOL		GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup* &CS) const;
#ifdef _EDITOR
	u32			GetTextureType	(const shared_str& tex_name) const;
#endif        
};
#endif