@echo off
setlocal enabledelayedexpansion

REM ==============================================
REM Visual Studio Version Detection Script
REM Detects VS version and sets VS_VERSION (vs2026/vs2022) and VS_PATH variables
REM No temporary files used
REM ==============================================

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_VERSION=vs2022"
set "VS_PATH="
set "MAJOR_VERSION=0"

REM echo ==============================================
REM echo Starting Visual Studio version detection...
REM echo ==============================================

if exist "%VSWHERE%" (
    REM echo [INFO] vswhere tool found
    
    REM Get latest VS installation version and path
    for /f "delims=" %%v in ('"%VSWHERE%" -nologo -latest -products * -property installationVersion 2^>nul') do (
        set "RAW_VERSION=%%v"
        REM Parse major version
        for /f "delims=." %%m in ("!RAW_VERSION!") do (
            set "MAJOR_VERSION=%%m"
        )
    )
    
    REM Get installation path
    for /f "delims=" %%p in ('"%VSWHERE%" -nologo -latest -products * -property installationPath 2^>nul') do (
        set "VS_PATH=%%p"
    )
    
    REM Determine VS version based on major version
    if !MAJOR_VERSION! GEQ 18 (
        set "VS_VERSION=vs2026"
        REM echo [INFO] Detected VS 2026 (version !RAW_VERSION!^)
    ) else if !MAJOR_VERSION! GEQ 17 (
        set "VS_VERSION=vs2022"
        REM echo [INFO] Detected VS 2022 (version !RAW_VERSION!^)
    ) else (
        REM echo [WARNING] Unknown VS version: !RAW_VERSION!
    )
    
    if defined VS_PATH (
        REM echo [INFO] Installation path: !VS_PATH!
    )
) else (
    echo [WARNING] vswhere tool not found, trying registry detection...
    
    REM Fallback: registry detection for VS 2026
    reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\18.0" /ve 2>nul >nul
    if not errorlevel 1 (
        set "VS_VERSION=vs2026"
        set "MAJOR_VERSION=18"
        REM echo [INFO] VS 2026 detected via registry
        goto :detected
    )
    
    reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\18.0" /ve 2>nul >nul
    if not errorlevel 1 (
        set "VS_VERSION=vs2026"
        set "MAJOR_VERSION=18"
        REM echo [INFO] VS 2026 detected via registry
        goto :detected
    )
    
    REM Fallback: registry detection for VS 2022
    reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\17.0" /ve 2>nul >nul
    if not errorlevel 1 (
        set "VS_VERSION=vs2022"
        set "MAJOR_VERSION=17"
        REM echo [INFO] VS 2022 detected via registry
        goto :detected
    )
)

:detected
REM echo ==============================================
REM echo Detection complete!
REM echo ==============================================

REM End local environment but preserve variables in current session
endlocal & set "VS_VERSION=%VS_VERSION%" & set "VS_PATH=%VS_PATH%" & set "VS_MAJOR_VERSION=%MAJOR_VERSION%"

REM Output final result
echo.
echo ==============================================
echo Visual Studio:
echo   VS_VERSION        = %VS_VERSION%
echo   VS_MAJOR_VERSION  = %VS_MAJOR_VERSION%
echo   VS_PATH           = %VS_PATH%
echo ==============================================
