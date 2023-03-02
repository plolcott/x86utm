echo on
rd /Q /S .vs\x86utm\v15\ipch
rd /Q /S Debug
rd /Q /S demo\Debug
del .vs\x86utm\v15\*.db
rd /Q /S x64
rd /Q /S Debug
rd /Q /S Release
del x86utm.exe
dir /s
pause 
