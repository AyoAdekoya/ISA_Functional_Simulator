#include <stdio.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>

#include "MemoryStore.h"
#include "RegisterInfo.h"

// --------------------------------------------------------------------------
// Reg data structure
// --------------------------------------------------------------------------

union REGS {
    RegisterInfo reg;
    uint64_t registers[REG_SIZE] {0};
};

union REGS regData;

uint64_t PC;

// --------------------------------------------------------------------------
// Decode constants
// --------------------------------------------------------------------------

// TODO Complete these enums, add more if necessary

enum OPCODES {
    // I-type opcodes
    OP_INTIMM  = 0b0010011, // Integer ALU immediate instructions addi, slli, slti, sltiu, xori, srli, srai, ori, andi
    OP_OFFIMM = 0b0000011, // Offset immediate instructions lb, lh, lw, ld, lbu, lhu lwu
    OP_WORIMM = 0b0011011, // Immediate Word insturctions addiw slliw srliw sraiw
    OP_LNKREG = 0b1100111, // Jump and Link Register Instruction jalr
    // R-type opcodes
    OP_REGFMT = 0b0110011, // Register Format Instructions add, sub, sll, slt, sltu, xor, srl, sra, or, and
    OP_REGWRD = 0b0111011, // Register Format with word instructions addw, subw, sllw, srlw, sraw
    // S-type opcodes
    OP_STRFMT = 0b0100011, //  Store format instructions sb, sh, sw, sd
    // SB-type opcodes
    OP_STRBYT = 0b1100011, // Store byte instructions beq, bne, blt, bge, bltu, bgeu
    // U-type opcodes
    OP_ADDIMM = 0b0010111, // Add Upper Immediate Instructions auipc
    OP_LDUIMM = 0b0110111, // Load Upper Immediate Instruction lui
    //UJ-type opcodes
    OP_JMPLNK = 0b1101111 // Jump and Link Instruction jal
};

enum FUNCT3 {
    // Operating on different sizes
    FUNCT3_BYT = 0b000, // lb, sb
    FUNCT3_HLW = 0b001, // lh, sh
    FUNCT3_WRD = 0b010, // lw, sw
    FUNCT3_DBL = 0b011, // ld, sd
    FUNCT3_BYU = 0b100, // lbu
    FUNCT3_HWU = 0b101, // lhu
    FUNCT3_WDU = 0b110, // lwu
    // For integer ALU instructions
    FUNCT3_ADD  = 0b000, // add addi, addiw, addw 
    FUNCT3_SUB = 0b000, // sub, subw
    // For immedieate instructions
    FUNCT3_SLL = 0b001, // slli slliw, sll, sllw
    FUNCT3_SET = 0b010, // slti, slt
    FUNCT3_STU = 0b011, // sltiu, sltu
    FUNCT3_XOR = 0b100, // xori, xor
    FUNCT3_SHIFT = 0b101, // srli, srai, srliw, sraiw, srl, sra, srlw, sraw
    FUNCT3_OR = 0b110, // ori, or
    FUNCT3_AND = 0b111, //andi, and
    // For SB Instructions
    FUNCT3_BEQ = 0b000, // beq
    FUNCT3_BNE = 0b001, // bne
    FUNCT3_BLT = 0b100, // blt
    FUNCT3_BGE = 0b101, // bge
    FUNCT3_BLU = 0b110, // blt
    FUNCT3_BGU = 0b111, // bgeu
    // For jalr instruction I
    FUNCT3_JAL = 0b000 // jalr
};

enum RI_FUNCT7 {
    // for add/sub instruction
    FUNCT7_ADD = 0b0000000, // add, addw
    FUNCT7_SUB = 0b0100000, // sub, subw
    // for shift instructions
    FUNCT7_SL = 0b0000000, // sll, slt, srl, srlw, sllw, slli, srli, slliw, srliw,
    FUNCT7_SA = 0b0100000, // sra, srai, sraiw, sraw
    // for conditional 
    // for logical operations
    FUNCT7_AND = 0b0000000, // and
    FUNCT7_OR = 0b0000000, //or
    FUNCT7_XOR = 0b0000000, //xor

};

// --------------------------------------------------------------------------
// Bit-level manipulation helpers
// --------------------------------------------------------------------------

// TODO You may wish to declare some helper functions for bit extractions
// and sign extensions

// --------------------------------------------------------------------------
// Utilities
// --------------------------------------------------------------------------

// initialize memory with program binary
bool initMemory(char *programFile, MemoryStore *myMem);

// dump registers and memory
void dump(MemoryStore *myMem);

// --------------------------------------------------------------------------
// Simulation functions
// --------------------------------------------------------------------------

// The simulator maintains the following struct as it simulates 
// RISC-V instructions. Feel free to add more fields if needed.
struct Instruction {
    uint64_t PC = 0;
    uint64_t instruction = 0; // raw instruction binary

    bool     isHalt = false;
    bool     isLegal = false;
    bool     isNop = false;

    bool     readsMem = false;
    bool     writesMem = false;
    bool     doesArithLogic = false;
    bool     writesRd = false;
    bool     readsRs1 = false;
    bool     readsRs2 = false;

    uint64_t opcode = 0;
    uint64_t funct3 = 0;
    uint64_t funct7 = 0;
    uint64_t rd = 0;
    uint64_t rs1 = 0;
    uint64_t rs2 = 0;

    uint64_t nextPC = 0;

    uint64_t op1Val = 0;
    uint64_t op2Val = 0;

    uint64_t arithResult = 0;
    uint64_t memAddress = 0;
    uint64_t memResult = 0;
};

// The following functions are the core of the simulator. Your task is to
// complete these functions in sim.cpp. Do not modify their signatures.
// However, feel free to declare more functions if needed.

// There is no strict rule on what each function should do, but the
// following comments give suggestions.

// Get raw instruction bits from memory
Instruction simFetch(uint64_t PC, MemoryStore *myMem);

// Determine instruction opcode, funct, reg names, and what resources to use
Instruction simDecode(Instruction inst);

// Collect reg operands for arith or addr gen
Instruction simOperandCollection(Instruction inst, REGS regData);

// Resolve next PC whether +4 or branch/jump target
Instruction simNextPCResolution(Instruction inst);

// Perform arithmetic/logic operations
Instruction simArithLogic(Instruction inst);

// Generate memory address for load/store instructions
Instruction simAddrGen(Instruction inst);

// Perform memory access for load/store instructions
Instruction simMemAccess(Instruction inst, MemoryStore *myMem);

// Write back results to registers
Instruction simCommit(Instruction inst, REGS &regData);

// Simulate the whole instruction using functions above
Instruction simInstruction(uint64_t &PC, MemoryStore *myMem, REGS &regData);