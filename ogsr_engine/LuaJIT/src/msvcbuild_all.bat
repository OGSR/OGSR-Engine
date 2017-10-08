@echo off

if not defined DevEnvDir (
  if defined VS150COMNTOOLS (
    set setup_env=call "%VS150COMNTOOLS%\..\..\VC\vcvarsall.bat"
  ) else (
    if defined VS140COMNTOOLS (
      set setup_env=call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat"
	) else (
      echo "VS150COMNTOOLS and VS140COMNTOOLS is not defined, aborting."
      exit /b 1
	)
  )
)

echo * Building LuaJIT x86...
%setup_env% x86
set LJTARGETARCH=x86
call msvcbuild.bat
if errorlevel 1 goto :fail

echo * Building LuaJIT x64...
%setup_env% x86_amd64
set LJTARGETARCH=x64
call msvcbuild.bat
if errorlevel 1 goto :fail

goto :end
:fail
echo Build failed.
:end
