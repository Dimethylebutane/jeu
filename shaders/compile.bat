@echo off
set output=%1

echo Compiling shaders...

set /A N=0
if not exist %output% mkdir %output%
if not exist %output%\shaders mkdir %output%\shaders

for /f %%f IN ('dir /b *.vert') do (
    start C:\lib\Vulkan_sdk\Bin\glslc.exe %%f -o %output%\%%~nf.spv
    set /a N+=1
)

for /f %%f IN ('dir /b *.frag') do (
    start C:\lib\Vulkan_sdk\Bin\glslc.exe %%f -o %output%\%%~nf.spv
    set /a N+=1
)

echo %N% shaders compiled
