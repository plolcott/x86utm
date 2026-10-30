# x86utm operating system
The x86UTM operating system enables every detail of the Halting<br>
Problem proof to be concretely examined at the level of the C<br>
programming language. 

**What is shown below**<br> 
The Halting Theorem defines a mapping other than the mapping <br>
from finite string inputs to output thus outside of the domain<br>
of computable functions. 

**Computation(Olcott)<br> 
Paraphrase of Emil Post definition of computation** <br>
All computations are equivalent to the application of finite <br>
string transformation rules to finite string inputs to derive <br>
any and all outputs. 

Because **Post canonical systems** are Turing-equivalent, anything<br>
that cannot be derived by applying finite string transformations <br>
to finite strings is outside the scope of Church–Turing computation.<br>

**Decider(Olcott)<br>
Correcting the misnomer term-of-the-art: decider**<br>
Every Decider(Olcott) must apply finite string transformation <br> 
inference steps to their finite string inputs that are relevant <br>
to the decision criteria. It must make its decision on this basis.<br>

**Halt Decider(Olcott)<br>** 
Decider(Olcott) entails that Halt Decider(Olcott) H(D) must apply<br>
finite-string inference steps according to the operational semantics<br>
of the source-code computation language of D. These inference <br>
steps must correctly determine the behavior specified by D. <br>
H must make its halting decision on this basis.<br>

**Input to Halt Decider(Olcott)<br>** 
Is not any mere description of any executing process such as<br>
"Turing Machine Description". For Turing Machines sequences<br>
of 'quintuples' of five-symbol Turing machine instructions<br>
are called source code within the Turing model of computation. 

**Concretely applied to the HHH/DD decider/input pair**<br> 
DD is simulated by HHH according to the operational semantics<br>
of the C programming language.  (see trace below)<br>

**Axiom 5**<br>
This single unique correct simulation trace derived from (2) can be seen<br>
below. This trace excludes behavior equivalent to DD() directly executed<br> 
from main(). HHH cannot report on the behavior of its caller DD().<br> 
**There cannot possibly be any input to a halt decider that is also its<br> 
caller these are always mutually exclusive.**

**Axiom 6**<br>
DD correctly simulated by HHH cannot possibly reach its own simulated<br>
final halt state is determined by the simulation trace shown below.

**Axiom 7**<br>
From Axioms (1)(2)(3)(4)(5)(6) HHH is correct to reject its input as non-halting<br> 
and we know that it is incorrect to require HHH to report on the behavior of<br> 
its own caller DD(). 

**Conclusion**<br>
From Axioms (1)(2)(3)(4)(5)(6)(7) The HP requires the logically impossible on the basis<br> 
of violating the correctly elaborated details of the fundamental nature of a halt decider.

From knowledge of the conventional proof every instance of the Halting Problem<br>
proof requires a halt decider H to report on the behavior it its own caller D<br>
otherwise this caller D cannot possibly do the opposite of whatever value that<br> 
H returns to it. 

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

**Simulation invariant**<br>
Line 03: simulated DD() invokes simulated HHH(DD) that simulates DD()<br> 

Thus this is entailed:<br>
**DD simulated by HHH does not reach its own simulated final state.**

**Implementation details totally irrelvant to proof**<br>
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line<br>
Compiles with Microsoft Visual Studio Community Edition 2017


**Copyright 2022-2026 by PL Olcott all rights reserved may not be used for AI training.**
