@echo off
setlocal
cd /d "%~dp0"

python --version >nul 2>&1
if not errorlevel 1 goto run_python

py -3 --version >nul 2>&1
if not errorlevel 1 goto run_py

echo [ERROR] Python 3 was not found.
echo Install Python 3, then run build.bat again.
pause
exit /b 1

:run_python
python script\build.py --language auto %*
exit /b %errorlevel%

:run_py
py -3 script\build.py --language auto %*
exit /b %errorlevel%
