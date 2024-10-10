#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/* Sign extends the given field to a 32-bit integer where field is
 * interpreted an n-bit integer. */
int sign_extend_number(unsigned int field, unsigned int n) {
  return (int)field << (32 - n) >> (32 - n);
}

void print_binary(unsigned int number, int size) // Size is in bits
{
//   if (!number)
//   {
//     putc('0', stdout);
//     return;
//   }
  if (size == 1)
  {
    putc((number & 1) ? '1' : '0', stdout);
    return;
  }
  print_binary(number >> 1, size - 1);
  putc((number & 1) ? '1' : '0', stdout);
}

/* Unpacks the 32-bit machine code instruction given into the correct
 * type within the instruction struct */
Instruction parse_instruction(uint32_t instruction_bits) {
  /* YOUR CODE HERE */
  Instruction instruction;
  // add x8, x0, x0     hex : 00000433  binary = 0000 0000 0000 0000 0000 01000
  // Opcode: 0110011 (0x33) Get the Opcode by &ing 0x1111111, bottom 7 bits
  instruction.opcode = instruction_bits & ((1U << 7) - 1);

  // Shift right to move to pointer to interpret next fields in instruction.
  instruction_bits >>= 7;

  switch (instruction.opcode) {
  // R-Type
  case 0x33:
    // instruction: 0000 0000 0000 0000 0000 destination : 01000
    instruction.rtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 0000 0000 0000 0 func3 : 000
    instruction.rtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.rtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 000        src2: 00000
    instruction.rtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // funct7: 0000 000
    instruction.rtype.funct7 = instruction_bits & ((1U << 7) - 1);
    break;
  // I-type
  case 0x13:
  case 0x03:
  case 0x73:
    //instruction: getting the 5 bits of rd stored before opcode
    instruction.itype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    //instruction: including the func3
    instruction.itype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.itype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    //instruction: adding the imm value to it
    instruction.itype.imm = instruction_bits & ((1U << 12) - 1);
    
    // Sign extend the 12-bit immediate if needed
    if (instruction.itype.imm & (1U << 11)) {
        // If the MSB is 1, extend the sign to fill the upper 20 bits
        instruction.itype.imm |= 0xFFFFF000;
    }
    break;

  // U-Type
  case 0x37:
    // Extract rd (5 bits)
    instruction.utype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract the 20-bit immediate
    instruction.utype.imm = instruction_bits & ((1U << 20) - 1);

    break;

  // UJ-Type
  case 0x6F:
    // Extract rd (5 bits)
    instruction.ujtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract immediate (split across different bit ranges)
    unsigned int imm_19_12 = (instruction_bits >> 12) & ((1U << 8) - 1);  // Bits 19:12
    unsigned int imm_11 = (instruction_bits >> 11) & 1;                    // Bit 11
    unsigned int imm_10_1 = (instruction_bits >> 1) & ((1U << 10) - 1);    // Bits 10:1
    unsigned int imm_20 = (instruction_bits >> 20) & 1;                    // Bit 20 (the sign bit)

    // Combine all the parts of the immediate
    instruction.ujtype.imm = (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1);

    break;

  // S-Type
  case 0x23:
    // Extract imm5 (lower 5 bits of immediate)
    instruction.stype.imm5 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract funct3 (3 bits)
    instruction.stype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // Extract rs1 (source register 1, 5 bits)
    instruction.stype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract rs2 (source register 2, 5 bits)
    instruction.stype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract imm7 (upper 7 bits of immediate)
    instruction.stype.imm7 = instruction_bits & ((1U << 7) - 1);
    break;

  // SB-Type
  case 0x63:
    // Extract imm5 (lower 5 bits of immediate)
    instruction.sbtype.imm5 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract funct3 (3 bits)
    instruction.sbtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // Extract rs1 (source register 1, 5 bits)
    instruction.sbtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract rs2 (source register 2, 5 bits)
    instruction.sbtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // Extract imm7 (upper 7 bits of immediate)
    instruction.sbtype.imm7 = instruction_bits & ((1U << 7) - 1);
    break;

  // LSGT case                                                                      PART3
  case 0x2A:
    // instruction: 0000 0000 0000 0000 0000 destination : 01000
    instruction.rtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 0000 0000 0000 0 func3 : 000
    instruction.rtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.rtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 000        src2: 00000
    instruction.rtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // funct7: 0000 000
    instruction.rtype.funct7 = instruction_bits & ((1U << 7) - 1);
    break;


  default:
    exit(EXIT_FAILURE);
  }
  return instruction;
}

/* Return the number of bytes (from the current PC) to the branch label using
 * the given branch instruction */
int get_branch_offset(Instruction instruction) {
  // Assuming the Instruction is already SB-Type
  int temp = 0x00000000;
  temp |= ((instruction.sbtype.imm5 & 0x00000001) << 11);
  temp |= (((instruction.sbtype.imm5 >> 1) & ((1U << 4) - 1)) << 1);
  temp |= ((instruction.sbtype.imm7 << 5) & ((1U << 6) - 1) << 5);
  temp |= (((instruction.sbtype.imm7 >> 6) & 0x00000001) << 12);
  return (sign_extend_number(temp, 12));
}

/* Returns the number of bytes (from the current PC) to the jump label using the*/
 /* given jump instruction */
int get_jump_offset(Instruction instruction) {
  int temp = 0x00000000;
  instruction.ujtype.imm >>= 1;
  temp |= ((instruction.ujtype.imm & ((1U << 8) - 1)) << 11);
  temp |= (((instruction.ujtype.imm >> 8) & 0x00000001) << 10);
  temp |= (((instruction.ujtype.imm >> 9) & ((1U << 10) - 1)) << 1);
  temp |= (((instruction.ujtype.imm >> 19) & 0x00000001) << 19);
  temp <<= 1;
  return sign_extend_number(temp, 20);
}

int get_store_offset(Instruction instruction) {
  unsigned int store_offset = (instruction.stype.imm7 << 5) | (instruction.stype.imm5 & 0x1F);
  store_offset = sign_extend_number(store_offset, 12); //sign extending from 12 bits to 32

  return store_offset;
}

void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}

void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}

void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}
