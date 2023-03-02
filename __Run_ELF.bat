echo off 
SET PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin
echo on

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.BAT" 

cl.exe /EHsc Test_ELF.cpp

if ERRORLEVEL 1 pause
del *.obj 

echo outputting now

Test_ELF Halt7.o > Halt7_ELF.txt
type Halt7_ELF.txt 

pause 