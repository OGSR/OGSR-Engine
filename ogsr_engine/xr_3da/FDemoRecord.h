// CDemoRecord.h: interface for the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "iinputreceiver.h"
#include "effector.h"

class ENGINE_API CDemoRecord :
	public CEffectorCam,
	public IInputReceiver
{
private:
	int			iCount;
	IWriter*	file;
	Fvector		m_HPB;
	Fvector		m_Position;
	Fmatrix		m_Camera;
	u32			m_Stage;

	Fvector		m_vT;
    Fvector		m_vR;
	Fvector		m_vVelocity;
	Fvector		m_vAngularVelocity;

	BOOL		m_bMakeCubeMap;
	BOOL		m_bMakeScreenshot;
	BOOL		m_bMakeLevelMap;
	BOOL		m_bOverlapped;

	float		m_fSpeed0;
	float		m_fSpeed1;
	float		m_fSpeed2;
	float		m_fSpeed3;
	float		m_fAngSpeed0;
	float		m_fAngSpeed1;
	float		m_fAngSpeed2;
	float		m_fAngSpeed3;

	void		MakeCubeMapFace			(Fvector &D, Fvector &N);
	void		MakeLevelMapProcess		();
	void		MakeScreenshotFace		();
	void		RecordKey				();
	void		MakeCubemap				();
	void		MakeScreenshot			();
	void		MakeLevelMapScreenshot	();
public:
				CDemoRecord				(const char *name, float life_time=60*60*1000);
	virtual		~CDemoRecord();

	virtual void IR_OnKeyboardPress		(int dik);
	virtual void IR_OnKeyboardHold		(int dik);
	virtual void IR_OnMouseMove			(int dx, int dy);
	virtual void IR_OnMouseHold			(int btn);
	
	virtual BOOL Overlapped				(){return m_bOverlapped;}
	virtual	BOOL Process				(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

#endif // !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
