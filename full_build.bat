@echo off
setlocal

@REM set "ROOT=%USERPROFILE%\.conan2"
@REM set "TARGET=windeployqt.exe"

@REM echo [0/6] Searching for %TARGET% in %ROOT% ...

@REM set "WINDEPLOYQT="

@REM for /r "%ROOT%" %%f in (*%TARGET%) do (
@REM     if /i "%%~nxf"=="%TARGET%" (
@REM         set "WINDEPLOYQT=%%~f"
@REM         goto :found
@REM     )
@REM )

@REM echo [ERROR] %TARGET% not found in %ROOT%
@REM exit /b 1

@REM :found
@REM echo Found windeployqt: %WINDEPLOYQT%
echo.

set "WINDEPLOYQT=C:\Users\USER1\.conan2\p\b\qt28524c8cef4eb\b\build\Release\qtbase\bin\windeployqt.exe"

echo [1/6] Installing Python dependencies...
poetry install --no-root || goto :error

echo [2/6] Installing Conan dependencies...
poetry run conan install . -pr:h=profiles/windows_msvc17.txt -pr:b=profiles/windows_msvc17.txt -s build_type=Release --build=missing || goto :error

echo [3/6] Generating CMake project...
cmake -S . -B build/Release -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release || goto :error

echo [4/6] Building project...
cmake --build build/Release --config Release -j || goto :error

echo [5/6] Deploying Qt libraries...
"%WINDEPLOYQT%" build\Release\src\Release\nibbles.exe --dir build\Release\src\Release --release || goto :error

echo [6/6] Done!
echo === BUILD SUCCESSFUL ===
exit /b 0

:error
echo === BUILD FAILED ===
exit /b 1




