# x86utm operating system
The x86UTM operating system enables every detail of the Halting Problem<br> 
proof to be concretely examined at the level of the C programming language. 

**<h3>Succinct proof that the Halting Problem Theorem is incorrect**</h3>
**All axioms refer to the simulation trace shown below**

**Axiom 1 --- Paraphrase of Emil Post definition of computation** <br>
All computations are equivalent to the application of finite string<br>
transformation rules to finite string inputs to derive any and all<br>
outputs. 

The rest of this proof will be restricted to the above model of<br>
computation thus **All computations are only allowed to apply finite<br>
string transformations to their finite string inputs**

**Axiom 2**<br>
**The brand new key insight that I had into the foundations of<br> 
computation is the notion of {correct finite string transformations} as<br> 
opposed to and contrast with {arbitrary finite string transformations}.**

If we need to know the sum of 2 + 3 there seems to be only a single<br>
sequence of finite string transformation rules that derive this within<br>
arithmetic operations on the integer type. This is one concrete example<br>
of how computation is limited to correct finite string transformations.<br>




The only correct finite string transformation rules that a halt<br>
decider H is allowed to apply to its finite string input D are<br>
specified strictly by the operational semantics of the source<br>
language of the computation. This applies universally to all<br>
models of computation, including the source code of Turing<br> 
Machines. 

**Concretely applied to the HHH/DD decider/input pair**<br> 
The only correct finite string transformation rules that HHH is<br> 
allowed to apply to its finite string input DD is specified by the<br> 
operational semantics of the C programming language. This<br> 
requires DD to be simulated by HHH according to these rules.<br> 

**HHH cannot correctly guess that DD will ignore its own line<br>
03 and leap to its own line 06**<br>


**Axiom 3**<br>
This single unique correct simulation trace derived from (2) can be seen<br>
below. This trace excludes behavior equivalent to DD() directly executed<br> 
from main(). HHH cannot report on the behavior of its caller DD().

**There cannot possibly be any input to a halt decider that is also its<br> 
caller these are always mutually exclusive.**

**Axiom 4**<br>
DD correctly simulated by HHH cannot possibly reach its own simulated<br>
final halt state is determined by the simulation trace shown below.

**Axiom 5**<br>
From Axioms (1)(2)(3)(4) HHH is correct to reject its input as non-halting<br> 
and we know that it is incorrect to require HHH to report on the behavior of<br> 
its own caller DD(). 

**Axiom 6**<br>
From Axioms (1)(2)(3)(4)(5) The HP requires the logically impossible on the basis<br> 
of violating the correctly elaborated details of the fundamental nature of a halt decider.

**Extra Axiom 7**<br> 
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
**Execution Trace**<br>
Line 11: main() invokes HHH(DD);

**keeps repeating unless aborted**<br>
Line 03: simulated DD() invokes simulated HHH(DD) that simulates DD()

**Simulation invariant**<br>
DD correctly simulated by HHH cannot possibly reach past its own line 03<br> 
(whether or not HHH ever aborts this simulation) thus<br> 
**DD correctly simulated by HHH cannot possibly reach its simulated final state.**

**Implementation details totally irrelvant to proof**<br>
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line<br>
Compiles with Microsoft Visual Studio Community Edition 2017
