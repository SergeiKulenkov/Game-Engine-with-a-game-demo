@echo off
pushd %~dp0\..\
call Vendor\premake\bin\premake5.exe --file=Build.lua vs2026
popd
pause