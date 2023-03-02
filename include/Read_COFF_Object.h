//  
//  This file depends on the COFF Object file format. 
//
//  This file reads a Microsoft COFF Object file and patches
//  all the otherwise relocatable data address references to
//  their actual physical file offsets. 
//
//  This allows the libx86emu executable to directly execute this 
//  adapted object file as long as it does not need  any functionality 
//  from the operating system. 
//
//  All global data must be intialized or space in the object file
//  is not allocated by the compiler. 
//  

#define PATCH_ADDRESS
//#define DEBUG_MODE
//#define SYMBOL_TABLE_DEBUG


//#ifdef _WIN32 // 2022-08-14 
  #define MAIN_NAME                              "_main"
  #define CODE_SEGMENT_NAME                      ".text$mn"
  #define Heap_PTR_NAME                          "_Heap_PTR"
  #define Heap_END_NAME                          "_Heap_END"
/***
#elif __linux__
  #define MAIN_NAME                              "main"
  #define CODE_SEGMENT_NAME                      ".text"
  #define Heap_PTR_NAME                          "Heap_PTR"
  #define Heap_END_NAME                          "Heap_END"
#endif
***/ 

/*** 
//
// Difference between Windows and Linux internal naming conventions
//
#ifdef _WIN32
  #define MAIN_NAME                              "_main"
  #define CODE_SEGMENT_NAME                      ".text$mn"
  #define Heap_PTR_NAME                          "_Heap_PTR"
  #define Heap_END_NAME                          "_Heap_END"
  #define Global_Execution_Trace_List_Index_NAME "_Global_Execution_Trace_List_Index"
  #define Global_Execution_Trace_List_NAME       "_Global_Execution_Trace_List"
  #define Local_Halt_Decider_Mode_NAME           "_Local_Halt_Decider_Mode"

  #define Current_Emulation_Level_NAME      "_Current_Emulation_Level"
  #define Global_Abort_Flag_NAME            "_Global_Abort_Flag"
  #define Debug_Trace_State_NAME            "_debug_trace_state"
  #define Halts_NAME                        "_Halts"
  #define Output_Debug_Trace_NAME           "_Output_Debug_Trace"
  #define Decode_Line_Of_Code_NAME          "_Decode_Line_Of_Code" 
  #define Output_Decoded_Instructions_NAME  "_Output_Decoded_Instructions"  // Made Obsolete

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

#elif __linux__
  #define MAIN_NAME                              "main"
  #define CODE_SEGMENT_NAME                      ".text"
  #define Heap_PTR_NAME                          "Heap_PTR"
  #define Heap_END_NAME                          "Heap_END"
  #define Global_Execution_Trace_List_Index_NAME "Global_Execution_Trace_List_Index"
  #define Global_Execution_Trace_List_NAME       "Global_Execution_Trace_List" 
  #define Local_Halt_Decider_Mode_NAME           "Local_Halt_Decider_Mode"

  #define Current_Emulation_Level_NAME      "Current_Emulation_Level"
  #define Global_Abort_Flag_NAME            "Global_Abort_Flag" 
  #define Debug_Trace_State_NAME            "debug_trace_state"
  #define Halts_NAME                        "Halts"
  #define Output_Debug_Trace_NAME           "Output_Debug_Trace"

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


#include <string>
#include <vector>
#include <map> 
#include <fstream> 
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>

#define  u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define  s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define STYP_TEXT 0x00000020          // Section contains executable code
#define STYP_DATA 0x00000040          // Section contains initialized data
#define DT_FUNCTION     0x20          // Symbol Table Type (bitflag) [function returning T]

#define COFF_HEADER_SIZE        20  // 20 % 4 == 0  32-bit bounday
#define SECTION_HEADER_SIZE     40  // 40 % 4 == 0  32-bit bounday
#define SYMBOL_TABLE_ENTRY_SIZE 18  // 18 % 4 == 2  Not 32-bit bounday
#define RELOCATION_ENTRY_SIZE   10  // 10 % 4 == 2  Not 32-bit bounday
//#define CODE_SECTION             1  // Forced to MS standard One based addressing 
//#define DATA_SECTION             2  // Forced to MS standard One based addressing 

// Intel 386 or later processors and compatible processors
// This is the only way to verify that the file is a COFF object file. 
#define _IMAGE_FILE_MACHINE_I386   0x14c  // Compiler says duplicate declaration or undefined


// Read Text file as std::string Line
inline int ReadFile(std::string FileName) 
{
  std::string Line; 
  std::ifstream fin;
  fin.open(FileName.c_str());
  if (!fin.is_open()) 
  {
    std::cout << FileName << " " << "Does Not Exist!"; 
    return -1; 
  }
  while (std::getline(fin, Line)) 
    std::cout << Line << "\n";     
  fin.close();
  return 0; 
}


struct COFF_Header
{
  u16  Machine;              // 0x14c = Intel 386 or later processors and compatible processors
  u16  Section_Count;        // Number of Sections           
  s32  Date_Time;            // Time & date stamp            
  s32  Symbol_Table_Offset;  // File pointer to Symbol Table 
  s32  Symbol_Table_Count;   // Number of Symbols            
  u16  Optional_Header_Size;  
  u16  Flags;                // COFF Object File Flags       
  void out(); 
};


void COFF_Header::out()
{
  printf("             Machine(%04X)\n",   (u16)Machine);  // 0x14c = Intel 386 or later processors 
  printf("       Section_Count(%04X)\n",   (u16)Section_Count);
  printf("           Date_Time(%08X)\n",   (u32)Date_Time); 
  printf(" Symbol_Table_Offset(%08X)\n",   (u32)Symbol_Table_Offset);
  printf("  Symbol_Table_Count(%08X)\n",   (u32)Symbol_Table_Count);
  printf("Optional_Header_Size(%04X)\n",   (u16)Optional_Header_Size);
  printf("               Flags(%04X)\n\n", (u16)Flags);
}


struct Section_Header
{
  char Section_Name[8];  
  s32  Physical_Address; 
  s32  Virtual_Address;  
  s32  Size_in_Bytes;     // Section size in bytes         
  s32  Data_Offset;       // Offset of Section Data        
  s32  Relocation_Offset; // Offset of Relocation Table    
  s32  Line_Num_Offset;   // Offset of Line Number Table   
  u16  Relocation_Count;  // Num Relocation table entries  
  u16  Line_Num_Count;    // Num Line Number table entries 
  s32  Section_Flags;     // Flags for this section        
  Section_Header();
  void out(); 
};


void Section_Header::out()
{
  char temp[9];
  memcpy(&temp[0], Section_Name, sizeof(temp)); 
  printf("     Section_Name(%s)\n", temp);
  printf(" Physical_Address(%X)\n",   Physical_Address);
  printf("  Virtual_Address(%X)\n",   Virtual_Address);
  printf("    Size_in_Bytes(%X)\n",   Size_in_Bytes);
  printf("      Data_Offset(%X)\n",   Data_Offset);
  printf("         DATA_END(%X)\n",   (Data_Offset + Size_in_Bytes - 1));
  printf("Relocation_Offset(%X)\n",   Relocation_Offset);
  printf("  Line_Num_Offset(%X)\n",   Line_Num_Offset);
  printf(" Relocation_Count(%X)\n",   Relocation_Count);
  printf("   Line_Num_Count(%X)\n",   Line_Num_Count);
  printf("    Section_Flags(%X)\n\n", Section_Flags); 
}


Section_Header::Section_Header()
{
  strcpy(Section_Name, "-EMPTY-"); 
  Physical_Address  = 0; 
  Virtual_Address   = 0; 
  Size_in_Bytes     = 0; 
  Data_Offset       = 0; 
  Relocation_Offset = 0; 
  Line_Num_Offset   = 0; 
  Relocation_Count  = 0; 
  Line_Num_Count    = 0; 
  Section_Flags     = 0; 
}


#define RELOC_ADDR32   6  // Relocate a 32-bit absolute reference
#define RELOC_REL32   20  // Relocate a 32-bit relative reference
struct Relocation_Entry
{
  s32 Code_Offset;   // Reference Address  
  s32 Symbol_Index;  // Symbol Table Index 
  u16 Type;          // Type of relocation (RELOC_ADDR32 or RELOC_REL32)
};


struct Symbol_Table_Entry
{
  union
  {
    char Symbol_Name[8];          // Symbol Name     
    struct
    {
      u32 Zeroes;       // if Zeroes != 0 then use  
      u32 Offset;       // Offset into String Table 
    }; 
  };
  s32   Value;           // Value of Symbol 
  s16   Section_Number;  // Section Number  
  u16   Type;            // Symbol Type     
  char  Storage_Class;   // Storage Class   
  char  Aux_Count;       // Auxiliary Count 
};


struct COFF_Symbol
{
  std::string    Name; 
  s32            Value;           // Value of Symbol 
  s16            Section_Number;  // Section Number  
  u16            Type;            // Symbol Type     
  char           Storage_Class;   // Storage Class   
  char           Aux_Count;       // Auxiliary Count 
  void out();
}; 


void COFF_Symbol::out() 
{
  printf("Symbol(%s) ",          Name.c_str()); 
  printf("Value:(%04X) ",       (u32)Value); 
  printf("Section_Number(%d) ", Section_Number); 
  printf("Type(%X) ",           Type); 
  printf("Storage_Class(%X) ",  Storage_Class); 
  printf("Aux_Count:(%d)\n",    Aux_Count); 
}


struct Function_Info
{
  std::string Name;
  bool Virtual_Machine_Instruction; 
  u32 Num_Params; // Number of parameters is used by the halt decider
                  // to find the machine language push instructions 
                  // corresponding to the function input parameters 
  u32 code_end;   // Machine address of "ret" (last instruction of function)
  Function_Info(std::string Name, bool VMI)
               { this->Name = Name; Virtual_Machine_Instruction = VMI; };
}; 


struct COFF_Reader
{
  u32 CODE_SECTION_INDEX;
  u32 DATA_SECTION_INDEX; 
  COFF_Header Header; 
  std::vector<unsigned char>   ObjectCode; 
  std::vector<Section_Header>  Sections; 
  std::vector<COFF_Symbol>     Symbols;  
  std::map<u32, Function_Info> FunctionNames; 

  u32 BytesRead;          // Size of the COFF Object File
  u32 Heap_PTR_variable;  // Address of Heap_PTR Variable 
  u32 Heap_END_variable;  // Address of Heap_End Variable 
  u32 Last_Address_Of_Operating_System;      // To limit the list to user code

/***
  // Execution Trace List used for Halt Deciding 
  u32 Global_Execution_Trace_List_variable_address;    // Address of Variable
  u32 Global_Execution_Trace_List_data_address;        // Address of Data 
  u32 Global_Execution_Trace_List_Index_address; 

  u32 Local_Halt_Decider_Mode_variable_address;        // Address of Variable
  //u32 Local_Halt_Decider_Mode_data_address;          // Address of Data 

  // Current Emulation Level used for Halt Deciding 
  u32 Current_Emulation_Level_variable_address; // Address of Variable
  u32 Current_Emulation_Level_data_address;     // Address of Data 
  
  // Global_Abort_Flag tells Halts() to quit
  u32 Global_Abort_Flag_variable_address; // Address of Variable
  u32 Global_Abort_Flag_data_address;     // Address of Data 

  // Halts() initial state to revert back to Emulation Level(0)
  // This allows the halt decider to return a value to its caller
  //
  u32 Debug_Trace_State_variable_address; // Address of Variable
  u32 Debug_Trace_State_data_address;     // Address of Data 
***/ 

  u32 Code_Entry_Point;   // The address of main() 
  bool Initialize(); 
  u32  read_obj_file(const char *filename); 
  u32  write_obj_file(const char *filename);
  void Header_Out();
  void Write_Batch_File();
  u32  PatchAddress_Targets(Section_Header& Code_Section);
  void Sections_Out();
  std::string Get_String(unsigned int* Offset);
  std::string Get_String(unsigned int Offset);
  void Symbol_Table_Out(); 
  std::string GetName(Symbol_Table_Entry& Temp);
  void Symbol_Table();  
  void DebugOut(); 
  u32 Get_Symbol_Offset(const char *symbolname, short Section_Number); 
  u32 Get_Symbol_Section(const char *symbolname); 
  u32 Get_Code_Offset() { return Sections[CODE_SECTION_INDEX].Data_Offset;   }
  u32 Get_Code_Size()   { return Sections[CODE_SECTION_INDEX].Size_in_Bytes; }
  u32 get_code_end()    { return Sections[CODE_SECTION_INDEX].Data_Offset +
                                 Sections[CODE_SECTION_INDEX].Size_in_Bytes - 1; }

  u32 Get_Data_Offset() { return Sections[DATA_SECTION_INDEX].Data_Offset;   } 
  u32 Get_Data_Size()   { return Sections[DATA_SECTION_INDEX].Size_in_Bytes; } 
  u32 Get_Data_End()    { return Sections[DATA_SECTION_INDEX].Data_Offset + 
                                 Sections[DATA_SECTION_INDEX].Size_in_Bytes - 1; } 
//COFF_Reader(){ Code_Index = 0; Data_Index = 0; }
  u32 GetCodeEntryPoint(); 
  void HexDump(); 
//void Get_Function_Names(bool SilentMode = true); 
  void Get_Function_Names(); 
  bool Virtual_Machine_Instruction(std::string Function_Name); 
  u32 get_code_end(u32 Address)
  {
    std::map<u32, Function_Info>::iterator Found;
    Found = FunctionNames.find(Address);
    if(Found != FunctionNames.end()) 
    {
//    printf("[%08x] %s Found!  code_end[%08x]\n", Address, Found->second.Name.c_str(), Found->second.code_end); 
      return Found->second.code_end; 
    }
    else
    {
      printf("[%08x] Not Found!\n", Address); 
      exit(0); 
    }
  }
}; 

/***
// 
// Determine if Function_Name is a Virtual Machine Instruction
//
bool COFF_Reader::Virtual_Machine_Instruction(std::string Function_Name)
{
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
***/ 

// Find all the functions in the Symbol Table 
// Inserts their Name and Address in FunctionNames std::map
// Mark Functions that are Virtual Machine Instructions
//void COFF_Reader::Get_Function_Names(bool SilentMode)
void COFF_Reader::Get_Function_Names()
{
#ifdef DEBUG
  printf("COFF_Reader::Get_Function_Names()\n"); 
#endif 
  u32 code_section = Get_Symbol_Section(MAIN_NAME);    // Make work for Linux
  for (u32 N = 0; N < Symbols.size(); N++) 
    if ((u16)Symbols[N].Section_Number == code_section && Symbols[N].Type & DT_FUNCTION)
    {  // Symbol is a Function 
//    if (!SilentMode)
//    {
#ifdef DEBUG
      if (Virtual_Machine_Instruction(Symbols[N].Name)) 
        printf("[%08x]  true   %s\n", (Symbols[N].Value + 
               Sections[code_section].Data_Offset), Symbols[N].Name.c_str()); 
      else
        printf("[%08x]  false  %s\n", (Symbols[N].Value + 
               Sections[code_section].Data_Offset), Symbols[N].Name.c_str()); 
#endif 
//    }
//    FunctionNames.insert(std::make_pair((Symbols[N].Value + 
// Sections[code_section].Data_Offset), Symbols[N].Name));
/***
      if (Virtual_Machine_Instruction(Symbols[N].Name)) 
        FunctionNames.insert(std::make_pair((Symbols[N].Value + 
        Sections[code_section].Data_Offset), Function_Info(Symbols[N].Name, true)));
      else
        FunctionNames.insert(std::make_pair((Symbols[N].Value + 
        Sections[code_section].Data_Offset), Function_Info(Symbols[N].Name, false)));
***/   
      FunctionNames.insert(std::make_pair((Symbols[N].Value + 
      Sections[code_section].Data_Offset), Function_Info(Symbols[N].Name, false)));
    } 
}


void COFF_Reader::HexDump()
{
  printf("; - - memory\n");
  printf(";           0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f");
  for (u32 N = 0; N < ObjectCode.size(); N++)
  {
    if (N == 0 || N % 16 == 0)
      printf("\n%08x:  %02x", N, ObjectCode[N]); 
    else 
      printf("  %02x", ObjectCode[N]); 
  }
  printf("\n"); 
}


u32 COFF_Reader::GetCodeEntryPoint()
{
  u32 code_section = Get_Symbol_Section(MAIN_NAME);    // Make work for Linux
  u32 main_offset  = Get_Symbol_Offset(MAIN_NAME, code_section); // Make work for Linux

//printf("[%08x] Entry Point for main()\n", main_offset); 
  return main_offset;
}


u32 COFF_Reader::Get_Symbol_Offset(const char *symbolname, short Section_Number)
{
  for (u32 N = 0; N < Symbols.size(); N++) 
  {
    if (Symbols[N].Name == symbolname && Symbols[N].Section_Number == Section_Number)
    {
//    printf(">>>> Symbolname[%s] Section_Number(%d)  Offset[%08x]\n", 
//            symbolname, Section_Number, (u32)(Symbols[N].Value + 
// Sections[Section_Number].Data_Offset)); 
      return Symbols[N].Value + Sections[Section_Number].Data_Offset; 
    }
  }
  return 0; 
}


u32 COFF_Reader::Get_Symbol_Section(const char *symbolname)
{
  for (u32 N = 0; N < Symbols.size(); N++) 
    if (Symbols[N].Name == symbolname)
      return Symbols[N].Section_Number;
  return 0; 
}


void COFF_Reader::DebugOut() 
{
  Header_Out(); 
  Sections_Out(); 
  Symbol_Table_Out();
}


bool COFF_Reader::Initialize() 
{
  Section_Header Temp;                      // force standard MS One based addressing 
  strcpy(Temp.Section_Name, "DUMMY");       // force standard MS One based addressing 
  Sections.push_back(Temp);                 // force standard MS One based addressing 
//printf("sizeof(Header)(%ld)\n", sizeof(Header));
  memcpy(&Header, &ObjectCode[0], COFF_HEADER_SIZE); 

  if (Header.Machine == _IMAGE_FILE_MACHINE_I386) 
    printf("Is an x86 based COFF Object File!\n"); 
  else 
  {
    printf("Is Not a COFF Object File --- Aborting COFF Processing !\n"); 
    return false; 
  }

  Sections.resize(Header.Section_Count+1);  // +1 makes room for force One based addressing
  memcpy(&Sections[1], &ObjectCode[sizeof(Header)], 
         Header.Section_Count * SECTION_HEADER_SIZE); 
  Symbol_Table();

//u32 CodeEntryPoint = GetCodeEntryPoint(); 
//printf("CodeEntryPoint[%08x]\n", GetCodeEntryPoint()); 

//CODE_SECTION_INDEX = Get_Symbol_Section(".text$mn");  
  CODE_SECTION_INDEX = Get_Symbol_Section(CODE_SEGMENT_NAME);   // Change for Linux

  DATA_SECTION_INDEX = Get_Symbol_Section(".data"); // Same for Windows and Linux ???
  printf("Section_Number(%d): %s\n", 
          Get_Symbol_Section(CODE_SEGMENT_NAME), CODE_SEGMENT_NAME);
  printf("Section_Number(%d): .data\n",Get_Symbol_Section(".data"));
  printf("CODE_SECTION_INDEX(%d)  DATA_SECTION_INDEX (%d)\n", 
          CODE_SECTION_INDEX, DATA_SECTION_INDEX ); 

  if (CODE_SECTION_INDEX == 0 || DATA_SECTION_INDEX == 0)
    printf("Initialization Failed!   Missing: "
           "CODE[%s] or DATA[.data] Sections\n", CODE_SEGMENT_NAME); 
/***
  for (u32 N = 1; N < Sections.size(); N++)
  {
    printf("[%08x] Section(%d)\n", Sections[N].Section_Flags, N);
    if (Sections[N].Section_Flags & STYP_TEXT)
      printf("[%08x]Code Section(%d)\n", Sections[N].Section_Flags, N);
    if (Sections[N].Section_Flags & STYP_DATA)
      printf("[%08x]Data Section(%d)\n", Sections[N].Section_Flags, N); 
  }
***/ 
#ifdef SYMBOL_TABLE_DEBUG
  DebugOut(); 
#endif
#ifdef PATCH_ADDRESS
  PatchAddress_Targets(Sections[CODE_SECTION_INDEX]); 
#endif
return true; 
}


std::string Convert_to_Sourcefile_Name(std::string filename)
{
  static std::string Sourcefile_Name; 
  for (int N = 0; filename[N] != '.'; N++) 
    Sourcefile_Name += filename[N];
  Sourcefile_Name += ".c";
  return Sourcefile_Name; 
}


// Read Binary file as a single block. 
u32 COFF_Reader::read_obj_file(const char *filename)
{
  u32 Bytes_Read = 0; 
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp)
  {
    std::fseek(fp, 0, SEEK_END);
    ObjectCode.resize(std::ftell(fp));
    std::rewind(fp);
    Bytes_Read = std::fread(&ObjectCode[0], 1, ObjectCode.size(), fp);
    std::fclose(fp);
    std::cout << "Read: " << Bytes_Read << " bytes from \"" << filename << "\"\n";     
    return Bytes_Read;
  }
  else
    std::cout << "Reading: " << filename << " failed!\n";     
  return 0; 
}


//
// Because the system has to patch any references in the CODE section 
// to their actual location in the DATA section this function is 
// provided for debugging purposes. A binary comparision of this
// file to the orginal object file finds all of the patches that 
// were made. 
//
u32 COFF_Reader::write_obj_file(const char *filename)
{
  u32 Bytes_Written = 0; 
  std::FILE *fp = std::fopen(filename, "wb");
  if (fp)
  {
    Bytes_Written = std::fwrite(&ObjectCode[0], 1, ObjectCode.size(), fp);
    std::fclose(fp);
    std::cout << "Wrote: " << Bytes_Written  << " bytes to " << filename << "\n";     
    return Bytes_Written; 
  }
  else
    std::cout << "Writing: " << filename << " failed!\n";     
  return 0; 
}



void COFF_Reader::Header_Out()
{
  printf("             Machine(%X)\n", Header.Machine); // 0x14c = Intel 386 or later processors
  printf("       Section_Count(%d)\n", Header.Section_Count);
  printf("           Date_Time(%X)\n", (u32)Header.Date_Time);
  printf(" Symbol_Table_Offset(%X)\n", (u32)Header.Symbol_Table_Offset);
  printf("  Symbol_Table_Count(%d)\n", (u32)Header.Symbol_Table_Count);
  printf("Optional_Header_Size(%d)\n", Header.Optional_Header_Size);
  printf("               Flags(%X)\n\n", Header.Flags);
}



//
// The batch file executes the Intel XED disassembler 
//
void COFF_Reader::Write_Batch_File()
{
  char name[9];
  memcpy(&name[0], Sections[CODE_SECTION_INDEX].Section_Name, sizeof(name)); 
//if (strcmp(name, ".text$mn"))        // returns 0 on success 
  if (strcmp(name, CODE_SEGMENT_NAME)) // Make work with Linux
  {
    std::FILE *fp = std::fopen("Run777.bat", "w");
    fprintf(fp, "echo off\necho...\necho...\n"); 
    fprintf(fp, "echo Could not find the Code Section in the Object File!(%s)\n", name); 
    fprintf(fp, "echo...\necho...\npause\n"); 
    std::fclose(fp);
  }
  else
  {
    write_obj_file("Halt7.bin"); 
    std::FILE *fp = std::fopen("Run777.bat", "w");
//  fprintf(fp, "xed -i Halt7.obj -b 0x%x > _Halt7_XED_BIN.tmp\n", 
// (u32)Sections[CODE_SECTION_INDEX].Data_Offset);  // 2020-09-07 REVERT
    fprintf(fp, "xed -i Halt7.bin -b 0x%x > _Halt7_XED_BIN.tmp\n", 
           (u32)Sections[CODE_SECTION_INDEX].Data_Offset);  // 2020-09-07 REVERT
    std::fclose(fp);
  }
}


//
//  This function patches all of the relocatatble data references
//  to their file offset targets. This enables libx86emu to directly 
//  execute the patched object file. 
//  
//  The object file: *.obj is patched and written to: *.bin 
//
u32 COFF_Reader::PatchAddress_Targets(Section_Header& Code_Section) 
{ 
  for (int M = 0; M < Code_Section.Relocation_Count; M++) 
  {
    Relocation_Entry Temp;
    unsigned int Offset = Code_Section.Relocation_Offset + (RELOCATION_ENTRY_SIZE * M); 
    memcpy(&Temp, &ObjectCode[Offset], 10); 
//  printf("[%02d] Code_Offset[%08x] Symbol_Index(%04x) Type(%04x)\n", 
// M, (u32)Temp.Code_Offset, (u32)Temp.Symbol_Index, Temp.Type); 
//Symbols[Temp.Symbol_Index].out(); 

    unsigned int Code_Segment   = Sections[CODE_SECTION_INDEX].Data_Offset; 
    unsigned int Patched_Code   = Code_Segment + Temp.Code_Offset; 
    unsigned int Target_Section = Symbols[Temp.Symbol_Index].Section_Number;
    unsigned int Patch_Target   = Sections[Target_Section].Data_Offset + 
                                  Symbols[Temp.Symbol_Index].Value;

    u32 Unpatched_Target        = (u32)ObjectCode[Patched_Code];
#ifdef __linux__
    Patch_Target += Unpatched_Target; 
#endif

#ifdef DEBUG_MODE
    if (Temp.Type == RELOC_ADDR32) 
      printf("ABSOLUTE:Patch_Target[%04x]  Patched_Code[%04x](%04x)  Name(%s)\n",  
             Patch_Target, Patched_Code, Unpatched_Target, 
             Symbols[Temp.Symbol_Index].Name.c_str()); 
    else if (Temp.Type == RELOC_REL32)
      printf("RELATIVE:Patch_Target[%04x]  Patched_Code[%04x](%04x)  Name(%s)\n",  
              Patch_Target, Patched_Code, Unpatched_Target, 
              Symbols[Temp.Symbol_Index].Name.c_str()); 
#endif 

    if (Temp.Type == RELOC_REL32)
      Patch_Target = (Patch_Target - (Patched_Code + 4));   // Make Relative to address of next instruction 
    else if (Temp.Type != RELOC_ADDR32 && Temp.Type != RELOC_REL32)
    {
      printf("FAILURE NOT PATCHED: Unknown Relocation_Entry.Type\n"); 
      return 0; // Failure 
    } 

    unsigned int* Code2Patch = (unsigned int*) &ObjectCode[Patched_Code];  
    *Code2Patch  = (unsigned int) Patch_Target;   
  }
  return 1;     // Success 
}


void COFF_Reader::Sections_Out()
{
  char temp[9];
  for (u32 N = 1; N < Sections.size(); N++)
  {
    printf("Section Number(%d)\n", N);
    memcpy(&temp[0], Sections[N].Section_Name, sizeof(temp)); 
    printf("     Section_Name(%s)\n", temp);
    printf(" Physical_Address(%X)\n",   (u32)Sections[N].Physical_Address);
    printf("  Virtual_Address(%X)\n",   (u32)Sections[N].Virtual_Address);
    printf("    Size_in_Bytes(%d)\n",   (u32)Sections[N].Size_in_Bytes);
    printf("      Data_Offset(%X)\n",   (u32)Sections[N].Data_Offset);
    printf("         DATA_END(%X)\n",   (u32)(Sections[N].Data_Offset + 
                                              Sections[N].Size_in_Bytes - 1));
    printf("Relocation_Offset(%X)\n",   (u32)Sections[N].Relocation_Offset);
    printf("  Line_Num_Offset(%X)\n",   (u32)Sections[N].Line_Num_Offset);
    printf(" Relocation_Count(%d)\n",        Sections[N].Relocation_Count);
    printf("   Line_Num_Count(%d)\n",        Sections[N].Line_Num_Count);
    printf("    Section_Flags(%X)\n\n", (u32)Sections[N].Section_Flags);
  } 
}


void COFF_Reader::Symbol_Table_Out()
{
  for (u32 N = 0; N < Symbols.size(); N++)
  {  
    printf("*** Symbol_Name:%s\n",       Symbols[N].Name.c_str()); 
    printf("         Value:%X\n",   (u32)Symbols[N].Value); 
    printf("Section_Number:%d\n",        Symbols[N].Section_Number); 
    printf("          Type:%X\n",        Symbols[N].Type); 
    printf(" Storage_Class:%X\n",        Symbols[N].Storage_Class); 
    printf("     Aux_Count:%d\n\n",      Symbols[N].Aux_Count); 
  }
}


// Gets a maximum 8 byte long string from the Object File
//
std::string COFF_Reader::Get_String(unsigned int* Offset)
{
  std::string String; 
  char tempstr[9] = {0,0,0, 0,0,0, 0,0,0};
  memcpy(&tempstr[0], Offset, 8); 
  String = tempstr; 
//printf("String(%s)\n", String.c_str());
  return String;
}


// Gets a Null terminated string from the Object File
//
std::string COFF_Reader::Get_String(unsigned int Offset)
{
  std::string String; 
  for (int N = Offset; ObjectCode[N] != 0; N++)
    String += ObjectCode[N];
//printf("String(%s)\n", String.c_str());
  return String;
}



std::string COFF_Reader::GetName(Symbol_Table_Entry& Temp)
{
  unsigned int String_Table_Offset = Header.Symbol_Table_Offset + 
               (Header.Symbol_Table_Count * SYMBOL_TABLE_ENTRY_SIZE); 
  std::string Name;
  if (Temp.Zeroes == 0)
    Name = Get_String(String_Table_Offset + Temp.Offset); 
  else 
    Name = Get_String((u32*) &Temp.Symbol_Name); 
  return Name; 
}


void COFF_Reader::Symbol_Table() 
{
  for (s32 N = 0; N < Header.Symbol_Table_Count; N++)
  {
    Symbol_Table_Entry Temp;
    COFF_Symbol        Temp2; 
    unsigned int Offset = Header.Symbol_Table_Offset + (18 * N); 
    memcpy(&Temp, &ObjectCode[Offset], 18); 
    Temp2.Value          = Temp.Value; 
    Temp2.Section_Number = Temp.Section_Number; 
    Temp2.Type           = Temp.Type;  
    Temp2.Storage_Class  = Temp.Storage_Class; 
    Temp2.Aux_Count      = Temp.Aux_Count; 
    Temp2.Name           = GetName(Temp); 
    Symbols.push_back(Temp2); 
    if (Temp2.Section_Number == 0)
    {
      if (Temp2.Storage_Class == 2)
      {
        if (Temp2.Value == 0)
        {
          printf("Cannot call any library functions: %s\n", Temp2.Name.c_str());
          exit(0); 
        }
        else 
        {
          printf("Must initialize all static data: %s\n", Temp2.Name.c_str()); 
          exit(0); 
        }
//    else 
//      printf("Don't know whats up with this: %s\n", Temp2.Name.c_str()); 
      } 
    }
  }
}

/**************
int main(int argc, char *argv[])
{
  COFF_Reader Reader; 
  if (argc != 2)
  {
    printf("Need Filename!\n"); 
    return -1; 
  }

  Reader.read_obj_file(argv[1]); 
  Reader.Initialize(); 
  
  Reader.Get_Symbol_Offset("_Heap_PTR",   DATA_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_Heap_END",   DATA_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_SaveState",  CODE_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_LoadState",  CODE_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_Allocate",   CODE_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_DebugStep",  CODE_SECTION_INDEX); 
  Reader.Get_Symbol_Offset("_CopyMemory", CODE_SECTION_INDEX); 

  Reader.DebugOut(); 
  Reader.Get_Code_Offset();
  Reader.Get_Data_Offset();

  std::string OutFileName = argv[1];  
// Change input file suffix to ".bin" 
  OutFileName = OutFileName.substr(0, OutFileName.size() - 3) + "bin"; 
  Reader.write_obj_file(OutFileName.c_str()); 
//Reader.Write_Batch_File(); 
}
*********************************/ 