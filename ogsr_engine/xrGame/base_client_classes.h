////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes.h
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xr_3da/CameraBase.h"
#include "script_export_space.h"
#include "xrServer_Space.h"
#include "ai/monsters/anomaly_detector.h"

class DLL_Pure;
class ISpatial;
class ISheduled;
class IRenderable;
class ICollidable;
class CObject;
class IRender_Visual;
class FHierrarhyVisual;
class CBlend;
class CKinematics;
class CKinematicsAnimated;
class CAnomalyDetector;

typedef class_exporter<DLL_Pure>	DLL_PureScript;
add_to_type_list(DLL_PureScript)
#undef script_type_list
#define script_type_list save_type_list(DLL_PureScript)

//typedef class_exporter<ISpatial>	ISpatialScript;
//add_to_type_list(ISpatialScript)
//#undef script_type_list
//#define script_type_list save_type_list(ISpatialScript)

typedef class_exporter<ISheduled>	ISheduledScript;
add_to_type_list(ISheduledScript)
#undef script_type_list
#define script_type_list save_type_list(ISheduledScript)

typedef class_exporter<IRenderable>	IRenderableScript;
add_to_type_list(IRenderableScript)
#undef script_type_list
#define script_type_list save_type_list(IRenderableScript)

typedef class_exporter<ICollidable>	ICollidableScript;
add_to_type_list(ICollidableScript)
#undef script_type_list
#define script_type_list save_type_list(ICollidableScript)

typedef class_exporter<CObject>		CObjectScript;
add_to_type_list(CObjectScript)
#undef script_type_list
#define script_type_list save_type_list(CObjectScript)

typedef class_exporter<CBlend>	CBlendScript;
add_to_type_list(CBlendScript)
#undef script_type_list
#define script_type_list save_type_list(CBlendScript)

typedef class_exporter<IRender_Visual>	IRender_VisualScript;
add_to_type_list(IRender_VisualScript)
#undef script_type_list
#define script_type_list save_type_list(IRender_VisualScript)

typedef class_exporter<SRotation>   CRotationScript;
add_to_type_list(CRotationScript)
#undef script_type_list
#define script_type_list save_type_list(CRotationScript)

/*
typedef class_exporter<FHierrarhyVisual>	FHierrarhyVisualScript;
add_to_type_list(FHierrarhyVisualScript)
#undef script_type_list
#define script_type_list save_type_list(FHierrarhyVisualScript)

typedef class_exporter<CKinematics>	CKinematicsScript;
add_to_type_list(CKinematicsScript)
#undef script_type_list
#define script_type_list save_type_list(CKinematicsScript)
*/

typedef class_exporter<CKinematicsAnimated>	CKinematicsAnimatedScript;
add_to_type_list(CKinematicsAnimatedScript)
#undef script_type_list
#define script_type_list save_type_list(CKinematicsAnimatedScript)

typedef class_exporter<CCameraBase>	CCameraBaseScript;
add_to_type_list(CCameraBaseScript)
#undef script_type_list
#define script_type_list save_type_list(CCameraBaseScript)

typedef class_exporter<CAnomalyDetector> CAnomalyDetectorScript;
add_to_type_list(CAnomalyDetectorScript)
#undef script_type_list
#define script_type_list save_type_list(CAnomalyDetectorScript)
