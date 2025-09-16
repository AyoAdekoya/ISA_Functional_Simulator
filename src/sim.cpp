#include "sim.h"

using namespace std;

// RV64I without csr, environment, or fence instructions

//           31          25 24 20 19 15 14    12 11          7 6      0
// R  type: | funct7       | rs2 | rs1 | funct3 | rd          | opcode |
// I  type: | imm[11:0]          | rs1 | funct3 | rd          | opcode |
// S  type: | imm[11:5]    | rs2 | rs1 | funct3 | imm[4:0]    | opcode |
// SB type: | imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1|11] | opcode |
// U  type: | imm[31:12]                        | rd          | opcode |
// UJ type: | imm[20|10:1|11|19:12]             | rd          | opcode |

// initialize memory with program binary
bool initMemory(char *programFile, MemoryStore *myMem) {
    // open instruction file
    ifstream infile;
    infile.open(programFile, ios::binary | ios::in);

    if (!infile.is_open()) {
        fprintf(stderr, "\tError open input file\n");
        return false;
    }

    // get length of the file and read instruction file into a buffer
    infile.seekg(0, ios::end);
    int length = infile.tellg();
    infile.seekg (0, ios::beg);

    char *buf = new char[length];
    infile.read(buf, length);
    infile.close();

    int memLength = length / sizeof(buf[0]);
    int i;
    for (i = 0; i < memLength; i++) {
        myMem->setMemValue(i * BYTE_SIZE, buf[i], BYTE_SIZE);
    }

    return true;
}

// dump registers and memory
void dump(MemoryStore *myMem) {

    dumpRegisterState(regData.reg);
    dumpMemoryState(myMem);
}

// TODO All functions below (except main) are incomplete.
// Only ADDI is implemented. Your task is to complete these functions.

// Get raw instruction bits from memory
Instruction simFetch(uint64_t PC, MemoryStore *myMem) {
    // fetch current instruction
    uint64_t instruction;
    myMem->getMemValue(PC, instruction, WORD_SIZE);
    instruction = (uint32_t)instruction;

    Instruction inst;
    inst.PC = PC;
    inst.instruction = instruction;
    return inst;
}

// Determine instruction opcode, funct, reg names, and what resources to use
Instruction simDecode(Instruction inst) {
    inst.opcode = inst.instruction & 0b1111111;
    inst.funct3 = inst.instruction >> 12 & 0b111;
    inst.rd = inst.instruction >> 7 & 0b11111;
    inst.rs1 = inst.instruction >> 15 & 0b11111;

    if (inst.instruction == 0xfeedfeed) {
        inst.isHalt = true;
        return inst; // halt instruction
    }
    if (inst.instruction == 0x00000013) {
        inst.isNop = true;
        return inst; // NOP instruction
    }
    inst.isLegal = true; // assume legal unless proven otherwise

    switch (inst.opcode) {
        // addi, slli, slti, sltiu, xori, srli, srai, ori, andi
        case OP_INTIMM:
            if (inst.funct3 == FUNCT3_ADD ||
                inst.funct3 == FUNCT3_SET || 
                inst.funct3 == FUNCT3_STU ||
                inst.funct3 == FUNCT3_SLL && inst.funct7 == FUNCT7_SL ||
                inst.funct3 == FUNCT3_XOR ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SA) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SL) ||
                inst.funct3 == FUNCT3_OR ||
                inst.funct3 == FUNCT3_AND) {
                inst.doesArithLogic = true;
                inst.writesRd = true;
                inst.readsRs1 = true;
                inst.readsRs2 = false;
                inst.writesMem = false;
                inst.readsMem = false;
            }
            else {
                inst.isLegal = false;
            }
            break;
        // lb, lh, lw, ld, lbu, lhu lwu
        case OP_OFFIMM:
            if (inst.funct3 == FUNCT3_BYT ||
                inst.funct3 == FUNCT3_HLW || 
                inst.funct3 == FUNCT3_WRD || 
                inst.funct3 == FUNCT3_DBL || 
                inst.funct3 == FUNCT3_BYU || 
                inst.funct3 == FUNCT3_HWU
            ){
                inst.doesArithLogic = true;
                inst.writesRd = true;
                inst.readsRs1 = true;
                inst.readsRs2 = false;
                inst.readsMem = true;
                inst.writesMem = false;
            }
            else{
                inst.isLegal = false;
            }
            break;
        // addiw slliw srliw sraiw
        case OP_WORIMM:
            if (inst.funct3 == FUNCT3_ADD || 
                (inst.funct3 == FUNCT3_SLL && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SL))
                {
                    inst.doesArithLogic = true;
                    inst.writesRd = true;
                    inst.readsRs1 = true;
                    inst.readsRs2 = false;
                    inst.readsMem = false;
                    inst.writesMem = false;  
                }
            else{
                inst.isLegal = false;
                }
            break;
        // Jump and Link Register Instruction jalr
        case OP_LNKREG:
            if (inst.funct3 = FUNCT3_JAL) {
                inst.doesArithLogic = false;
                inst.writesRd = true;
                inst.readsRs1 = true;
                inst.readsRs2 = false;
                inst.readsMem = false;
                inst.writesMem = false;
            }
            else{
                inst.isLegal = false;
                }
            break;
        // Register Format Instructions add, sub, sll, slt, sltu, xor, srl, sra, or, and
        case OP_REGFMT:
            if ((inst.funct3 == FUNCT3_ADD && inst.funct7 == FUNCT7_ADD) ||
                (inst.funct3 == FUNCT3_SUB && inst.funct7 == FUNCT7_SUB) ||
                (inst.funct3 == FUNCT3_SLL && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_SET && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_STU && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_XOR && inst.funct7 == FUNCT7_XOR) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SA) ||
                (inst.funct3 == FUNCT3_OR && inst.funct7 == FUNCT7_OR) ||
                (inst.funct3 == FUNCT3_AND && inst.funct7 == FUNCT7_AND)
            ) {
                inst.doesArithLogic = true;
                inst.writesRd = true;
                inst.readsRs1 = true;
                inst.readsRs2 = true;
                inst.readsMem = false;
                inst.writesMem = false;
            }
            else {
                inst.isLegal = false;
            }
            break;
        // Register Format with word instructions addw, subw, sllw, srlw, sraw
        case OP_REGWRD:
            if ((inst.funct3 == FUNCT3_ADD &&  inst.funct7 == FUNCT7_ADD) ||
                (inst.funct3 == FUNCT3_SUB && inst.funct7 == FUNCT7_SUB) ||
                (inst.funct3 == FUNCT3_SLL && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SL) ||
                (inst.funct3 == FUNCT3_SHIFT && inst.funct7 == FUNCT7_SA))
                {
                    inst.doesArithLogic = true;
                    inst.writesRd = true;
                    inst.readsRs1 = true;
                    inst.readsRs2 = true;
                    inst.writesMem = false;
                    inst.readsMem = false;
                }
            else {
                inst.isLegal = false;
            }
            break;
        //  Store format instructions sb, sh, sw, sd
        case OP_STRFMT:
            if (inst.funct3 == FUNCT3_BYT ||
                inst.funct3 == FUNCT3_HLW || 
                inst.funct3 == FUNCT3_WRD || 
                inst.funct3 == FUNCT3_DBL)
                {
                    inst.doesArithLogic = false;
                    inst.writesRd = false;
                    inst.readsRs1 = true;
                    inst.readsRs2 = true;
                    inst.writesMem = true;
                    inst.readsMem = false;
                }
            else {
                inst.isLegal = false;
            }
            break;
        // Store byte instructions beq, bne, blt, bge, bltu, bgeu
        case OP_STRBYT:
            if (inst.funct3 == FUNCT3_BEQ || 
                inst.funct3 == FUNCT3_BNE ||
                inst.funct3 == FUNCT3_BLT ||
                inst.funct3 == FUNCT3_BGE ||
                inst.funct3 == FUNCT3_BLU ||
                inst.funct3 == FUNCT3_BGU) 
            {
                inst.doesArithLogic = false;
                inst.readsRs1 = true;
                inst.readsRs2 = true;
                inst.writesRd = false;
                inst.writesMem = true;
                inst.readsMem = false;
            }
            else {
                inst.isLegal = false;
            }
            break;
        // Add Upper Immediate Instructions auipc
        case OP_ADDIMM:
            inst.doesArithLogic = true;
            inst.readsRs1 = false;
            inst.readsRs2 = false;
            inst.writesRd = false;
            inst.writesMem = false;
            inst.readsMem = false;
            break;
        // Load Upper Immediate Instruction lui
        case OP_LDUIMM:
            inst.doesArithLogic = false;
            inst.readsRs1 = false;
            inst.readsRs2 = false;
            inst.writesRd = true;
            inst.writesMem = false;
            inst.readsMem = true;
            break;
        // Jump and Link Instruction jal
        case OP_JMPLNK:
            inst.doesArithLogic = false;
            inst.readsRs1 = false;
            inst.readsRs2 = false;
            inst.writesRd = true;
            inst.writesMem = false;
            inst.readsMem = true;
            break;
        default:
            inst.isLegal = false;
    }
    return inst;
}

// Collect reg operands for arith or addr gen
Instruction simOperandCollection(Instruction inst, REGS regData) {
    
    inst.op1Val = regData.registers[inst.rs1];

    return inst;
}

// Resolve next PC whether +4 or branch/jump target
Instruction simNextPCResolution(Instruction inst) {

    inst.nextPC = inst.PC + 4;

    return inst;
}

// Perform arithmetic/logic operations
Instruction simArithLogic(Instruction inst) {
    uint64_t imm12  = inst.instruction >> 20 & 0b111111111111;
    uint64_t sext_imm12 = (imm12 & 0x800) ? (imm12 | 0xFFFFFFFFFFFFF000) : imm12;

    inst.arithResult = inst.op1Val + sext_imm12;
      
    return inst;
}

// Generate memory address for load/store instructions
Instruction simAddrGen(Instruction inst) {
    return inst;
}

// Perform memory access for load/store instructions
Instruction simMemAccess(Instruction inst, MemoryStore *myMem) {
    return inst;
}

// Write back results to registers
Instruction simCommit(Instruction inst, REGS &regData) {

    // regData here is passed by reference, so changes will be reflected in original
    regData.registers[inst.rd] = inst.arithResult;

    return inst;
}

// Simulate the whole instruction using functions above
Instruction simInstruction(uint64_t &PC, MemoryStore *myMem, REGS &regData) {
    Instruction inst = simFetch(PC, myMem);
    inst = simDecode(inst);
    if (!inst.isLegal || inst.isHalt) return inst;
    inst = simOperandCollection(inst, regData);
    inst = simNextPCResolution(inst);
    inst = simArithLogic(inst);
    inst = simAddrGen(inst);
    inst = simMemAccess(inst, myMem);
    inst = simCommit(inst, regData);
    PC = inst.nextPC;
    return inst;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <instruction_file>\n", argv[0]);
        return -1;
    }

    // initialize memory store with buffer contents
    MemoryStore *myMem = createMemoryStore();
    if (!initMemory(argv[1], myMem)) {
        fprintf(stderr, "Failed to initialize memory with program binary.\n");
        return -1;
    }

    // initialize registers and program counter
    regData.reg = {};
    PC = 0;
    bool err = false;
    
    // start simulation
    while (!err) {
        Instruction inst = simInstruction(PC, myMem, regData);
        if (inst.isHalt) {
            // Normal dump and exit
            dump(myMem);
            return 0;
        }
        if (!inst.isLegal) {
            fprintf(stderr, "Illegal instruction encountered at PC: 0x%lx\n", inst.PC);
            err = true;
        }
    }

    // dump and exit with error
    dump(myMem);
    exit(127);
    return -1;
}



