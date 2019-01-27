@echo off 
for /r %%i in (*.frag, *.vert) do C:/VulkanSDK/1.1.85.0/Bin32/glslangValidator.exe -V %%i
pause