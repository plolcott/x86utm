# x86utm operating system

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line

The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 
No one understands the x86 language any more so these implementation details can be ignored. When we simply example the execution trace of 
DD by HHH in C the issue becomes clear. HHH simulates DD that calls HHH(DD) to repeat this process continually. For this much of the process
HHH is a pure function of its input. 

**Proof Theoretic Semantics halt prover HHH correctly determines that its input DD is ungrounded in its atomic 
base of the operational semantics of the C programming language.** 

**When proof theoretic semantics (PTS) halt prover HHH correctly simulates its input DD until HHH correctly
determines that its simulated DD cannot possibly reach its own simulated final state in any finite number 
of steps then HHH is necessarily correct to abort its simulation of DD and reject this input as a non-terminating proof.**
```
typedef int (*ptr)();
u32 HHH(ptr P); 

01 int DD() 
02 {
03   int Halt_Status = HHH(DD); 
04   if (Halt_Status)   
05     HERE: goto HERE; 
06   return Halt_Status; 
07 }
08  
09 void main()  
10 {  
11  HHH(DD);  
12 }
```
**Ordinary software engineering conclusively proves that DD correctly simulated by HHH cannot possibly
reach its own simulated return instruction and terminate normally.**

**Execution Trace**<br>
Line 11: main() invokes HHH(DD);

**keeps repeating (unless aborted)**<br>
Line 03: simulated DD() invokes simulated HHH(DD) that simulates DD()

**Simulation invariant:**<br>
DD correctly simulated by HHH cannot possibly reach past its own line 03.

**DD correctly simulated by HHH cannot possibly reach its simulated final state in 1 to ∞ steps of correct simulation.**

Simulating termination analyzer HHH correctly predicts that its simlated DD() would never stop running unless HHH aborts its simulation of DD. It does this by recognizing a behavior pattern that is very similar to infinite recursion. 

When DD calls H to simulate itself this comparable to calling H to call itself and can result in something like infinite recursion. Because there are no control flow instructions in DD to stop this the recursive simulation continues until HHH aborts it. 

When the simulation of DD is aborted this is comparable to a divide by zero error thus is not construed as DD halting. 

**Compiles with Microsoft Visual Studio Community Edition 2017**
