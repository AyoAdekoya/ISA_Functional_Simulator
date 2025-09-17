#include "sim.h"

using namespace std;

constexpr int NUM_OPCODE = 128; // 7 bit opcode
constexpr int NUM_FUNCT3 = 8; //  3 bit funct 3 fields
constexpr int NUM_FUNCT7 = 128; // 7 bit funct 7 fields

InscDecode decodeNon7[NUM_OPCODE][NUM_FUNCT3]; // a table to store the opcode and fuct3 functions
InscDecode decode7[NUM_OPCODE][NUM_FUNCT3][NUM_FUNCT7]; // a table to store the R format commands

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

static void InstDecode() {
    decodeNon7[OP_INTIMM][FUNCT3_ADD] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAddi
    };

    decodeNon7[OP_INTIMM][FUNCT3_SLL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSlli
    };

    decodeNon7[OP_INTIMM][FUNCT3_SET] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSlti
    };

    decodeNon7[OP_INTIMM][FUNCT3_STU] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSltiu
    };

    decodeNon7[OP_INTIMM][FUNCT3_XOR] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeXori
    };

    decode7[OP_INTIMM][FUNCT3_SHIFT][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSrli
    };

    decode7[OP_INTIMM][FUNCT3_SHIFT][FUNCT7_SA] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSrai
    };

    decodeNon7[OP_INTIMM][FUNCT3_OR] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeOri
    };

    decodeNon7[OP_INTIMM][FUNCT3_ADD] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAddi
    };

    decodeNon7[OP_INTIMM][FUNCT3_ADD] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAddi
    };

    decodeNon7[OP_INTIMM][FUNCT3_AND] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAndi
    };

    decodeNon7[OP_OFFIMM][FUNCT3_BYT] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLb
    };

    decodeNon7[OP_OFFIMM][FUNCT3_HLW] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLh
    };

    decodeNon7[OP_OFFIMM][FUNCT3_WRD] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLw
    };

    decodeNon7[OP_OFFIMM][FUNCT3_DBL] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLd
    };

    decodeNon7[OP_OFFIMM][FUNCT3_BYU] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLbu
    };

    decodeNon7[OP_OFFIMM][FUNCT3_HWU] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLhu
    };

    decodeNon7[OP_OFFIMM][FUNCT3_WDU] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = true,
        .writesMem = false,
        .execution = executeLwu
    };

    decode7[OP_WORIMM][FUNCT3_SLL][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSlliw
    };

    decode7[OP_WORIMM][FUNCT3_SHIFT][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSrliw
    };

    decode7[OP_WORIMM][FUNCT3_SHIFT][FUNCT7_SA] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSraiw
    };

    decodeNon7[OP_LNKREG][FUNCT3_JAL] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeJalr
    };

    decode7[OP_REGFMT][FUNCT3_ADD][FUNCT7_ADD] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAdd
    };

    decode7[OP_REGFMT][FUNCT3_SUB][FUNCT7_SUB] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSub
    };

    decode7[OP_REGFMT][FUNCT3_SLL][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSll
    };

    decode7[OP_REGFMT][FUNCT3_SET][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSlt
    };

    decode7[OP_REGFMT][FUNCT3_STU][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSltu
    };

    decode7[OP_REGFMT][FUNCT3_XOR][FUNCT7_XOR] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeXor
    };

    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSrl
    };

    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SA] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSra
    };

    decode7[OP_REGFMT][FUNCT3_OR][FUNCT7_OR] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeOr
    };

    decode7[OP_REGFMT][FUNCT3_AND][FUNCT7_AND] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAnd
    };

    decode7[OP_REGWRD][FUNCT3_ADD][FUNCT7_ADD] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAddw
    };

    decode7[OP_REGFMT][FUNCT3_SUB][FUNCT7_SUB] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSubw
    };

    decode7[OP_REGFMT][FUNCT3_SLL][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSllw
    };

    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SL] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSrlw
    };

    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SA] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = true,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeSraw
    };

    decodeNon7[OP_STRFMT][FUNCT3_BYT] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = true,
        .execution = executeSb
    };

    decodeNon7[OP_STRFMT][FUNCT3_HLW] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = true,
        .execution = executeSh
    };

    decodeNon7[OP_STRFMT][FUNCT3_WRD] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = true,
        .execution = executeSw
    };

    decodeNon7[OP_STRFMT][FUNCT3_DBL] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = true,
        .execution = executeSd
    };

    decodeNon7[OP_STRBYT][FUNCT3_BEQ] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBeq
    };

    decodeNon7[OP_STRBYT][FUNCT3_BNE] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBne
    };

    decodeNon7[OP_STRBYT][FUNCT3_BLT] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBlt
    };

    decodeNon7[OP_STRBYT][FUNCT3_BGE] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBge
    };

    decodeNon7[OP_STRBYT][FUNCT3_BLU] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBlt
    };

    decodeNon7[OP_STRBYT][FUNCT3_BGU] = {
        .isLegal = true,
        .doesArithLogic = true,
        .writesRd = false,
        .readsRs1 = true,
        .readsRs2 = true,
        .readsMem = false,
        .writesMem = false,
        .execution = executeBgeu
    };

    decodeNon7[OP_ADDIMM][0] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = false,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeAuipc
    };

    decodeNon7[OP_LDUIMM][0] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = false,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeLui
    };

    decodeNon7[OP_JMPLNK][0] = {
        .isLegal = true,
        .doesArithLogic = false,
        .writesRd = true,
        .readsRs1 = false,
        .readsRs2 = false,
        .readsMem = false,
        .writesMem = false,
        .execution = executeJal
    };
}

// Determine instruction opcode, funct, reg names, and what resources to use
Instruction simDecode(Instruction inst) {
    // Fill in the decode table ahead of time
    InstDecode();

    inst.opcode = inst.instruction & 0b1111111;

    if (inst.opcode != OP_STRFMT && inst.opcode != OP_STRBYT) {
        inst.rd = inst.instruction >> 7 & 0b11111;
    }

    if (inst.opcode != OP_ADDIMM &&
        inst.opcode != OP_LDUIMM &&
        inst.opcode != OP_JMPLNK)
    {
        inst.funct3 = inst.instruction >> 12 & 0b111;
        inst.rs1 = inst.instruction >> 15 & 0b11111;
    }

    if (inst.opcode == OP_REGFMT ||
        inst.opcode == OP_REGWRD ||
        inst.opcode == OP_STRFMT || 
        inst.opcode == OP_STRBYT) 
    {
        inst.rs2 = inst.instruction >> 20 & 0b11111;
    }

    if (inst.opcode == OP_REGFMT || inst.opcode == OP_REGWRD) 
    {
        inst.funct7 = inst.instruction >> 25 & 0b1111111;
    }
    

    if (inst.instruction == 0xfeedfeed) {
        inst.isHalt = true;
        return inst; // halt instruction
    }
    if (inst.instruction == 0x00000013) {
        inst.isNop = true;
        return inst; // NOP instruction
    }
    //inst.isLegal = true; // assume legal unless proven otherwise

    if (inst.opcode == OP_REGFMT || inst.opcode == OP_REGWRD) 
    {
        InscDecode decode = decode7[inst.opcode][inst.funct3][inst.funct7];
        inst.isLegal = decode.isLegal;
        inst.doesArithLogic = decode.doesArithLogic;
        inst.writesRd = decode.writesRd;
        inst.readsRs1 = decode.readsRs1;
        inst.readsRs2 = decode.readsRs2;
        inst.readsMem = decode.readsMem;
        inst.writesMem = decode.writesMem;
    }
    else{
        InscDecode decode = decodeNon7[inst.opcode][inst.funct3];
        inst.isLegal = decode.isLegal;
        inst.doesArithLogic = decode.doesArithLogic;
        inst.writesRd = decode.writesRd;
        inst.readsRs1 = decode.readsRs1;
        inst.readsRs2 = decode.readsRs2;
        inst.readsMem = decode.readsMem;
        inst.writesMem = decode.writesMem;
    }
    return inst;
}

// Collect reg operands for arith or addr gen
Instruction simOperandCollection(Instruction inst, REGS regData) {

    if (inst.opcode != OP_ADDIMM &&
        inst.opcode != OP_LDUIMM &&
        inst.opcode != OP_JMPLNK) {
        inst.op1Val = regData.registers[inst.rs1];
    }

    if (inst.opcode == OP_REGFMT ||
        inst.opcode == OP_REGWRD ||
        inst.opcode == OP_STRFMT || 
        inst.opcode == OP_STRBYT) 
    {
        inst.op2Val = regData.registers[inst.rs2];
    }
    return inst;
}

// Resolve next PC whether +4 or branch/jump target
Instruction simNextPCResolution(Instruction inst) {

    inst.nextPC = inst.PC + 4;

    return inst;
}

static void executeAdd(Instruction& inst){};
static void executeAddw(Instruction& inst){};
static void executeAddi(Instruction& inst){};
static void executeAddiw(Instruction& inst){};
static void executeAnd(Instruction& inst){};
static void executeAndi(Instruction& inst){};
static void executeAuipc(Instruction& inst){};
static void executeBeq(Instruction& inst){};
static void executeBge(Instruction& inst){};
static void executeBgeu(Instruction& inst){};
static void executeBlt(Instruction& inst){};
static void executeBltu(Instruction& inst){};
static void executeBne(Instruction& inst){};
static void executeJal(Instruction& inst){};
static void executeJalr(Instruction& inst){};
static void executeLb(Instruction& inst){};
static void executeLbu(Instruction& inst){};
static void executeLd(Instruction& inst){};
static void executeLh(Instruction& inst){};
static void executeLhu(Instruction& inst){};
static void executeLui(Instruction& inst){};
static void executeLw(Instruction& inst){};
static void executeLwu(Instruction& inst){};
static void executeOr(Instruction& inst){};
static void executeOri(Instruction& inst){};
static void executeSb(Instruction& inst){};
static void executeSd(Instruction& inst){};
static void executeSh(Instruction& inst){};
static void executeSll(Instruction& inst){};
static void executeSllw(Instruction& inst){};
static void executeSlli(Instruction& inst){};
static void executeSlliw(Instruction& inst){};
static void executeSlt(Instruction& inst){};
static void executeSlti(Instruction& inst){};
static void executeSltiu(Instruction& inst){};
static void executeSltu(Instruction& inst){};
static void executeSra(Instruction& inst){};
static void executeSraw(Instruction& inst){};
static void executeSrai(Instruction& inst){};
static void executeSraiw(Instruction& inst){};
static void executeSrl(Instruction& inst){};
static void executeSrlw(Instruction& inst){};
static void executeSrli(Instruction& inst){};
static void executeSrliw(Instruction& inst){};
static void executeSub(Instruction& inst){};
static void executeSubw(Instruction& inst){};
static void executeSw(Instruction& inst){};
static void executeXor(Instruction& inst){};
static void executeXori(Instruction& inst){};

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
    if (inst.opcode != OP_STRFMT && inst.opcode != OP_STRBYT) {
        regData.registers[inst.rd] = inst.arithResult;
    }
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



