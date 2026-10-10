# x86utm operating system
**<h3>Overview of what will be shown below</h3>**<br>
The x86UTM operating system enables every detail of the Halting Problem<br> 
proof to be concretely examined at the level of the C programming language. 

The key mistake of the halting problem proofs is that they require a halt<br> 
decider to report on the behavior of something other than the behavior specified<br>
by its finite string input.

This is outside of the Church-Turing scope when this scope is understood to<br>
only allow finite string transformation rules to be applied to finite string<br>
inputs to derive any any all outputs.

**Succinct proof that the Halting Problem proofs are incorrect**

**Axiom 1**  ---  **Paraphrase of Emil Post**<br>
All computations are essentially the application of finite string<br>
transformation rules to finite string inputs to derive any and all<br>
outputs. All computations are only allowed to apply finite string<br>
transformations to their inputs.<br>

**Axiom 2**<br>
The only correct finite string transformation rules that HHH is allowed to<br>
apply to its finite string input DD is specified by the operational semantics<br> 
of the C programming language. 

**Axiom 3***<br>
This is a single unique sequence making every other sequence incorrect. 

**Axiom 4**<br>
This sequence does not derive the behavior of DD() executed from main(). <br>
HHH cannot report on the behavior of its caller DD(). 

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
Ordinary software engineering conclusively proves that DD correctly simulated by HHH cannot possibly reach its own simulated return instruction and terminate normally.

**Execution Trace**<br>
Line 11: main() invokes HHH(DD);

**keeps repeating (unless aborted)**<br>
Line 03: simulated DD() invokes simulated HHH(DD) that simulates DD()

**Axiom 5** **Simulation invariant:**<br>
DD correctly simulated by HHH cannot possibly reach past its own line 03 whether or not HHH ever aborts this simulation. 

**Axiom 6**
**DD correctly simulated by HHH cannot possibly reach its simulated final state.**

**Implementation details**
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line
Compiles with Microsoft Visual Studio Community Edition 2017
