////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes.h
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class DLL_Pure;
class ISheduled;
class IRenderable;
class ICollidable;
class CObject;
class CAnomalyDetector;
class CPatrolPoint;
class CPatrolPath;
struct SRotation;
class CPHCapture;

typedef class_exporter<DLL_Pure> DLL_PureScript;
add_to_type_list(DLL_PureScript)
#undef script_type_list
#define script_type_list save_type_list(DLL_PureScript)

    typedef class_exporter<ISheduled> ISheduledScript;
add_to_type_list(ISheduledScript)
#undef script_type_list
#define script_type_list save_type_list(ISheduledScript)

    typedef class_exporter<IRenderable> IRenderableScript;
add_to_type_list(IRenderableScript)
#undef script_type_list
#define script_type_list save_type_list(IRenderableScript)

    typedef class_exporter<ICollidable> ICollidableScript;
add_to_type_list(ICollidableScript)
#undef script_type_list
#define script_type_list save_type_list(ICollidableScript)

    typedef class_exporter<CObject> CObjectScript;
add_to_type_list(CObjectScript)
#undef script_type_list
#define script_type_list save_type_list(CObjectScript)

    typedef class_exporter<SRotation> CRotationScript;
add_to_type_list(CRotationScript)
#undef script_type_list
#define script_type_list save_type_list(CRotationScript)

    typedef class_exporter<CAnomalyDetector> CAnomalyDetectorScript;
add_to_type_list(CAnomalyDetectorScript)
#undef script_type_list
#define script_type_list save_type_list(CAnomalyDetectorScript)

    class CPatrolPointScript
{
public:
    static LPCSTR getName(CPatrolPoint*);
    static void setName(CPatrolPoint*, LPCSTR);
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPatrolPointScript)
#undef script_type_list
#define script_type_list save_type_list(CPatrolPointScript)

    class CPatrolPathScript
{
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPatrolPathScript)
#undef script_type_list
#define script_type_list save_type_list(CPatrolPathScript)

    using ITextureScript = class_exporter<ITexture>;
add_to_type_list(ITextureScript)
#undef script_type_list
#define script_type_list save_type_list(ITextureScript)

    typedef class_exporter<CPHCapture> CPHCaptureScript;
add_to_type_list(CPHCaptureScript)
#undef script_type_list
#define script_type_list save_type_list(CPHCaptureScript)
