@echo off


setlocal
set pdir=%~dp0%

rem call "%pdir%generate_compile_commands.bat"
call "E:\Windows Development Tools\Visual Studio\VC\Auxiliary\Build\vcvarsall.bat" x64
set glfw_lp=%pdir%libs\glfw\lib-vc2022
set freetype_lp=%pdir%libs\freetype\release_static\vs2015-2022\win64

set cflags=/std:c++20 /W4 /Zi /EHsc /MP4 /nologo /Fe:edex.exe
set cflags=%cflags% /wd4100 /wd4189 /wd4996 /wd4267 /wd4201
rem set cflags=%cflags% /EP
set cflags=%cflags% /DDEBUG_MEMLEAK
set cflags=%cflags% /D_DEBUG
set cflags=%cflags% /MTd
set cflags=%cflags% /I %pdir%libs\glfw\include
set cflags=%cflags% /I %pdir%libs\freetype\include
set cflags=%cflags% /I %pdir%libs\glad

set lflags=/link glfw3_mt.lib opengl32.lib freetype.lib shell32.lib gdi32.lib user32.lib kernel32.lib
set lflags=%lflags% /LIBPATH:%glfw_lp%
set lflags=%lflags% /LIBPATH:%freetype_lp%
set lflags=%lflags% /ignore:4099

set cfiles=%pdir%libs\glad\glad.c
pushd "code"
for /R %%i in (*.cpp) do call set cfiles=%%i %%cfiles%%
popd

if exist "%pdir%bin" rmdir /S /Q %pdir%bin
if not exist "%pdir%bin" mkdir %pdir%bin
pushd "%pdir%bin"
cl %cflags% %cfiles% %lflags%
popd

rem %pdir%bin\edex.exe

endlocal
