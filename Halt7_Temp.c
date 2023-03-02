#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <time.h> 
#pragma warning (disable: 4717)
//#define OUTPUT_SIMULATED_LINE

#define u8  uint8_t 
#define u32 uint32_t 
#define u16 uint16_t 

#define  s8 int8_t
#define s16 int16_t
#define s32 int32_t
typedef void (*ptr)();


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
#define OTHER 0xFF // Not a Control Flow Insrtuction 
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

// Empty Stub Functions of Virtual Machine Instructions 
//void OutputHex(u32 H) {}
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


u32 Infinite_Loop_Needs_To_Be_Aborted_Trace
    (Decoded_Line_Of_Code* execution_trace, Decoded_Line_Of_Code *current) 
{
  Decoded_Line_Of_Code *traced; 
  u32 Conditional_Branch_Count = 0; 

  u32* ptr = (u32*)execution_trace;  // 2021-04-06 
  u32 size = ptr[-1];                // 2021-04-06  
  u32 next2last = (size/sizeof(Decoded_Line_Of_Code)) -2;
  for (s32 N = next2last; N >= 0; N--) 
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
  for (s32 N = next2last; N >= 0; N--) 
  {
    traced = &execution_trace[N]; 
    if (traced->Simplified_Opcode == JCC)                 // JCC 
      Conditional_Branch_Count++; 

    if (current->Simplified_Opcode == CALL)
      if (current->Simplified_Opcode == traced->Simplified_Opcode)  // CALL
        if (current->Address == traced->Address)              // from same address 
          if (current->Decode_Target == traced->Decode_Target)// to Same Function 
            if (Conditional_Branch_Count == 0)              // no escape
              return 2; 
  }
  return 0;
}


u32 Infinite_Simulation_Needs_To_Be_Aborted_Trace
    (Decoded_Line_Of_Code* execution_trace, Decoded_Line_Of_Code *current, u32 P, u32 I) 
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

    if (traced->Simplified_Opcode == PUSH && traced->Decode_Target == I && Count_PUSH_Instructions == 2) 
      Num_PUSH_Matched++; 

    if (Num_PUSH_Matched == 2 && N == 0 && Conditional_Branch_Count == 0) 
      return 3; 
  }
  return 0;
}


u32 Needs_To_Be_Aborted(Decoded_Line_Of_Code* execution_trace, u32 Address_of_H, u32 P, u32 I)
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
      Aborted = Infinite_Simulation_Needs_To_Be_Aborted_Trace(execution_trace, current, P, I); 
    else 
      Aborted = Infinite_Recursion_Needs_To_Be_Aborted_Trace(execution_trace, current);  
  }
  else if (current->Simplified_Opcode == JMP) 
    Aborted = Infinite_Loop_Needs_To_Be_Aborted_Trace(execution_trace, current); 
  return Aborted; 
}  


//
// This is called every time the a line ocf x86 code is emulated 
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
    Aborted = Needs_To_Be_Aborted((Decoded_Line_Of_Code*)*execution_trace, Address_of_H, P, I); 
  } 
  if (Aborted) // 2021-01-26 Must be aborted 
  {
    OutputString(Halt_Decider_Name);
    if (Aborted == 1)
      OutputString("Infinite Loop Detected Simulation Stopped\n\n");
    if (Aborted == 2)
      OutputString("Infinite Recursion Detected Simulation Stopped\n\n"); 
    if (Aborted == 3)
      OutputString("Infinitely Recursive Simulation Detected Simulation Stopped\n\n");
    return 0;        
  }
  return 1;         // 2021-01-26 Need not be aborted
}


// This only works with ONE PARAMETER to the called function 
void Init_slave_state(u32 P, u32 I, u32 End_Of_Code, 
                      Registers* slave_state, u32* slave_stack) 
{
  u32 Top_of_Stack;
  u32 Capacity;
  u32 Size;

  Top_of_Stack = StackPush(slave_stack, I);   // Data for Function to invoke 
  Top_of_Stack = StackPush(slave_stack, End_Of_Code); // Return Address in Halts() 

  SaveState(slave_state);    // Based on this point in execution 
  Capacity = slave_stack[-2]; 
  Size = slave_stack[-1];

  slave_state->EIP = P;      // Function to invoke
  slave_state->ESP = Top_of_Stack; 
  slave_state->EBP = Top_of_Stack; 
}


u32 H(ptr P, ptr I)
{ 
HERE:
  u32 End_Of_Code; 
  u32 Address_of_H;              // 2022-06-17 
  u32 code_end                  = get_code_end((u32)P); 
  Decoded_Line_Of_Code *decoded = (Decoded_Line_Of_Code*) 
                                   Allocate(sizeof(Decoded_Line_Of_Code));  
  Registers*  master_state      = (Registers*) Allocate(sizeof(Registers)); 
  Registers*  slave_state       = (Registers*) Allocate(sizeof(Registers)); 
  u32*        slave_stack       = Allocate(0x10000); // 64k;        
  u32  execution_trace = (u32)Allocate(sizeof(Decoded_Line_Of_Code) * 10000); 
                                  // 1000 lines of x86 code
  __asm lea eax, HERE             // 2022-06-18  
  __asm sub eax, 6                // 2022-06-18 
  __asm mov Address_of_H, eax     // 2022-06-18 
  __asm mov eax, END_OF_CODE 
  __asm mov End_Of_Code, eax 

  Init_slave_state((u32)P, (u32)I, End_Of_Code, slave_state, slave_stack); 
  Output("\nH: Begin Simulation   Execution Trace Stored at:", execution_trace); 
  Output("Address_of_H:", Address_of_H); // 2022-06-11 
  if (Decide_Halting("H: ", &execution_trace, &decoded, code_end, &master_state, 
                     &slave_state, &slave_stack, Address_of_H, (u32)P, (u32)I))
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


void P(ptr x) 
{
  int Halt_Status = H(x, x); 
  if (Halt_Status) 
    HERE: goto HERE; 
  return; 
} 

int main() 
{ 
  Output("Input_Halts = ", H(P, P));  
} 
