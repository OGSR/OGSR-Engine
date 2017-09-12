@echo off
call build_pyste_nt basic ../example/basic.pyste
call build_pyste_nt enums ../example/enums.pyste
call build_pyste_nt header_test ../example/header_test.pyste
call build_pyste_nt nested ../example/nested.pyste
call build_pyste_nt operators ../example/operators.pyste
call build_pyste_nt templates ../example/templates.pyste
call build_pyste_nt virtual ../example/virtual.pyste
call build_pyste_nt wrappertest ../example/wrappertest.pyste
call build_pyste_nt unions ../example/unions.pyste


runtests.py

if errorlevel != 0 goto end

rm *.dll
rm *.pyc

:end
