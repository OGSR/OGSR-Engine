#include "stdafx.h"
#include "ETextureParams.h"

void STextureParams::Load(IReader& F)
{
    R_ASSERT(F.find_chunk(THM_CHUNK_TEXTUREPARAM));
    F.r					(&fmt,sizeof(ETFormat));
    flags.assign(F.r_u32());
    border_color= F.r_u32();
    fade_color	= F.r_u32();
    fade_amount	= F.r_u32();
    mip_filter	= F.r_u32();
    width		= F.r_u32();
    height		= F.r_u32();

    if (F.find_chunk(THM_CHUNK_TEXTURE_TYPE)){
        type	= (ETType)F.r_u32();
    }

    if (F.find_chunk(THM_CHUNK_DETAIL_EXT)){
        F.r_stringZ(detail_name);
        detail_scale = F.r_float();
    }

    if (F.find_chunk(THM_CHUNK_MATERIAL)){
    	material		= (ETMaterial)F.r_u32		();
	    material_weight = F.r_float	();
    }

    if (F.find_chunk(THM_CHUNK_BUMP)){
	    bump_virtual_height	= F.r_float				();
	    bump_mode			= (ETBumpMode)F.r_u32	();
        if (bump_mode<STextureParams::tbmNone){
        	bump_mode		= STextureParams::tbmNone; //.. временно (до полного убирания Autogen)
        }
    	F.r_stringZ			(bump_name);
    }

    if (F.find_chunk(THM_CHUNK_EXT_NORMALMAP))
	    F.r_stringZ			(ext_normal_map_name);

	if (F.find_chunk(THM_CHUNK_FADE_DELAY))
		fade_delay			= F.r_u8();
}

void STextureParams::Save(IWriter& F)
{
    F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
    F.w				(&fmt,sizeof(ETFormat));
    F.w_u32			(flags.get());
    F.w_u32			(border_color);
    F.w_u32			(fade_color);
    F.w_u32			(fade_amount);
    F.w_u32			(mip_filter);
    F.w_u32			(width);
    F.w_u32			(height);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TEXTURE_TYPE);
    F.w_u32			(type);
    F.close_chunk	();


    F.open_chunk	(THM_CHUNK_DETAIL_EXT);
    F.w_stringZ		(detail_name);
    F.w_float		(detail_scale);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_MATERIAL);
    F.w_u32			(material);
	F.w_float		(material_weight);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_BUMP);
	F.w_float		(bump_virtual_height);
    F.w_u32			(bump_mode);
    F.w_stringZ		(bump_name);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_EXT_NORMALMAP);
    F.w_stringZ		(ext_normal_map_name);
    F.close_chunk	();

	F.open_chunk	(THM_CHUNK_FADE_DELAY);
	F.w_u8			(fade_delay);
	F.close_chunk	();
}
