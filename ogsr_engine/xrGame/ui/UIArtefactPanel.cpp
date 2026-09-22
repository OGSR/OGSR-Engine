#include "StdAfx.h"
#include "UIArtefactPanel.h"
#include "UIInventoryUtilities.h"
#include "UIXmlInit.h"
#include "../artifact.h"

void CUIArtefactPanel::InitFromXML(CUIXml& xml, LPCSTR path, int index)
{
    CUIXmlInit::InitWindow(xml, path, index, this);
    m_cell_size.x = xml.ReadAttribFlt(path, index, "cell_width");
    m_cell_size.y = xml.ReadAttribFlt(path, index, "cell_height");
    m_fScale      = xml.ReadAttribFlt(path, index, "scale", 1);
    m_iIndent     = xml.ReadAttribFlt(path, index, "indent", 1);
    m_bVert       = xml.ReadAttribInt(path, index, "vert") == 1;
}

void CUIArtefactPanel::InitIcons(const TIItemContainer& artefacts)
{
    m_si.SetShader(InventoryUtilities::GetEquipmentIconsShader());
    m_vRects.clear();

    for (const auto& art : artefacts)
    {
        const auto artefact = smart_cast<CArtefact*>(art);
        if (artefact)
        {
            m_vRects.push_back(&(artefact->m_icon_params));
        }
    }
}

void CUIArtefactPanel::Draw()
{
    float x = 0.0f;
    float y = 0.0f;
    float iHeight;
    float iWidth;

    Frect rect;
    GetAbsoluteRect(rect);
    x = rect.left;
    y = rect.top;

    float _s = m_cell_size.x / m_cell_size.y;

    for (const auto& params : m_vRects)
    {
        params->set_shader(&m_si);
        const auto& r = params->original_rect();
        iHeight = m_fScale * (r.bottom - r.top);
        iWidth = _s * m_fScale * (r.right - r.left);

        m_si.SetRect(0, 0, iWidth, iHeight);

        m_si.SetPos(x, y);
        if (!m_bVert)
            x = x + m_iIndent + iWidth;
        else
            y = y + m_iIndent + iHeight;

        m_si.Render();
    }

    CUIWindow::Draw();
}
