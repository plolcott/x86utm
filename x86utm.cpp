/***
  All output functions for lines of x86 code 
  Machine_Code_Out
  Disassembled_line_out
  Disassembled_line_out
  Disassembled_line_source_out
  Disassembled_line_out2
***/ 
//
//  Source code for the x86utm operating system. 
//  Copyright PL Olcott 2020, 2021, 2022 
//  based on libx86emu: x86emu-demo.c
//  https://github.com/wfeldt/libx86emu 
//
#define TRACE_USER_CODE_ONLY 
#define LOCAL_HALT_DECIDER_MODE 
#define MAX_INSTRUCTIONS 20000000 // 10,000,000 
//#define MAX_INSTRUCTIONS 100 // 10,000,000 
//#define DOT_DASH_PREFIX

#ifndef LOCAL_HALT_DECIDER_MODE 
  #define GLOBAL_HALT_DECIDER_MODE 
#endif
//#define DEBUG_FUNCTION_CALLS  

#define HEAP_SIZE  0x1000000 //  16 MB    // 2021-04-22 Increased to 16 MB
#define STACK_SIZE 0x100000  //   1 MB    // 2021-04-22 Increased to  1 MB
#define NOP_OPCODE 0x90
#define INT_3      0xCC 
// The INT3 instruction is a one-byte-instruction defined for use by 
// debuggers to temporarily replace an instruction in a running program 
// in order to set a code breakpoint. The opcode for INT3 is 0xCC. 

#define ZEROES     0x00  
#define HALT       0xF4
#define RETURN     0xC3
#define SPACE      0x20
#define FILL_VALUE NOP_OPCODE
// NOP, no-op, or NOOP (pronounced "no op"; short for no operation) 
// is a machine language instruction and its assembly language mnemonic, 
// programming language statement, or computer protocol command that 
// does nothing.

#define MAX_MACHINE_CODE_BYTES 7 // was 5 or 8
#define NOP_OPCODE 0x90

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "include/Read_COFF_Object.h"
#include "include/x86emu.h" 
#if (_MSC_VER >= 1900)  // (Visual Studio 2015 version 14.0)
#include "include/getopt.h"
#else
#include <getopt.h>
#endif 

struct X86_UTM; 
void Execute_Instruction(x86emu_t *emu, unsigned flags, 
                        X86_UTM& x86utm, COFF_Reader& Reader); 

u32 Allocate(x86emu_t *emu, u32 Heap_PTR_variable, u32 Heap_END_variable, u32 size); 

const u32 EXECUTING = 0;
const u32 HALTED    = 1;
const u32 ABORTED   = 2;


void help(void);
void flush_log(x86emu_t *emu, char *buf, unsigned size);
x86emu_t *emu_new(void);
int emu_init(x86emu_t *emu, char *file);
void emu_run(char *file);

// already defined in x86emu_int.h
#define MODE_HALTED		((emu)->x86.mode & _MODE_HALTED) 


struct option option_list[] = { 
  { "help",       0, NULL, 'h'  }, // List this menu
//{ "load_begin", 1, NULL, 'l'  }, // Offset in the file to begin loading 
  { "code_begin", 1, NULL, 'b'  }, // Offset in the file of Code Segment
  { "code_size",  1, NULL, 's'  }, // Size of Code Segment
  { "code_run",   1, NULL, 'r'  }, // Offset in the file to Begin Executing  
  { "max",        1, NULL, 'm'  }, // Maximum number of instructions to execute
  { 0,0,0,0 }  // ISO C forbids empty initializer braces 
};


struct Command_Line_Options {  // gcc C++ requires a name 
//unsigned load_begin; 
  unsigned code_begin; // Code segment begin address
  unsigned code_size;  // Code segment size
  unsigned code_run;   // 
  unsigned max_instructions;
  char *file;
} options;


//
// Difference between Windows and Linux internal naming conventions
//
//#ifdef _WIN32
/***
  #define MAIN_NAME                         "_main"     // 2022-08-14
  #define CODE_SEGMENT_NAME                 ".text$mn"  // 2022-08-14
  #define Heap_PTR_NAME                     "_Heap_PTR" // 2022-08-14
  #define Heap_END_NAME                     "_Heap_END" // 2022-08-14
***/ 
//#define Global_Execution_Trace_List_Index_NAME "_Global_Execution_Trace_List_Index"
//#define Global_Execution_Trace_List_NAME       "_Global_Execution_Trace_List"
//#define Local_Halt_Decider_Mode_NAME           "_Local_Halt_Decider_Mode"

//#define Current_Emulation_Level_NAME      "_Current_Emulation_Level"
//#define Global_Abort_Flag_NAME            "_Global_Abort_Flag"
//#define Debug_Trace_State_NAME            "_debug_trace_state"

  #define Halts_NAME                        "_Halts"
  #define Output_Debug_Trace_NAME           "_Output_Debug_Trace"
  #define Decode_Line_Of_Code_NAME          "_Decode_Line_Of_Code" 
  #define Output_Decoded_Instructions_NAME  "_Output_Decoded_Instructions"  // Made Obsolete

  #define Copy_Code_NAME                    "_Copy_Code"
  #define Identical_Code_NAME               "_Identical_Code"

  #define SaveState_NAME                    "_SaveState"
  #define LoadState_NAME                    "_LoadState"
  #define Allocate_NAME                     "_Allocate"
  #define DebugStep_NAME                    "_DebugStep"
  #define OutputString_NAME                 "_OutputString"
  #define OutputHex_NAME                    "_OutputHex"
  #define Output_NAME                       "_Output"
// New Virtual Machine Instructions  
  #define PushBack_NAME                     "_PushBack"
  #define StackPush_NAME                    "_StackPush"
  #define Output_Regs_NAME                  "_Output_Registers"  
  #define Global_Halts_NAME                 "_Global_Halts"
  #define get_code_end_NAME                 "_get_code_end"
  #define Last_Address_Of_OS_NAME           "_Last_Address_Of_Operating_System"

  #define Output_Saved_Regs_NAME            "_Output_Saved_Registers"
  #define Output_Stack_NAME                 "_Output_Stack"
  #define Output_slave_stack_NAME           "_Output_slave_stack"
  #define Output_integer_list_NAME          "_Output_integer_list"  // Model for add new VMI 
  #define Output_Decoded_NAME               "_Output_Decoded"       // Model for add new VMI 
//#define Simulate_NAME                     "_Simulate"  
/***
#elif __linux__
  #define MAIN_NAME                         "main"
  #define CODE_SEGMENT_NAME                 ".text"
  #define Heap_PTR_NAME                     "Heap_PTR"
  #define Heap_END_NAME                     "Heap_END"

//#define Global_Execution_Trace_List_Index_NAME "Global_Execution_Trace_List_Index"
//#define Global_Execution_Trace_List_NAME       "Global_Execution_Trace_List" 
//#define Local_Halt_Decider_Mode_NAME           "Local_Halt_Decider_Mode"

//#define Current_Emulation_Level_NAME      "Current_Emulation_Level"
//#define Global_Abort_Flag_NAME            "Global_Abort_Flag" 
//#define Debug_Trace_State_NAME            "debug_trace_state"

  #define Halts_NAME                        "Halts"
  #define Output_Debug_Trace_NAME           "Output_Debug_Trace"

  #define Copy_Code_NAME                    "Copy_Code"
  #define Identical_Code_NAME               "Identical_Code"

  #define SaveState_NAME                    "SaveState"
  #define LoadState_NAME                    "LoadState"
  #define Allocate_NAME                     "Allocate"
  #define DebugStep_NAME                    "DebugStep"
  #define OutputString_NAME                 "OutputString"
  #define OutputHex_NAME                    "OutputHex"
  #define Output_NAME                       "Output"
// New Virtual Machine Instructions 
  #define PushBack_NAME                     "PushBack"
  #define StackPush_NAME                    "StackPush"
  #define Output_Regs_NAME                  "Output_Registers"
  #define Global_Halts_NAME                 "Global_Halts"
  #define get_code_end_NAME                 "get_code_end"
  #define Last_Address_Of_OS_NAME           "Last_Address_Of_Operating_System"

  #define Output_Saved_Regs_NAME            "Output_Saved_Registers"
  #define Output_Stack_NAME                 "Output_Stack"
  #define Output_slave_stack_NAME           "Output_slave_stack"
  #define Output_integer_list_NAME          "Output_integer_list"
  #define Decode_Line_Of_Code_NAME          "Decode_Line_Of_Code" 
  #define Output_Decoded_Instructions_NAME  "Output_Decoded_Instructions"  // Made Obsolete
  #define Output_Decoded_NAME               "Output_Decoded"  // Model for add new VMI 
//#define Simulate_NAME                     "Simulate"
#endif
***/ 

// 
// Determine if Function_Name is a Virtual Machine Instruction
//
bool Is_Virtual_Machine_Instruction(std::string Function_Name)
{
  if (Function_Name == Copy_Code_NAME)
    return true;

  if (Function_Name == Identical_Code_NAME)
    return true;

  if (Function_Name == SaveState_NAME)
    return true;
  
  else if (Function_Name == LoadState_NAME)
    return true;
  
  else if (Function_Name == Allocate_NAME)
    return true;
  
  else if (Function_Name == DebugStep_NAME)
    return true;

  else if (Function_Name == Output_Debug_Trace_NAME)
    return true;

  else if (Function_Name == OutputString_NAME)
    return true;

  else if (Function_Name == OutputHex_NAME)
    return true;

  else if (Function_Name == Output_NAME)
    return true;

  else if (Function_Name == PushBack_NAME)
    return true;

  else if (Function_Name == StackPush_NAME)
    return true;

  else if (Function_Name == Output_Regs_NAME)
    return true;

  else if (Function_Name == Global_Halts_NAME)
    return true;

  else if (Function_Name == get_code_end_NAME)
    return true;

  else if (Function_Name == Last_Address_Of_OS_NAME)  // 2021-08-26 
    return true;                                      // 2021-08-26 

  else if (Function_Name == Output_Saved_Regs_NAME)
    return true;

  else if (Function_Name == Output_Stack_NAME)
    return true;

  else if (Function_Name == Output_slave_stack_NAME)
    return true;

  else if (Function_Name == Output_integer_list_NAME)
    return true;

  else if (Function_Name == Decode_Line_Of_Code_NAME) 
    return true;

  else if (Function_Name == Output_Decoded_Instructions_NAME) // Made Obsolete
    return true;

  else if (Function_Name == Output_Decoded_NAME) 
    return true;

//else if (Function_Name == Simulate_NAME)
//  return true; 
  return false; 
}


/*
 * Parse options, then run emulation.
 */
int main(int argc, char **argv)
{
  int i;
  char *str;

//options.max_instructions = 10000000;

  opterr = 0;


  while((i = getopt_long(argc, argv, "hb:s:r:m:", option_list, NULL)) != -1) 
  {
    switch(i) {

//    case 'l':
//      options.load_begin = strtoul(optarg, NULL, 0);
//      printf("--options.load_begin[%08x]\n", options.load_begin); 
//      break;

      case 'b':
        options.code_begin = strtoul(optarg, NULL, 0);
        printf("--options.code_begin[%08x]\n", options.code_begin); 
        break;

      case 's':
        options.code_size = strtoul(optarg, &str, 0);
        printf("--options.code_size.[%08x]\n", options.code_size); 

        break;

      case 'r':
        options.code_run = strtoul(optarg, NULL, 0); 
        printf("--options.code_run..[%08x]\n", options.code_run); 
        break;

      case 'm':
        options.max_instructions = strtoul(optarg, NULL, 0);
        printf("options.max_instructions(%d)\n", options.max_instructions); 
        break;

      default:
        printf("default[%08x]\n", i);  
        help();
        return i == 'h' ? 0 : 1;
    }
  }

  if(argc == optind + 1) {
    options.file = argv[optind];
  }
  else {
    help();
    return 1;
  }

  emu_run(options.file);

  return 0;
}


/*
 * Display short usage message.
 */
void help()
{
  printf(
    "Usage: x86emu-demo [OPTIONS] FILE\n"
    "\n"
    "Load FILE and run x86 emulation.\n"
   "\n"
    "Options:\n"
//  "  -l, --load  ADDRESS  Load FILE at ADDRESS into memory.\n" 
    "  -b, --begin ADDRESS  Address of beginning of code segment."
    "   (for disassembly listing)\n"  
    "  -s, --size  ADDRESS  Size of code segment."
    "                   (for disassembly listing)\n"  
    "  -r, --run   ADDRESS  Start emulation at ADDRESS.\n" 
    "  -m, --max   N        Stop after emulating N instructions.\n" 
    "  -h, --help           Show this text\n" 
  );
}


/*
 * Write emulation log to console.
 */
void flush_log(x86emu_t *emu, char *buf, unsigned size)
{
  if(!buf || !size) return;

  fwrite(buf, size, 1, stdout);
}


/*
 * Create new emulation object.
 */
x86emu_t *emu_new()
{
  x86emu_t *emu = x86emu_new(X86EMU_PERM_R | X86EMU_PERM_W | X86EMU_PERM_X, 0);

  /* log buf size of 1000000 is purely arbitrary */
  x86emu_set_log(emu, 1000000, flush_log);

  emu->log.trace = X86EMU_TRACE_DEFAULT;

  return emu;
}


/*
 * Setup registers and memory.
 */
int emu_init(x86emu_t *emu, COFF_Reader& Reader)
{
  x86emu_set_seg_register(emu, emu->x86.R_CS_SEL, 0); // 2020-10-06
//emu->x86.R_EIP = options.code_start;  // 2020-08-11
//if(options.bits_32) {
    /* set default data/address size to 32 bit */
    emu->x86.R_CS_ACC |= (1 << 10);
    emu->x86.R_SS_ACC |= (1 << 10);
    /* maximize descriptor limits */
    emu->x86.R_CS_LIMIT =
    emu->x86.R_DS_LIMIT =
    emu->x86.R_ES_LIMIT =
    emu->x86.R_FS_LIMIT =
    emu->x86.R_GS_LIMIT =
    emu->x86.R_SS_LIMIT = ~0;
  
//printf("code_begin[%08x] code_size[%08x]  code_run[%08x] max_instructions(%d)\n",
//        options.code_begin, options.code_size, options.code_run, 
// options.max_instructions);

// Write whole COFF file to emux86 Memory
  for (unsigned int addr = 0; addr < Reader.ObjectCode.size(); addr++) 
    x86emu_write_byte(emu, addr, Reader.ObjectCode[addr]);

  // Initialize Heap to x86emu Memory
  u32 Heap_PTR_value = Reader.ObjectCode.size();     // Heap_PTR Value
  u32 Heap_END_value = Heap_PTR_value + HEAP_SIZE;   // Heap_END Value
  for (unsigned int addr = Heap_PTR_value; addr < Heap_END_value; addr++) 
    x86emu_write_byte(emu, addr, FILL_VALUE);  // Fill Mmeory with No Ops
  return 1;
}


// FUNCTIONS THAT IMPLEMENT THE X86UTM OPERATING SYSTEM 
// FUNCTIONS THAT IMPLEMENT THE X86UTM OPERATING SYSTEM 
// FUNCTIONS THAT IMPLEMENT THE X86UTM OPERATING SYSTEM 
// FUNCTIONS THAT IMPLEMENT THE X86UTM OPERATING SYSTEM 
// FUNCTIONS THAT IMPLEMENT THE X86UTM OPERATING SYSTEM 

#define   JMP 0xEB // Simplifed OpCode for all forms of JMP
#define  CALL 0xE8 // Simplifed OpCode for all forms of CALL
#define   JCC 0x7F // Simplifed OpCode for all forms of Jump on Condition
#define   RET 0xC3 // Simplifed OpCode for all forms of Return 
#define   HLT 0xF4 // Conventional OpCode for Halt 
#define  PUSH 0x68 // Conventional OpCode for Halt 
#define   MOV 0xC7 // Conventional OpCode for Halt 
#define OTHER 0xFF // Not a Control Flow Insrtuction 


const char* Get_Simplified_Opcode_String(u32 Simplified_Opcode)
{
  switch(Simplified_Opcode)
  {
  case JMP:
    return "JMP "; 
  case CALL:
    return "CALL";
  case JCC:
    return "JCC ";
  case RET:
    return "RET ";
  case HLT:
    return "HLT ";
  case PUSH:
    return "PUSH";
  case MOV:
    return "MOV"; 
  default:
    return "????";
  }
}

struct Line_Of_Code 
{
  u32 Address;
  u32 ESP; // Current Stack Pointer
  u32 TOS; // Current Value on the top of the stack
  u32 NumBytes;
  u32 Simplified_Opcode; // {JMP, CALL, JCC, RET, HLT}
  u32 Decode_Target;     // Target of {JMP, CALL, JCC} 
  std::string Disassembly_text; 
  Line_Of_Code()  
  {
    this->Address           = 0;
    this->ESP               = 0;
    this->TOS               = 0; 
    this->NumBytes          = 0;
    this->Simplified_Opcode = 0; 
    this->Decode_Target     = 0; 
  };

  // Uses the C version FROM x86emu.h for intialization.  
  Line_Of_Code(const LineOfCode& loc)  
  {
    this->Address           = loc.Address;
    this->ESP               = loc.ESP;
    this->TOS               = loc.TOS; 
    this->NumBytes          = loc.NumBytes;
    this->Simplified_Opcode = 0;  
    this->Decode_Target     = 0;  
    this->Disassembly_text  = loc.Disassembly_text;  
  };
  // Uses the C version FROM x86emu.h for intialization. 
  Line_Of_Code(const Line_Of_Code& loc)  
  {
    this->Address           = loc.Address;
    this->ESP               = loc.ESP;
    this->TOS               = loc.TOS; 
    this->NumBytes          = loc.NumBytes;
    this->Simplified_Opcode = loc.Simplified_Opcode;  
    this->Decode_Target     = loc.Decode_Target;   
    this->Disassembly_text  = loc.Disassembly_text;  
  }; 
  void out(x86emu_t *emu);
};


// Used by Local Halt Decider in the emulated code
struct Decoded_Line_Of_Code 
{
  u32 Address;
  u32 ESP; 
  u32 TOS; // value on top of stack 
  u32 NumBytes;
  u32 Simplified_Opcode; // {JMP, CALL, JCC, RET, HLT}
  u32 Decode_Target;     // Target of {JMP, CALL, JCC} 
  Decoded_Line_Of_Code(Line_Of_Code & loc)
  {
    this->Address           = loc.Address;
    this->ESP               = loc.ESP; 
    this->TOS               = loc.TOS; 
    this->NumBytes          = loc.NumBytes;
    this->Simplified_Opcode = loc.Simplified_Opcode; 
    this->Decode_Target     = loc.Decode_Target;     
  }
  void Out() 
  {
#ifdef DOT_DASH_PREFIX
   printf("...[%08x][%08x][%08x](%02d) %02x %08x\n", 
     Address, ESP, TOS, NumBytes, Simplified_Opcode, Decode_Target); 
#else
   printf("[%08x][%08x][%08x](%02d) %02x %08x\n", 
     Address, ESP, TOS, NumBytes, Simplified_Opcode, Decode_Target); 
#endif 
  }
}; 


std::vector<Line_Of_Code> Execution_Trace; 


void Decode_Control_Flow_Instruction(x86emu_t *emu, Line_Of_Code& loc) 
{
  u8 Opcode_1 = x86emu_read_byte(emu, loc.Address);
  u8 Opcode_2 = x86emu_read_byte(emu, loc.Address + 1);
  u32 Target = 0x777; // handle possibly unintialized warning 2022-08-07 see below
//printf("Decode_Control_Flow_Instruction([%08x][%02x][%02x])\n", 
//     loc.Address, Opcode_1, Opcode_2);

  std::string Operator_Name;  // Get Operator Name
  for (u32 N = 0; N < loc.Disassembly_text.size() && 
                      loc.Disassembly_text[N] != SPACE; N++)
    Operator_Name += loc.Disassembly_text[N];

  if (Operator_Name == "jmp")        // JMP IS DECODED
  {
    if (loc.NumBytes == 2 && Opcode_1 == 0xEB)
    {
      s8 Offset = x86emu_read_byte(emu, loc.Address+1);
      Target = loc.Address + loc.NumBytes + Offset; 
    }
    else if (loc.NumBytes == 5 && Opcode_1 == 0xE9)
    {
      s32 Offset = x86emu_read_dword(emu, loc.Address+1);
      Target = loc.Address + loc.NumBytes + Offset; 
    }
    else 
      printf("[%08x] JMP not decoded correctly! [%02x](%d)\n", 
             loc.Address, Opcode_1, loc.NumBytes); 
    loc.Simplified_Opcode = JMP;     // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = Target;  // Target of {JMP, CALL, JCC} 
  }

  else if (Operator_Name == "mov")  // MOV IS DECODED       // 2023-07-01
  {
    if (loc.NumBytes == 7 && Opcode_1 == 0xC7 && Opcode_2 == 0x45)
    {
      s8  Offset    = x86emu_read_byte(emu, loc.Address+2);
      u32 address   = emu->x86.R_EBP + Offset; 
      u32 immediate = x86emu_read_dword(emu, loc.Address+3); 
      u32 data      = x86emu_read_dword(emu, address); 
//    printf("---[%08x] mov [%08x], %08x\n",     // 2023-07-01
//            loc.Address, address, immediate);  // 2023-07-01
    }
//  [00001ff4] c7 45 dc 78563412 mov [ebp-24],12345678
//  printf("[%08x] mov instruction[%02x][%02x](%d)\n",      // 2023-07-01
//          loc.Address, Opcode_1, Opcode_2, loc.NumBytes); // 2023-07-01
  }

  else if (Operator_Name == "call")  // CALL IS DECODED 
  {
    if (loc.NumBytes == 5 && Opcode_1 == 0xE8)
    {
      s32 Offset = x86emu_read_dword(emu, loc.Address+1);
      Target     = loc.Address + loc.NumBytes + Offset; 
    }


//  call dword [ebp+08] is decoded here
//  We are not going to decode other combinations until they are needed
//  Opcode_2 contains register number: 0x50-0x57: {EAX ECX EDX EBX ESP EBP ESI EDI}
    else if (loc.NumBytes == 3 && Opcode_1 == 0xFF && Opcode_2 == 0x55)
    {
      s8 Offset = x86emu_read_byte(emu, loc.Address+2);
      Target    = x86emu_read_dword(emu, emu->x86.R_EBP + Offset); 
//    printf("CD_Decode_Control_Flow_Instruction([%02d][%08x][%08x])\n", 
//            Offset, emu->x86.R_EBP, Target);
      printf("[%08x] call (absolute) [%08x]\n", loc.Address, Target); 
      if (Target == 0)
        exit(0); 
    } 

    else if (loc.NumBytes == 6 && Opcode_1 == 0xFF && Opcode_2 == 0x15)
    {
//    [00103978][0015e434][0010397e] ff15 b301 0000         call dword [000001b3]
      s32 Offset = x86emu_read_dword(emu, loc.Address+2);
      Target     = x86emu_read_dword(emu, Offset); 
      printf("CD_Decode_Control_Flow_Instruction([%02d][%08x][%08x])\n", 
              Offset, emu->x86.R_EBP, Target);
      printf("[%08x] call (absolute) [%08x]\n", loc.Address, Target); 
      if (Target == 0)
        exit(0); 
    } 

    else 
      printf("[%08x] CALL not decoded correctly!\n", loc.Address); 
    loc.Simplified_Opcode = CALL;     // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = Target;  // Target of {JMP, CALL, JCC} 
  }

  else if (Operator_Name == "push")  // CALL IS DECODED 
  {
    if (loc.NumBytes == 5 && Opcode_1 == 0x68)
      loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}
    else if (loc.NumBytes == 1 && (Opcode_1 >= 0x50 && Opcode_1 <= 0x57)) 
      loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}

    else if (loc.NumBytes == 3 && Opcode_1 >= 0xff && Opcode_2 == 0x75) 
      loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}

    else if (loc.NumBytes == 2 && (Opcode_1 >= 0x6a)) 
      loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}

    else if (loc.NumBytes == 6 && Opcode_1 >= 0xff && Opcode_2 == 0x35) 
    {
      loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}
      u32 Target = x86emu_read_dword(emu, loc.Address+2);
      loc.Decode_Target     = Target;
      return; // prevent fall through to wrong assignment of loc.Decode_Target
    } 
    else 
    {
      printf("[%08x](%d) PUSH not decoded correctly!\n", loc.Address, loc.NumBytes); 
      printf("Here are the bytes:");
      for (u32 N = 0; N < loc.NumBytes; N++)
        printf("[%02x]", x86emu_read_byte(emu, loc.Address+N)); 
      printf("\n");
      exit(0); // Make sure that user notices this error
    }
//  loc.Simplified_Opcode = PUSH; // {JMP, CALL, JCC, RET, HLT, PUSH}
    loc.Decode_Target     = loc.TOS;    //     
  }

  // Single Opcode JCC (Jump-Condition-Code)
  else if (Opcode_1 > 0x70 && Opcode_1 <= 0x7f) 
  {
    if (loc.NumBytes == 2)
    {
      s8 Offset = x86emu_read_byte(emu, loc.Address+1);
      Target = loc.Address + loc.NumBytes + Offset; 
    }
    else
      printf("[%08x] JCC not decoded correctly!\n", loc.Address); 
    loc.Simplified_Opcode = JCC;     // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = Target;  // Target of {JMP, CALL, JCC} 
  }

  // Double Opcode JCC (Jump-Condition-Code)
  else if (Opcode_1  == 0x0f && (Opcode_2 >= 0x80 && Opcode_2 <= 0x8f))   
  {
    if (loc.NumBytes == 6)
    {
      s32 Offset = x86emu_read_dword(emu, loc.Address+2);
      Target = loc.Address + loc.NumBytes + Offset; 
    }
    else
      printf("[%08x] JCC not decoded correctly! [%02x][%02x](%d)\n",
              loc.Address, Opcode_1, Opcode_2, loc.NumBytes); 
    loc.Simplified_Opcode = JCC;     // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = Target;  // Target of {JMP, CALL, JCC}  // handle possibly unintialized warning 2022-08-07 HERE
  }

  else if (Operator_Name == "ret")
  {
    loc.Simplified_Opcode = RET;   // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = 0;     // Target of {JMP, CALL, JCC} 
  }

  else if (Operator_Name == "hlt")
  {
    loc.Simplified_Opcode = HLT;   // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = 0;     // Target of {JMP, CALL, JCC} 
  }
  
  else  // Not a Control Flow Instruction 
  {
    loc.Simplified_Opcode = OTHER;     // {JMP, CALL, JCC, RET, HLT}
    loc.Decode_Target     = 0;     // Target of {JMP, CALL, JCC} 
  }
/***
  printf("Decode_Control_Flow_Instruction([%08x][%02x][%02x])  [%02x][%08x]\n", 
         loc.Address, Opcode_1, Opcode_2, loc.Simplified_Opcode, loc.Decode_Target);
***/ 
}


struct X86_UTM 
{
  std::vector<Line_Of_Code> Disassembly;
  u32 code_begin; 
  u32 code_end; 
  u32 code_run; 
  void Disassembly_Out(X86_UTM x86utm){ } 
}; 


void Machine_Code_Out(x86emu_t *emu, u32 Address, u32 Length) 
{
  if (Length > MAX_MACHINE_CODE_BYTES)
    for (u32 N = 0; N < MAX_MACHINE_CODE_BYTES; N++)
      printf("??"); 
  else
  {
    for (u32 N = Address; N < Address + Length; N++)
      printf("%02x", x86emu_read_byte(emu, N));  
    for (u32 N = Length; N < MAX_MACHINE_CODE_BYTES; N++)
      printf("  "); 
  }
}


//
// Disassembly AFTER the instruction has been executed
//
void Disassembled_line_out(x86emu_t *emu)
{ 
Line_Of_Code loc(emu->line_of_code); 
//printf("Disassembled_line_out (1)\n");

//printf("[%08x][%08x](%02d)  ", 
//        emu->line_of_code.Address, emu->line_of_code.ESP, 
//printf("...[%08x][%08x][%08x](%02d)  ", 
#ifdef DOT_DASH_PREFIX
  printf("...[%08x][%08x][%08x] ", 
          emu->line_of_code.Address, 
          emu->line_of_code.ESP, 
          emu->line_of_code.TOS); 
#else
  printf("[%08x][%08x][%08x] ", 
          emu->line_of_code.Address, 
          emu->line_of_code.ESP, 
          emu->line_of_code.TOS); 
#endif
  Machine_Code_Out(emu, emu->line_of_code.Address, 
                   emu->line_of_code.NumBytes); 
  printf(" %s\n", emu->line_of_code.Disassembly_text);  
}


//
// Disassembly AFTER the instruction has been executed
//
void Disassembled_line_out(x86emu_t *emu, Line_Of_Code& loc)
{ 
//printf("Disassembled_line_out (2)\n");
//printf("[%08x](%02d)  ", loc.Address, loc.NumBytes); 

#ifdef DOT_DASH_PREFIX
  printf("...[%08x][%08x][%08x] ", 
          loc.Address, 
          loc.ESP, 
          loc.TOS); 
#else
  printf("[%08x][%08x][%08x] ", 
          loc.Address, 
          loc.ESP, 
          loc.TOS); 
#endif

  Machine_Code_Out(emu, loc.Address, loc.NumBytes); 
  printf(" %s\n", loc.Disassembly_text.c_str()); 
}


//
// Disassembly AFTER the instruction has been executed
//
void Disassembled_line_source_out(x86emu_t *emu, Line_Of_Code& loc)
{ 
//printf("Disassembled_line_source_out\n");
//printf("[%08x](%02d)  ", loc.Address, loc.NumBytes); 
  printf("[%08x] ", loc.Address); 
  Machine_Code_Out(emu, loc.Address, loc.NumBytes); 
  printf(" %s\n", loc.Disassembly_text.c_str()); 
}


//
// Disassembly AFTER the instruction has been executed
//
void Disassembled_line_out2(x86emu_t *emu, Line_Of_Code& loc)
{ 
//printf("Disassembled_line_out2\n");
//printf("---[%08x][%08x][%08x](%02d)  ", 
//  loc.Address, loc.ESP, loc.TOS, loc.NumBytes); 
#ifdef DOT_DASH_PREFIX
  printf("---[%08x][%08x][%08x] ", loc.Address, loc.ESP, loc.TOS); 
#else
  printf("[%08x][%08x][%08x] ", loc.Address, loc.ESP, loc.TOS); 
#endif
  Machine_Code_Out(emu, loc.Address, loc.NumBytes); 
  printf(" %s\n", loc.Disassembly_text.c_str()); 
}


// We need this to obtain the function call of a Virtual Machine Instruction. 
Line_Of_Code& Get_Line_Of_Code_From_Machine_Address(x86emu_t *emu, 
              X86_UTM& x86utm, COFF_Reader& Reader, u32 Machine_Address) 
{
  static Line_Of_Code Empty; 
  for (u32 N = 0; N < x86utm.Disassembly.size(); N++)
    if (x86utm.Disassembly[N].Address == Machine_Address)
      return x86utm.Disassembly[N];
  return Empty;   // Should Never Occur
}

void output_numbytes(const char* CH, int numbytes)
{
  for (int N = 1; N <= numbytes; N++)
    printf("%s", CH);
}


void Disassembly_Listing(x86emu_t *emu, X86_UTM& x86utm, COFF_Reader& Reader) 
{
  std::map<u32, Function_Info>::iterator Found;
  printf("===============================\n");
  printf("Disassembly Listing\n"); 
  u32 size_in_bytes    = 0;  
  u32 function_address = 0; 
  for (u32 N = 0; N < x86utm.Disassembly.size(); N++)
  {
//  u32 size_in_bytes = 0; 
    u32 OpCode = x86emu_read_byte(emu, x86utm.Disassembly[N].Address);
//  printf("OpCode (%02x)\n", OpCode); 

    if (OpCode != INT_3)
    {
      u32 Address = x86utm.Disassembly[N].Address; 
      Found = Reader.FunctionNames.find(Address);
      if(Found != Reader.FunctionNames.end()) 
      {
        printf("%s()\n", Found->second.Name.c_str()); 
        function_address = x86utm.Disassembly[N].Address; 
      }
//    Disassembled_line_out(emu, x86utm.Disassembly[N]); 
      Disassembled_line_source_out(emu, x86utm.Disassembly[N]); 
      size_in_bytes += x86utm.Disassembly[N].NumBytes; 
    }

     if (OpCode == RETURN) 
     {
       printf("Size in bytes:(%04d) [%08x]\n\n", size_in_bytes, x86utm.Disassembly[N].Address);
       Found = Reader.FunctionNames.find(function_address);
       if(Found != Reader.FunctionNames.end()) 
       {
         Found->second.code_end = x86utm.Disassembly[N].Address;  // 2021-03-18 
//       printf("[%08x] %s [%08x]\n\n", Found->first, Found->second.Name.c_str(), Found->second.code_end);
       }
       size_in_bytes = 0;  
     }
  }
//printf("===============================\n");
/***
 printf(" machine   stack     stack     machine    assembly\n");
 printf(" address   address   data      code       language\n");
 printf(" ========  ========  ========  =========  =============\n");
***/ 
 
 printf(" machine   stack     stack     machine     "); 
   output_numbytes(" ", (MAX_MACHINE_CODE_BYTES * 2) - 11); 
   printf("assembly\n");

 printf(" address   address   data      code        "); 
   output_numbytes(" ", (MAX_MACHINE_CODE_BYTES * 2) - 11); 
   printf("language\n");

 printf(" ========  ========  ========  "); 
   output_numbytes("=", (MAX_MACHINE_CODE_BYTES * 2)); 
   printf(" =============\n");
}


//
// Finds the FIRST and LAST ADDRESS of the named function.
// Return the value of the LAST ADDRESS.
//
// This LAST ADDRESS is used by the global halt decider to 
// ignore the exection trace of the halt decider and the 
// operating system function calls. 
//
u32 Disassembly_Listing_Find_Function(x86emu_t *emu, 
                                      X86_UTM& x86utm, 
                                      COFF_Reader& Reader, 
                                      std::string Function_Name) 
{
  u32 Address = 0;
  bool Found_Named_Function = false; 
  std::map<u32, Function_Info>::iterator Found;
//printf("Disassembly_Listing_Find_Function\n"); 
  for (u32 N = 0; N < x86utm.Disassembly.size(); N++)
  {
    u32 OpCode = x86emu_read_byte(emu, x86utm.Disassembly[N].Address);
    if (OpCode != INT_3)
    {
      Address = x86utm.Disassembly[N].Address; 
      Found = Reader.FunctionNames.find(Address);
      if(Found != Reader.FunctionNames.end()) 
      {
        if (Function_Name == Found->second.Name.c_str())
        {
          Found_Named_Function = true; 
//        printf("Found The Function: [%s](%s)\n", Found->second.Name.c_str(), 
//                Function_Name.c_str()); 
//        Disassembled_line_out(emu, x86utm.Disassembly[N]); 
        }
      }
    }

   if (OpCode == RETURN && Found_Named_Function) 
   {
//  Disassembled_line_out(emu, x86utm.Disassembly[N]); 
//  printf("===============================\n\n");
    return Address; 
   }
  }
  exit(0); 
  return 0xffffffff; // Should never get here
}



u32 get_disassembly_listing(unsigned flags, COFF_Reader& Reader, X86_UTM& x86utm)
{
  // Microsoft "C" uses the the 0xCC "int 3" OpCode as a padding byte between 
  // functions. The x86emu disassembler gets confused by this padding character
  // so we change it to the 0x90 "nop" Opcode. 
//printf("get_disassembly_listing()\n"); 
  for (u32 N = x86utm.code_begin; N <= x86utm.code_end; N++)
    if (Reader.ObjectCode[N] == RETURN) 
    {
      N++;
      while (Reader.ObjectCode[N] == INT_3)
         Reader.ObjectCode[N++] = FILL_VALUE;
    }

  printf("code_begin[%08x]  code_end[%08x]\n", x86utm.code_begin, x86utm.code_end);
  x86emu_t *emu2 = emu_new();
  int ok = 0;
  ok = emu_init(emu2, Reader);
  if (!ok)
    return 0; 

  emu2->max_instr = 1;                 // 2020-05-26
  emu2->x86.R_EIP = x86utm.code_begin;        // 2020-07-25
  u32 Counter = 0; // eliminate warning 2022-08-07 
  while (emu2->x86.R_EIP <= x86utm.code_end)  // 2020-05-26
  {					                   // 2020-05-26
//  printf("EIP[%08x]----", emu2->x86.R_EIP, Counter++);  
//  printf("%s\n", emu->line_of_code.Disassembly_text); 

    // This calls the disassembly printf in decode x86emu_run()
    x86emu_run(emu2, flags, TRUE);     
//  Disassembled_line_out(emu2);
    Line_Of_Code loc(emu2->line_of_code); 
    x86utm.Disassembly.push_back(loc);
    emu2->max_instr++;                 // 2020-05-26
  }                                    // 2020-05-26
//disassembly_listing_out(emu2); 
//emu->Disassembly = emu2->Disassembly; 
  x86emu_done(emu2); 
//printf("===============================\n");
  return 1; 
}


// Resets to Not Halted
void ResetHalted(x86emu_t *emu) 
{
//printf("(1)emu->x86.mode:%08x\n", emu->x86.mode);
#ifdef DEBUG_FUNCTION_CALLS
  printf("_MODE_HALTED:%08x  ~_MODE_HALTED:%08x\n",
          _MODE_HALTED, ~_MODE_HALTED);
#endif 
  emu->x86.mode &= ~_MODE_HALTED;
//printf("(2)emu->x86.mode:%08x\n", emu->x86.mode);
}


void memory_block_out(x86emu_t *emu, u32 BytesRead)
{
  printf("; - - memory\n");
  printf(";           0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f");
  for (u32 N = 0; N < BytesRead; N++)
  {
    if (N == 0 || N % 16 == 0)
      printf("\n%08x:  %02x", N, x86emu_read_byte(emu, N)); 
    else 
      printf("  %02x", x86emu_read_byte(emu, N)); 
  }
  printf("\n"); 
}


// Address of Heap_PTR Variable
void heap_memory_out(x86emu_t *emu, COFF_Reader& Reader) 
{
  u32 First_Address = Reader.BytesRead / 0x10 * 0x10;
  u32 Num_Spaces    = Reader.BytesRead - First_Address; 
//printf("Reader.BytesRead[%08x]  First_Address[%08x]  Num_Spaces[%08x]\n", 
//Reader.BytesRead, First_Address, Num_Spaces); 
  u32 HeapTop = x86emu_read_dword(emu, Reader.Heap_PTR_variable); 
  printf("; - - heap\n");
  printf(";           0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f");
  for (u32 N = Reader.BytesRead; N < HeapTop; N++)
  {
    if (Num_Spaces)
    {
      printf("\n%08x:", First_Address); 
      for (u32 S = 1; S <= Num_Spaces; S++)
        printf("    "); 
      Num_Spaces = 0; 
    }
    if (N % 16 == 0)
      printf("\n%08x:  %02x", N, x86emu_read_byte(emu, N)); 
    else 
      printf("  %02x", x86emu_read_byte(emu, N)); 
  }
  printf("\n"); 
}






// VIRTUAL MACHINE INSTRUCTIONS 
// VIRTUAL MACHINE INSTRUCTIONS 
// VIRTUAL MACHINE INSTRUCTIONS 
// VIRTUAL MACHINE INSTRUCTIONS 
// VIRTUAL MACHINE INSTRUCTIONS 

void OutputString(x86emu_t *emu, u32 PTR)
{
  u8 CH = x86emu_read_byte(emu, PTR); 
  while (CH != 0)
  { 
    printf("%c", CH);
    CH = x86emu_read_byte(emu, ++PTR);
  }
//printf("\n"); // 2022-06-27
}


void OutputHex(x86emu_t *emu, u32 H)
{
  printf("%x\n", H); 
}


void Output(x86emu_t *emu, u32 PTR, u32 H)
{
  u8 CH = x86emu_read_byte(emu, PTR++); 
  while (CH != 0)
  { 
    printf("%c", CH);
    CH = x86emu_read_byte(emu, PTR++);
  }
  printf("%x\n", H);
}


/***
//  
// Works like std::vector
// 
void PushBack(x86emu_t *emu, u32 integer_list, u32 M)
{
  s32 Capacity = x86emu_read_dword(emu, (u32)(integer_list-8)); // In bytes
  s32     Size = x86emu_read_dword(emu, (u32)(integer_list-4)); // In bytes
  s32 MaxSize  = Capacity - 4;
#ifdef DEBUG_FUNCTION_CALLS
//printf("PushBack: integer_list[%08x].PushBack(%x) Capacity(%d)"
//       " Size(%d) MaxSize(%d)\n", 
//        (u32)integer_list, M, Capacity, Size, MaxSize);
#endif 
  if (Size <= MaxSize)  // Room for one more u32
  { 
    x86emu_write_dword(emu, (u32)(integer_list + Size),  M); 
    x86emu_write_dword(emu, (u32)(integer_list - 4),  Size + 4); 
  }
  else
  {
    printf("Ran out of memory@ Address[%08x]\n", (u32)integer_list);
    exit(0); 
  }
}
***/ 



//  
// Works like std::vector
// 
void PushBack(x86emu_t *emu, u32 stdvector, u32 data_ptr, u32 size_in_bytes)
{
  s32 Capacity = x86emu_read_dword(emu, (u32)(stdvector-8)); // In bytes
  s32     Size = x86emu_read_dword(emu, (u32)(stdvector-4)); // In bytes
  s32 MaxSize  = Capacity - size_in_bytes;
  if (Size <= MaxSize)  // Room for one more struct
  {
    u32 Machine_Address = x86emu_read_dword(emu, data_ptr); // eliminate warning 2022-08-07 
    for (u32 N = 0; N < size_in_bytes; N += 4) 
    {
      u32 Data = x86emu_read_dword(emu, data_ptr + N); 
      x86emu_write_dword(emu, (u32)(stdvector + Size + N), Data); 
    }
    x86emu_write_dword(emu, (u32)(stdvector - 4), Size + size_in_bytes); 
//  printf("[%08x] PushBack Orig_Size(%03d)  New_Size(%03d)\n", Machine_Address, Size, x86emu_read_dword(emu, (u32)(stdvector-4))); 
  }
  else
  {
    printf("Ran out of memory@ Address[%08x]\n", (u32)stdvector);
    exit(0); 
  }
}  



void Output_integer_list(x86emu_t *emu, u32 integer_list)
{
  s32 Capacity = x86emu_read_dword(emu, (u32)(integer_list-8)); // In bytes
  s32     Size = x86emu_read_dword(emu, (u32)(integer_list-4)); // In bytes
  printf("\nOutput_integer_list() [%08x]  size(%d)  capacity(%d)\n", 
         integer_list, Size, Capacity); 
  for (u32 Index = (u32)integer_list; Index < (u32)(integer_list + Size); Index += 4) 
    printf("std_vector[%08x]-->%08x\n",  Index, x86emu_read_dword(emu, Index)); 
} 



u32 Simulate(x86emu_t *emu, u32 P, u32 Params)
{
  u32 NumParams = x86emu_read_dword(emu, Params); 
//printf("Simulate:[%08x](%d) {", P, NumParams);
  printf("Simulate() [%08x] {", P);
  for (u32 N = 1; N < NumParams; N++)
    printf("%08x, ", x86emu_read_dword(emu, Params + N*4));
  printf("%08x}\n", x86emu_read_dword(emu, Params + NumParams*4)); 

  return 1; 
}


// 2021-02-11 
void Output_Trace_List_Line(x86emu_t *emu,  X86_UTM& x86utm, 
                            COFF_Reader& Reader,  Line_Of_Code& loc)
{
#ifdef DOT_DASH_PREFIX
    printf("---[%08x][%08x][%08x](%02d)[%02x]  ", 
            loc.Address, loc.ESP, loc.TOS, loc.NumBytes, loc.Simplified_Opcode); 
#else
    printf("[%08x][%08x][%08x](%02d)[%02x]  ", 
            loc.Address, loc.ESP, loc.TOS, loc.NumBytes, loc.Simplified_Opcode); 
#endif
    Machine_Code_Out(emu, loc.Address, loc.NumBytes);

    if (loc.Simplified_Opcode == JMP  || 
        loc.Simplified_Opcode == CALL ||
        loc.Simplified_Opcode == JCC)
      printf("%-20s   --%s %08x\n", loc.Disassembly_text.c_str(), 
             Get_Simplified_Opcode_String(loc.Simplified_Opcode), loc.Decode_Target); 

    else if (loc.Simplified_Opcode == PUSH)  
      printf("%-20s   --%s %08x\n", loc.Disassembly_text.c_str(), 
             Get_Simplified_Opcode_String(loc.Simplified_Opcode), loc.TOS); 
    else if (loc.Simplified_Opcode == RET)
      printf("%-20s   --RET\n", loc.Disassembly_text.c_str()); 
    else if (loc.Simplified_Opcode == HLT)
      printf("%-20s   --HLT\n", loc.Disassembly_text.c_str()); 
    else
      printf("%-20s\n", loc.Disassembly_text.c_str()); 
}



void Output_Debug_Trace(x86emu_t *emu,  X86_UTM& x86utm, COFF_Reader& Reader)
{
  printf("\nOutput_Debug_Trace() Trace_List.size(%d)\n", Execution_Trace.size()); 
  for (u32 N = 0; N < Execution_Trace.size(); N++) 
  {
    Output_Trace_List_Line(emu, x86utm, Reader,  Execution_Trace[N]); 
//  if (Halt_Decision.Index == N) 
//      Halt_Decision.Output();
  } 
} 


void Line_Of_Code_Out(Line_Of_Code& loc, u32 N)
{
  printf("Execution_Trace[%02d][%08x][%02x]%s  %08x\n", 
         N, loc.Address, loc.Simplified_Opcode, 
         Get_Simplified_Opcode_String(loc.Simplified_Opcode),
         loc.Decode_Target); 
}


void Output_Debug_List(x86emu_t *emu,  X86_UTM& x86utm, COFF_Reader& Reader)
{
  printf("\nOutput_Debug_Trace() Trace_List.size(%d)\n", Execution_Trace.size()); 
  for (u32 N = 0; N < Execution_Trace.size(); N++) 
  {
    Line_Of_Code_Out(Execution_Trace[N], N); 
/***
    printf("Execution_Trace[%02d][%08x][%02x]%s  %08x\n", 
           N, Execution_Trace[N].Address,            
           Execution_Trace[N].Simplified_Opcode, 
           Get_Simplified_Opcode_String(Execution_Trace[N].Simplified_Opcode),
           Execution_Trace[N].Decode_Target);
***/
//  Output_Trace_List_Line(emu, x86utm, Reader,  Execution_Trace[N]); 
//  if (Halt_Decision.Index == N) 
//      Halt_Decision.Output();
  } 
} 




//
// Used to intialize the Slave Stack
//
u32 StackPush(x86emu_t *emu, u32 slave_stack, u32 M)
{
  s32 Capacity = x86emu_read_dword(emu, (u32)(slave_stack-8)); // In bytes
  s32     Size = x86emu_read_dword(emu, (u32)(slave_stack-4)); // In bytes
  s32 MaxSize  = Capacity - 4;    // Room for one more

#ifdef DEBUG_FUNCTION_CALLS
  printf("StackPush(): slave_stack[%08x].PushBack(%x) Capacity(%d) Size(%d) MaxSize(%d)\n", 
          (u32)slave_stack, M, Capacity, Size, MaxSize);
#endif 
  if (Size <= MaxSize)  // Room for one more u32
  {
    u32 Top_of_Stack = (u32)slave_stack + (Capacity - Size);   
    Top_of_Stack -= 4;  // New TopOfStack
#ifdef DEBUG_FUNCTION_CALLS
    printf("slave_stack[%08x] StackPush(%08x):Top_of_Stack[%08x]\n", 
            slave_stack, M, Top_of_Stack); 
#endif 
    x86emu_write_dword(emu, (u32)(Top_of_Stack),  M); 
    x86emu_write_dword(emu, (u32)(slave_stack - 4),  Size + 4); 

    // return value to Virtual Machine 
    emu->x86.R_EAX = Top_of_Stack;      
    return Top_of_Stack; 
  }
  else
  {
    printf("Ran out of memory@ Address[%08x]\n", (u32)slave_stack);
    exit(0); 
  }
}


void Output_slave_stack(x86emu_t *emu, u32 slave_stack)
{ 
  s32 Capacity         = x86emu_read_dword(emu, (u32)slave_stack-8); // In bytes
  s32 Size             = x86emu_read_dword(emu, (u32)slave_stack-4); // In bytes
//printf("Output_slave_stack Capacity[%08x] Size[%08x]\n", Capacity, Size);
  u32 Top_of_Stack = (u32)slave_stack + (Capacity - Size);   
  u32 Stack_Bottom = (u32)slave_stack + Capacity;
  printf("Output_slave_stack[%08x] Top[%08x] Bottom[%08x]\n", 
          slave_stack, Top_of_Stack, Stack_Bottom);
  for (u32 Index = Top_of_Stack; Index < Stack_Bottom; Index += 4) 
    printf("  slave_stack[%08x]-->%08x\n",  Index, x86emu_read_dword(emu, Index)); 
}


// Output system registers 
void Output_Registers(x86emu_t *emu) 
{
  printf("EIP[%08x]--",  emu->x86.R_EIP);
  printf("EAX[%08x]--",  emu->x86.R_EAX); 
  printf("EBX[%08x]--",  emu->x86.R_EBX);
  printf("ECX[%08x]---", emu->x86.R_ECX);
  printf("EDX[%08x]\n",  emu->x86.R_EDX);
  printf("ESI[%08x]--",  emu->x86.R_ESI);
  printf("EDI[%08x]--",  emu->x86.R_EDI);
  printf("EBP[%08x]--",  emu->x86.R_EBP);
  printf("ESP[%08x]--",  emu->x86.R_ESP);
  printf("EFLG[%08x]\n", emu->x86.R_EFLG);
//printf("---CS[%04x]--",  emu->x86.R_CS); 
//printf("SS[%04x]--",     emu->x86.R_SS);
//printf("DS[%04x]--",     emu->x86.R_DS);
//printf("ES[%04x]--",     emu->x86.R_ES);
//printf("FS[%04x]--",     emu->x86.R_FS);
//printf("GS[%04x]\n",     emu->x86.R_GS); 
} 


void Output_Saved_Registers(x86emu_t *emu, u32 state)
{
  printf("EIP[%08x]--",  x86emu_read_dword(emu, state));
  printf("EAX[%08x]--",  x86emu_read_dword(emu, state  +4)); 
  printf("EBX[%08x]--",  x86emu_read_dword(emu, state  +8));
  printf("ECX[%08x]---", x86emu_read_dword(emu, state +12));
  printf("EDX[%08x]\n",  x86emu_read_dword(emu, state +16));
  printf("ESI[%08x]--",  x86emu_read_dword(emu, state +20));
  printf("EDI[%08x]--",  x86emu_read_dword(emu, state +24));
  printf("EBP[%08x]--",  x86emu_read_dword(emu, state +28));
  printf("ESP[%08x]--",  x86emu_read_dword(emu, state +32));
  printf("EFLG[%08x]\n", x86emu_read_dword(emu, state +36));
//printf("---CS[%04x]--",   x86emu_read_word(emu, state +40)); 
//printf("SS[%04x]--",      x86emu_read_word(emu, state +42));
//printf("DS[%04x]--",      x86emu_read_word(emu, state +44));
//printf("ES[%04x]--",      x86emu_read_word(emu, state +46));
//printf("FS[%04x]--",      x86emu_read_word(emu, state +48));
//printf("GS[%04x]\n",      x86emu_read_word(emu, state +50)); 
} 


// Used for outputting local stack data
// to verify that it recieved its paramters correctly
void Output_Stack(x86emu_t *emu, s32 Num_Locals, u32 Num_Params)
{
  u32 Start = emu->x86.R_EBP + (Num_Locals * -4);
  u32 Stop  = emu->x86.R_EBP + (Num_Params * 4) + 4; 
  printf("Output_Stack() EBP[%08x]  Num_Locals(%d)  Num_Params(%d)\n", 
          emu->x86.R_EBP, Num_Locals, Num_Params); 
  for (u32 Index = Start; Index <= Stop; Index += 4)
    printf("  Stack[%08x]-->%08x\n",  Index, x86emu_read_dword(emu, Index));
}


// Takes pointer to C function 
// returns pointer to x86utm string type 
//
u32 Copy_Code(x86emu_t *emu, COFF_Reader& Reader, u32 source) 
{
  // switched to this method because random byte patterns c3 cc might fail
  u32 code_size = Reader.get_code_end(source) - source + 1; 
  u32 destination = Allocate(emu, Reader.Heap_PTR_variable, Reader.Heap_END_variable, code_size); 
//printf("destination[%08x][%08x][%08x] code_size:%04d\n", destination, 
//  x86emu_read_dword(emu, destination - 4), x86emu_read_dword(emu, destination - 8), code_size); 

  for (u32 N = 0; N < code_size ; N++)
  {
    u32 OpCode = x86emu_read_byte(emu, source + N); 
//  printf("[%08x][%08x] Opcode[%02x]\n", source + N, destination + N, OpCode); 
    x86emu_write_byte(emu, destination + N, OpCode);  // Copy OpCodes to destination
  }

  x86emu_write_dword(emu, destination - 4, code_size);  // write length of string
//printf("destination[%08x][%08x][%08x] code_size:%04d\n", destination, 
//  x86emu_read_dword(emu, destination - 4), x86emu_read_dword(emu, destination - 8), code_size); 

  emu->x86.R_EAX = destination;   
  return destination; 
}

/************************************************************************
  Tested every combination 

  Output("Identical_Code():", Identical_Code(code1, code1)); // 1
  Output("Identical_Code():", Identical_Code(code1, copy1)); // 1
  Output("Identical_Code():", Identical_Code(copy1, code1)); // 1
  Output("Identical_Code():", Identical_Code(copy1, copy1)); // 1
  Output("Identical_Code():", Identical_Code(code2, code2)); // 1
  Output("Identical_Code():", Identical_Code(code2, copy2)); // 1
  Output("Identical_Code():", Identical_Code(copy2, code2)); // 1
  Output("Identical_Code():", Identical_Code(copy2, copy2)); // 1

  Output("Identical_Code():", Identical_Code(code1, code2)); // 0
  Output("Identical_Code():", Identical_Code(code1, copy2)); // 0
  Output("Identical_Code():", Identical_Code(copy1, code2)); // 0
  Output("Identical_Code():", Identical_Code(copy1, copy2)); // 0
  Output("Identical_Code():", Identical_Code(code2, code1)); // 0
  Output("Identical_Code():", Identical_Code(code2, copy1)); // 0
  Output("Identical_Code():", Identical_Code(copy2, code1)); // 0
  Output("Identical_Code():", Identical_Code(copy2, copy1)); // 0
**************************************************************************/\
// Compares a function's machine code 
// Param1: Machine code address or x86utm string of copy of machine code 
// Param2: Machine code address or x86utm string of copy of machine code 
int Identical_Code(x86emu_t *emu, COFF_Reader& Reader, u32 source, u32 destination) 
{
  u32 size1 = Reader.get_code_end(source); // address of function 
  u32 size2 = Reader.get_code_end(destination);

  if (size1 == 0)                                  // not address of function 
    size1 = x86emu_read_dword(emu, source - 4);    // get length of x86utm string
  else
    size1 = size1 - source + 1; // convert end address to size of function 

  if (size2 == 0)                                       // not address of function 
    size2 = x86emu_read_dword(emu, destination - 4);    // get length of x86utm string
  else
    size2 = size2 - destination + 1; // convert end address to size of function 
  
  if (size1 != size2)
  {
//  printf("***source[%08x](%04d) destination[%08x](%04d)\n", source, size1, destination, size2); 
    emu->x86.R_EAX = 0;   
    return 0;
  }
//printf("---source[%08x](%04d) destination[%08x](%04d)\n", source, size1, destination, size2); 
  for (u32 N = 0; N < size1; N++)
  {
//  printf("+++source[%08x](%04d) destination[%08x](%04d)\n", source+N, size1, destination+N, size2); 
    if (x86emu_read_byte(emu, source + N) != x86emu_read_byte(emu, destination + N))
    {
      emu->x86.R_EAX = 0;   
      return 0;
    }
  }
  emu->x86.R_EAX = 1;   
  return 1;
}


// Virtual Machine Instruction 
void SaveState(x86emu_t *emu, u32 state)
{ 
//printf("---SaveState(%08x)\n", state);
//Output_Registers(emu);  // Registers to be Saved
  x86emu_write_dword(emu, state, emu->x86.R_EIP);
  x86emu_write_dword(emu, state+4,  emu->x86.R_EAX);
  x86emu_write_dword(emu, state+8,  emu->x86.R_EBX);
  x86emu_write_dword(emu, state+12, emu->x86.R_ECX);
  x86emu_write_dword(emu, state+16, emu->x86.R_EDX);
  x86emu_write_dword(emu, state+20, emu->x86.R_ESI);
  x86emu_write_dword(emu, state+24, emu->x86.R_EDI);
  x86emu_write_dword(emu, state+28, emu->x86.R_EBP);
  x86emu_write_dword(emu, state+32, emu->x86.R_ESP);
  x86emu_write_dword(emu, state+36, emu->x86.R_EFLG);
  x86emu_write_word(emu,  state+40, emu->x86.R_CS);
  x86emu_write_word(emu,  state+42, emu->x86.R_SS);
  x86emu_write_word(emu,  state+44, emu->x86.R_DS);
  x86emu_write_word(emu,  state+46, emu->x86.R_ES);
  x86emu_write_word(emu,  state+48, emu->x86.R_FS);
  x86emu_write_word(emu,  state+50, emu->x86.R_GS);
//Output_Regs(emu, state); // Saved Registers
}


// Virtual Machine Instruction 
void LoadState(x86emu_t *emu,  u32 state)
{ 
//printf("---LoadState(%08x)\n", state); 
//Output_Saved_Registers(emu, state);  // Saved Registers
  emu->x86.R_EIP  = x86emu_read_dword(emu, state);
  emu->x86.R_EAX  = x86emu_read_dword(emu, state +4);
  emu->x86.R_EBX  = x86emu_read_dword(emu, state +8);
  emu->x86.R_ECX  = x86emu_read_dword(emu, state+12);
  emu->x86.R_EDX  = x86emu_read_dword(emu, state+16);
  emu->x86.R_ESI  = x86emu_read_dword(emu, state+20);
  emu->x86.R_EDI  = x86emu_read_dword(emu, state+24);
  emu->x86.R_EBP  = x86emu_read_dword(emu, state+28);
  emu->x86.R_ESP  = x86emu_read_dword(emu, state+32);
  emu->x86.R_EFLG = x86emu_read_dword(emu, state+36);
  emu->x86.R_CS   = x86emu_read_word(emu,  state+40);
  emu->x86.R_SS   = x86emu_read_word(emu,  state+42);
  emu->x86.R_DS   = x86emu_read_word(emu,  state+44);
  emu->x86.R_ES   = x86emu_read_word(emu,  state+46);
  emu->x86.R_FS   = x86emu_read_word(emu,  state+48);
  emu->x86.R_GS   = x86emu_read_word(emu,  state+50); 
//Output_Regs(emu);         // Loaded Registers
}


// Address of Heap_PTR and Heap_END Variables 
u32 Allocate(x86emu_t *emu, u32 Heap_PTR_variable, u32 Heap_END_variable, u32 size) 
{
  u32 top = x86emu_read_dword(emu, Heap_PTR_variable); 
  u32 end = x86emu_read_dword(emu, Heap_END_variable); 
#ifdef DEBUG_FUNCTION_CALLS
  printf("  --Initial Heap_PTR_variable[%08x](%08x) Heap_END_variable[%08x](%08x)\n", 
         Heap_PTR_variable, top, Heap_END_variable, end);  
#endif 
// Zero out newly allocated memory
// for (u32 N = top; N < top + size; N++)  // 2020-10-28
//   x86emu_write_dword(emu, N, 0);        // 2020-10-28

  if (top + size + 8 > end)
  {
    // return Failure value to Virtual Machine 
    emu->x86.R_EAX = 0;                             
    printf("  --Allocate(%d) failed\n", size); 
    return 0;     
  } 
  // Updated Heap Allocation 
  x86emu_write_dword(emu, Heap_PTR_variable, top + size + 8); 

  // Write Allocation,   advance 4 bytes
  x86emu_write_dword(emu, top, size);  top += 4;     

  // Write initial Size, advance 4 bytes
  x86emu_write_dword(emu, top, 0);     top += 4;     

//u32 X = x86emu_read_dword(emu, top - 8);
//u32 Y = x86emu_read_dword(emu, top - 4);
//u32 Z = x86emu_read_dword(emu, top);
#ifdef DEBUG_FUNCTION_CALLS
  printf("  --Allocation[%08x](%08x) size[%08x](%08x) data[%08x](%04x)\n", 
          top-8, x86emu_read_dword(emu, top - 8), 
          top-4, x86emu_read_dword(emu, top - 4), 
          top, x86emu_read_dword(emu, top));  
#endif 

  // Allocation Address / bytes allocated
//printf("--Allocation[%08x](%08x)\n", top, x86emu_read_dword(emu, top - 8)); 

  u32 newtop = x86emu_read_dword(emu, Heap_PTR_variable); // eliminate warning 2022-08-07 
#ifdef DEBUG_FUNCTION_CALLS
  printf("  --Final   Heap_PTR_variable[%08x](%08x) Heap_END_variable[%08x](%08x)\n", 
         Heap_PTR_variable, newtop, Heap_END_variable, end);  
#endif 
  // return value to Virtual Machine 
  emu->x86.R_EAX = top;                             
  return top; 
}



void Output_Decoded(x86emu_t *emu, X86_UTM& x86utm, COFF_Reader& Reader, u32 decoded)
{
// Outputs full instruction 
  u32 Address = x86emu_read_dword(emu, decoded+0); 
  Line_Of_Code& loc =
  Get_Line_Of_Code_From_Machine_Address(emu, x86utm, Reader, Address); 
  loc.ESP = x86emu_read_dword(emu, decoded+4); 
  loc.TOS = x86emu_read_dword(emu, decoded+8); 
//printf("Output_Decoded(1)\n"); 
//Disassembled_line_out(emu, loc); 
//printf("Output_Decoded(2)\n"); 

// Outputs decoded instruction 
//  printf(">>>[%08x][%08x][%08x](%02d)  %x %08x\n", 
    printf("+++[%08x][%08x][%08x](%02d)  %04s %08x\n",  
            x86emu_read_dword(emu, decoded+0),    // Address
            x86emu_read_dword(emu, decoded+4),    // ESP
            x86emu_read_dword(emu, decoded+8),    // TOS
            x86emu_read_dword(emu, decoded+12),   // NumBytes
            Get_Simplified_Opcode_String(x86emu_read_dword(emu, decoded+16)), 
//          x86emu_read_dword(emu, decoded+16),   // Simplified_Opcode 
            x86emu_read_dword(emu, decoded+20));  // Decode_Target     

    Address      = x86emu_read_dword(emu, decoded+0);
    u32 NumBytes = x86emu_read_dword(emu, decoded+12);
    printf("Machine_Code_Bytes:");
    for (u32 N = 0; N < NumBytes; N++)
    {
      u32 Machine_Code_Byte = x86emu_read_byte(emu, Address+N); 
      printf("%02x", Machine_Code_Byte);
    }
    printf("\n"); 
//printf("Output_Decoded(3)\n"); 
}


// Decoded_Line_Of_Code      
// Output_Decoded_Instructions(emu, x86utm, Reader, Stack0); 


// Used by Local Halt Decider in the emulated code
void Output_Decoded_Instructions(x86emu_t *emu, X86_UTM& x86utm, 
                                 COFF_Reader& Reader, u32 integer_list)
{ 
//printf("\nBEGIN---Output_Decoded_Instructions(%08x)\n", integer_list); 
  s32 Capacity = x86emu_read_dword(emu, (u32)(integer_list-8)); // In bytes
  s32     Size = x86emu_read_dword(emu, (u32)(integer_list-4)); // In bytes
  printf("\nOutput_Decoded_Instructions() [%08x]  size(%d)  capacity(%d)\n", 
         integer_list, Size, Capacity); 
//for (u32 Index = (u32)integer_list; Index < (u32)(integer_list + Size); Index += 4) 
  for (u32 Index = (u32)integer_list; Index < (u32)(integer_list + Size); Index += sizeof(Decoded_Line_Of_Code)) 
  {
//  printf("std_vector[%08x]-->%08x\n",  Index, x86emu_read_dword(emu, Index)); 
    u32 Address = x86emu_read_dword(emu, Index); 
    u32 ESP     = x86emu_read_dword(emu, Index + 4); 
    u32 TOS     = x86emu_read_dword(emu, Index + 8); 

    Line_Of_Code& loc = 
    Get_Line_Of_Code_From_Machine_Address(emu, x86utm, Reader, Address); 
    Decode_Control_Flow_Instruction(emu, loc);
    loc.ESP = ESP;
    loc.TOS = TOS; 
    Disassembled_line_out2(emu, loc);     // 2024-06-19 
//  printf("Address[%08x]\n", Address); 
  }
  printf("END---Output_Decoded_Instructions(%08x)\n\n", integer_list); 
} 



// Used by Local Halt Decider in the emulated code
void Decode_Line_Of_Code(x86emu_t *emu, X86_UTM& x86utm, 
                         COFF_Reader& Reader, u32 Index, u32 code_line) 
{
//printf("---Decode_Line_Of_Code(%08x, %08x)\n", Index, code_line); 
//Line_Of_Code& loc = 
//Get_Line_Of_Code_From_Machine_Address(emu, x86utm, Reader, Address); 
  Line_Of_Code& loc = Execution_Trace[Index]; 
  Decode_Control_Flow_Instruction(emu, loc);  // 2021-03-11 
  Decoded_Line_Of_Code decoded(loc); 
  x86emu_write_dword(emu, code_line,    decoded.Address);
  x86emu_write_dword(emu, code_line+4,  decoded.ESP);
  x86emu_write_dword(emu, code_line+8,  decoded.TOS);
  x86emu_write_dword(emu, code_line+12, decoded.NumBytes); 
  x86emu_write_dword(emu, code_line+16, decoded.Simplified_Opcode);
  x86emu_write_dword(emu, code_line+20, decoded.Decode_Target);
}
 


// Within our current design the emulated virtual machine will 
// always return HALTED from this function whenever it terminates 
// normally because it is forced to return to the last instruction 
// of Halts whenever it terminates normally. 
//
// Virtual Machine Instruction 
u32 DebugStep(x86emu_t *emu, unsigned flags, X86_UTM& x86utm, 
              COFF_Reader& Reader, u32 master_state, u32 slave_state, u32 decoded) 
{
//printf("DebugStep()\n");
  u32 Halt_Status = EXECUTING; 
/***
  u32 Emulation_Level = 
        x86emu_read_dword(emu, Reader.Current_Emulation_Level_variable_address); 
***/ 
  u32 EIP_1 = x86emu_read_dword(emu, slave_state);  // eliminate warning 2022-08-07 

  SaveState(emu, master_state); // Saves master process state          
  LoadState(emu, (u32)slave_state);  // Changes to slave process state      

// The currently executed slave instruction. 
#ifdef TRACE_USER_CODE_ONLY_XXX
  if (emu->x86.R_EIP > Reader.Last_Address_Of_Operating_System)
    printf("user_code %08x\n", emu->x86.R_EIP );
  Output_Saved_Registers(emu, slave_state); 
#endif

//if (emu->x86.R_EIP > Reader.Last_Address_Of_Operating_System)
//  printf("DebugStep [%08x][%08x]\n", emu->x86.R_EIP, emu->x86.R_ESP );

//Halt_Status = Execute_Instruction(emu, flags, x86utm, Reader); // 2021-04-08
//printf("Before Execute_Instruction() in DebugStep()[%08x]\n", emu->x86.R_EIP); 
  Execute_Instruction(emu, flags, x86utm, Reader);               // 2021-04-08
//printf("After  Execute_Instruction() in DebugStep()[%08x]\n", emu->x86.R_EIP); 

  Line_Of_Code loc(emu->line_of_code);
  Decode_Control_Flow_Instruction(emu, loc);
  Decoded_Line_Of_Code decoded_line(loc); 

  x86emu_write_dword(emu, decoded+0,  decoded_line.Address); 
  x86emu_write_dword(emu, decoded+4,  decoded_line.ESP); 
  x86emu_write_dword(emu, decoded+8,  decoded_line.TOS); 
  x86emu_write_dword(emu, decoded+12, decoded_line.NumBytes); 
  x86emu_write_dword(emu, decoded+16, decoded_line.Simplified_Opcode); 
  x86emu_write_dword(emu, decoded+20, decoded_line.Decode_Target); 

//decoded_line.Out(); 

//printf("DebugStep[%08x]\n", emu->line_of_code.Address); 
  if (Halt_Status == HALTED)  // so that the master_state will not be halted
    ResetHalted(emu); 

  SaveState(emu, slave_state);  // Saves the slave process state       

// The next instruction after the currently executed slave instruction. 
#ifdef TRACE_USER_CODE_ONLY_XXX
  if (emu->x86.R_EIP > Reader.Last_Address_Of_Operating_System) 
    printf("user_code %08x\n", emu->x86.R_EIP );
  Output_Saved_Registers(emu, slave_state); 
#endif

  u32 EIP_2 = x86emu_read_dword(emu, slave_state); // eliminate warning 2022-08-07 

  LoadState(emu, master_state); // Restores the master process state   
/***
  Emulation_Level = 
        x86emu_read_dword(emu, Reader.Current_Emulation_Level_variable_address); 
***/ 
  emu->x86.R_EAX = Halt_Status;  // EXECUTING(0)  HALTED(1)  ABORTED(2)
  return Halt_Status; 
}


void Push_Local_Stack(x86emu_t *emu, u32 N)
{
  emu->x86.R_ESP -= 4; 
  x86emu_write_dword(emu, emu->x86.R_ESP, N); 
}

/***
// This is the only function that returns the ABORTED status
// EXECUTING(0)  HALTED(1)  ABORTED(2)
u32 Needs_To_Be_Aborted_Trace(x86emu_t *emu,  X86_UTM& x86utm, 
                              COFF_Reader& Reader, Line_Of_Code& loc)
{ 
  u32 integer_list = Reader.Global_Execution_Trace_List_data_address; 
  s32     Capacity = x86emu_read_dword(emu, (u32)(integer_list-8)); // In bytes
  s32         Size = x86emu_read_dword(emu, (u32)(integer_list-4)); // In bytes
//Output_Debug_List(emu, x86utm, Reader); 

#ifdef DEBUG_FUNCTION_CALLS
  printf("\nHalt_Decider_Trace() [%08x]  size(%d)  capacity(%d)\n", 
         integer_list, Size, Capacity); 
#endif 
  u32 Count_Conditional_Branch_Instructions = 0; 
  static u32 Count_Decisions = 1; 

  u32 next2last = Execution_Trace.size() - 2;
  u32      last = Execution_Trace.size() - 1;

  for (s32 N = next2last; N >= 0; N--) 
  {
//  Output_Debug_List(emu,  x86utm, Reader);  // 2021-02-14 
    Line_Of_Code& traced = Execution_Trace[N]; 
//  Line_Of_Code_Out(traced, N); 
//  printf("---Halt_Decider_Trace[%08x][%08x] %s\n", N, traced.Address,
//         Get_Simplified_Opcode_String(traced.Simplified_Opcode)); 
#ifdef DEBUG_FUNCTION_CALLS
    printf("Halt_Decider_Trace[%02d]", N); 
    Output_Trace_List_Line(emu,  x86utm, Reader, traced);
#endif 
    if (traced.Simplified_Opcode == JCC) 
      Count_Conditional_Branch_Instructions++;
    if (loc.Simplified_Opcode == JMP)                     // JMP 
      if (loc.Decode_Target <= loc.Address)               // upward
        if (traced.Address == loc.Decode_Target)          // to this address
          if (Count_Conditional_Branch_Instructions == 0) // no escape
          { 
            printf("Global Halt Decider: " 
                   "Infinite Loop Detected Simulation Stopped\n"); 
            return ABORTED; 
          }
    if (loc.Simplified_Opcode == CALL)
      if (loc.Simplified_Opcode == traced.Simplified_Opcode)  // CALL
        if (loc.Address == traced.Address)                    // from same address
          if (loc.Decode_Target == traced.Decode_Target)      // to same Function 
            if (Count_Conditional_Branch_Instructions == 0)   // no escape 
            { 
              printf("Global Halt Decider: " 
                     "Infinite Recursion Detected Simulation Stopped\n");
              return ABORTED; 
            } 
  }
  return 0;
} 


u32 Needs_To_Be_Aborted(x86emu_t *emu, X86_UTM& x86utm, COFF_Reader& Reader)
{    
#ifndef GLOBAL_HALT_DECIDER_MODE
  return 0; 
#endif 
  if (Execution_Trace.size() == 0)
    return 0; 

   u32 last = Execution_Trace.size() - 1; 
   Line_Of_Code loc(Execution_Trace[last]); 

//Line_Of_Code loc(emu->line_of_code);
//Decode_Control_Flow_Instruction(emu, loc);

  if (loc.Simplified_Opcode == CALL) 
    if (Needs_To_Be_Aborted_Trace(emu, x86utm, Reader, loc))
      return ABORTED; 

  if (loc.Simplified_Opcode == JMP) 
    if (Needs_To_Be_Aborted_Trace(emu, x86utm, Reader, loc))
      return ABORTED; 
  return 0; 
}
***/ 

// Keeps track of invocations of Virtual Machine Instructions 
void Append_If_User_Code(x86emu_t *emu, X86_UTM& x86utm, COFF_Reader& Reader)
{
// Haven't executed the call instruction yet
// This makes loc.ESP, and loc.TOP incorrect 
  Line_Of_Code& loc = 
  Get_Line_Of_Code_From_Machine_Address(emu, x86utm, Reader, emu->x86.R_EIP); 

  u32 TOS = x86emu_read_dword(emu, emu->x86.R_ESP); 
  loc.ESP = emu->x86.R_ESP; 
  loc.TOS = TOS; 

//Decode_Control_Flow_Instruction(emu, loc);
  if (emu->x86.R_EIP > Reader.Last_Address_Of_Operating_System)
  {
    Execution_Trace.push_back(loc);   // 2021-02-09 
//  PushBack(emu, Reader.Global_Execution_Trace_List_data_address, // 2022-06-30 
//           emu->x86.R_EIP, sizeof(u32)); 
    Disassembled_line_out2(emu, loc);     // 2024-06-19
//  Disassembled_line_out(emu); 
//  Output_Debug_Trace2(emu, x86utm, Reader); // 2020-12-29 
  }
#ifndef TRACE_USER_CODE_ONLY
//Disassembled_line_out(emu, loc);  // 2021-04-15
  Disassembled_line_out2(emu, loc);  // 2021-04-15 
#endif 
}

// get_code_end() has been fixed to work with dynamic memory 
// 
// Dynamic memory has been filled with NOP 
// (pronounced "no op"; short for no operation)
// 
u32 get_code_end(x86emu_t *emu, COFF_Reader& Reader, u32 Address)
{
  u32 retval = Reader.get_code_end(Address); 
  if (retval == 0)  // Function not in static memory, thus in dynamic memory
  {
//  printf("get_code_end[%08x] not a known function!\n", Address); 
    u32 N        = 0;  
    u8 code_byte = 0;
    for (; code_byte != 0x90 && N < 1000; N++)
    {
      code_byte = x86emu_read_byte(emu, Address+N);
//    printf("[%08x][%03d]code_byte[%02x]\n", Address+N, N, code_byte); 
    }
    retval = Address+N-2; 
  }
  return retval; 
}


void Virtual_Machine_Instruction(x86emu_t *emu, unsigned flags, X86_UTM& x86utm, 
                                 COFF_Reader& Reader,
                                 std::map<u32, Function_Info>::iterator &Found) 
{ 
  Append_If_User_Code(emu, x86utm, Reader); 
  u32 Stack0 = x86emu_read_dword(emu, emu->x86.R_ESP);
  u32 Stack1 = x86emu_read_dword(emu, emu->x86.R_ESP + 4);
  u32 Stack2 = x86emu_read_dword(emu, emu->x86.R_ESP + 8); // 2021-03-20

  if (Found->second.Name == Copy_Code_NAME)
    Copy_Code(emu, Reader, (u32)Stack0);  

  if (Found->second.Name == Identical_Code_NAME)
    Identical_Code(emu, Reader, (u32)Stack0, (u32)Stack1);  

  if (Found->second.Name == SaveState_NAME)
    SaveState(emu, (u32)Stack0);  
  else if (Found->second.Name == Allocate_NAME)
    Allocate(emu, Reader.Heap_PTR_variable, Reader.Heap_END_variable, Stack0); 
  else if (Found->second.Name == OutputString_NAME)
    OutputString(emu, Stack0); 
  else if (Found->second.Name == OutputHex_NAME)
    OutputHex(emu, Stack0); 
  else if (Found->second.Name == Output_NAME)
    Output(emu, Stack0, Stack1); 
  else if (Found->second.Name == PushBack_NAME)
    PushBack(emu, Stack0, Stack1, Stack2); 
  else if (Found->second.Name == StackPush_NAME)
    StackPush(emu, Stack0, Stack1); 
  else if (Found->second.Name == Output_integer_list_NAME)
    Output_integer_list(emu, Stack0); 
  else if (Found->second.Name == Output_slave_stack_NAME)
    Output_slave_stack(emu, Stack0); 
  else if (Found->second.Name == Output_Regs_NAME)
    Output_Registers(emu); 
//else if (Found->second.Name == Global_Halts_NAME) {}  
  else if (Found->second.Name == get_code_end_NAME) 
//  emu->x86.R_EAX = Reader.get_code_end(Stack0);       // 2023-07-03
    emu->x86.R_EAX = get_code_end(emu, Reader, Stack0); // 2023-07-03

  else if (Found->second.Name == Last_Address_Of_OS_NAME)     // 2021-08-26 
    emu->x86.R_EAX = Reader.Last_Address_Of_Operating_System; // 2021-08-26 

  else if (Found->second.Name == Output_Saved_Regs_NAME)
    Output_Saved_Registers(emu, Stack0); 
  else if (Found->second.Name == Output_Stack_NAME)
    Output_Stack(emu, Stack0, Stack1); 
  else if (Found->second.Name  == DebugStep_NAME)                                // eliminate warning 2022-08-07 
    u32 Halted = DebugStep(emu, flags, x86utm, Reader, Stack0, Stack1, Stack2);  // eliminate warning 2022-08-07 
  else if (Found->second.Name == Decode_Line_Of_Code_NAME)
    Decode_Line_Of_Code(emu, x86utm, Reader, Stack0, Stack1);
  else if (Found->second.Name == Output_Decoded_Instructions_NAME)
    Output_Decoded_Instructions(emu, x86utm, Reader, Stack0); 
  else if (Found->second.Name == Output_Decoded_NAME)
//  Output_Decoded(emu, Stack0); 
    Output_Decoded(emu, x86utm, Reader, Stack0); 
//else if (Found->second.Name == Simulate_NAME)
//  Simulate(emu, Stack0, Stack1); 
  else if (Found->second.Name  == Output_Debug_Trace_NAME)
    Output_Debug_Trace(emu, x86utm, Reader); 
  else if (Found->second.Name == LoadState_NAME)
    LoadState(emu, (u32)Stack0);  
} 




//
// Displays the name of the function invoked by function pointer
//
void Named_Function_PTR(x86emu_t *emu, unsigned flags,
                        X86_UTM& x86utm, COFF_Reader& Reader)
{
  u8 OpCode  = x86emu_read_byte(emu, emu->x86.R_EIP); 
  u8 OpCode2 = x86emu_read_byte(emu, emu->x86.R_EIP+1); 
  s8 OpCode3 = x86emu_read_byte(emu, emu->x86.R_EIP+2); 
  std::map<u32, Function_Info>::iterator Found;
  u32 Target = x86emu_read_dword(emu, emu->x86.R_EIP+1) + 5 + emu->x86.R_EIP;
  if (OpCode == 0xff && OpCode2 == 0x55)  // call dword [ebp+??] 
  {
    std::map<u32, Function_Info>::iterator Found = Reader.FunctionNames.find(Target); 
    u32 Called_Function_Address = x86emu_read_dword(emu, emu->x86.R_EBP + OpCode3); 
    Found = Reader.FunctionNames.find(Called_Function_Address); 
//  if(Found != Reader.FunctionNames.end()) 
//    printf("Calling:%s()\n", Found->second.Name.c_str()); 
//  else
//    printf("Called Function Address[%08x]\n", Called_Function_Address); 
  }
}


u32 Test_Halting(x86emu_t *emu, unsigned flags, 
                 X86_UTM& x86utm, COFF_Reader& Reader)
{
/***
  u32 Emulation_Level = 
      x86emu_read_dword(emu, Reader.Current_Emulation_Level_variable_address); 
***/ 
  // Execution Trace of all user code

/***
  if (emu->line_of_code.Address > Reader.Last_Address_Of_Operating_System)
  {
    PushBack(emu, Reader.Global_Execution_Trace_List_data_address, 
             emu->line_of_code.Address, sizeof(u32));     
    Line_Of_Code loc(emu->line_of_code); 
//  Decode_Control_Flow_Instruction(emu, loc);  // removed 2021-07-30
    Execution_Trace.push_back(loc);   // 2021-02-13
    x86emu_write_dword(emu, Reader.Global_Execution_Trace_List_Index_address, 
                       Execution_Trace.size());  // 2021-02-10 

    if (Needs_To_Be_Aborted(emu, x86utm, Reader)) 
      return ABORTED; 
  }
***/ 

  emu->max_instr++;                 
  if (MODE_HALTED || emu->max_instr > MAX_INSTRUCTIONS || emu->x86.R_EIP > x86utm.code_end)
    return HALTED;
  return EXECUTING; 
}


// Returns ONE if the current instruction is a call to a 
// Virtual Machine Instruction (VMI) or ZERO if not
u32 Test_VMI(x86emu_t *emu, COFF_Reader& Reader, u32 EIP)
{ 
  printf("Test_VMI[%08x]\n", EIP);
  u8 OpCode0  = x86emu_read_byte(emu, EIP+0);
  u8 OpCode1  = x86emu_read_byte(emu, EIP+1);
  u8 OpCode2  = x86emu_read_byte(emu, EIP+2);

  if (OpCode1 == 0xe8)  // call Relative-32
  {
    printf("Test_VMI:OpCode1 == 0xe8\n"); // 2023-06-29
    std::map<u32, Function_Info>::iterator Found;  
    u32 Target = x86emu_read_dword(emu, EIP + 1) + 5 + EIP;
    Found = Reader.FunctionNames.find(Target);
    if (Found != Reader.FunctionNames.end())
      return 1; 
  }
  else if (OpCode1 == 0xff && OpCode2 == 0x55)               // 2023-06-29
    printf("Test_VMI:OpCode1 == 0xff && OpCode2 == 0x55\n"); // 2023-06-29
/*** 
    if (Found != Reader.FunctionNames.end() &&
        Found->second.Virtual_Machine_Instruction) // Function call to VMI
    {
      if (Found->second.Name == DebugStep_NAME)    // Function call to DebugStep()
      {
        u32 slavestate = x86emu_read_dword(emu, emu->x86.R_ESP + 4); 
        u32 EIP        = x86emu_read_dword(emu, slavestate); 
        u32 Target2    = x86emu_read_dword(emu, EIP + 1) + 5 + EIP;
        return EIP; 
      }
    }
***/ 
   return 0; 
}


// Returns EIP of the next instruction after DebugStep() 
// or ZERO if current instruction is not a call to DebugStep() 
u32 TestDebugStep(x86emu_t *emu, COFF_Reader& Reader)
{ 
  u8 OpCode  = x86emu_read_byte(emu, emu->x86.R_EIP);
  if (OpCode == 0xe8)  // call Relative-32
  {
    std::map<u32, Function_Info>::iterator Found;  
    u32 Target = x86emu_read_dword(emu, emu->x86.R_EIP + 1) + 5 + emu->x86.R_EIP;
    Found = Reader.FunctionNames.find(Target);
    if (Found != Reader.FunctionNames.end() && 
//      Found->second.Virtual_Machine_Instruction) // Function call to VMI
        Is_Virtual_Machine_Instruction(Found->second.Name)) // 2022_06_29 
    {
      if (Found->second.Name == DebugStep_NAME)    // Function call to DebugStep()
      {
        u32 slavestate = x86emu_read_dword(emu, emu->x86.R_ESP + 4); 
        u32 EIP        = x86emu_read_dword(emu, slavestate); 
        u32 Target2    = x86emu_read_dword(emu, EIP + 1) + 5 + EIP; // eliminate warning 2022-08-07 
        return EIP; 
      }
    }
  }
  return 0; 
}


void Execute_Instruction(x86emu_t *emu, unsigned flags,
                         X86_UTM& x86utm, COFF_Reader& Reader)
{
  std::map<u32, Function_Info>::iterator Found;
  u32 Target  = x86emu_read_dword(emu, emu->x86.R_EIP + 1) + 5 + emu->x86.R_EIP;
  u8 OpCode1  =  x86emu_read_byte(emu, emu->x86.R_EIP+0);
  u8 OpCode2  =  x86emu_read_byte(emu, emu->x86.R_EIP+1);
  u8 OpCode3  =  x86emu_read_byte(emu, emu->x86.R_EIP+2);
  Named_Function_PTR(emu, flags, x86utm, Reader); 
  u32 VMI_Flag = 0; // eliminate warning 2022-08-07 

  if (OpCode1 == 0xff && OpCode2 == 0x55)  // call Relative-32
  {
    s8  Offset = x86emu_read_byte(emu, emu->x86.R_EIP+2);
    Target     = x86emu_read_dword(emu, emu->x86.R_EBP + Offset); 
//  printf("EI_Decode_Control_Flow_Instruction([%02d][%08x][%08x])\n", 
//          Offset, emu->x86.R_EBP, Target);

    Found = Reader.FunctionNames.find(Target);
    if (Found != Reader.FunctionNames.end() && 
        Is_Virtual_Machine_Instruction(Found->second.Name)) // 2023_06_29 
    {
      Virtual_Machine_Instruction(emu, flags, x86utm, Reader, Found);
      emu->x86.R_EIP += 3; 
    }
  }

  if (OpCode1 == 0xe8)  // call Relative-32
  {
    Found = Reader.FunctionNames.find(Target);
    if (Found != Reader.FunctionNames.end() && 
//      Found->second.Virtual_Machine_Instruction) // Function call to VMI
        Is_Virtual_Machine_Instruction(Found->second.Name)) // 2022_06_29 
    {
      Virtual_Machine_Instruction(emu, flags, x86utm, Reader, Found);
      emu->x86.R_EIP += 5; 
    }
  }
  x86emu_run(emu, flags, FALSE);           
}

/***
// (A)       == ~VMI
// (A)(B)    ==  VMI ~VMI
// (A)(B)(C) ==  VMI  VMI ~VMI
void OutputDebugTrace(x86emu_t *emu, COFF_Reader& Reader, 
                      u32 Saved_EIP, u32 TEST_DebugStep, u32 EIP,
                      u32 Saved_ESP, u32 Saved_TOS)
{
  u32 Tested_VMI = Test_VMI(emu, Reader, Saved_EIP); 
  if (TEST_DebugStep)                            // Call DebugStep()
  {
    printf("(A)Executes:[%08x]\n", Saved_EIP); 
    printf("(B)Executes:[%08x]\n", TEST_DebugStep); 
    if (TEST_DebugStep != EIP) 
      printf("(C)Executes:[%08x]\n", EIP);
  }
  else if (Tested_VMI)                           // Call VMI
  {
    printf("(A)Executes:[%08x]\n", Saved_EIP);   // Call VMI
    if (Saved_EIP != EIP) 
      printf("(B)Executes:[%08x]\n", EIP);       // ~VMI 
  }
    else
      printf("(A)Executes:[%08x]\n", Saved_EIP); // ~VMI 
}
***/                       


u32 Halts(x86emu_t *emu, unsigned flags, 
          X86_UTM& x86utm, COFF_Reader& Reader) 
{
  u32 SAVE_EBP = emu->x86.R_EBP; 
  u32 Halt_Status = EXECUTING; 
  u32 code_end = Reader.get_code_end(emu->x86.R_EIP); 
  while (Halt_Status == EXECUTING && emu->x86.R_EIP <= code_end)            
  {
    u32 Saved_EIP      = emu->x86.R_EIP; 
    u32 Saved_ESP      = emu->x86.R_ESP; 
    u32 Saved_TOS      = x86emu_read_dword(emu, emu->x86.R_ESP); 
    u32 TEST_DebugStep = TestDebugStep(emu, Reader);  // eliminate warning 2022-08-07 
/*** 
    if (TEST_DebugStep) 
      printf("(A)Executes:[%08x]\n(B)Executes:[%08x]\n", 
              emu->x86.R_EIP, TEST_DebugStep); 
    else 
      printf("(A)Executes:[%08x]\n", emu->x86.R_EIP); // Not DebugStep() 
***/ 
    Execute_Instruction(emu, flags, x86utm, Reader); 

#ifdef TRACE_USER_CODE_ONLY  // 2024-06-19
  if (emu->line_of_code.Address > Reader.Last_Address_Of_Operating_System) 
#endif 
    Disassembled_line_out(emu);  

/***
    OutputDebugTrace(emu, Reader, Saved_EIP, TEST_DebugStep, 
                     emu->line_of_code.Address, Saved_ESP, Saved_TOS);

    if (TEST_DebugStep && TEST_DebugStep != emu->line_of_code.Address) 
      printf("(C)Executes:[%08x]\n", emu->line_of_code.Address);
***/ 

    if (Saved_EIP  >= x86utm.code_end)  // 2022-07-01 fix [* fault 06] bug
      return 0;                         // 2022-07-01 fix [* fault 06] bug

    Halt_Status = Test_Halting(emu, flags, x86utm, Reader); 
    if (emu->max_instr % 1000000 == 0) 
      fprintf(stderr,"%lld\n", emu->max_instr); 
  }
  emu->x86.R_EAX = Halt_Status;   // may be redundant 
  emu->x86.R_EBP = SAVE_EBP; 
  return Halt_Status; 
} 


void emu_run(char *filename)  // FIXED RECURSION 
{
  printf("Convert_to_Sourcefile_Name[%s]\n", 
          Convert_to_Sourcefile_Name(filename).c_str());

  X86_UTM x86utm; 
  unsigned flags = X86EMU_RUN_MAX_INSTR | X86EMU_RUN_NO_EXEC | X86EMU_RUN_NO_CODE;
  //| X86EMU_RUN_LOOP;
  COFF_Reader Reader;
  u32 BytesRead  = Reader.read_obj_file(filename); 
  if(BytesRead == 0) return;
  bool COFF_Object_File = Reader.Initialize(); 

  if (COFF_Object_File)  // Input file is a COFF Object file 
  {
    Reader.Code_Entry_Point = Reader.GetCodeEntryPoint(); // Find main() 
    Reader.Heap_PTR_variable = 
           Reader.Get_Symbol_Offset(Heap_PTR_NAME,  Reader.DATA_SECTION_INDEX); 
    Reader.Heap_END_variable = 
           Reader.Get_Symbol_Offset(Heap_END_NAME,  Reader.DATA_SECTION_INDEX); 

    Reader.BytesRead = BytesRead; // Beginning of the Heap 
    Reader.Get_Function_Names();  

    x86utm.code_begin = Reader.Get_Code_Offset(); 
    x86utm.code_end   = Reader.get_code_end(); 
    x86utm.code_run   = Reader.Code_Entry_Point;  
  }
  else
  {
    x86utm.code_begin = options.code_begin;
    x86utm.code_end   = options.code_begin + options.code_size;
    x86utm.code_run   = options.code_run; 
  }

  x86emu_t *emu = emu_new();
  int ok = emu_init(emu, Reader);
  get_disassembly_listing(flags, Reader, x86utm); 
//Disassembly_Listing(emu, x86utm, Reader); 

  if(ok) {
    // Initialize Heap_PTR and Heap_END to point to the HEAP_SIZE memory area

    // Initialize Heap_PTR
    x86emu_write_dword(emu, Reader.Heap_PTR_variable, Reader.BytesRead);                    

    // Initialize Heap_END
    x86emu_write_dword(emu, Reader.Heap_END_variable, 
                       Reader.BytesRead + HEAP_SIZE);

    // Allocate STACK_SIZE portion of Heap_Space to the Stack
    u32 stackspace = 
        Allocate(emu, Reader.Heap_PTR_variable, Reader.Heap_END_variable,
                 STACK_SIZE); // Allocate 64K to Stack
    u32 TopOfStack = stackspace + STACK_SIZE; // 2020-09-30 
//  printf("Top of Stack(%08x) has been allocated from the Heap\n", TopOfStack); 
    emu->x86.R_ESP = TopOfStack;     // 64k allocated to Stack from HeapSpace
//  emu->x86.R_EBP = TopOfStack;     // 64k allocated to Stack from HeapSpace


/***
    // Global_Execution_Trace_List is in the emulated "C" code
    Reader.Global_Execution_Trace_List_variable_address  
      = Reader.Get_Symbol_Offset(Global_Execution_Trace_List_NAME,  
        Reader.DATA_SECTION_INDEX); 

    // Global_Execution_Trace_List is in the emulated "C" code
    Reader.Global_Execution_Trace_List_data_address      
      =  Allocate(emu, Reader.Heap_PTR_variable, 
                  Reader.Heap_END_variable, 1560);   // 1571 or [* fault 06] error

    printf("[%08x] Reader.Global_Execution_Trace_List_variable\n", 
            Reader.Global_Execution_Trace_List_variable_address); 

    printf("[%08x] Reader.Global_Execution_Trace_List_data\n", 
            Reader.Global_Execution_Trace_List_data_address); 
***/ 

/***    
    // Write address of Allocated memory to the emulated "C" code 
    x86emu_write_dword(emu, Reader.Global_Execution_Trace_List_variable_address,
                       Reader.Global_Execution_Trace_List_data_address); 

    // Global_Execution_Trace_List is in the emulated "C" code
    Reader.Global_Execution_Trace_List_Index_address  
      = Reader.Get_Symbol_Offset(Global_Execution_Trace_List_Index_NAME,  
                                 Reader.DATA_SECTION_INDEX); 

    // Get LOCAL_HALT_DECIDER_MODE Variable Address in User Code 
    Reader.Local_Halt_Decider_Mode_variable_address  
      = Reader.Get_Symbol_Offset(Local_Halt_Decider_Mode_NAME,  
                                 Reader.DATA_SECTION_INDEX); 

#ifndef LOCAL_HALT_DECIDER_MODE 
    // Disable LOCAL_HALT_DECIDER_MODE in User Code 
    x86emu_write_dword(emu, Reader.Local_Halt_Decider_Mode_variable_address, 0);
#else
    // Enable LOCAL_HALT_DECIDER_MODE in User Code 
    x86emu_write_dword(emu, 
                       Reader.Local_Halt_Decider_Mode_variable_address, 0x111);
#endif 

    // Current_Emulation_Level is in the emulated "C" code
    Reader.Current_Emulation_Level_variable_address  
      = Reader.Get_Symbol_Offset(Current_Emulation_Level_NAME,  
                                 Reader.DATA_SECTION_INDEX); 

    // Write Current_Emulation_Level Value of  "0" to the emulated "C" code 
    x86emu_write_dword(emu, Reader.Current_Emulation_Level_variable_address, 0);

    // Global_Abort_Flag is in the emulated "C" code
    Reader.Global_Abort_Flag_variable_address  
      = Reader.Get_Symbol_Offset(Global_Abort_Flag_NAME, 
                                 Reader.DATA_SECTION_INDEX); 

    // Write Global_Abort_Flag Value of  "0" to the emulated "C" code 
    x86emu_write_dword(emu, Reader.Global_Abort_Flag_variable_address, 0);  
***/ 
    // This limits the scope of the halt decider to User Code
    Reader.Last_Address_Of_Operating_System = 
        Disassembly_Listing_Find_Function(emu, x86utm, Reader, Halts_NAME); 
printf("Bullfrog(777)\n");
    printf("[%08x] Entry Point for main()\n", Reader.Code_Entry_Point ); 

    printf("Heap_PTR_variable[%08x] Heap_END_variable[%08x]  BytesRead(%x)\n", 
      Reader.Heap_PTR_variable, Reader.Heap_END_variable, BytesRead);

    printf("Top of Stack(%08x) has been allocated from the Heap\n", TopOfStack);

    printf("emu->x86.R_ESP(%08x) has been assigned to TopOfStack\n", 
            emu->x86.R_ESP); 

    printf("[%08x] Reader.Last_Address_Of_Operating_System\n", 
            Reader.Last_Address_Of_Operating_System); 

/***
    printf("[%08x] Reader.Global_Execution_Trace_List_variable\n", 
            Reader.Global_Execution_Trace_List_variable_address); 
    printf("[%08x] Reader.Global_Execution_Trace_List_data\n", 
            Reader.Global_Execution_Trace_List_data_address); 
    printf("[%08x] Reader.Local_Halt_Decider_Mode_variable_address\n", 
            Reader.Local_Halt_Decider_Mode_variable_address);
    printf("[%08x] Reader.Reader.Local_Halt_Decider_Mode\n", 
           x86emu_read_dword(emu, 
                             Reader.Local_Halt_Decider_Mode_variable_address));
//  printf("===============================\n");
***/ 

    emu->x86.R_EIP = Reader.Code_Entry_Point;  
    x86emu_reset_access_stats(emu);
    emu->max_instr = 1; 
    std::string Temp; 
    Disassembly_Listing(emu, x86utm, Reader); 
    u32 Halt_Status = EXECUTING;  // eliminate warning 2022-08-07 
/***
#ifdef GLOBAL_HALT_DECIDER_MODE 
    printf("Begin Global Halt Decider Simulation at Machine Address: %x\n", 
           emu->x86.R_EIP);
#endif 
***/ 
    Halts(emu, flags, x86utm, Reader);  // 2021-03-12
//  while (Halt_Status == EXECUTING && emu->x86.R_EIP <= x86utm.code_end) 
//    Halt_Status = Execute_Instruction(emu, flags, x86utm, Reader); 

//  printf("Halt_Status(%d)  emu->x86.R_EIP(%08x)  x86utm.code_end(%08x)\n", 
//          Halt_Status, emu->x86.R_EIP, x86utm.code_end);
//  if (emu->x86.R_EIP == x86utm.code_end)
//  if (Halt_Status == HALTED)
//    printf("Global Halt Decider: Simulation Terminated Normally\n");
/***
    u32 N = Reader.Global_Execution_Trace_List_data_address;
    s32 Size = x86emu_read_dword(emu, (u32)(N-4)); // In bytes
    s32 Number_of_User_Instructions = Size / 4;  
***/ 
//  printf("Number_of_User_Instructions(%d)\n", Number_of_User_Instructions);

//  Output_Debug_Trace(emu, x86utm, Reader);   
    x86emu_clear_log(emu, 1);
  }
  double Number_of_Pages = (double)emu->x86.R_TSC / 67.0; 
  printf("Number of Instructions Executed(%lld) == %0.0f Pages\n", emu->x86.R_TSC, Number_of_Pages); 
  x86emu_done(emu);
//printf("sizeof(Decoded_Line_Of_Code)(%d)\n", sizeof(Decoded_Line_Of_Code));
}
