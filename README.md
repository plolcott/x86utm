# x86utm operating system

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 

The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 

**When simulating (partial) halt decider H correctly simulates its input D until H correctly determines that its simulated D would never stop running unless aborted then H is necessarily correct to abort its simulation and reject this input as non-halting.**
```
01 int D(int (*x)()) 
02 {
03   int Halt_Status = H(x, x); 
04   if (Halt_Status)   
05     HERE: goto HERE; 
06   return Halt_Status; 
07 }
```

**Ordinary software engineering conclusively proves that D correctly simulated by H cannot possibly
reach its own simulated return instruction and terminate normally.**

Simulating halt decider H correctly predicts that its simlated D(D) would never stop running unless H aborts its simulation of D. It does this by recognizing a behavior pattern that is very similar to infinite recursion. 

When D calls H to simulate itself this comparable to calling H to call itself and can result in something like infinite recursion. Because there are no control flow instructions in D to stop this the recursive simulation continues until H aborts it. 

When the simulation of D is aborted this is comparable to a divide by zero error thus is not construed as D halting. 

**Compiles with Microsoft Visual Studio Community Edition 2017**
