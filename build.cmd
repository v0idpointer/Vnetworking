@echo off
powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\build.ps1 %*
exit /b %ERRORLEVEL%