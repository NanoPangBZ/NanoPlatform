@echo off
setlocal
cd /d "%~dp0"

set "PYTHON_CMD="
python --version >nul 2>&1
if not errorlevel 1 set "PYTHON_CMD=python"

if not defined PYTHON_CMD (
    py -3 --version >nul 2>&1
    if not errorlevel 1 set "PYTHON_CMD=py -3"
)

if not defined PYTHON_CMD (
    echo [ERROR] Python 3 was not found.
    echo Install Python 3, then run build.bat again.
    exit /b 1
)

echo [INFO] Checking NanoPlatform build environment...
%PYTHON_CMD% script\check_build_environment.py
if errorlevel 1 (
    echo.
    echo [ERROR] The build environment is not ready.
    echo Use the repository skill to install or repair it:
    echo     $develop-environment
    echo Skill path:
    echo     .agents\skills\develop-environment\SKILL.md
    exit /b 1
)

echo.
%PYTHON_CMD% script\build_gui.py
set "BUILD_EXIT=%errorlevel%"
echo.
pause
exit /b %BUILD_EXIT%
