#pragma once

void	uber_deffer	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace=0, bool DO_NOT_FINISH=false);
void	uber_forward(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, u32 _aref, bool env = false, LPCSTR _detail_replace = 0, bool DO_NOT_FINISH = false);
