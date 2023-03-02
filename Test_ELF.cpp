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

#include "include/Read_ELF_Object.h"


int main(int argc, char *argv[])
{
  ELF_Reader Reader; 
  if (argc != 2)
  {
    printf("Need Filename!\n"); 
    return -1; 
  } 
  Reader.read_obj_file(argv[1]); 
  printf("Reader.ObjectCode.size(%d)\n", Reader.ObjectCode.size()); 
  Reader.Header.out(); 
  Reader.Initialize();
  Reader.Sections_out(); 
  Reader.Symbols_out(); 
  Reader.Relocations_out(); 
  Reader.write_obj_file("Halt7.ooo");
}
