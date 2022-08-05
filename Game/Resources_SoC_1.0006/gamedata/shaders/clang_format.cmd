
for /r %%i in (*.h *.ds *.hs *.cs *.gs *.ps *.vs) do clang-format.exe -style=file -i "%%i"

pause