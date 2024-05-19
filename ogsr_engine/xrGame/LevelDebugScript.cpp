#include "StdAfx.h"

#include "LevelDebugScript.h"
#include "Level.h"
#include "debug_renderer.h"

void DBG_ScriptSphere::Render() { Level().debug_renderer().draw_ellipse(m_mat, m_color.get(), m_hud); }

void DBG_ScriptBox::Render() { Level().debug_renderer().draw_obb(m_mat, m_size, m_color.get(), m_hud); }

void DBG_ScriptLine::Render() { Level().debug_renderer().draw_line(Fidentity, m_point_a, m_point_b, m_color.get(), m_hud); }