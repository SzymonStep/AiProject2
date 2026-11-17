@echo off
REM ===========================================================================
REM  build.bat  --  One-click build for Windows, no make required.
REM
REM  Project : CodeQuest
REM  Author  : Szymon Stepniak
REM
REM  Uses the compiler that ships with the raylib Windows installer, so the
REM  only prerequisite is raylib itself. Set RAYLIB_DIR if you installed it
REM  somewhere other than C:\raylib.
REM
REM    build.bat          build only
REM    build.bat run      build then launch
REM ===========================================================================

if "%RAYLIB_DIR%"=="" set RAYLIB_DIR=C:\raylib

set CC=%RAYLIB_DIR%\w64devkit\bin\gcc.exe
set INC=%RAYLIB_DIR%\w64devkit\x86_64-w64-mingw32\include
set LIB=%RAYLIB_DIR%\w64devkit\x86_64-w64-mingw32\lib

if not exist "%CC%" (
    echo.
    echo  ERROR: could not find the raylib compiler at:
    echo    %CC%
    echo.
    echo  Install raylib for Windows, or set RAYLIB_DIR to where it lives:
    echo    set RAYLIB_DIR=D:\path\to\raylib
    echo.
    exit /b 1
)

if not exist build mkdir build

echo Building CodeQuest...
"%CC%" src\*.c -o build\CodeQuest.exe ^
    -std=c99 -Wall -Wextra -O2 -Isrc -I"%INC%" ^
    -L"%LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

if errorlevel 1 (
    echo.
    echo  Build FAILED.
    exit /b 1
)

echo Build OK  ^-^>  build\CodeQuest.exe

if /I "%1"=="run" (
    echo Launching...
    start "" build\CodeQuest.exe
)
