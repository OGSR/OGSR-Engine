#include "stdafx.h"
#include "dxUISequenceVideoItem.h"

dxUISequenceVideoItem::dxUISequenceVideoItem() { m_texture = nullptr; }

void dxUISequenceVideoItem::Copy(IUISequenceVideoItem& _in) { *this = *((dxUISequenceVideoItem*)&_in); }

void dxUISequenceVideoItem::CaptureTexture() { m_texture = RCache.get_ActiveTexture(0); } // TODO cmd_list ??