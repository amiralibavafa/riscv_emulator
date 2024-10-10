//Part2.c
#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *, Byte *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(uint32_t instruction_bits, Processor *processor,Byte *memory) {    
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            execute_rtype(instruction, processor, memory);
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;
        case 0x73:
            execute_ecall(processor, memory);
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x6F:
            execute_jal(instruction, processor);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            break;
        case 0x2a: //Custome LSGT                                                       PART3
            execute_rtype(instruction, processor, memory);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.rtype.funct3){
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                  // Add
                  processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) +
                      ((sWord)processor->R[instruction.rtype.rs2]);

                  break;
                case 0x1:
                  // Mul
                  processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) *
                      ((sWord)processor->R[instruction.rtype.rs2]);
                  break;
                case 0x20:
                    // Sub
                    processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) -
                      ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SLL
                    processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) <<
                      ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x1:
                    // MULH
                    processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1] >> 31) *
                      ((sWord)processor->R[instruction.rtype.rs2] >> 31);
                    break;
            }
            break;
        case 0x2:
            // SLT
            if (instruction.opcode == 0x33){
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.rtype.rs1]) <
                ((sWord)processor->R[instruction.rtype.rs2]) ? 1 : 0;
            }

            //LSGT                                                                          PART3
            else if (instruction.opcode == 0x2a) { 
                // Load the value from memory at the address in rs2
                sWord mem_value = load(memory, processor->R[instruction.rtype.rs2], LENGTH_WORD);

                // Compare rs1 with MEM[rs2]
                if ((sWord) processor->R[instruction.rtype.rs1] > mem_value) {
                    // If rs1 > MEM[rs2], set rd to MEM[rs2]
                    processor->R[instruction.rtype.rd] = mem_value;
                }
            }
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // XOR
                    processor->R[instruction.rtype.rd] =
                    ((sWord)processor->R[instruction.rtype.rs1]) ^
                    ((sWord)processor->R[instruction.rtype.rs2]);

                    break;
                case 0x1:
                    // DIV
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) /
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SRL
                    processor->R[instruction.rtype.rd] =
                        ((Word)processor->R[instruction.rtype.rs1]) >>  //Using Word unsigned for Logical shift  
                        ((sWord)processor->R[instruction.rtype.rs2]);        
                    break;
                case 0x20:
                    // SRA
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) >>
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // OR                   
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) |
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) |
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x1:
                    // REM
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) %
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x7:
            // AND
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.rtype.rs1]) &
                ((sWord)processor->R[instruction.rtype.rs2]);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    (processor->PC) += 4;
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch (instruction.itype.funct3) {
        case 0x0:
            // ADDI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) +
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        case 0x1:
            // SLLI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) <<
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        case 0x2:
            // STLI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) <
                ((sWord)sign_extend_number(instruction.itype.imm, 12));            
            break;
        case 0x4:
            // XORI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) ^
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        case 0x5:
            // Shift Right (You must handle both logical and arithmetic)
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) >>
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        case 0x6:
            // ORI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) |
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        case 0x7:
            // ANDI
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) &
                ((sWord)sign_extend_number(instruction.itype.imm, 12));
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}

void execute_ecall(Processor *p, Byte *memory) {
    Register i;
    
    // syscall number is given by a0 (x10)
    // argument is given by a1
    switch(p->R[10]) {
        case 1: // print an integer
            printf("%d",p->R[11]);
            break;
        case 4: // print a string
            for(i=p->R[11];i<MEMORY_SPACE && load(memory,i,LENGTH_BYTE);i++) {
                printf("%c",load(memory,i,LENGTH_BYTE));
            }
            break;
        case 10: // exit
            printf("exiting the simulator\n");
            exit(0);
            break;
        case 11: // print a character
            printf("%c",p->R[11]);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]);
            exit(-1);
            break;
    }
    p->PC += 4;
}

void execute_branch(Instruction instruction, Processor *processor) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            // BEQ
            if ((sWord)processor->R[instruction.sbtype.rs1] == (sWord)processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);  // branch taken, update PC with offset
                return;
            }
            break;
        case 0x1:
            // BNE
            if ((sWord)processor->R[instruction.sbtype.rs1] != (sWord)processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);  // branch taken, update PC with offset
                return;
            }
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }

    // If the branch is not taken, increment the PC to the next instruction
    processor->PC += 4;
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.itype.funct3) {
        case 0x0:
            // LB
            processor->R[instruction.itype.rd] = 
            sign_extend_number(
                load(memory, ((sWord)processor->R[instruction.itype.rs1]) + 
                    ((sWord)sign_extend_number(instruction.itype.imm, 12)),
                        LENGTH_BYTE), 
                            8);
            break;

        case 0x1:
            // LH
            processor->R[instruction.itype.rd] = 
            sign_extend_number(
                load(memory, ((sWord)processor->R[instruction.itype.rs1]) + 
                    ((sWord)sign_extend_number(instruction.itype.imm, 12)),
                        LENGTH_HALF_WORD),
                            16);
            break;

        case 0x2:
            // LW
            processor->R[instruction.itype.rd] =
            load(memory, ((sWord)processor->R[instruction.itype.rs1]) +
                ((sWord)sign_extend_number(instruction.itype.imm, 12)),
                    LENGTH_WORD);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.stype.funct3) {
        case 0x0:
            // SB
            store(
                memory, 
                ((sWord)processor->R[instruction.stype.rs1]) + 
                    ((sWord)get_store_offset(instruction)), 
                LENGTH_WORD, 
                processor->R[instruction.stype.rs2]);
            break;
        case 0x1:
            // SH
            store(
                memory, 
                ((sWord)processor->R[instruction.stype.rs1]) + 
                    ((sWord)get_store_offset(instruction)), 
                LENGTH_HALF_WORD, 
                processor->R[instruction.stype.rs2]);
            break;
        case 0x2:
            // SW
            store(
                memory,
                ((sWord)processor->R[instruction.stype.rs1]) +
                    ((sWord)get_store_offset(instruction)),
                LENGTH_WORD,
                processor->R[instruction.stype.rs2]);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}

void execute_jal(Instruction instruction, Processor *processor) {
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC = processor->PC + ((sWord)get_jump_offset(instruction));
}

void execute_lui(Instruction instruction, Processor *processor) {
    processor->R[instruction.utype.rd] = sign_extend_number(instruction.utype.imm, 32) << 12;
    processor->PC += 4;
}

void execute_slt(Instruction instruction, Processor *processor, Byte *memory) {
    // SLT
    if (((sWord)processor->R[instruction.rtype.rs1]) < ((sWord)load(memory, (sWord)processor->R[instruction.rtype.rs2], LENGTH_WORD)))
    {
        processor->R[instruction.rtype.rd] = (sWord)load(memory, (sWord)processor->R[instruction.rtype.rs2], LENGTH_WORD);
    }
    processor->PC += 4;
}

void store(Byte *memory, Address address, Alignment alignment, Word value) {
    if (alignment == LENGTH_WORD)
    {
        memory[address] = (Byte)(value & 0x000000ff);
        memory[address + 1] = (Byte)(value & 0x0000ff00) >> 8;
        memory[address + 2] = (Byte)(value & 0x00ff0000) >> 16;
        memory[address + 3] = (Byte)(value & 0xff000000) >> 24;
    }
    else if(alignment == LENGTH_HALF_WORD){
        memory[address] = (Byte)(value & 0x00ff);
        memory[address+1] = (Byte)((value & 0xff00)>>8);
    }
    else if(alignment == LENGTH_BYTE){
        memory[address] = (Byte)(value & 0xff);
    }
}

Word load(Byte *memory, Address address, Alignment alignment) {
    Word word = 0x00000000;
    if (alignment == LENGTH_WORD)
    {
        word |= memory[address];
        word|= (memory[address + 1] << 8);
        word|= (memory[address + 2] << 16);
        word|= (memory[address + 3] << 24);   
    }
    else if(alignment == LENGTH_HALF_WORD){
        word |= memory[address];
        word |= (memory[address+1] << 8);
    }
    else if(alignment == LENGTH_BYTE){
        word |= memory[address];
    }
    return word;
}
