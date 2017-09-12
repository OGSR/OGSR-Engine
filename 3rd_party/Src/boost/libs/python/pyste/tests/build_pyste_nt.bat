@echo off
setlocal
set MODULE_NAME=%1
set PYSTE_FILE=%2
set BOOST_ROOT=d:/programming/libraries/boost-cvs
set PYTHON_ROOT=c:/python
set STLPORT_ROOT=d:/programming/libraries/stlport-4.5.3
set PYSTE_FILE_DIR=%@PATH[%PYSTE_FILE]

python ../src/pyste.py -I%PYSTE_FILE_DIR --out=%MODULE_NAME.cpp --module=%MODULE_NAME %PYSTE_FILE

icl /nologo /LD /GR /GX -I%PYSTE_FILE_DIR -I%STLPORT_ROOT/stlport -I%BOOST_ROOT/boost -I%PYTHON_ROOT/include %MODULE_NAME.cpp /link /libpath:%PYTHON_ROOT/libs /libpath:%BOOST_ROOT/lib /libpath:%STLPORT_ROOT/lib boost_python.lib

rm %MODULE_NAME.cpp
rm %MODULE_NAME.exp
rm %MODULE_NAME.lib
rm %MODULE_NAME.obj

endlocal
