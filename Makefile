Halt7.o : Halt7.c    
	gcc -c -m32 -masm=intel  -fno-stack-protector Halt7.c    -o Halt7.o
x86utm_exe : x86utm.o mem.o decode.o ops.o api.o ops2.o prim_ops.o 
	g++ -o x86utm_exe -m32 x86utm.o mem.o decode.o ops.o api.o ops2.o prim_ops.o 
x86utm.o : x86utm.cpp 
	g++ -o x86utm.o -c -m32 -g -O2 -std=gnu++11 x86utm.cpp 
mem.o : mem.c 
	gcc -o mem.o      -c -m32 -g -O2 -Wall mem.c      
decode.o   : decode.c 
	gcc -o decode.o   -c -m32 -g -O2 -Wall decode.c   
ops.o      : ops.c 
	gcc -o ops.o      -c -m32 -g -O2 -Wall ops.c      
api.o      : api.c 
	gcc -o api.o      -c -m32 -g -O2 -Wall api.c      
ops2.o     : ops2.c 
	gcc -o ops2.o     -c -m32 -g -O2 -Wall ops2.c     
prim_ops.o : prim_ops.c 
	gcc -o prim_ops.o -c -m32 -g -O2 -Wall prim_ops.c 
clean :
	rm x86utm_exe *.o 