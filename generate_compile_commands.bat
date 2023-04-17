@ECHO OFF

SETLOCAL EnableDelayedExpansion

set pdir=%~dp0%
set glfw_lp=%pdir%libs\glfw\lib-mingw-w64
set freetype_lp=%pdir%libs\freetype\release_static\vs2015-2022\win64

set CC=clang
set CFLAGS=
set CFLAGS=%CFLAGS% -Wno-unused-parameter -Wno-switch
set CFLAGS=%CFLAGS% -Wno-unused-function
set CFLAGS=%CFLAGS% -o edex.exe
set CFLAGS=%CFLAGS% -I %pdir%libs\glad
set CFLAGS=%CFLAGS% -I %pdir%libs\glfw\include
set CFLAGS=%CFLAGS% -I %pdir%libs\freetype\include
set CFLAGS=%CFLAGS% -mwindows
set CFLAGS=%CFLAGS% -MJ %pdir%compile_commands.json

set LFLAGS=-lopengl32
set LFLAGS=%LFLAGS% -lshell32
set LFLAGS=%LFLAGS% -lgdi32
set LFLAGS=%LFLAGS% -luser32
set LFLAGS=%LFLAGS% -lkernel32
set LFLAGS=%LFLAGS% -lglfw3
set LFLAGS=%LFLAGS% -lfreetype

set LFLAGS=%LFLAGS% -L %glfw_lp%
set LFLAGS=%LFLAGS% -L %freetype_lp%

set CFILES=%pdir%\libs\glad\glad.c
pushd "code"
for /R %%c in (*.cpp) do call set CFILES=%%c %%CFILES%%
popd

if not exist "bin" mkdir bin

pushd "bin"
%CC% %CFLAGS% %CFILES% %LFLAGS% 2>NUL
popd

set cp=
for /f %%i in ('echo [') do call set cp=%%i
for /f "Tokens=* Delims=" %%i in ('type compile_commands.json') do call set cp=%%cp%% %%i
for /f %%i in ('echo ]') do call set cp=%%cp%% %%i

echo %cp% > %pdir%compile_commands.json

ENDLOCAL
