#include "stdafx.h"
#pragma hdrstop

#include "tss_def.h"

IDirect3DStateBlock9* SimulatorStates::record	()
{
	CHK_DX(HW.pDevice->BeginStateBlock());
	for (u32 it=0; it<States.size(); it++)
	{
		State& S	= States[it];
		switch (S.type)
		{
		case 0:	CHK_DX(HW.pDevice->SetRenderState		((D3DRENDERSTATETYPE)S.v1,S.v2));				break;
		case 1: CHK_DX(HW.pDevice->SetTextureStageState	(S.v1,(D3DTEXTURESTAGESTATETYPE)S.v2,S.v3));	break;
		case 2: 
			{
				CHK_DX(HW.pDevice->SetSamplerState		(S.v1,
														(D3DSAMPLERSTATETYPE)S.v2,
														((D3DSAMPLERSTATETYPE)S.v2==D3DSAMP_MAGFILTER&&S.v3==D3DTEXF_ANISOTROPIC)?D3DTEXF_LINEAR:S.v3));
			}break;
		}
	}
	IDirect3DStateBlock9*	SB = 0;
	CHK_DX	(HW.pDevice->EndStateBlock(&SB));
	return	SB;
}

void	SimulatorStates::set_RS	(u32 a, u32 b)
{
	// Search duplicates
	for (int t=0; t<int(States.size()); t++)
	{
		State& S	= States[t];
		if ((0==S.type)&&(a==S.v1)) {
			States.erase(States.begin()+t);
			break;
		}
	}

	// Register
	State		st;
	st.set_RS	(a,b);
	States.push_back(st);
}

void	SimulatorStates::set_TSS	(u32 a, u32 b, u32 c)
{
	// Search duplicates
	for (int t=0; t<int(States.size()); t++)
	{
		State& S	= States[t];
		if ((1==S.type)&&(a==S.v1)&&(b==S.v2)) {
			States.erase(States.begin()+t);
			break;
		}
	}

	// Register
	State		st;
	st.set_TSS	(a,b,c);
	States.push_back(st);
}

void	SimulatorStates::set_SAMP	(u32 a, u32 b, u32 c)
{
	// Search duplicates
	for (int t=0; t<int(States.size()); t++)
	{
		State& S	= States[t];
		if ((2==S.type)&&(a==S.v1)&&(b==S.v2)) {
			States.erase(States.begin()+t);
			break;
		}
	}

	// Register
	State		st;
	st.set_SAMP	(a,b,c);
	States.push_back(st);
}

BOOL	SimulatorStates::equal	(SimulatorStates& S)
{
	if (States.size()!=S.States.size())												return FALSE;
	if (0!=memcmp(&*States.begin(),&*S.States.begin(),States.size()*sizeof(State))) return FALSE;
	return TRUE;
}

void	SimulatorStates::clear	()
{
	States.clear();
}
