#pragma once

enum ECameraStyle       {
	csCamDebug,
	csFirstEye,
	csLookAt,
    csMax,
	csFixed,
	cs_forcedword = u32(-1)
};

enum ECamEffectorType		{
	cefDemo		=0,
	cefNext
};

enum EEffectorPPType	{
	ppeNext		=0,
};

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CEffectorCam;
class ENGINE_API CEffectorPP;

