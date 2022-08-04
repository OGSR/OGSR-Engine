#pragma once

#include "../xrCore/xrCore.h"
#include "../Include/xrAPI/xrAPI.h"
#include "Utils/FastDelegate.hpp"

// Our headers
#include "engine.h"
#include "defines.h"
#include "device.h"
#include "fs.h"

#include "../xrcdb/xrxrc.h"

#include "../xrSound/sound.h"

extern ENGINE_API CInifile* pGameIni;

#define READ_IF_EXISTS(ltx, method, section, name, default_value) ((ltx->line_exist(section, name)) ? (ltx->method(section, name)) : (default_value))
