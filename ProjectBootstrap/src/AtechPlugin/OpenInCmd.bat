@echo off
setlocal EnableExtensions
pushd "%~dp0" >nul 2>&1 || exit /b 1
set "OUT_DLL=%CD%\Atech.dll"
set "OBJ_DIR=%TEMP%\AtechBuild"

if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%" >nul 2>&1

where g++.exe >nul 2>&1
if not errorlevel 1 (
    echo Building DLL with MinGW g++...

    g++ -O2 -Wall -I. -c Atechplugin.cpp -o "%OBJ_DIR%\Atechplugin.o"
    if errorlevel 1 exit /b 1

    g++ -O2 -Wall -I. -c ATFM.cpp -o "%OBJ_DIR%\ATFM.o"
    if errorlevel 1 exit /b 1

    g++ -O2 -Wall -I. -c CDMlinker.cpp -o "%OBJ_DIR%\CDMlinker.o"
    if errorlevel 1 exit /b 1

    g++ -shared -o "%OUT_DLL%" "%OBJ_DIR%\Atechplugin.o" "%OBJ_DIR%\ATFM.o" "%OBJ_DIR%\CDMlinker.o" "%CD%\EuroScopeStub.cpp"
    if errorlevel 1 exit /b 1

    goto :done
)

where cl.exe >nul 2>&1
if not errorlevel 1 (
    echo Building DLL with Microsoft C/C++ compiler...
    cl /nologo /EHsc /LD /Fe"%OUT_DLL%" /I. *.cpp /link /NOLOGO /DLL /OUT:"%OUT_DLL%"
    goto :done
)

echo No supported C/C++ compiler was found.
echo Install Visual Studio Build Tools or MinGW and try again.
exit /b 1

:done
popd >nul 2>&1
if exist "%OUT_DLL%" (
    echo DLL created successfully: %OUT_DLL%
) else (
    echo DLL build failed.
    exit /b 1
)
