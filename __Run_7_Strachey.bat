echo off 
SET PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin
echo on

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.BAT" 

cl.exe /EHsc x86utm.cpp api.c decode.c mem.c ops.c ops2.c prim_ops.c

if ERRORLEVEL 1 pause
del *.obj 

cl  /GS- /c /arch:IA32 Halt7_Strachey.c
if ERRORLEVEL 1 pause

x86utm Halt7_Strachey.obj > Halt7_Strachey.txt
pause 