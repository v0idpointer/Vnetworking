@echo off
powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\clean.ps1 %*
exit /b %ERRORLEVEL%