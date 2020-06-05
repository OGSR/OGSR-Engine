#ifndef dxConsoleRender_included
#define dxConsoleRender_included
#pragma once

#include "..\..\Include\xrRender\ConsoleRender.h"

class dxConsoleRender : public IConsoleRender
{
public:
	dxConsoleRender();

	virtual void Copy(IConsoleRender &_in);
	virtual void OnRender(bool bGame);

private:

#if defined(USE_DX10) || defined(USE_DX11)
	ref_shader	m_Shader;
	ref_geom	m_Geom;
#endif
};

#endif	//	ConsoleRender_included