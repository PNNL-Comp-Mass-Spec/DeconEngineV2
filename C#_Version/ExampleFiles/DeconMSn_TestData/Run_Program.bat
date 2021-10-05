@echo off

set ExePath=DeconMSn.exe 

if exist %ExePath% goto DoWork
if exist ..\%ExePath% set ExePath=..\%ExePath% && goto DoWork

if exist ..\..\DeconMSn\bin\Release\%ExePath% set ExePath=..\..\DeconMSn\bin\Release\%ExePath% && goto DoWork
if exist ..\..\DeconMSn\bin\Debug\%ExePath% set ExePath=..\..\DeconMSn\bin\Debug\%ExePath% && goto DoWork

echo Executable not found: %ExePath%
goto Done

:DoWork
echo.
echo Procesing with %ExePath%
echo.

%ExePath% -F500 -L1000 -XCDTA QC_Shew_19_01_top3_500ng-run1_11Apr19_Brandi_Thermo-10408462.raw

:Done

pause
