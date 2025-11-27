@echo off
setlocal

set "ROOT=%USERPROFILE%\.conan2"
set "TARGET=windeployqt.exe"

echo [0/4] Searching for %TARGET% in %ROOT% ...

set "WINDEPLOYQT="

for /r "%ROOT%" %%f in (*%TARGET%) do (
    if /i "%%~nxf"=="%TARGET%" (
        set "WINDEPLOYQT=%%~f"
        goto :found
    )
)

echo [ERROR] %TARGET% not found in %ROOT%
exit /b 1

:found
echo Found windeployqt: %WINDEPLOYQT%
echo.

echo [1/4] Generating CMake project...
cmake -S . -B build/Release -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release || goto :error

echo [2/4] Building project...
cmake --build build/Release --config Release -j || goto :error

echo [3/4] Deploying Qt libraries...
"%WINDEPLOYQT%" build\Release\src\Release\nibbles.exe --dir build\Release\src\Release --release || goto :error

echo [4/4] Done!
echo === BUILD SUCCESSFUL ===
exit /b 0

:error
echo === BUILD FAILED ===
exit /b 1