@echo off
setlocal

cd /d "%~dp0"

set "CONFIGURATION_GA=ReleaseTracyProfiler"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo vswhere.exe not found. Install VS 2022 with the Desktop C++ workload.
    exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
if not defined MSBUILD (
    echo MSBuild.exe not found.
    exit /b 1
)

echo Building Engine.sln Release^|x64 with CONFIGURATION_GA=%CONFIGURATION_GA%
echo Using "%MSBUILD%"
echo.

"%MSBUILD%" Engine.sln /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m
set "ERR=%ERRORLEVEL%"

echo.
if %ERR% neq 0 (
    echo Build failed. Errorlevel %ERR%.
    exit /b %ERR%
)

echo Build succeeded. Output: "%cd%\bin_x64"
echo CONFIGURATION_GA was only set for this script.
exit /b 0
