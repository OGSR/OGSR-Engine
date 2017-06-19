#include "stdafx.h"
#include "xr_effgamma.h"
#include "device.h"

IC u16 clr2gamma(float c)
{
	int C=iFloor(c);
	clamp		(C,0,65535);
	return u16	(C);
}
void CGammaControl::Update() 
{
	if (HW.pDevice) {
		D3DGAMMARAMP G;
		GenLUT(G);
		HW.pDevice->SetGammaRamp(0,D3DSGR_NO_CALIBRATION,&G);
	}
}
void CGammaControl::GenLUT(D3DGAMMARAMP &G)
{
	float og	= 1.f / (fGamma + EPS);
	float B		= fBrightness/2.f;
	float C		= fContrast/2.f;
	for (int i=0; i<256; i++) {
//		float	c		= 65535.f*(powf(float(i)/255, og) + fBrightness);
		float	c		= (C+.5f)*powf(i/255.f,og)*65535.f + (B-0.5f)*32768.f - C*32768.f+16384.f;
		G.red[i]		= clr2gamma(c*cBalance.r);
		G.green[i]		= clr2gamma(c*cBalance.g);
		G.blue[i]		= clr2gamma(c*cBalance.b);
	}
}
