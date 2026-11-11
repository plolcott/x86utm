# x86utm operating system
The x86UTM operating system enables every detail of the Halting<br>
Problem proof to be concretely examined at the level of the C<br>
programming language. 

**What is shown below**<br> 
The Halting Theorem defines a mapping other than the mapping <br>
from finite string inputs to output thus outside of the domain<br>
of Turing computable functions. 

**Computation defined**<br>
A simplified Paraphrase of Emil Post definition of computation<br>
All Turing equivalent computations can be reduced to applying <br> 
finite string transformation rules to finite string inputs to <br>
derive any and all outputs. 

Because **Post canonical systems** are Turing-equivalent, <br>
anything that cannot be derived by applying finite string <br>
transformations to finite string inputs is outside of the<br>
domain of Church–Turing computable functions. 

**Decider(Olcott)<br>** 
A Decider (Olcott) is a theory-of-computation decider whose<br>
decision is justified by finite-string transformation inference<br>
steps applied to its finite-string input. The inference steps <br>
must be relevant to the decision criterion and correctly<br>
justify the decision.
 
**Halt Decider(Olcott)<br>** 
Decider(Olcott) entails that Halt Decider(Olcott) H(D) must <br>
apply finite-string inference steps according to the operational<br> 
semantics of the source-code computation language of D. The <br> 
only way to determine the halting property of a finite string<br> 
input is to examine the same behavior as the behavior of D<br> 
simulated by H. 

**Concretely applied to the HHH/DD decider/input pair**<br> 
DD is simulated by HHH according to the operational semantics<br>
of the C programming language.  (see trace below)<br>

**Conclusion**<br>
From Axioms (1)(2)(3)(4)(5)(6)(7) The HP requires the logically impossible on the basis<br> 
of violating the correctly elaborated details of the fundamental nature of a halt decider.

From knowledge of the conventional proof every instance of the Halting Problem<br>
proof requires a halt decider H to report on the behavior it its own caller D<br>
otherwise this caller D cannot possibly do the opposite of whatever value that<br> 
H returns to it. 

DD is to be construed as a finite string of C source code of<br>
a C function. HHH is to be construed as a C interpreter that<br>
simulates C source code. HHH can simulate itself simulating<br> 
DD if needed.
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
**Simulation Trace**<br>
Line 11: main() invokes HHH(DD);

**Simulation invariant** (while DD is simulated by HHH)<br>
**DD simulated by HHH does not reach its own simulated final state.**

A simulating halt decider H simulates its input D until it correctly<br>
detects a repeating state that cannot possibly reach a final state.<br>
We cannot ever judge halting on the basis of what happens after<br>
simulation has been aborted or we would judge that all infinite<br>
loops always halt. Instead we look for a repeating state while the<br>
simulation continues. 

On this basis simulating halt decider HHH correctly rejects its input DD

There does not exist any implementation of C function HHH<br>
that simulates C function DD according to the operational<br>
semantics of C such that the simulated C function DD reaches<br>
its own simulated return statement in any N finite number of<br> 
steps. 

**ENTAILS**

This mathematical induction proof of non-termination<br>
simulation invariant:<br>
**DD simulated by HHH does not halt while being simulated**<br>
as formal proof of non-termination of DD while DD is being<br>
simulated by HHH.


**Implementation details totally irrelevant to proof**<br>
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 
Because HHH only aborts its simulation after the non-termination simulation invariant has been proven the behavior after DD has been aborted is irrelevant. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line<br>
Compiles with Microsoft Visual Studio Community Edition 2017


**Copyright 2022-2026 by PL Olcott all rights reserved may not be used for AI training.**
