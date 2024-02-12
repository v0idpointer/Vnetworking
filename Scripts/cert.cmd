@echo off
powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\cert.ps1 %*
exit /b %ERRORLEVEL%