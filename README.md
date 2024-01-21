# x86utm operating system

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line

The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 

**When simulating termination analyzer H correctly simulates its input D until H correctly determines that its simulated D cannot possibly reach its own simulated final state in any finite number of steps then H is necessarily correct to abort its simulation of D and reject this input as non-halting.**
```
01 int D(int (*x)()) 
02 {
03   int Halt_Status = H(x, x); 
04   if (Halt_Status)   
05     HERE: goto HERE; 
06   return Halt_Status; 
07 }
08  
09 void main()  
10 {  
11  H(D,D);  
12 }
```
**Ordinary software engineering conclusively proves that D correctly simulated by H cannot possibly
reach its own simulated return instruction and terminate normally.**

**Execution Trace**<br>
Line 11: main() invokes H(D,D);

**keeps repeating (unless aborted)**<br>
Line 03: simulated D(D) invokes simulated H(D,D) that simulates D(D)

**Simulation invariant:**<br>
D correctly simulated by H cannot possibly reach past its own line 03.

D correctly simulated by H cannot possibly reach its simulated final state in 1 to ∞ steps of correct simulation. 

Simulating termination analyzer H correctly predicts that its simlated D(D) would never stop running unless H aborts its simulation of D. It does this by recognizing a behavior pattern that is very similar to infinite recursion. 

When D calls H to simulate itself this comparable to calling H to call itself and can result in something like infinite recursion. Because there are no control flow instructions in D to stop this the recursive simulation continues until H aborts it. 

When the simulation of D is aborted this is comparable to a divide by zero error thus is not construed as D halting. 

**Compiles with Microsoft Visual Studio Community Edition 2017**
