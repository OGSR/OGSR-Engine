#pragma once

//	Reduces amount of work if the texture was not changed.
//	Stores used textures in a separate map to avoid removal of
//	of color map textures from memory.

class ColorMapManager
{
public:
    ColorMapManager();

    void SetTextures(const shared_str& tex0, const shared_str& tex1);

private:
    void UpdateTexture(const shared_str& strTexName, int iTex);

private:

    DEFINE_MAP_PRED(shared_str, ref_texture, map_Tex, map_TexIt, pred_shared_str);

private:
    ref_texture m_CMap[2];
    shared_str m_strCMap[2];

    map_Tex m_TexCache;
};
