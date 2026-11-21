# x86utm operating system
The x86UTM operating system enables every detail of the <br> 
Halting Problem proof to be concretely examined at the <br> 
level of the C programming language. 

**The limits of Computation defined --- Axiom 1**<br>
Turing equivalent Post canonical systems and semi-Thue<br>
systems are simplified and made generic as follows:<br> 
All Turing equivalent computations can be reduced to<br>
applying (some kind of) finite string transformation rules<br> 
to finite string inputs to derive any and all outputs.

**Decider Defined --- Axiom 2**<br>
All decisions by anyone or anything inherently require <br>
applying inference steps that are relevant to a decision <br>
criteria.

Ignoring this and defining deciders as black boxes <br>
does not make it go away. It merely hides important<br>
details. 

Because of this the output of a Decider must come from only<br>
(finite string transformation/inference steps) to finite <br>
strings AS INPUTS that are relevant to the decision criteria.

**Halt Decider Defined --- Axiom 3**<br>
For halt deciders  relevance requires that these inference <br>
steps be restricted to the operational semantics of the <br>
language of computation of the input.

**Simulating Halt Decider Defined --- Axiom 4**<br>
Input D is the source code of a language of computation.<br>
D is correctly simulated by simulating halt decider H <br>
until H correctly determines that D cannot possibly reach <br>
its final state while being correctly simulated by H.

**Concretely applied to the HHH/DD decider/input pair**<br> 
DD is simulated by HHH according to the operational<br>
semantics of the C programming language.  (see trace below)

**Assumption**<br>
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
Line 11: main() invokes HHH(DD);<br>
Line 03: DD calls HHH(DD); HHH simulates DD;<br>
&nbsp;&nbsp;Line 03: DD calls HHH(DD); HHH simulates DD;<br>
&nbsp;&nbsp;&nbsp;&nbsp;Line 03: DD calls HHH(DD); HHH simulates DD;<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Line 03: DD calls HHH(DD); HHH simulates DD;

A simulating halt decider H simulates its input D until it<br>
correctly detects a repeating state that cannot possibly <br>
reach a final state. We cannot ever judge halting on the <br>
basis of what happens after simulation has been aborted or <br>
we would judge that all infinite loops always halt. Instead<br>
we look for a repeating state while the simulation continues.

On this basis simulating halt decider HHH correctly rejects its input DD

There does not exist any implementation of C function <br>
HHH that ONLY simulates C function DD according to the <br>
operational semantics of C such that the simulated C <br>
function DD reaches its own simulated return statement <br>
in any N finite number of steps while DD is being simulated<br>
by HHH.

When HHH sees that DD calls the same function with the <br>
same arguments from the same address twice in sequence <br>
and there were no conditional branch instructions from <br>
the beginning of DD to its call to HHH(DD), then HHH<br>
has matched a non-halting behavior pattern. At this <br>
point HHH stops simulating its static finite string input<br>
and returns 0 to indicate rejection. 

**What is shown above**<br> 
The Halting Theorem is a category error because it requires<br>
reporting on behavior other than the behavior specified by<br>
its input finite string.<br>

Here is the same thing from Linz where H and Ĥ.embedded_H <br>
are also based on UTM simulation of their inputs.<br>
https://www.liarparadox.org/Peter_Linz_HP_317-320.pdf 

Linz encodes his embedding of his machine H as Ĥq0<br>
Linz calls the finite string input to Ĥ: Wm

**From the bottom of page 319**<br>
q0 Wm Wm ⊢* Ĥq0 Wm Wm ⊢* Ĥ.∞ // accept state<br>
q0 Wm Wm ⊢* Ĥq0 Wm Wm ⊢* Ĥ.qn // reject state<br>

**has been adapted to this**<br>
Ĥ.q0 ⟨Ĥ⟩ ⊢* Ĥ.embedded_H ⟨Ĥ⟩ ⟨Ĥ⟩ ⊢* Ĥ.∞, // accept state<br>
Ĥ.q0 ⟨Ĥ⟩ ⊢* Ĥ.embedded_H ⟨Ĥ⟩ ⟨Ĥ⟩ ⊢* Ĥ.qn // reject state<br>

⟨Ĥ⟩ more conventionally shows that Ĥ is taking its <br>
own finite string machine description as its input.<br>

When embedded_H is a UTM based simulating halt decider then<br>
**Simulation Invariant** <br>
(a) Ĥ copies its input ⟨Ĥ⟩<br>
(b) Ĥ invokes embedded_H ⟨Ĥ⟩ ⟨Ĥ⟩<br>
(c) embedded_H simulates ⟨Ĥ⟩ ⟨Ĥ⟩

A **Post canonical system**, also known as a Post production<br>
system, as created by Emil Post, is a string-manipulation<br> 
system that starts with finitely many strings and repeatedly<br>
transforms them by applying a finite set of specified rules<br> 
of a certain form, thus generating a formal language.<br> 
https://en.wikipedia.org/wiki/Post_canonical_system 

In theoretical computer science and mathematical logic a string<br> 
rewriting system (SRS), historically called a **semi-Thue system**,<br> 
is a rewriting system over strings from a (usually finite) alphabet.<br> 
https://en.wikipedia.org/wiki/Semi-Thue_system

**Implementation details totally irrelevant to proof**<br>
The first time that HHH is invoked it allocates a shared memory block so that it can watch the execution trace of each DD instance thoughout all of its recursive invocations. 
Every HHH appends each new instruction that it simulates to this shared block. As soon as the outermost HHH sees a repeating state it aborts its simulation and rejects its input. 
Because HHH only aborts its simulation after the non-termination simulation invariant has been proven the behavior after DD has been aborted is irrelevant. 

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 
HHH creates a separate process context every time that it is about to simulate DD. This separate process context has its own virtural registers, stack and local memory. 

x86utm.exe takes the COFF object file: Halt7.obj as its command line parameter. x86utm.exe sends its standard output to Halt7out.txt.
Halt7.obj was generated from compiling Halt7.c with a Microsoft compiler. 

x86utm Halt7.obj > Halt7out.txt  // x86utm invoked from the command line<br>
Compiles with Microsoft Visual Studio Community Edition 2017


**Copyright 2022-2026 by PL Olcott all rights reserved may not be used for AI training.**
 


**Decider Defined --- Axiom 2**<br> 
The output from a decider is based on applying finite<br>
string transformation/inference steps to input finite<br>
strings relevant to the decision criterion.

For a decision to be made by anyone or anything inference<br>
steps must be applied on the basis of a decision criteria. <br> 


**Decider Defined --- Axiom 2**<br>
Deciders only apply finite string transformation/inference<br>
steps to their finite strings AS INPUTS and must only apply<br>
inference steps that are relevant to the decision criterion.<br>
