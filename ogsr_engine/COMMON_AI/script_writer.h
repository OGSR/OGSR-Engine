
#pragma once

#include "script_export_space.h"

typedef class_exporter<IWriter> CScriptWriter;
add_to_type_list(CScriptWriter)
#undef script_type_list
#define script_type_list save_type_list(CScriptWriter)