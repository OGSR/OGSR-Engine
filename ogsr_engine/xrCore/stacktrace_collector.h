#pragma once

std::string BuildStackTrace(const char* header);
std::string BuildStackTrace(const char* header, PCONTEXT threadCtx);
