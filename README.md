# x86utm operating system

The x86UTM operating system enables the every detail of the Halting Problem 
proof to be concretely examined at the level of the C programming language. 

The key mistake of the halting theorem is that it requires a halt decider 
to report on the behavior of something other than the behavior specified 
by its finite string input.

This is outside of the Church-Turing scope when this scope is understood to
only allow finite string transformation rules to be applied to finite string
inputs to derive any any all outputs.
```
// paraphrase of Emil Post
All computations are essentially the application
of finite string transformation rules to finite
string inputs to derive any and all outputs.
```
The only correct finite string transformation rules that HHH is allowed to
apply to its finite string input DD are specified by the operational semantics 
of the C programming language. This is a single unique sequence making every
other sequence incorrect. 

```
<MIT Professor Sipser agreed to ONLY these verbatim words 10/13/2022>
    If simulating halt decider H correctly simulates its 
    input D until H correctly determines that its simulated D 
    would never stop running unless aborted then <br><br>

    H can abort its simulation of D and correctly report that D 
    specifies a non-halting sequence of configurations. 
<MIT Professor Sipser agreed to ONLY these verbatim words 10/13/2022>
```
The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 
When we simply examine the trace of DD correctly simulated by HHH in C the issue becomes clear. HHH simulates DD that calls HHH(DD) to repeat 
this process continually.

```
typedef int (*ptr)();
int HHH(ptr P); 

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
**Correct simulation is defined as DD simulated by HHH according to the (operational) semantics of the C programming language.**

**Ordinary software engineering conclusively proves that DD correctly simulated by HHH cannot possibly reach its own simulated return instruction and terminate normally.**

**Execution Trace**<br>
Line 11: main() invokes HHH(DD);

**keeps repeating (unless aborted)**<br>
Line 03: simulated DD() invokes simulated HHH(DD) that simulates DD()

**Simulation invariant:**<br>
DD correctly simulated by HHH cannot possibly reach past its own line 03.

**DD correctly simulated by HHH cannot possibly reach its simulated final state in 1 to ∞ steps of correct simulation.**

Simulating termination analyzer HHH correctly predicts that its simulated DD would never stop running unless HHH aborts its simulation of DD. It does this by recognizing a behavior pattern that is very similar to infinite recursion. 

When DD calls HHH to simulate itself this comparable to calling HHH to call itself and can result in something like infinite recursion. Because there are no control flow instructions in DD to stop this the recursive simulation continues until HHH aborts it. 

When the simulation of DD is aborted this is comparable to a divide by zero error thus is not construed as DD halting. 

**Implementation details**
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line
Compiles with Microsoft Visual Studio Community Edition 2017
