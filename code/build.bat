@echo off

:: Setup environment for VS 2022
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: SDL2 paths
set SDL2_INCLUDE=C:\SDL2\include
set SDL2_LIB=C:\SDL2\lib\x64

:: Common compiler flags
set CFLAGS=/nologo /Od /EHsc /I"%SDL2_INCLUDE%"

:: Common linker flags
set LFLAGS=/link /SUBSYSTEM:CONSOLE "%SDL2_LIB%\SDL2.lib" "%SDL2_LIB%\SDL2main.lib" user32.lib gdi32.lib Shell32.lib

:: Create build folder
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

echo Compiling clothsim.c...
cl %CFLAGS% ..\code\clothsim.c %LFLAGS% /Fe:clothsim.exe

echo Done. Checking for output...
if exist clothsim.exe (
    echo SUCCESS: clothsim.exe created.
) else (
    echo ERROR: No .exe file generated.
)

popd
