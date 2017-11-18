@echo off

for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -products * -property installationPath`) do (
  if exist "%%i\Common7\Tools\vsdevcmd.bat" (
    set setup_env=call "%%i\Common7\Tools\vsdevcmd.bat" -arch=%1
	goto :build
  )
)

if not defined setup_env (
  echo "Сompiler v.141 not found, aborting."
  goto :end
)

:build
echo * Building LuaJIT %1...
%setup_env% %1
set LJTARGETARCH=%1
call msvcbuild.bat
if errorlevel 1 goto :fail

goto :end
:fail
echo Build failed.
:end
