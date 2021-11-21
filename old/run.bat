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

dir .\maps\dt /b |^
findstr .osz > temp.txt
set /p dt=<temp.txt

dir .\maps\freemod /b |^
findstr .osz > temp.txt
set /p fm=<temp.txt

dir .\maps\hd /b |^
findstr .osz > temp.txt
set /p hd=<temp.txt

dir .\maps\hr /b |^
findstr .osz > temp.txt
set /p hr=<temp.txt

dir .\maps\nomod /b |^
findstr .osz > temp.txt
set /p nm=<temp.txt

echo %dt%
echo %fm%
echo %hd%
echo %hr%
echo %nm%

for /d %%i in (.\zipsx\*) do (
    .\reader.exe "%%i" %dt%:::%fm%:::%hd%:::%hr%:::%nm%:::
)

PAUSE

exit /b