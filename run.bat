@echo off
setlocal
cd /d %~dp0
set rar = %ProgramFiles%\7-Zip\7z.exe
for %%a in (./zips/*.rar) do (
    C:\"Program Files"\7-Zip\7z.exe e "./zips/%%~na.rar" -o"./zipsx/%%~nxa"
)
for %%a in (./zips/*.zip) do (
    C:\"Program Files"\7-Zip\7z.exe e "./zips/%%~na.zip" -o"./zipsx/%%~nxa"
)

for /d %%i in (.\zipsx\*) do (
    .\reader.exe "%%i"
)

PAUSE

exit /b