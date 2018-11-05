#pragma once

std::stringstream BuildStackTrace(const char* header);
std::stringstream BuildStackTrace(const char* header, PCONTEXT threadCtx);
