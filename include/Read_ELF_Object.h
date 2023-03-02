//  
//  This file depends on the ELF Object file format. 
//
//  This file reads an ELF Object file and patches
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


// 52 Bytes of Halt7.o (Object File Header)
// Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
// 00000000  7F 45 4C 46 01 01 01 00 00 00 00 00 00 00 00 00  
// 00000010  01 00 03 00 01 00 00 00 00 00 00 00 00 00 00 00  
// 00000020  90 1D 00 00 00 00 00 00 34 00 00 00 00 00 28 00  
// 00000030  0D 00 0A 00 

#include "elf.h" 

//typedef uint32_t ElfN_Addr;
//typedef uint32_t ElfN_Off;

/***
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;
typedef  int32_t Elf32_Sword; 
***/ 


class ELF_Header:         public Elf32_Ehdr 
{
  public: 
  void out()
  {
    printf("    Signature %02x%c%c%c\n", e_ident[0], e_ident[1], e_ident[2], e_ident[3]);
    printf("     EI_CLASS %02x\n", e_ident[4]);
    printf("     EI_DATA  %02x\n", e_ident[5]);
    printf("     EI_VERSI %02x\n", e_ident[6]);
    printf("     EI_OSABI %02x\n", e_ident[7]);
    printf("EI_ABIVERSION %02x\n", e_ident[8]);

    printf("       e_type %02x\n", e_type);
    printf("    e_machine %02x\n", e_machine);
    printf("    e_version %04x\n", e_version);
    printf("      e_entry %04x\n", e_entry);
    printf("      e_phoff %04x\n", e_phoff);
    printf("      e_shoff %04x\n", e_shoff);

    printf("      e_flags %04x\n", e_flags); 
    printf("     e_ehsize %02d Decimal\n", e_ehsize); 
    printf("  e_phentsize %02d Decimal\n", e_phentsize);
    printf("      e_phnum %02d Decimal\n", e_phnum);
    printf("  e_shentsize %02d Decimal\n", e_shentsize);
    printf("      e_shnum %02d Decimal\n", e_shnum); 
    printf("   e_shstrndx %02x\n", e_shstrndx); 
  }
}; 

class ELF_Program:        public Elf32_Phdr 
{
  public: 
  void out()
  {
  }
}; 

class ELF_Section:        public Elf32_Shdr 
{
  public: 
  void out()
  {
    printf("sh_name:%04x  ",      sh_name);
    printf("sh_type:%04x  ",      sh_type);
    printf("sh_flags:%04x  ",     sh_flags);
//  printf("sh_addr:%04x  ",      sh_addr);
    printf("sh_offset:%04x  ",    sh_offset);
    printf("sh_size:%04x\n",     sh_size);
//  printf("sh_link:%04x  ",      sh_link);
//  printf("sh_info:%04x  ",      sh_info);
//  printf("sh_addralign:%04x  ", sh_addralign);
//  printf("sh_entsize:%04x\n",   sh_entsize);
  }
}; 

class ELF_Symbol:        public Elf32_Sym  
{
  public: 
  void out()
  {
    printf("st_name[%08x]   ",  st_name);
    printf("st_value[%08x]   ", st_value);
    printf("st_size[%08x]  ",   st_size);
    printf("st_info[%02x]   ",  st_info);
    printf("st_other[%03x]   ", st_other);
    printf("st_shndx[%04x]\n",  st_shndx);
  }
}; 

class ELF_Relocation:     public Elf32_Rel  
{
  public: 
  void out()
  {
    printf("r_offset[%08x]  ",  r_offset);
    printf("r_info[%08x]  ",    r_info);
  }
}; 

class ELF_Relocation2:    public Elf32_Rela 
{
  public: 
  void out()
  {
    printf("r_offset[%08x]   ", r_offset);
    printf("r_info[%08x]   ",   r_info);
    printf("r_addend[%08x]\n",  r_addend);
  }
}; 


struct ELF_Reader
{
  u32 code_segment;
  u32 code_size;

  u32 data_segment; 
  u32 data_size; 

  u32 string_segment; 
  u32 string_size; 

  std::vector<unsigned char>  ObjectCode; 
  ELF_Header                  Header; 
  std::vector<ELF_Program>    Program_Headers; 
  std::vector<ELF_Section>    Sections; 
  std::vector<ELF_Symbol>     Symbols;     
  std::vector<ELF_Relocation> Relocations;     
  std::vector<ELF_Relocation> Relocations2;    

  std::map<u32, std::string> FunctionNames; 
  u32 BytesRead;          // Size of the COFF Object File
  u32 Heap_PTR_variable;  // Address of Heap_PTR Variable 
  u32 Heap_END_variable;  // Address of Heap_End Variable 
  u32 Last_Address_Of_Operating_System;      // To limit the list to user code 

  void Initialize(); 
  u32 read_obj_file(const char *filename); 
  u32 ELF_Reader::write_obj_file(const char *filename); 

  void Relocations_out() 
  {
    for (int I = 0; I < Relocations.size(); I++) 
    {
      printf("[%02d] ", I);
      Relocations[I].out();

      u32 Symbol_Index  = ELF32_R_SYM(Relocations[I].r_info); 
      u32 Relocate_Type = ELF32_R_TYPE(Relocations[I].r_info); 
      u32 Name_Index    = string_segment + Symbols[Symbol_Index].st_name; 
      std::string Name  = (char*) &ObjectCode[Name_Index]; 
      u32 Patch_Address = code_segment + Symbols[Symbol_Index].st_value; 
      u32 Address2Patch = code_segment + Relocations[I].r_offset; 

      if (Relocate_Type == 1)
        Patch_Address = data_segment + Symbols[Symbol_Index].st_value; 
      if (Symbols[Symbol_Index].st_name)
        printf("Address2Patch[%08x]  Patch_Address[%08x]  ", Address2Patch, Patch_Address); 
      else 
        printf("Address2Patch[%08x]  Patch_Address[%08x]  ", 0, 0); 
      printf("%-46s\n", Name.c_str());
      if (Symbols[Symbol_Index].st_name) 
      {
        u32* Address2Change = (u32*) &ObjectCode[Address2Patch]; 
        *Address2Change  = Patch_Address; // Patch The Address
//      ObjectCode[Address2Patch] = Patch_Address; // Patch The Address
      }
    } 
/*****************
    for (int I = 0; I < Relocations2.size(); I++) 
    {
      u32 Symbol_Index  = ELF32_R_SYM(Relocations2[I].r_info); 
      u32 Relocate_Type = ELF32_R_TYPE(Relocations2[I].r_info); 
      printf("[%02d] ", I);
      Relocations2[I].out();
      printf("\n");
    }
***********************/
  }

  void Symbols_out() 
  {
    for (int N = 0; N < Symbols.size(); N++) 
    {
      if (Symbols[N].st_size)
      {
        // offset to code_segment or data_segment 
        u32 Index   = Symbols[N].st_shndx; 
        u32 Offset  = Sections[Index].sh_offset; 
        u32 Address = Offset + Symbols[N].st_value; 
        printf("[%02x] Address[%08x]  ", N, Address); 
      }
      else 
        printf("[%02x] Address[--None--]  ", N); 
      u32 Index = string_segment + Symbols[N].st_name; 
      std::string Name = (char*) &ObjectCode[Index]; 
      printf("%-46s", Name.c_str());  
      Symbols[N].out();
    }
  }

  void Sections_out() 
  { 
    for (uint32_t N = 0; N < Sections.size(); N++) 
    { 
      uint32_t Index = Sections[Header.e_shstrndx].sh_offset + Sections[N].sh_name; 
      std::string Name = (char*) &ObjectCode[Index]; 
      printf("[%04d] %-16s", N, Name.c_str());  
      Sections[N].out(); 
    } 
  } 
}; 


void ELF_Reader::Initialize()
{
  for (uint32_t N = 0; N < Sections.size(); N++) 
  { 
    uint32_t Index = Sections[Header.e_shstrndx].sh_offset + Sections[N].sh_name; 
    std::string Name = (char*) &ObjectCode[Index]; 

    if (Name == ".text" && Sections[N].sh_type == SHT_PROGBITS)
    {

      this->code_segment = Sections[N].sh_offset; 
      this->code_size    = Sections[N].sh_size; 
      printf("[%02d] .text            PROGBITS\n", N); 
    }

    if (Name == ".rel.text" && Sections[N].sh_type == SHT_REL)
    {
      Relocations.resize(Sections[N].sh_size / sizeof(ELF_Relocation));  
      std::memcpy(&Relocations[0], &ObjectCode[Sections[N].sh_offset], Sections[N].sh_size);  
      printf("Relocation.size(%d)\n", Relocations.size()); 
      printf("[%02d] .rel.text        REL\n", N); 
    }

    if (Name == ".data" && Sections[N].sh_type == SHT_PROGBITS)
    {
      this->data_segment = Sections[N].sh_offset; 
      this->data_size    = Sections[N].sh_size; 
      printf("[%02d] .data            PROGBITS\n", N); 
    }

    if (Name == ".bss" && Sections[N].sh_type == SHT_NOBITS)
      printf("[%02d] .bss             NOBITS\n", N); 

    if (Name == ".rodata" && Sections[N].sh_type == SHT_PROGBITS)
      printf("[%02d] .rodata          PROGBITS\n", N); 

    if (Name == ".comment" && Sections[N].sh_type == SHT_PROGBITS)
      printf("[%02d] .comment         PROGBITS\n", N); 

    if (Name == ".note.GNU-stack" && Sections[N].sh_type == SHT_PROGBITS)
      printf("[%02d] .note.GNU-stack  PROGBITS\n", N); 

    if (Name == ".eh_frame" && Sections[N].sh_type == SHT_PROGBITS)
      printf("[%02d] .eh_frame        PROGBITS\n", N); 

    if (Name == ".rel.eh_frame" && Sections[N].sh_type == SHT_REL)
    {
//    Relocations2.resize(Sections[N].sh_size / sizeof(ELF_Relocation));  
//    std::memcpy(&Relocations2[0], &ObjectCode[Sections[N].sh_offset], Sections[N].sh_size);  
//    printf("Relocation.size(%d)\n", Relocations2.size());
      printf("[%02d] .rel.eh_frame    REL\n", N); 
    }

    if (Name == ".shstrtab" && Sections[N].sh_type == SHT_STRTAB)
      printf("[%02d] .shstrtab        STRTAB\n", N);  

    if (Name == ".symtab" && Sections[N].sh_type == SHT_SYMTAB)
    {
      Symbols.resize(Sections[N].sh_size / sizeof(ELF_Symbol));  
      std::memcpy(&Symbols[0], &ObjectCode[Sections[N].sh_offset], Sections[N].sh_size);  
      printf("Symbols.size(%d)\n", Symbols.size());
      printf("[%02d] .symtab          SYMTAB\n", N); 
    }

    if (Name == ".strtab" && Sections[N].sh_type == SHT_STRTAB)
    {
      this->string_segment = Sections[N].sh_offset; 
      this->string_size    = Sections[N].sh_size; 
      printf("[%02d] .strtab          STRTAB\n", N); 
    }
  } 
}

  
  





// Read Binary file as a single block. 
u32 ELF_Reader::read_obj_file(const char *filename)
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
    std::memcpy(&Header, &ObjectCode[0], sizeof(Header)); 

    uint32_t Section_Header_Size       = Header.e_shentsize; 
    uint16_t Num_Section_Headers       = Header.e_shnum; 
    uint16_t Section_Header_Offset     = Header.e_shoff;
    uint32_t Num_Section_Header_Bytes  = Section_Header_Size * Num_Section_Headers; 
    Sections.resize(Num_Section_Headers);
    std::memcpy(&Sections[0], &ObjectCode[Section_Header_Offset], Num_Section_Header_Bytes);  
    return Bytes_Read;
  }
  else
    std::cout << "Reading: " << filename << " failed!\n";     
  return 0; 
}


// Read Binary file as a single block. 
u32 ELF_Reader::write_obj_file(const char *filename)
{
  u32 Bytes_Written = 0; 
  std::FILE *fp = std::fopen(filename, "wb");
  if (fp)
  {
    Bytes_Written = std::fwrite(&ObjectCode[0], 1, ObjectCode.size(), fp);
    std::fclose(fp);
    std::cout << "Wrote: " << Bytes_Written << " bytes from \"" << filename << "\"\n";     
    return Bytes_Written;
  }
  else
    std::cout << "Writing: " << filename << " failed!\n";     
  return 0; 
}
