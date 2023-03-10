@echo off
set output=%1
set vulkPath=%2

echo Compiling shaders...

set /A N=0
if not exist %output% mkdir %output%
if not exist %output%\shaders mkdir %output%\shaders

for /f %%f IN ('dir /b *.vert') do (
    start %vulkPath%\Bin\glslc.exe %%f -o %output%\%%~nf.spv
    set /a N+=1
)

for /f %%f IN ('dir /b *.frag') do (
    start %vulkPath%\Bin\glslc.exe %%f -o %output%\%%~nf.spv
    set /a N+=1
)

echo %N% shaders compiled
