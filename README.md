# RISC-V Emulator

## Overview

The RISC-V Emulator is a framework designed to simulate RISC-V machine code execution. The project is divided into three parts:

1. **Disassembler**: Translate RISC-V machine code into human-readable assembly code.
2. **Emulator**: Execute RISC-V machine code, handling various instruction types.
3. **Custom Instructions**: Implement support for custom RISC-V instructions.

## Project Structure

The provided files include:

- `riscv.h`: C header file for the functions you are implementing.
- `riscv.c`: C source file for the program loader and main function.
- `part1.c`: Code for the disassembler.
- `part2.c`: Code for the emulator.
- `utils.c`: Utility functions to support disassembly and execution.
- `utils.h`: File that contains the format for instructions to print for part 1.
- `types.h`: C header file for the data types you will be dealing with. It is critical that you read and understand the definitions in this file before starting the project. If they seem unclear, consult the bitfields and unions presentation included in this repository.
- `Makefile`: File that records all dependencies.
- `code/input`: Directory containing various files to run tests.
- `code/ref`: Directory with reference outputs (`_.solution` for part 1 and `_.trace` for part 2).

## Getting Started

### Prerequisites

Ensure you have the following installed:

- **C Compiler**: GCC or any compatible C compiler.
- **Make**: A build tool to compile the project.

### Installation

1. **Download the Project**: Clone or download the ZIP file of the RISC-V Emulator project.

   ```bash
   git clone <repository_url>
   Compile the Project:

   #Use the Makefile to compile the emulator:
   
   make riscv
   
   #Running the Program
   #After successfully compiling the project, you can run the emulator with the following commands:
   
   #Example Test Cases
   #To run a test program with the random input:
   
   ./riscv -r -e ./code/input/simple.input
   
   #To disassemble a custom input file:
   
   ./riscv -d ./code/input/custom_sgt.input
