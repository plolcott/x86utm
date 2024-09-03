//  Copyright PL Olcott 2020, 2021, 2022 
//
#define UBUNTU_1604
//#define OUTPUT_SIMULATED_LINE
#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <time.h> 
#ifdef _WIN32     
  #define HALT __asm hlt
  #pragma warning (disable: 4717) // 'function': recursive on all control paths, function will cause runtime stack overflow
#endif 

#define u8  uint8_t 
//#define u32 uint32_t 
typedef uint32_t u32;

#define u16 uint16_t 
#define  s8 int8_t
#define s16 int16_t
#define s32 int32_t
//typedef void (*ptr)(u32);
typedef void (*ptr)();
typedef int (*ptr2)();


typedef struct x86_Registers
{ 
  u32  EIP;
  u32  EAX;
  u32  EBX;
  u32  ECX;
  u32  EDX;
  u32  ESI;
  u32  EDI;
  u32  EBP;
  u32  ESP;
  u32  EFLG;
  u16  CS;
  u16  SS;
  u16  DS;
  u16  ES;
  u16  FS;
  u16  GS;
} Registers; 


#define   JMP 0xEB // Simplifed OpCode for all forms of JMP
#define  CALL 0xE8 // Simplifed OpCode for all forms of CALL
#define   JCC 0x7F // Simplifed OpCode for all forms of Jump on Condition
#define   RET 0xC3 // Simplifed OpCode for all forms of Return 
#define  PUSH 0x68 // Simplifed OpCode for all forms of PUSH
#define OTHER 0xFF // Not a Control Flow Instruction 
#define   HLT 0xF4 // Conventional OpCode for Halt 


typedef struct Decoded
{
  u32 Address;        
  u32 ESP;          // Current value of ESP
  u32 TOS;          // Current value of Top of Stack
  u32 NumBytes;       
  u32 Simplified_Opcode;
  u32 Decode_Target; 
} Decoded_Line_Of_Code;


u8 BEGIN[]   = "BEGIN STATIC DATA"; // Required to force allocation
u32 Heap_PTR = 0x11111111;  // forces memory allocation 
u32 Heap_END = 0x22222222;  // forces memory allocation 
u8 END[]     = "END STATIC DATA";     // Required to force allocation

//void __stack_chk_fail(){}; // Required for objconv on Linux 

// Empty Stub Functions of Virtual Machine Instructions 
// x86utm operating system calls 
void OutputString(char* S) {}
void Output(char* S, u32 N) {}
u32* Allocate(u32 size) { return 0; }
void SaveState(Registers* state) {}
void LoadState(Registers* state) {}
u32  DebugStep(Registers* master_state, 
               Registers* slave_state, Decoded_Line_Of_Code* decoded) { return 0; } 
void PushBack(u32 stdvector, u32 data_ptr, u32 size_in_bytes) {}
u32  StackPush(u32* S, u32 M) { return 0; }
u32  get_code_end(u32 EIP){ return 0; } 
u32  Last_Address_Of_Operating_System(){ return 0; };  // 2021-08-26 
void Output_Decoded(u32 decoded){}; 


// Copy machine code of function (delimited by 0xcc)
void CopyMachineCode(u8* source, u8** destination)
{
  u32 size; 
  for (size = 0; source[size] != 0xcc; size++) 
    ;
  *destination = (u8*) Allocate(size); 
  for (u32 N = 0; N < size; N++) 
  {
    Output("source[N]: ", source[N]); 
    *destination[N] = source[N];  
  } 
  ((u32*)*destination)[-1] = size; 
  Output("CopyMachineCode destination[-1]: ", ((u32*)*destination)[-1]);
  Output("CopyMachineCode destination[-2]: ", ((u32*)*destination)[-2]);
}; 


u32 Infinite_Loop_Needs_To_Be_Aborted_Trace
    (Decoded_Line_Of_Code* execution_trace, Decoded_Line_Of_Code *current) 
{
  Decoded_Line_Of_Code *traced; 
  u32 Conditional_Branch_Count = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
//for (s32 N = next2last; N >= 0; N--)  // 2022-09-07  Otto J. Makela Portability issue
  s32 N;                                // 2022-09-07  Otto J. Makela Portability issue
  for (N = next2last; N >= 0; N--)      // 2022-09-07  Otto J. Makela Portability issue
  {
    traced = &execution_trace[N]; 
    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Conditional_Branch_Count++; 

    if (current->Simplified_Opcode == JMP)                // JMP 
      if (current->Decode_Target <= current->Address)     // upward
        if (traced->Address == current->Decode_Target)    // to this address 
          if (Conditional_Branch_Count == 0)              // no escape
            return 1; 
  }
  return 0;
}

u32 Infinite_Recursion_Needs_To_Be_Aborted_Trace
    (Decoded_Line_Of_Code* execution_trace, Decoded_Line_Of_Code *current) 
{
  Decoded_Line_Of_Code *traced; 
  u32 Conditional_Branch_Count = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
//for (s32 N = next2last; N >= 0; N--)  // 2022-09-07  Otto J. Makela Portability issue
  s32 N;                                // 2022-09-07  Otto J. Makela Portability issue
  for (N = next2last; N >= 0; N--)      // 2022-09-07  Otto J. Makela Portability issue
  {
    traced = &execution_trace[N]; 
    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Conditional_Branch_Count++; 

    if (current->Simplified_Opcode == CALL)
      if (current->Simplified_Opcode == traced->Simplified_Opcode)  // CALL
        if (current->Address == traced->Address)              // from same address 
          if (current->Decode_Target == traced->Decode_Target)// to Same Function 
            if (Conditional_Branch_Count == 0)                // no escape
              return 2; 
  }
  return 0;
}


////////////
u32 Infinite_Simulation_Needs_To_Be_Aborted_Trace0
    (Decoded_Line_Of_Code* execution_trace, Decoded_Line_Of_Code *current, u32 P) 
{ 
  Decoded_Line_Of_Code *traced; 
  u32 Count_PUSH_Instructions  = 0; 
  u32 Num_PUSH_Matched         = 0; 
  u32 Conditional_Branch_Count = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
  for (s32 N = next2last; N >= 0; N--) 
  {
    traced = &execution_trace[N]; 
    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Conditional_Branch_Count++; 

    if (traced->Simplified_Opcode == PUSH)                // PUSH
      Count_PUSH_Instructions++; 

    if (traced->Simplified_Opcode == PUSH && traced->Decode_Target == P && Count_PUSH_Instructions == 1)  
      Num_PUSH_Matched++; 

    if (Num_PUSH_Matched == 1 && N == 0 && Conditional_Branch_Count == 0) 
      return 3; 
  }
  return 0;
}


u32 Needs_To_Be_Aborted0(Decoded_Line_Of_Code* execution_trace, u32 Address_of_H, u32 P)
{
  u32 Aborted = 0; 
  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 last = (size / sizeof(Decoded_Line_Of_Code)) - 1; 
  Decoded_Line_Of_Code* current = &execution_trace[last]; 
//Output("Needs_To_Be_Aborted0 size:", size/sizeof(Decoded_Line_Of_Code)); 

  if (current->Simplified_Opcode == CALL) 
  {
    if (current->Decode_Target == Address_of_H)
      Aborted = Infinite_Simulation_Needs_To_Be_Aborted_Trace0(execution_trace, current, P); 
    else 
      Aborted = Infinite_Recursion_Needs_To_Be_Aborted_Trace(execution_trace, current);  
  }
  else if (current->Simplified_Opcode == JMP) 
    Aborted = Infinite_Loop_Needs_To_Be_Aborted_Trace(execution_trace, current); 
  return Aborted; 
}  


//
// This is called every time the a line of x86 code is emulated 
//
u32 Decide_Halting0(char*                   Halt_Decider_Name,
                   u32*                    execution_trace,
                   Decoded_Line_Of_Code**  decoded,
                   u32                     code_end,
                   Registers**             master_state,
                   Registers**             slave_state,
                   u32**                   slave_stack, 
                   u32                     Address_of_H, u32 P) 
{
  u32 Aborted = 0;
  while (Aborted == 0)
  { 
    u32 EIP = (*slave_state)->EIP; // Save EIP of instruction to be executed 
    DebugStep(*master_state, *slave_state, *decoded); // Execute this instruction 
    if (EIP == code_end)           // last instruction of P "ret" 
      return 1;                    // input has halted 

#ifdef OUTPUT_SIMULATED_LINE
    Output_Decoded((u32)*decoded);
#endif
    PushBack(*execution_trace, (u32)*decoded, sizeof(Decoded_Line_Of_Code));  
    Aborted = Needs_To_Be_Aborted0((Decoded_Line_Of_Code*)*execution_trace, Address_of_H, P); 
  } 
  if (Aborted) // 2021-01-26 Must be aborted 
  {
    OutputString(Halt_Decider_Name);
    if (Aborted == 1)
      OutputString("Infinite Loop Detected Simulation Stopped\n\n");
    if (Aborted == 2)
      OutputString("Infinite Recursion Detected Simulation Stopped\n\n"); 
    if (Aborted == 3)
      OutputString("Simulation Detected Simulation Stopped\n\n");
    return 0;        
  }
  return 1;         // 2021-01-26 Need not be aborted
}


// This only works with one ZERO PARAMETERS  to the called function 
void Init_slave_state0(u32 P, u32 End_Of_Code, 
                       Registers* slave_state, u32* slave_stack) 
{
  u32 Top_of_Stack;
  u32 Capacity;
  u32 Size;

  Top_of_Stack = StackPush(slave_stack, End_Of_Code); // Return Address in Halts() 
  SaveState(slave_state);    // Based on this point in execution 
  Capacity = slave_stack[-2]; 
  Size = slave_stack[-1];

  slave_state->EIP = P;      // Function to invoke
  slave_state->ESP = Top_of_Stack; 
  slave_state->EBP = Top_of_Stack; 
}


u32 Infinite_Simulation_Needs_To_Be_Aborted_Trace
    (Decoded_Line_Of_Code* execution_trace, 
     Decoded_Line_Of_Code *current, u32 P, u32 I) 
{ 
  Decoded_Line_Of_Code *traced; 
  u32 Count_PUSH_Instructions  = 0; 
  u32 Num_PUSH_Matched         = 0; 
  u32 Conditional_Branch_Count = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
//for (s32 N = next2last; N >= 0; N--)  // 2022-09-07  Otto J. Makela Portability issue
  s32 N;                                // 2022-09-07  Otto J. Makela Portability issue
  for (N = next2last; N >= 0; N--)      // 2022-09-07  Otto J. Makela Portability issue
  {
    traced = &execution_trace[N]; 
    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Conditional_Branch_Count++; 

    if (traced->Simplified_Opcode == PUSH)                // PUSH
      Count_PUSH_Instructions++; 

    if (traced->Simplified_Opcode == PUSH && 
        traced->Decode_Target == P && Count_PUSH_Instructions == 1)  
      Num_PUSH_Matched++; 

    if (traced->Simplified_Opcode == PUSH && 
        traced->Decode_Target == I && Count_PUSH_Instructions == 2) 
      Num_PUSH_Matched++; 

    if (Num_PUSH_Matched == 2 && N == 0 && Conditional_Branch_Count == 0) 
      return 3; 
  }
  return 0;
}


u32 Needs_To_Be_Aborted(Decoded_Line_Of_Code* execution_trace, 
                        u32 Address_of_H, u32 P, u32 I)
{
  u32 Aborted = 0; 
  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
//Output("Needs_To_Be_Aborted(size):", size); 
  u32 last = (size / sizeof(Decoded_Line_Of_Code)) - 1; 
  Decoded_Line_Of_Code* current = &execution_trace[last]; 

  if (current->Simplified_Opcode == CALL) 
  {
    if (current->Decode_Target == Address_of_H)
      Aborted = Infinite_Simulation_Needs_To_Be_Aborted_Trace
                (execution_trace, current, P, I); 
    else 
      Aborted = Infinite_Recursion_Needs_To_Be_Aborted_Trace
                (execution_trace, current);  
  }
  else if (current->Simplified_Opcode == JMP) 
    Aborted = Infinite_Loop_Needs_To_Be_Aborted_Trace(execution_trace, current); 
  return Aborted; 
}  


//
// This is called every time the a line of x86 code is emulated 
//
u32 Decide_Halting(char*                   Halt_Decider_Name,
                   u32*                    execution_trace,
                   Decoded_Line_Of_Code**  decoded,
                   u32                     code_end,
                   Registers**             master_state,
                   Registers**             slave_state,
                   u32**                   slave_stack, 
                   u32                     Address_of_H, u32 P, u32 I) 
{
  u32 Aborted = 0;
  while (Aborted == 0)
  { 
    u32 EIP = (*slave_state)->EIP; // Save EIP of instruction to be executed 
    DebugStep(*master_state, *slave_state, *decoded); // Execute this instruction 
    if (EIP == code_end)           // last instruction of P "ret" 
      return 1;                    // input has halted 

#ifdef OUTPUT_SIMULATED_LINE
    Output_Decoded((u32)*decoded);
#endif
//  When we are not recursively simulatng H we don't need this is statement 
//  if (EIP > Last_Address_Of_Operating_System())  // Don't examine any OS code 
    PushBack(*execution_trace, (u32)*decoded, sizeof(Decoded_Line_Of_Code));  
    Aborted = Needs_To_Be_Aborted((Decoded_Line_Of_Code*)*execution_trace, 
                                   Address_of_H, P, I); 
  } 
  if (Aborted) // 2021-01-26 Must be aborted 
  {
    OutputString(Halt_Decider_Name);
    if (Aborted == 1)
      OutputString((char*)"Infinite Loop Detected Simulation Stopped\n\n");
    if (Aborted == 2)
      OutputString((char*)"Infinite Recursion Detected Simulation Stopped\n\n"); 
    if (Aborted == 3)
      OutputString((char*)"Recursive Simulation Detected "
                   "Simulation Stopped\n\n");
    return 0;        
  }
  return 1;         // 2021-01-26 Need not be aborted
}


// This only works with ONE PARAMETER to the called function 
void Init_slave_state(u32 P, u32 I, u32 End_Of_Code, 
                      Registers* slave_state, u32* slave_stack) 
{
  u32 Top_of_Stack;
//u32 Capacity;
//u32 Size;

  Top_of_Stack = StackPush(slave_stack, I);   // Data for Function to invoke 
  Top_of_Stack = StackPush(slave_stack, End_Of_Code); // Return Address in Halts() 

  SaveState(slave_state);    // Based on this point in execution 
//Capacity = slave_stack[-2]; 
//Size = slave_stack[-1];

  slave_state->EIP = P;      // Function to invoke
  slave_state->ESP = Top_of_Stack; 
  slave_state->EBP = Top_of_Stack; 
}



u32 Needs_To_Be_Aborted_Trace_HH(Decoded_Line_Of_Code* execution_trace, 
                                 Decoded_Line_Of_Code *current) 
{
//OutputString("Needs_To_Be_Aborted_Trace"); 
//Output_Decoded((u32)current); 

  Decoded_Line_Of_Code *traced; 
  u32 Count_Conditional_Branch_Instructions = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  

  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
//for (s32 N = next2last; N >= 0; N--) // 2022-09-07  Otto J. Makela Portability issue
  s32 N;                               // 2022-09-07  Otto J. Makela Portability issue
  for (N = next2last; N >= 0; N--) // 2022-09-07  Otto J. Makela Portability issue
  {
    traced = &execution_trace[N]; 
//  Output("traced->Address:", traced->Address); 

    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Count_Conditional_Branch_Instructions++; 

    if (current->Simplified_Opcode == JMP)                // JMP 
      if (current->Decode_Target <= current->Address)     // upward
        if (traced->Address == current->Decode_Target)    // to this address 
          if (Count_Conditional_Branch_Instructions == 0) // no escape
          { 
            OutputString((char*)"Local Halt Decider: "
                         "Infinite Loop Detected Simulation Stopped\n\n");
            return 1; 
          }
    if (current->Simplified_Opcode == CALL)
      if (current->Simplified_Opcode == traced->Simplified_Opcode)  // CALL
        if (current->Address == traced->Address)              // from same address 
          if (current->Decode_Target == traced->Decode_Target)// to Same Function 
            if (Count_Conditional_Branch_Instructions == 0)   // no escape
            { 
              OutputString((char*)"Local Halt Decider: "
                           "Infinite Recursion Detected Simulation Stopped\n\n");

              return 1; 
            } 
  }
  return 0;
}

//u32 Halts(u32 P, u32 I); 
u32 Needs_To_Be_Aborted_HH(Decoded_Line_Of_Code* execution_trace)
{
  u32 Aborted = 0; 
  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  

//Output("Needs_To_Be_Aborted(size):", size); 
  u32 last = (size / sizeof(Decoded_Line_Of_Code)) - 1; 
  Decoded_Line_Of_Code* current = &execution_trace[last]; 

  if (current->Simplified_Opcode == CALL) 
    Aborted = Needs_To_Be_Aborted_Trace_HH(execution_trace, current); 
  else if (current->Simplified_Opcode == JMP) 
    Aborted = Needs_To_Be_Aborted_Trace_HH(execution_trace, current); 
  return Aborted; 
}  



u32 Decide_Halting_HH(u32**                   Aborted,
                      u32**                   execution_trace,
                      Decoded_Line_Of_Code**  decoded,
                      u32                     code_end,
                      u32                     End_Of_Code,               
                      Registers**             master_state,
                      Registers**             slave_state,
                      u32**                   slave_stack, 
                      u32                     Root)
{
  u32 aborted_temp = 0;  // 2024-06-05
  u32 Current_Length_Of_Execution_Trace = 0; 
//while (**Aborted == 0)
  while (aborted_temp == 0) // 2024-06-05
  { 
    u32 EIP = (*slave_state)->EIP; // Save EIP of instruction to be executed 
    DebugStep(*master_state, *slave_state, *decoded); // Execute this instruction 

//  Output_Decoded((u32)*decoded);

    if (EIP > Last_Address_Of_Operating_System())  // Don't examine any OS code 
    {
      PushBack(**execution_trace, (u32)*decoded, sizeof(Decoded_Line_Of_Code));  
//    Output_Decoded((u32)*decoded);
    }

    if (EIP == code_end)
      return 1; 

    if (Root)  // Master UTM halt decider 
    {
      u32* address = (u32*)**execution_trace; 
      u32 size  = (u32)*(address-1); 

      // Detects that a slave appended an instruction to its execution_trace 
      if (size > Current_Length_Of_Execution_Trace) 
      {                                             
        Current_Length_Of_Execution_Trace = size;
//      **Aborted = 
        aborted_temp =     // 2024-06-05                                   // 2024-08-27 
        Needs_To_Be_Aborted_HH((Decoded_Line_Of_Code*)**execution_trace);  // 2024-08-27 
      }
    } 
  } 
//if (**Aborted == 1) // 2021-01-26 Must be aborted 
  if (aborted_temp == 1) // 2021-01-26 Must be aborted 
    return 0;        
  return 1;           // 2021-01-26 Need not be aborted
}


u32 Init_Halts_HH(u32**                   Aborted,
                  u32**                   execution_trace,
                  Decoded_Line_Of_Code**  decoded,
                  u32*                    code_end,
                  u32                     P, 
                  Registers**             master_state,
                  Registers**             slave_state,
                  u32**                   slave_stack) 
{
  *decoded      = (Decoded_Line_Of_Code*) Allocate(sizeof(Decoded_Line_Of_Code)); 
  *code_end     = get_code_end(P);
  *master_state = (Registers*) Allocate(sizeof(Registers)); 
  *slave_state  = (Registers*) Allocate(sizeof(Registers)); 
  *slave_stack  = Allocate(0x10000); // 64k
  Output((char*)"New slave_stack at:", (u32)*slave_stack);
  if (**execution_trace == 0x90909090)
  {
//  Global_Recursion_Depth = 0; 
    **Aborted = 0;
    **execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
    Output((char*)"\nBegin Local Halt Decider Simulation   "
           "Execution Trace Stored at:", **execution_trace); 
    return 1; 
  } 
  return 0;  
}


//==============================================
// Original version of H that uses static local data and is
// capable of recursive simulations. I restored this one 
// because it was too difficult for people to see that H(P,P) 
// matches the infinite recursion behavior pattern with only 
// a single call from, P to H(P,P). 
// 
u32 HH1(ptr P, ptr I)
{ 
  u32* Aborted; 
  u32* execution_trace;
  u32  End_Of_Code; 
  goto SKIP; 

DATA1: 
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

DATA2:
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

SKIP:
#ifdef _WIN32
  __asm lea eax, DATA1
  __asm mov Aborted, eax          // Data stored directly in the function body
  __asm lea eax, DATA2
  __asm mov execution_trace, eax  // Data stored directly in the function body
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 
#elif __linux__
  __asm__("lea eax, DATA1");
  __asm__("mov Aborted, eax");          // Data stored directly in the function body
  __asm__("lea eax, DATA2");
  __asm__("mov execution_trace, eax");  // Data stored directly in the function body
  __asm__("mov eax, END_OF_CODE"); 
  __asm__("mov End_Of_Code, eax"); 
#endif

  Decoded_Line_Of_Code *decoded;  
  u32 code_end;                   
  Registers*  master_state;       
  Registers*  slave_state;        
         u32* slave_stack;        

  u32 Root = Init_Halts_HH(&Aborted, &execution_trace, &decoded, &code_end, (u32)P,
                           &master_state, &slave_state, &slave_stack);
//Output("H_Root:", Root);  

  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 

  if (Decide_Halting_HH(&Aborted, &execution_trace, &decoded, 
                        code_end, End_Of_Code, &master_state, 
                        &slave_state, &slave_stack, Root))
      goto END_OF_CODE; 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 0;  // Does not halt
END_OF_CODE: 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 1; // Input has normally terminated 
} 
//========================================

// Original version of H that uses static local data and is
// capable of recursive simulations. I restored this one 
// because it was too difficult for people to see that H(P,P) 
// matches the infinite recursion behavior pattern with only 
// a single call from, P to H(P,P). 
// 
u32 HH(ptr P, ptr I)
{ 
  u32* Aborted; 
  u32* execution_trace;
  u32  End_Of_Code; 
  goto SKIP; 

DATA1: 
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

DATA2:
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

SKIP:
#ifdef _WIN32
  __asm lea eax, DATA1
  __asm mov Aborted, eax          // Data stored directly in the function body
  __asm lea eax, DATA2
  __asm mov execution_trace, eax  // Data stored directly in the function body
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 
#elif __linux__
  __asm__("lea eax, DATA1");
  __asm__("mov Aborted, eax");          // Data stored directly in the function body
  __asm__("lea eax, DATA2");
  __asm__("mov execution_trace, eax");  // Data stored directly in the function body
  __asm__("mov eax, END_OF_CODE"); 
  __asm__("mov End_Of_Code, eax"); 
#endif

  Decoded_Line_Of_Code *decoded;  
  u32 code_end;                   
  Registers*  master_state;       
  Registers*  slave_state;        
         u32* slave_stack;        

  u32 Root = Init_Halts_HH(&Aborted, &execution_trace, &decoded, &code_end, (u32)P,
                           &master_state, &slave_state, &slave_stack);
//Output("H_Root:", Root);  

  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 

  if (Decide_Halting_HH(&Aborted, &execution_trace, &decoded, 
                        code_end, End_Of_Code, &master_state, 
                        &slave_state, &slave_stack, Root))
      goto END_OF_CODE; 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 0;  // Does not halt
END_OF_CODE: 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 1; // Input has normally terminated 
} 

// Same as HHH exactly DDD does not call HH1(DDD)
u32 HHH1(ptr P)
{ 
  u32* Aborted; 
  u32* execution_trace;
  u32  End_Of_Code; 
  goto SKIP; 

DATA1: 
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

DATA2:
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

SKIP:
#ifdef _WIN32
  __asm lea eax, DATA1
  __asm mov Aborted, eax          // Data stored directly in the function body
  __asm lea eax, DATA2
  __asm mov execution_trace, eax  // Data stored directly in the function body
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 
#elif __linux__
  __asm__("lea eax, DATA1");
  __asm__("mov Aborted, eax");          // Data stored directly in the function body
  __asm__("lea eax, DATA2");
  __asm__("mov execution_trace, eax");  // Data stored directly in the function body
  __asm__("mov eax, END_OF_CODE"); 
  __asm__("mov End_Of_Code, eax"); 
#endif

  Decoded_Line_Of_Code *decoded;  
  u32 code_end;                   
  Registers*  master_state;       
  Registers*  slave_state;        
         u32* slave_stack;        

  u32 Root = Init_Halts_HH(&Aborted, &execution_trace, &decoded, &code_end, (u32)P,
                           &master_state, &slave_state, &slave_stack);
//Output("H_Root:", Root);  

//Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Init_slave_state0((u32)P, End_Of_Code, slave_state, slave_stack); // 2024-06-16 
  if (Decide_Halting_HH(&Aborted, &execution_trace, &decoded, 
                        code_end, End_Of_Code, &master_state, 
                        &slave_state, &slave_stack, Root))
      goto END_OF_CODE; 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 0;  // Does not halt
END_OF_CODE: 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 1; // Input has normally terminated 
} 




u32 HHH(ptr P)
{ 
  u32* Aborted; 
  u32* execution_trace;
  u32  End_Of_Code; 
  goto SKIP; 

DATA1: 
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

DATA2:
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

SKIP:
#ifdef _WIN32
  __asm lea eax, DATA1
  __asm mov Aborted, eax          // Data stored directly in the function body
  __asm lea eax, DATA2
  __asm mov execution_trace, eax  // Data stored directly in the function body
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 
#elif __linux__
  __asm__("lea eax, DATA1");
  __asm__("mov Aborted, eax");          // Data stored directly in the function body
  __asm__("lea eax, DATA2");
  __asm__("mov execution_trace, eax");  // Data stored directly in the function body
  __asm__("mov eax, END_OF_CODE"); 
  __asm__("mov End_Of_Code, eax"); 
#endif

  Decoded_Line_Of_Code *decoded;  
  u32 code_end;                   
  Registers*  master_state;       
  Registers*  slave_state;        
         u32* slave_stack;        

  u32 Root = Init_Halts_HH(&Aborted, &execution_trace, &decoded, &code_end, (u32)P,
                           &master_state, &slave_state, &slave_stack);
//Output("H_Root:", Root);  

//Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Init_slave_state0((u32)P, End_Of_Code, slave_state, slave_stack); // 2024-06-16 
  if (Decide_Halting_HH(&Aborted, &execution_trace, &decoded, 
                        code_end, End_Of_Code, &master_state, 
                        &slave_state, &slave_stack, Root))
      goto END_OF_CODE; 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 0;  // Does not halt
END_OF_CODE: 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 1; // Input has normally terminated 
} 



// Original version of H that uses static local data and is
// capable of recursive simulations. I restored this one 
// because it was too difficult for people to see that H(P,P) 
// matches the infinite recursion behavior pattern with only 
// a single call from, P to H(P,P). 
// 
u32 Address_of_Sipser_H(ptr P)
{ 
  u32* Aborted; 
  u32* execution_trace;
  u32  End_Of_Code; 
  goto SKIP; 

DATA1: 
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

DATA2:
#ifdef _WIN32
  __asm nop  // The purpose of creating static local memory 
  __asm nop  // directly in the function body is to make it 
  __asm nop  // clear that a Turing machine computatation has
  __asm nop  // this ability by simply writing to its own tape
#elif __linux__
  __asm__("nop");  // The purpose of creating static local memory 
  __asm__("nop");  // directly in the function body is to make it 
  __asm__("nop");  // clear that a Turing machine computatation has
  __asm__("nop");  // this ability by simply writing to its own tape
#endif

SKIP:
#ifdef _WIN32
  __asm lea eax, DATA1
  __asm mov Aborted, eax          // Data stored directly in the function body
  __asm lea eax, DATA2
  __asm mov execution_trace, eax  // Data stored directly in the function body
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 
#elif __linux__
  __asm__("lea eax, DATA1");
  __asm__("mov Aborted, eax");          // Data stored directly in the function body
  __asm__("lea eax, DATA2");
  __asm__("mov execution_trace, eax");  // Data stored directly in the function body
  __asm__("mov eax, END_OF_CODE"); 
  __asm__("mov End_Of_Code, eax"); 
#endif

  Decoded_Line_Of_Code *decoded;  
  u32 code_end;                   
  Registers*  master_state;       
  Registers*  slave_state;        
         u32* slave_stack;        

  u32 Root = Init_Halts_HH(&Aborted, &execution_trace, &decoded, &code_end, (u32)P,
                           &master_state, &slave_state, &slave_stack);
//Output("H_Root:", Root);  

//Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Init_slave_state0((u32)P, End_Of_Code, slave_state, slave_stack); // 2024-06-16 

  if (Decide_Halting_HH(&Aborted, &execution_trace, &decoded, 
                        code_end, End_Of_Code, &master_state, 
                        &slave_state, &slave_stack, Root))
      goto END_OF_CODE; 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 0;  // Does not halt
END_OF_CODE: 
// Output_Decoded_Instructions(*execution_trace);
  *Aborted         = 0x90909090;
  *execution_trace = 0x90909090;
  return 1; // Input has normally terminated 
} 



// Identical to H, yet at a differnt machine address. 
// 
u32 H1(ptr P, ptr I)
{ 
  u32 End_Of_Code               = get_code_end((u32)H1); 
  u32 Address_of_H1             = (u32)H1;       // 2022-08-15 
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
                                  // 10000 lines of x86 code
  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Output((char*)"\nH1: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output((char*)"Address_of_H1:", Address_of_H1); // 2022-06-11 
  if (Decide_Halting((char*)"H1: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_H1, (u32)P, (u32)I))
  {
    OutputString((char*)"H1: End Simulation   Input Terminated Normally\n\n"); 
    return 1; // Input has normally terminated 
  }
  return 0;  // Does not halt
} 



// Matches the infinite recursion behavior pattern with a single call 
// from P to H(P,P).  The instance of H that is simulating P is counted 
// as the first call of the infinitre recusion behavior pattern. 
// When P calls H(P,P) this is counted as the second call. 
// This requires P to know its own machine address so that it can see
// that itself is beng called from P. 
// 
u32 H(ptr P, void* I)
{ 
  u32 End_Of_Code               = get_code_end((u32)H); 
  u32 Address_of_H              = (u32)H;         
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
                                                 // 10000 lines of x86 code
//Output("Address_of_H:", Address_of_H); 
  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Output((char*)"\nH: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output((char*)"Address_of_H:", Address_of_H); // 2022-06-11 
  if (Decide_Halting((char*)"H: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_H, (u32)P, (u32)I))
  {
    OutputString((char*)"H: End Simulation   Input Terminated Normally\n\n"); 
    return 1; // Input has normally terminated 
  }
  return 0;  // Does not halt
} 


u32 Sipser_H(ptr2 P, ptr2 I)
{ 
  u32 End_Of_Code               = get_code_end((u32)H); 
  u32 Address_of_Sipser_H       = (u32)Sipser_H;         
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
                                                 // 10000 lines of x86 code
//Output("Address_of_Sipser_H:", Address_of_Sipser_H); 
  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Output((char*)"\nSipser_H: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output((char*)"Address_of_Sipser_H:", Address_of_Sipser_H); // 2022-06-11 
  if (Decide_Halting((char*)"Sipser_H: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_Sipser_H, (u32)P, (u32)I))
  {
    OutputString((char*)"Sipser_H: End Simulation   Input Terminated Normally\n\n"); 
    return 1; // Input has normally terminated 
  }
  return 0;  // Does not halt
} 


u32 Kozen_K(ptr2 P, ptr2 I)
{ 
  u32 End_Of_Code               = get_code_end((u32)H); 
  u32 Address_of_Kozen_K        = (u32)Kozen_K;         
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
                                                 // 10000 lines of x86 code
//Output("Address_of_Sipser_H:", Address_of_Sipser_H); 
  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Output((char*)"\nH: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output((char*)"Address_of_Kozen_K:", Address_of_Kozen_K); // 2022-06-11 
  if (Decide_Halting((char*)"Kozen_K: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_Kozen_K, (u32)P, (u32)I))
  {
    OutputString((char*)"Kozen_K: End Simulation   Input Terminated Normally\n\n"); 
    return 1; // Input has normally terminated 
  }
  return 0;  // Does not halt
} 


u32 H0(ptr P)
{ 
  u32 End_Of_Code               = get_code_end((u32)H0); ; 
  u32 Address_of_H0              = (u32)H0;      
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 1000); 
  Init_slave_state0((u32)P, End_Of_Code, slave_state, slave_stack); 
  Output("\nH0: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output("Address_of_H0:", Address_of_H0); // 2022-06-11 
  if (Decide_Halting0("H0: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_H0, (u32)P))
      goto END_OF_CODE; 
  return 0;  // Does not halt
END_OF_CODE: 
  OutputString("H: End Simulation   Input Terminated Normally\n\n"); 
  return 1; // Input has normally terminated 
} 


u32 T(ptr P)
{ 
  u32 End_Of_Code               = get_code_end((u32)T); ; 
  u32 Address_of_T              = (u32)T;      
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 1000); 
  Init_slave_state0((u32)P, End_Of_Code, slave_state, slave_stack); 
  Output("\nT: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output("Address_of_T:", Address_of_T); // 2022-06-11 
  if (Decide_Halting0("T: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_T, (u32)P))
      goto END_OF_CODE; 
  return 0;  // Does not halt
END_OF_CODE: 
  OutputString("H: End Simulation   Input Terminated Normally\n\n"); 
  return 1; // Input has normally terminated 
} 


// Dummy Place holder needed to know where 
// the x86utm operating system is located. 
// THIS FUNCTION MAY BE OBSOLETE 
u32 Halts(u32 P, u32 I)
{
  return 0; 
}

void Infinite_Loop2()
{
L1: goto L3;
L2: goto L1;
L3: goto L2;
}

void Infinite_Loop3() {
L1: goto L3;
L2: goto L4;
L3: goto L2;
L4: return; 
}

u32 Add3(u32 x)
{
  return x + 3; 
}

void HR(u32 y);

void PR(u32 x)
{ 
  HR(x);
}

void HR(u32 y)
{
  PR(y); 
}

/***
(1) HR is called twice in sequence from the same machine address of PR.
(2) With the same arguments to HR. 
(3) With no control flow instructions between the invocation of PR and the call to HR from PR
***/ 


int Simulate(ptr x, ptr y) 
{
  x(y); 
  return 1; 
}


void Py(ptr x) 
{
  int Halt_Status = Simulate(x, x); 
  if (Halt_Status) 
    HERE: goto HERE; 
  return; 
} 

void Pz(ptr x)
{
  Simulate(x, x); 
}

void Px(void (*x)())
{
  (void) H(x, x);
  return;
}


typedef int (*ptr2)();



int Kozen_N(ptr2 x) 
{
  if ( Kozen_K(x,x) ) 
    HERE: goto HERE;
  return 1; 
}


void P(ptr x) 
{
  int Halt_Status = H(x, x); 
  if (Halt_Status) 
    HERE: goto HERE; 
  return; 
} 

void E(void (*x)()) 
{
  H(x, x); 
} 


int D0() 
{
  int Halt_Status = H0(D0); 
  if (Halt_Status)   
    HERE: goto HERE; 
  return Halt_Status; 
} 


int X(ptr P, ptr I)  
{
  P(I); 
  return 0; 
}

int Y(ptr P)  
{
  X(P, P);
  return 1; 
}

void B(int (*x)()) 
{
  H(x, x); 
  return; 
} 

void Infinite_Recursion3(u32 N)
{
  Infinite_Recursion3(N); 
}

void Infinite_Recursion2(u32 N)
{
    H(Infinite_Recursion2, (ptr)N);
} 

int factorial(int n) 
{
//Output("factorial:", n); 
  if (n >= 1)
    return n*factorial(n-1);
  else
    return 1;
}


void This_Halts()
{
  return; 
}

void Recursion_Chain_01(int M);
void Recursion_Chain_02(int M);
void Recursion_Chain_03(int M);

void Recursion_Chain_03(int M)
{
  Recursion_Chain_01(M);
}

void Recursion_Chain_02(int M)
{
  Recursion_Chain_03(M);
}

void Recursion_Chain_01(int M)
{
  Recursion_Chain_02(M);
}



int D(int (*x)()) 
{
  int Halt_Status = H(x, x); 
  if (Halt_Status)   
    HERE: goto HERE; 
  return Halt_Status; 
} 


int PP(ptr2 x) 
{
  int Halt_Status = HH(x, x); 
  if (Halt_Status) 
    HERE: goto HERE; 
  return Halt_Status; 
} 


void HHHxyz(ptr P, ptr I)
{
  P(I); 
}


// rec routine P
//   §L :if T[P] go to L
//     Return § 
// https://academic.oup.com/comjnl/article/7/4/313/354243 
void Strachey_P() 
{
  L: if (HHH(Strachey_P)) goto L; 
  return; 
}

void Infinite_Recursion()
{
  Infinite_Recursion(); 
}

void Infinite_Loop() 
{
  HERE: goto HERE;
  return; 
}


int DD(int (*x)()) 
{
  int Halt_Status = HH(x, x); 
  if (Halt_Status) 
    HERE: goto HERE; 
  return Halt_Status; 
} 

int Sipser_D(int (*M)()) 
{
  if (HH(M, M) )
    return 0;
  return 1;
}

// HHH(DDD) and HHH1(DDD) are the standard names for DDD input 
// DDD calls HHH(DDD). HHH1 is identical to HHH. 

// HH(DD,DD) and HH1(DD,DD) are the standard names for (DD,DD) input
// DD calls HH(DD,DD) and HH1 is identical to HH. 

void DDD() 
{
  HHH(DDD); 
  return; 
} 

int main() 
{ 
//Output("Input_Halts = ", HH(Sipser_D, Sipser_D)); 
//Output("Input_Halts = ", HHH(Infinite_Loop));
//Output("Input_Halts = ", HHH(Infinite_Recursion));
  Output("Input_Halts = ", HHH(DDD));
//Output("Input_Halts = ", HH(DD,DD));
//  Output("Input_Halts = ", HH1(DD,DD));
//  Output("Input_Halts = ", HHH1(DDD));
//Output("Input_Halts = ", HHH(Strachey_P));
  return 0; 
} 
 
 

