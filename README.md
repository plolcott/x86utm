# x86utm operating system

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 

The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 

A simulating halt decider correctly predicts what the behavior of its input would be if this simulated input never had its simulation aborted. It does this by correctly recognizing several non-halting behavior patterns in a finite number of steps of correct simulation. 
```
01 int D(int (*x)()) 
02 {
03   int Halt_Status = H(x, x); 
04   if (Halt_Status)   
05     HERE: goto HERE; 
06   return Halt_Status; 
07 }
```
**Ordinary software engineering conclusively proves that D correctly simulated** <br>
**by H cannnot possibly reach its own "return" instruction and halt.** 

**THIS KEEPS REPEATING---> D calls H to simulate D(D)<br> never reaching past line 03** 

Simulating halt decider H correctly predicts that D(D) would never stop running unless H aborts its simulation of D. It does this by recognizing a behavior pattern that is very similar to infinite recursion. 

When D calls H to simulate itself this comparable to calling H to call itself and can result in something like infinite recursion. Because there are no control flow instructions in D to stop this the recursive simulation continues until H aborts it. 

When the simulation of D is aborted this is comparable to a divide by zero error thus is not construed as D halting. 

**Compiles with Microsoft Visual Studio Community Edition 2017**
