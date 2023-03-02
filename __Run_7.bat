echo off 
SET PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin
echo on

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.BAT" 

REM cl /EHsc Parse_Function_Headers.cpp
REM if ERRORLEVEL 1 pause
REM Parse_Function_Headers Halt7.c > Halt7out.txt
REM type Halt7out.txt
REM if ERRORLEVEL 1 pause 
REM if ERRORLEVEL 1 exit 


REM cl.exe / O2 /EHsc x86utm.cpp api.c decode.c mem.c ops.c ops2.c prim_ops.c
cl.exe /EHsc x86utm.cpp api.c decode.c mem.c ops.c ops2.c prim_ops.c

if ERRORLEVEL 1 pause
del *.obj 

REM cl /FaHalt7.asm  /c /arch:IA32 /EHsc Halt7.c 
REM cl /FaHalt7.asm  /c /arch:IA32 Halt7.c 
REM cl     /GS-      /c /arch:IA32 Halt7.c 
REM cl  /GS- /FaHalt7.asm  /c /arch:IA32 Halt7.c 
REM cl  /GS- /FAcu /FaHalt7.lst  /c /arch:IA32 Halt7.c 
cl  /GS- /FA /FaHalt7.asm  /c /arch:IA32 Halt7.c 
if ERRORLEVEL 1 pause

REM ml -c Halt7.asm
REM pause

REM x86utm  --32 -l 0 -s 0xB4  Halt7a.bin > Halt7aout.txt
REM x86utm  --32 -l 0 -s 0x6db Halt7.bin > Halt7out.txt
REM x86utm  --32 -l 0 -s 0x6db Halt7.obj > Halt7out.txt

REM x86utm  -b 0x1ef -s 0x508 -r 0x6df -m 100000 Halt7.obj > Halt7out.txt
x86utm Halt7.obj > Halt7out.txt
pause 