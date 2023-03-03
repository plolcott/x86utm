# x86utm operating system

The x86utm operating system enables functions written in C to be simulated in Debug Step mode by another C function using an x86 emulator. 

The key purpose of x86utm was to examine the halting theorem's counter-example inputs at the high level of the C programming language. 

A simulating halt decider correctly predicts what the behavior of its input would be if this simulated input never had its simulation aborted. It does this by correctly recognizing several non-halting behavior patterns in a finite number of steps of correct simulation. 
```
int D(int (*x)()) 
{
  int Halt_Status = H(x, x); 
  if (Halt_Status)   // This code is never reached on D(D)
    HERE: goto HERE; // This code is never reached on D(D)
  return Halt_Status; 
}
```
Simulating halt decider H correctly predicts that D(D) would never stop running unless H aborts its simlation of D. 
