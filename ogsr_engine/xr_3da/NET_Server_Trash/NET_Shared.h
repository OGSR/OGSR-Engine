#pragma once
#include "../Engine.h"

//Дефайны директплея, надо убрать.
#define DPNSEND_GUARANTEED 0x0008
#define DPNSEND_NOCOMPLETE 0x0002
#define DPNSEND_NONSEQUENTIAL 0x0010
#define DPNSEND_PRIORITY_HIGH 0x0080

#include "net_utils.h"
#include "net_messages.h"

ENGINE_API extern ClientID BroadcastCID;
