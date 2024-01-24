#include "stdafx.h"


#include "Blender.h"

// Engine
#include "../../../xr_3da/render.h"

IBlenderXr* IBlenderXr::Create(CLASS_ID cls) { return ::RImplementation.blender_create(cls); }

void IBlenderXr::Destroy(IBlenderXr*& B) { ::RImplementation.blender_destroy(B); }
