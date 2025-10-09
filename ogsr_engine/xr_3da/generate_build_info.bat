@echo off
type nul > ogsr_build_info.hpp
for /f %%a in ('powershell -NoProfile -Command "Get-Date -Format dd.MM.yyyy_HH:mm:ss"') do set datetime=%%a
set BUILD_DATE=%datetime:~0,10%
set BUILD_TIME=%datetime:~11%
set BUILD_YEAR=%datetime:~6,4%
::echo #pragma once> ogsr_build_info.hpp
echo #define BUILD_DATE "%BUILD_DATE%">> ogsr_build_info.hpp
echo #define BUILD_TIME "%BUILD_TIME%">> ogsr_build_info.hpp
echo #define BUILD_YEAR "%BUILD_YEAR%">> ogsr_build_info.hpp
IF DEFINED APPVEYOR_BUILD_VERSION (
echo #define APPVEYOR_BUILD_VERSION "%APPVEYOR_BUILD_VERSION%">> ogsr_build_info.hpp
)
IF DEFINED APPVEYOR_REPO_NAME (
echo #define APPVEYOR_REPO_NAME "%APPVEYOR_REPO_NAME%">> ogsr_build_info.hpp
)
IF DEFINED CONFIGURATION_GA (
echo #define CONFIGURATION_GA "%CONFIGURATION_GA%">> ogsr_build_info.hpp
)