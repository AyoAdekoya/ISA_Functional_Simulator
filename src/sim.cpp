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
static void executeAdd(Instruction&);
static void executeAddw(Instruction&);
static void executeAddi(Instruction&);
static void executeAddiw(Instruction&);
static void executeAnd(Instruction&);
static void executeAndi(Instruction&);
static void executeAuipc(Instruction&);
static void executeBeq(Instruction&);
static void executeBge(Instruction&);
static void executeBgeu(Instruction&);
static void executeBlt(Instruction&);
static void executeBltu(Instruction&);
static void executeBne(Instruction&);
static void executeJal(Instruction&);
static void executeJalr(Instruction&);
static void executeLb(Instruction&);
static void executeLbu(Instruction&);
static void executeLd(Instruction&);
static void executeLh(Instruction&);
static void executeLhu(Instruction&);
static void executeLui(Instruction&);
static void executeLw(Instruction&);
static void executeLwu(Instruction&);
static void executeOr(Instruction&);
static void executeOri(Instruction&);
static void executeSb(Instruction&);
static void executeSd(Instruction&);
static void executeSh(Instruction&);
static void executeSll(Instruction&);
static void executeSllw(Instruction&);
static void executeSlli(Instruction&);
static void executeSlliw(Instruction&);
static void executeSlt(Instruction&);
static void executeSlti(Instruction&);
static void executeSltiu(Instruction&);
static void executeSltu(Instruction&);
static void executeSra(Instruction&);
static void executeSraw(Instruction&);
static void executeSrai(Instruction&);
static void executeSraiw(Instruction&);
static void executeSrl(Instruction&);
static void executeSrlw(Instruction&);
static void executeSrli(Instruction&);
static void executeSrliw(Instruction&);
static void executeSub(Instruction&);
static void executeSubw(Instruction&);
static void executeSw(Instruction&);
static void executeXor(Instruction&);
static void executeXori(Instruction&);

// Create a table for each instruction that pre-fills all the flags
// and points the instruction to the function that executes it.
static void InstDecode() {
    InscDecode d;

    // Decoding I type Instructions
    // OP_INTIMM group
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeAddi;
    decodeNon7[OP_INTIMM][FUNCT3_ADD] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSlli;
    decode7[OP_INTIMM][FUNCT3_SLL][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSlti;
    decodeNon7[OP_INTIMM][FUNCT3_SET] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSltiu;
    decodeNon7[OP_INTIMM][FUNCT3_STU] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeXori;
    decodeNon7[OP_INTIMM][FUNCT3_XOR] = d;

    // OP_INTIMM with implicit funct7 fields
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSrli;
    decode7[OP_INTIMM][FUNCT3_SHIFT][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSrai;
    // Special case for Srai
    decode7[OP_INTIMM][FUNCT3_SHIFT][0b100000] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeOri;
    decodeNon7[OP_INTIMM][FUNCT3_OR] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeAndi;
    decodeNon7[OP_INTIMM][FUNCT3_AND] = d;

    // OP_OFFIMM (loads)
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLb;
    decodeNon7[OP_OFFIMM][FUNCT3_BYT] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLh;
    decodeNon7[OP_OFFIMM][FUNCT3_HLW] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLw;
    decodeNon7[OP_OFFIMM][FUNCT3_WRD] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLd;
    decodeNon7[OP_OFFIMM][FUNCT3_DBL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLbu;
    decodeNon7[OP_OFFIMM][FUNCT3_BYU] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLhu;
    decodeNon7[OP_OFFIMM][FUNCT3_HWU] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = true; d.writesMem = false;
    d.execution = executeLwu;
    decodeNon7[OP_OFFIMM][FUNCT3_WDU] = d;

    // OP_WORIMM (functions using immediates but operating on words)
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeAddiw;
    decodeNon7[OP_WORIMM][FUNCT3_ADD]= d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSlliw;
    decode7[OP_WORIMM][FUNCT3_SLL][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSrliw;
    decode7[OP_WORIMM][FUNCT3_SHIFT][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeSraiw;
    decode7[OP_WORIMM][FUNCT3_SHIFT][FUNCT7_SA] = d;

    // OP_LNKREG (JALR)
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeJalr;
    decodeNon7[OP_LNKREG][FUNCT3_JAL] = d;

    // R-type instructions
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeAdd;
    decode7[OP_REGFMT][FUNCT3_ADD][FUNCT7_ADD] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSub;
    decode7[OP_REGFMT][FUNCT3_SUB][FUNCT7_SUB] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSll;
    decode7[OP_REGFMT][FUNCT3_SLL][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSlt;
    decode7[OP_REGFMT][FUNCT3_SET][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSltu;
    decode7[OP_REGFMT][FUNCT3_STU][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeXor;
    decode7[OP_REGFMT][FUNCT3_XOR][FUNCT7_XOR] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSrl;
    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SL] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSra;
    decode7[OP_REGFMT][FUNCT3_SHIFT][FUNCT7_SA] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeOr;
    decode7[OP_REGFMT][FUNCT3_OR][FUNCT7_OR] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeAnd;
    decode7[OP_REGFMT][FUNCT3_AND][FUNCT7_AND] = d;

    // OP_REGWRD 32-bit (word) R-type
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeAddw;
    decode7[OP_REGWRD][FUNCT3_ADD][FUNCT7_ADD] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSubw;
    decode7[OP_REGWRD][FUNCT3_SUB][FUNCT7_SUB] = d; // corrected: word sub under OP_REGWRD

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSllw;
    decode7[OP_REGWRD][FUNCT3_SLL][FUNCT7_SL] = d; // corrected: sllw under OP_REGWRD

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSrlw;
    decode7[OP_REGWRD][FUNCT3_SHIFT][FUNCT7_SL] = d; // corrected: srlw

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = true;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeSraw;
    decode7[OP_REGWRD][FUNCT3_SHIFT][FUNCT7_SA] = d; // corrected: sraw

    // OP_STRFMT: Stores
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = true;
    d.execution = executeSb;
    decodeNon7[OP_STRFMT][FUNCT3_BYT] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = true;
    d.execution = executeSh;
    decodeNon7[OP_STRFMT][FUNCT3_HLW] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = true;
    d.execution = executeSw;
    decodeNon7[OP_STRFMT][FUNCT3_WRD] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = true;
    d.execution = executeSd;
    decodeNon7[OP_STRFMT][FUNCT3_DBL] = d;

    // OP_STRBYT (SB Formats) Branches 
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBeq;
    decodeNon7[OP_STRBYT][FUNCT3_BEQ] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBne;
    decodeNon7[OP_STRBYT][FUNCT3_BNE] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBlt;
    decodeNon7[OP_STRBYT][FUNCT3_BLT] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBge;
    decodeNon7[OP_STRBYT][FUNCT3_BGE] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBltu;
    decodeNon7[OP_STRBYT][FUNCT3_BLU] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = true; d.writesRd = false;
    d.readsRs1 = true; d.readsRs2 = true; d.readsMem = false; d.writesMem = false;
    d.execution = executeBgeu;
    decodeNon7[OP_STRBYT][FUNCT3_BGU] = d;

    // AUIPC, LUI, JAL
    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = false; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeAuipc;
    decodeNon7[OP_ADDIMM][0] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = false; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeLui;
    decodeNon7[OP_LDUIMM][0] = d;

    d = InscDecode{};
    d.isLegal = true; d.doesArithLogic = false; d.writesRd = true;
    d.readsRs1 = false; d.readsRs2 = false; d.readsMem = false; d.writesMem = false;
    d.execution = executeJal;
    decodeNon7[OP_JMPLNK][0] = d;
}

// Determine instruction opcode, funct, reg names, and what resources to use
Instruction simDecode(Instruction inst) {
    //Boolean to keep track of the special case Immediates with implicit funct7s
    bool special_immediate = false;

    // Separate the opcode and store it in the instruction struct
    inst.opcode = inst.instruction & 0b1111111;

    // If the instruction isn't a S or SB type then get the rd address
    if (inst.opcode != OP_STRFMT && inst.opcode != OP_STRBYT) {
        inst.rd = inst.instruction >> 7 & 0b11111;
    }

    // If the instruction isn't a UJ or U type then extract the rs1 and funct3
    if (inst.opcode != OP_ADDIMM &&
        inst.opcode != OP_LDUIMM &&
        inst.opcode != OP_JMPLNK)
    {
        inst.funct3 = inst.instruction >> 12 & 0b111;
        inst.rs1 = inst.instruction >> 15 & 0b11111;
    }

    // If the instruction is a R, S or SB then extract the rs2 address
    if (inst.opcode == OP_REGFMT ||
        inst.opcode == OP_REGWRD ||
        inst.opcode == OP_STRFMT || 
        inst.opcode == OP_STRBYT) 
    {
        inst.rs2 = inst.instruction >> 20 & 0b11111;
    }

    // If the instruction is an R type get it's funct 7 field
    if (inst.opcode == OP_REGFMT || inst.opcode == OP_REGWRD) 
    {
        inst.funct7 = inst.instruction >> 25 & 0b1111111;
    }

    // If the instruction is a I-type shift with implicit funct 7 bits
    // extract the bits into the funct7 and set the special_immediate flag
    // for reuse later

    // Case for when its a 6 bit imm
    if ((inst.opcode == OP_INTIMM && inst.funct3 == FUNCT3_SLL) ||
        (inst.opcode == OP_INTIMM && inst.funct3 == FUNCT3_SHIFT)){
            special_immediate = true;
            inst.funct7 = inst.instruction >> 25 & 0b1111111;
            inst.funct7 &= 0xFE;
    }
    
    // Case for when its a 5 bit imm
    if ((inst.opcode == OP_WORIMM && inst.funct3 == FUNCT3_SLL)||
        (inst.opcode == OP_WORIMM && inst.funct3 == FUNCT3_SHIFT)){
            special_immediate = true;
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

    // If the instruction has a funct 7 field (R type and I type shifts)
    // then use the decode table for funct 7s
    if (inst.opcode == OP_REGFMT || inst.opcode == OP_REGWRD || special_immediate) 
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
    // If the code doesn't have a funct7 field then use the regular decode table
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
    if (inst.opcode != OP_STRBYT &&
        inst.opcode != OP_JMPLNK &&
        inst.opcode != OP_LNKREG)
    {
        inst.nextPC = inst.PC + 4;
    }

    return inst;
}

// IMMGEN
// Functions that extract and sign extend the immediates for different instructions
// Generate and Sign extend the Immediates for the SB type instructions
static uint64_t extractSBImmediates(Instruction& inst) {
    uint64_t imm12 = 0;
    imm12 |= (inst.instruction >> 31 & 0b1) << 12;
    imm12 |= (inst.instruction >> 25 & 0b111111) << 5;
    imm12 |= (inst.instruction >> 7 & 0b1) << 11;
    imm12 |= (inst.instruction >> 8 & 0b1111) << 1;

    uint64_t sext_imm12 = (imm12 & 0x1000) ? imm12 | 0xFFFFFFFFFFFFE000 : imm12;
    
    return sext_imm12;
}

// Generate and Sign extend the Immediates for the I type instructions
// without implicit funct7 fields
static uint64_t extractIImmediates(Instruction& inst, int length = 64) {
    uint64_t imm12  = inst.instruction >> 20 & 0b111111111111;
    if (length == 64) {
        imm12 = (imm12 & 0x800) ? (imm12 | 0xFFFFFFFFFFFFF000) : imm12;
    }
    else if(length == 32) {
        imm12 = (imm12 & 0x800) ? (imm12 | 0xFFFFF000) : imm12;
    }
    return imm12;
}

// Generate and Sign extedn the Immediates for the S-type Instructions
static uint64_t extractSImmediates(Instruction& inst) {
    uint64_t imm12 = 0;
    imm12 |= (inst.instruction >> 7 & 0b11111);
    imm12 |= (inst.instruction >> 25 & 0b1111111) << 5;

    uint64_t sext_imm12 = (imm12 & 0x800) ? (imm12 | 0xFFFFFFFFFFFFF000) : imm12;

    return sext_imm12;
}

// ALU + MB
// Arithmetic and Logic Block + Memory Block
// Functions that handle each instruction.
static void executeAdd(Instruction& inst){
    inst.arithResult = inst.op1Val + inst.op2Val;
};
static void executeAddw(Instruction& inst){
    uint32_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint32_t register2 = inst.op2Val & 0xFFFFFFFF;
    uint32_t result = register1 + register2;
    uint64_t sext_result = (result & 0x8000) ? (result | 0xFFFFFFFF00000000) : result;

    inst.arithResult = sext_result;
};
static void executeAddi(Instruction& inst){
    uint64_t sext_imm12 = extractIImmediates(inst);

    inst.arithResult = inst.op1Val + sext_imm12;
};
static void executeAddiw(Instruction& inst){
    uint32_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint32_t sext32_imm12 = extractIImmediates(inst,32);
    uint32_t result = register1 + sext32_imm12;
    uint64_t sext_result = (result & 0x8000) ? (result | 0xFFFFFFFF00000000) : result;

    inst.arithResult = sext_result;
};
static void executeAnd(Instruction& inst){
    inst.arithResult = inst.op1Val & inst.op2Val;
};
static void executeAndi(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.arithResult = inst.op1Val & sext_imm12;
};
static void executeAuipc(Instruction& inst){
    uint64_t imm20 = (inst.instruction >> 12) & 0b11111111111111111111;
    uint64_t upper_imm20 = imm20 << 12;
    uint64_t sext_upper_imm20 = (imm20 & 0x80000) ? (upper_imm20 | 0xFFFFFFFFFFF00000) : upper_imm20;
    inst.arithResult = inst.PC + sext_upper_imm20;
};
static void executeBeq(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    if (inst.op1Val == inst.op2Val) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeBge(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    int64_t rs1 = (int64_t) inst.op1Val;
    int64_t rs2 = (int64_t) inst.op2Val;
    if (rs1 >= rs2) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeBgeu(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    if (inst.op1Val >= inst.op2Val) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeBlt(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    int64_t rs1 = (int64_t) inst.op1Val;
    int64_t rs2 = (int64_t) inst.op2Val;
    if (rs1< rs2) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeBltu(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    if (inst.op1Val < inst.op2Val) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeBne(Instruction& inst){
    uint64_t immediate = extractSBImmediates(inst);
    if (inst.op1Val != inst.op2Val) {
        inst.nextPC = inst.PC + immediate;
    }
    else {
        inst.nextPC = inst.PC + 4;
    }
};
static void executeJal(Instruction& inst){
    uint64_t imm20 = 0;
    imm20 |= (inst.instruction >> 12 & 0b11111111) << 12;
    imm20 |= (inst.instruction >> 20 & 0b1) << 11;
    imm20 |= (inst.instruction >> 21 & 0b1111111111) << 1;
    imm20 |= (inst.instruction >> 31 & 0b1) << 20;

    uint64_t sext_imm20 = (imm20 & 0x100000) ? imm20 | 0xFFFFFFFFFFE00000 : imm20;

    inst.arithResult = inst.PC + 4;
    inst.nextPC = inst.PC + sext_imm20;
    
};
static void executeJalr(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.arithResult = inst.PC + 4;
    inst.nextPC = (inst.op1Val + sext_imm12) & ~1ULL; //The LSB of the branch address is set to 0 
};
static void executeLb(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLbu(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLd(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLh(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLhu(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLui(Instruction& inst){
    uint64_t imm20 = (inst.instruction >> 12) & 0b11111111111111111111;
    uint64_t upper_imm20 = imm20 << 12;
    uint64_t sext_upper_imm20 = (upper_imm20 & 0x80000000) ? (upper_imm20 | 0xFFFFFFFF00000000) : upper_imm20;
    inst.arithResult = sext_upper_imm20;
};
static void executeLw(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);
    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeLwu(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);
    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeOr(Instruction& inst){
    inst.arithResult = inst.op1Val | inst.op2Val;
};
static void executeOri(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);

    inst.arithResult = inst.op1Val | sext_imm12;
};
static void executeSb(Instruction& inst){
    uint64_t sext_imm12 = extractSImmediates(inst);
    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeSd(Instruction& inst){
    uint64_t sext_imm12 = extractSImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeSh(Instruction& inst){
    uint64_t sext_imm12 = extractSImmediates(inst);

    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeSll(Instruction& inst){
    uint64_t shift_amount = inst.op2Val & 0b111111;
    uint64_t shift_result = inst.op1Val << shift_amount;
    inst.arithResult = shift_result;
};
static void executeSllw(Instruction& inst){
    uint32_t shift_amount = inst.op2Val & 0b11111;
    uint32_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint32_t shift_result = register1 << shift_amount;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? 
    (shift_result | 0xFFFFFFFF00000000) : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSlli(Instruction& inst){
    uint64_t imm6  = inst.instruction >> 20 & 0b111111;
    inst.arithResult = inst.op1Val << imm6;
};
static void executeSlliw(Instruction& inst){
    uint32_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint32_t imm6  = inst.instruction >> 20 & 0b111111;
    uint32_t shift_result = register1 << imm6;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? shift_result | 0xFFFFFFFF00000000 : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSlt(Instruction& inst){
    uint64_t register1 = inst.op1Val ^ 0x8000000000000000;
    uint64_t register2 = inst.op2Val ^ 0x8000000000000000;
    inst.arithResult = (register1 < register2) ? 1:0;
};
static void executeSlti(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);
    uint64_t register1 = inst.op1Val ^ 0x8000000000000000;
    uint64_t tran_sext_imm12 = sext_imm12 ^ 0x8000000000000000;
    inst.arithResult = (register1 < tran_sext_imm12) ? 1:0;
};
static void executeSltiu(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);
    inst.arithResult = (inst.op1Val < sext_imm12) ? 1:0;
};
static void executeSltu(Instruction& inst){
    inst.arithResult = (inst.op1Val < inst.op2Val) ? 1 : 0;
};
static void executeSra(Instruction& inst){
    uint64_t shift_amount = inst.op2Val & 0b111111;
    int64_t rs1 = (int64_t) inst.op1Val;
    int64_t result = rs1 >> shift_amount;
    inst.arithResult  = (uint64_t) result;
};
static void executeSraw(Instruction& inst){
    uint32_t shift_amount = inst.op2Val & 0b11111;
    int32_t register1 = (int32_t) (inst.op1Val & 0xFFFFFFFF);
    int32_t shift_result = register1 >> shift_amount;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? 
    (shift_result | 0xFFFFFFFF00000000) : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSrai(Instruction& inst){
    uint64_t imm6  = inst.instruction >> 20 & 0b111111;
    int64_t rs1 = (int64_t) inst.op1Val;
    int64_t result = rs1 >> (int64_t) imm6;
    inst.arithResult  = (uint64_t) result;
};
static void executeSraiw(Instruction& inst){
    uint64_t imm6  = inst.instruction >> 20 & 0b111111;
    int32_t register1 = (int32_t) (inst.op1Val & 0xFFFFFFFF);
    int32_t shift_result = register1 >> imm6;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? 
    (shift_result | 0xFFFFFFFF00000000) : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSrl(Instruction& inst){
    uint64_t shift_amount = inst.op2Val & 0b111111;
    inst.arithResult = inst.op1Val >> shift_amount;
};
static void executeSrlw(Instruction& inst){
    uint64_t shift_amount = inst.op2Val & 0b11111;
    uint64_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint64_t shift_result = register1 >> shift_amount;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? 
    (shift_result | 0xFFFFFFFF00000000) : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSrli(Instruction& inst){
    uint64_t imm6  = inst.instruction >> 20 & 0b111111;
    inst.arithResult = inst.op1Val >> imm6;
};
static void executeSrliw(Instruction& inst){
    uint64_t imm6  = inst.instruction >> 20 & 0b111111;
    uint64_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint64_t shift_result = register1 >> imm6;
    uint64_t sext_shift_result = (shift_result & 0x80000000) ? 
    (shift_result | 0xFFFFFFFF00000000) : shift_result;
    inst.arithResult = sext_shift_result;
};
static void executeSub(Instruction& inst){
    inst.arithResult = inst.op1Val - inst.op2Val;
};
static void executeSubw(Instruction& inst){
    uint64_t register1 = inst.op1Val & 0xFFFFFFFF;
    uint64_t register2 = inst.op2Val & 0xFFFFFFFF;
    uint64_t result = register1 - register2;
    uint64_t sext_result = (result & 0x80000000) ? (result | 0xFFFFFFFF00000000) : result;

    inst.arithResult = sext_result;
};
static void executeSw(Instruction& inst){
    uint64_t sext_imm12 = extractSImmediates(inst);
    
    inst.memAddress = inst.op1Val + sext_imm12;
};
static void executeXor(Instruction& inst){
    inst.arithResult = inst.op1Val ^ inst.op2Val;
};
static void executeXori(Instruction& inst){
    uint64_t sext_imm12  = extractIImmediates(inst);
    inst.arithResult = inst.op1Val ^ sext_imm12;
};

// Perform arithmetic/logic operations
Instruction simArithLogic(Instruction inst) {
    // mark functions that are not R type but have a funct7
    bool special_immediate = false;
    if ((inst.opcode == OP_INTIMM && inst.funct3 == FUNCT3_SLL) ||
        (inst.opcode == OP_INTIMM && inst.funct3 == FUNCT3_SHIFT) ||
        (inst.opcode == OP_WORIMM && inst.funct3 == FUNCT3_SLL) ||
        (inst.opcode == OP_WORIMM && inst.funct3 == FUNCT3_SHIFT)) {
        special_immediate = true;
    }

    // Choose decode entry and call the execution function pointer if legal
    if (inst.opcode == OP_REGFMT || inst.opcode == OP_REGWRD || special_immediate) {
        InscDecode decode = decode7[inst.opcode][inst.funct3][inst.funct7];
        inst.isLegal = decode.isLegal;
        if (decode.execution && decode.isLegal) {
            decode.execution(inst);
        }
    } else {
        InscDecode decode = decodeNon7[inst.opcode][inst.funct3];
        inst.isLegal = decode.isLegal;
        if (decode.execution && decode.isLegal) {
            decode.execution(inst);
        }
    }
      
    return inst;
}

// Generate memory address for load/store instructions
Instruction simAddrGen(Instruction inst) {
    return inst;
}

// Perform memory access for load/store instructions
Instruction simMemAccess(Instruction inst, MemoryStore *myMem) {
    switch (inst.opcode) {
        case OP_OFFIMM: {
            uint64_t result = 0;
            if (inst.funct3 == FUNCT3_BYT) {
                myMem->getMemValue(inst.memAddress, result, BYTE_SIZE);
                result = result & 0b11111111;
                uint64_t sext_result = (result & 0x80) ? result | 0xFFFFFFFFFFFFFF00 : result;
                inst.arithResult = sext_result;
            }
            else if(inst.funct3 == FUNCT3_HLW) {
                myMem->getMemValue(inst.memAddress, result, HALF_SIZE);
                result = result & 0xFFFF;
                uint64_t sext_result = (result & 0x8000) ? result | 0xFFFFFFFFFFFF0000 : result;
                inst.arithResult = sext_result;
            }
            else if (inst.funct3 == FUNCT3_WRD) {
                myMem->getMemValue(inst.memAddress, result, WORD_SIZE);
                result = result & 0xFFFFFFFF;
                uint64_t sext_result = (result & 0x80000000) ? result | 0xFFFFFFFF00000000 : result;
                inst.arithResult = sext_result;
            }
            else if (inst.funct3 == FUNCT3_DBL) {
                myMem->getMemValue(inst.memAddress, result, DOUBLE_SIZE);
                inst.arithResult = result;
            }
            else if (inst.funct3 == FUNCT3_BYU) {
                myMem-> getMemValue(inst.memAddress, result, BYTE_SIZE);
                result = result & 0b11111111;
                inst.arithResult = result;
            }
            else if(inst.funct3 == FUNCT3_HWU) {
                myMem-> getMemValue(inst.memAddress, result, HALF_SIZE);
                result = result & 0xFFFF;
                inst.arithResult = result;
            }
            else if(inst.funct3 ==  FUNCT3_WDU) {
                myMem-> getMemValue(inst.memAddress, result, WORD_SIZE);
                result = result & 0xFFFFFFFF;
                inst.arithResult = result;
            }
            break;
        }
        case OP_STRFMT: {
            uint64_t register2 = 0;
            if (inst.funct3 == FUNCT3_BYT) {
                register2 = inst.op2Val & 0b11111111;
                myMem->setMemValue(inst.memAddress, register2, BYTE_SIZE);
            }
            else if (inst.funct3 == FUNCT3_HLW) {
                register2 = inst.op2Val & 0xFFFF;
                myMem->setMemValue(inst.memAddress, register2, HALF_SIZE);
            }
            else if (inst.funct3 == FUNCT3_WRD) {
                register2 = inst.op2Val & 0xFFFFFFFF;
                myMem->setMemValue(inst.memAddress, register2, WORD_SIZE);
            }
            else if (inst.funct3 == FUNCT3_DBL) {
                register2 = inst.op2Val;
                myMem->setMemValue(inst.memAddress, register2, DOUBLE_SIZE);
            }
        }  
    }
    return inst;
}

// Write back results to registers
Instruction simCommit(Instruction inst, REGS &regData) {

    // regData here is passed by reference, so changes will be reflected in original
    if (inst.writesRd && inst.rd != 0) {
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
    // Fill in the decode table ahead of time
    InstDecode();

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
            fprintf(stderr, "Illegal at PC=0x%lx: opcode=0x%02x funct3=0x%x funct7=0x%02x\n",
            inst.PC, (unsigned)inst.opcode, (unsigned)inst.funct3, (unsigned)((inst.instruction >> 25) & 0x7F));
            fprintf(stderr, "instr32=0x%08x  bytes=[%02x %02x %02x %02x]\n",
            (unsigned)(inst.instruction & 0xFFFFFFFF),
            (unsigned)(inst.instruction & 0xFF),
            (unsigned)((inst.instruction >> 8) & 0xFF),
            (unsigned)((inst.instruction >> 16) & 0xFF),
            (unsigned)((inst.instruction >> 24) & 0xFF));
            err = true;
        }
    }

    // dump and exit with error
    dump(myMem);
    exit(127);
    return -1;
}



