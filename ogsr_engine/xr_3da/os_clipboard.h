////////////////////////////////////////////////////////////////////////////
// Module : os_clipboard.h
// Created : 21.02.2008
// Modified : 04.03.2008
// Author : Evgeniy Sokolov
// Description : os clipboard class
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace os_clipboard
{
	XRCORE_API void copy_to_clipboard(LPCSTR buf);
	XRCORE_API void paste_from_clipboard(LPSTR buf, u32 const& buf_size);
} // namespace os_clipboard


