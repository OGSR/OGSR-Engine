#pragma once

std::stringstream BuildStackTrace(const char* header, u16 maxFramesCount = 512);
std::stringstream BuildStackTrace(const char* header, PCONTEXT threadCtx, u16 maxFramesCount = 512);
