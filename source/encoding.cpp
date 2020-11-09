#pragma once
#include <cstdint>
#include <cassert>
#include <cstdio>

#include "encoding.h"


// decode rd field
static inline uint32_t dec_rd(uint32_t inst) {
  return (inst & 0x00000F80) >> 7;
}

// decode rs1 field
static inline uint32_t dec_rs1(uint32_t inst) {
  return (inst & 0x000F8000) >> 15;
}

// decode rs2 field
static inline uint32_t dec_rs2(uint32_t inst) {
  return (inst & 0x01F00000) >> 20;
}

// decoded funct3 field
static inline uint32_t dec_funct3(uint32_t inst) {
  return (inst & 0x00007000) >> 12;
}

// decode funct7 field
static inline uint32_t dec_funct7(uint32_t inst) {
  return (inst & 0xFE000000) >> 25;
}

// decode utype instruction immediate
static inline uint32_t dec_imm_u(uint32_t inst) {
  return inst & 0xFFFFF000;
}

// decode jtype instruction immediate
static inline int32_t dec_imm_j(uint32_t inst) {
  uint32_t dst = 0;
  dst |= (inst & 0x80000000);
  dst |= (inst & 0x000FF000) << 11;
  dst |= (inst & 0x00100000) << 2;
  dst |= (inst & 0x7FE00000) >> 9;
  // note: shifted to 2nd least significant bit
  return ((int32_t)dst) >> 11;
}

// decode itype instruction immediate
static inline int32_t dec_imm_i(uint32_t inst) {
  return ((int32_t)(inst & 0xFFF00000)) >> 20;
}

// decode csr instruction immediate (same as itype, zero extend)
static inline uint32_t dec_csr(uint32_t inst) {
  return (inst & 0xFFF00000) >> 20;
}

// decode btype instruction immediate
static inline int32_t dec_imm_b(uint32_t inst) {
  uint32_t dst = 0;
  dst |= (inst & 0x80000000);
  dst |= (inst & 0x00000080) << 23;
  dst |= (inst & 0x7E000000) >> 1;
  dst |= (inst & 0x00000F00) << 12;
  // note: shifted to 2nd least significant bit
  return ((int32_t)dst) >> 19;
}

// decode stype instruction immediate
static inline int32_t dec_imm_s(uint32_t inst) {
  uint32_t dst = 0;
  dst |= (inst & 0xFE000000);
  dst |= (inst & 0x00000F80) << 13;
  return ((int32_t)dst) >> 20;
}

// encode rd field
static inline uint32_t enc_rd(uint32_t field) {
  return (field << 7) & 0x00000F80;
}

// encode rs1 field
static inline uint32_t enc_rs1(uint32_t field) {
  return (field << 15) & 0x000F8000;
}

// encode rs2 field
static inline uint32_t enc_rs2(uint32_t field) {
  return (field << 20) & 0x01F00000;
}

// encode funct3 field
static inline uint32_t enc_funct3(uint32_t field) {
  return (field << 12) & 0x00007000;
}

// encode funct7 field
static inline uint32_t enc_funct7(uint32_t field) {
  return (field << 25) & 0xFE000000;
}

// encode utype immediate
static inline uint32_t enc_imm_u(uint32_t field) {
  return (field & 0xFFFFF000);
}

// encode jtype immediate
static inline int32_t enc_imm_j(uint32_t field) {
  uint32_t out = 0;
  out |= (field << 11) & 0x80000000;
  out |= (field      ) & 0x000FF000;
  out |= (field <<  9) & 0x00100000;
  out |= (field << 20) & 0x7FE00000;
  return out;
}

// encode itype immediate
static inline int32_t enc_imm_i(uint32_t field) {
  return (field << 20) & 0xFFF00000;
}

// encode csr immediate (same as itype, zero extend)
static inline uint32_t enc_csr(uint32_t field) {
  return (field << 20) & 0xFFF00000;
}

// encode btype immediate
static inline int32_t enc_imm_b(uint32_t field) {
  uint32_t out = 0;
  out |= ((field << 19)      ) & 0x80000000;
  out |= ((field << 19) >> 23) & 0x00000080;
  out |= ((field << 19) <<  1) & 0x7E000000;
  out |= ((field << 19) >> 12) & 0x00000F00;
  return out;
}

// encode stype immediate
static inline int32_t enc_imm_s(uint32_t field) {
  uint32_t out = 0;
  out |= (field << 20) & 0xFE000000;
  out |= (field <<  7) & 0x00000F80;
  return out;
}

static bool dec_load(uint32_t raw, rv_inst_t &out) {
  const uint32_t func3 = dec_funct3(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.imm = dec_imm_i(raw);
  switch (func3) {
  case 0:  // LD
    out.type = rv_inst_lb;
    break;
  case 1:  // LH
    out.type = rv_inst_lh;
    break;
  case 2:  // LW
    out.type = rv_inst_lw;
    break;
  case 3:  // LD
    out.type = rv_inst_ld;
    break;
  case 4:  // LBU
    out.type = rv_inst_lbu;
    break;
  case 5:  // LHU
    out.type = rv_inst_lhu;
    break;
  case 6:  // LWU
    out.type = rv_inst_lwu;
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_misc_mem(uint32_t raw, rv_inst_t &out) {
  const uint32_t funct3 = dec_funct3(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.imm = dec_imm_i(raw);
  switch (funct3) {
  case 0:
    out.type = rv_inst_fence;
    break;
  case 1:
    out.type = rv_inst_fencei;
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_op_imm(uint32_t raw, rv_inst_t &out) {
  const uint32_t func3 = dec_funct3(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.imm = dec_imm_i(raw);
  switch (func3) {
  case 0:  // ADDI
    out.type = rv_inst_addi;
    break;
  case 2:  // SLTI
    out.type = rv_inst_slti;
    break;
  case 3:  // SLTIU
    out.type = rv_inst_sltiu;
    break;
  case 4:  // XORI
    out.type = rv_inst_xori;
    break;
  case 6:  // ORI
    out.type = rv_inst_ori;
    break;
  case 7:  // ANDI
    out.type = rv_inst_andi;
    break;
  case 1:
    // note: 63 because of RV64I width
    switch (out.imm & ~63u) {
    case 0x000:  // SLLI
      out.type = rv_inst_slli;
      break;
    default:
      return false;
    }
    out.imm &= 63;  // shamt
    break;
  case 5:
    // note: 63 because of RV64I width
    switch (out.imm & ~63u) {
    case 0x000:  // SRLI
      out.type = rv_inst_srli;
      break;
    case 0x400:  // SRAI
      out.type = rv_inst_srai;
      break;
    default:
      return false;
    }
    out.imm &= 63;  // shamt
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_auipc(uint32_t raw, rv_inst_t &out) {
  out.type = rv_inst_auipc;
  out.rd = dec_rd(raw);
  out.imm = dec_imm_u(raw);
  return true;
}

static bool dec_op_imm_32(uint32_t raw, rv_inst_t &out) {
  const uint32_t funct3 = dec_funct3(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  const uint32_t funct7 = dec_funct7(raw);
  switch (funct3) {
  case 0:
    out.type = rv_inst_addiw;
    out.imm = dec_imm_i(raw);
    break;
  case 1: // SLLIW
    if (funct7 != 0) {
      return false;
    }
    out.type = rv_inst_slliw;
    out.imm = dec_rs2(raw); // shamnt
    break;
  case 5:
    switch (funct7) {
    case 0x00: // SRLIW
      out.type = rv_inst_srliw;
      out.imm = dec_rs2(raw); // shamnt
      break;
    case 0x20: // SRAIW
      out.type = rv_inst_sraiw;
      out.imm = dec_rs2(raw); // shamnt
      break;
    default:
      return false;
    }
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_store(uint32_t raw, rv_inst_t &out) {
  const uint32_t func3 = dec_funct3(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.rs2 = dec_rs2(raw);
  out.imm = dec_imm_s(raw);
  switch (func3) {
  case 0:  // SB
    out.type = rv_inst_sb;
    break;
  case 1:  // SH
    out.type = rv_inst_sh;
    break;
  case 2:  // SW
    out.type = rv_inst_sw;
    break;
  case 3:  // SD
    out.type = rv_inst_sd;
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_op(uint32_t raw, rv_inst_t &out) {
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.rs2 = dec_rs2(raw);
  const uint32_t funct3 = dec_funct3(raw);
  const uint32_t funct7 = dec_funct7(raw);

  // validate funct7 field
  if (funct7 != 0) {
    // the only other option
    if (funct7 != 0b0100000) {
      return false;
    }
    // must be ADD/SUB or SRL/SRA
    if (funct3 != 0b000 || funct3 != 0b101) {
      return false;
    }
  }

  switch (funct3) {
  case 0:
    switch (funct7) {
    case 0x00:  // ADD
      out.type = rv_inst_add;
      break;
    case 0x20:  // SUB
      out.type = rv_inst_sub;
      break;
    default:
      return false;
    }
    break;
  case 1:  // SLL
    out.type = rv_inst_sll;
    break;
  case 2:  // SLT
    out.type = rv_inst_slt;
    break;
  case 3:  // SLTU
    out.type = rv_inst_sltu;
    break;
  case 4:  // XOR
    out.type = rv_inst_xor;
    break;
  case 5:
    switch (funct7) {
    case 0x00:  // SRL
      out.type = rv_inst_srl;
      break;
    case 0x20:  // SRA
      out.type = rv_inst_sra;
      break;
    default:
      return false;
    }
    break;
  case 6:  // OR
    out.type = rv_inst_or;
    break;
  case 7:  // AND
    out.type = rv_inst_and;
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_lui(uint32_t raw, rv_inst_t &out) {
  out.type = rv_inst_lui;
  out.rd = dec_rd(raw);
  out.imm = dec_imm_u(raw);
  return true;
}

static bool dec_op_32(uint32_t raw, rv_inst_t &out) {
  const uint32_t funct3 = dec_funct3(raw);
  const uint32_t funct7 = dec_funct7(raw);
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.rs2 = dec_rs2(raw);

  if (funct7 != 0) {
    // the only other option
    if (funct7 != 0b0100000) {
      return false;
    }
    // must be SRLW/SRAW or ADDW/SUBW
    if (funct3 != 0b101 || funct3 != 0b000) {
      return false;
    }
  }

  switch (funct3) {
  case 0:
    switch (funct7) {
    case 0x00:  // ADDW
      out.type = rv_inst_addw;
      break;
    case 0x20:  // SUBW
      out.type = rv_inst_subw;
      break;
    default:
      return false;
    }
    break;
  case 1:  // SLLW
    out.type = rv_inst_sllw;
    break;
  case 5:
    switch (funct7) {
    case 0x00:  // SRLW
      out.type = rv_inst_srlw;
      break;
    case 0x20:  // SRAW
      out.type = rv_inst_sraw;
      break;
    default:
      return false;
    }
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_branch(uint32_t raw, rv_inst_t &out) {
  const uint32_t funct3 = dec_funct3(raw);
  out.rs1 = dec_rs1(raw);
  out.rs2 = dec_rs2(raw);
  out.imm = dec_imm_b(raw);
  switch (funct3) {
  case 0:  // BEQ
    out.type = rv_inst_beq;
    break;
  case 1:  // BNE
    out.type = rv_inst_bne;
    break;
  case 4:  // BLT
    out.type = rv_inst_blt;
    break;
  case 5:  // BGE
    out.type = rv_inst_bge;
    break;
  case 6:  // BLTU
    out.type = rv_inst_bltu;
    break;
  case 7:  // BGEU
    out.type = rv_inst_bgeu;
    break;
  default:
    return false;
  }
  return true;
}

static bool dec_jalr(uint32_t raw, rv_inst_t &out) {
  if (dec_funct3(raw) != 0) {
    return false;
  }
  out.type = rv_inst_jalr;
  out.rd = dec_rd(raw);
  out.rs1 = dec_rs1(raw);
  out.imm = dec_imm_i(raw);
  return true;
}

static bool dec_jal(uint32_t raw, rv_inst_t &out) {
  out.type = rv_inst_jal;
  out.rd = dec_rd(raw);
  out.imm = dec_imm_j(raw);
  return true;
}

static bool dec_system(uint32_t raw, rv_inst_t &out) {
  const uint32_t funct3 = dec_funct3(raw);
  const uint32_t imm = dec_imm_i(raw);
  const uint32_t rs2 = dec_rs2(raw);

  switch (funct3) {
  case 0:
    if (!(dec_rd(raw) == 0 && dec_rs1(raw) == 0)) {
      return false;
    }
    switch (imm) {
    case 0:  // ECALL
      out.type = rv_inst_ecall;
      break;
    case 1:  // EBREAK
      out.type = rv_inst_ebreak;
      break;
    case 0x302:  // MRET
      if (rs2 != 0b00010) {
        return false;
      }
      out.type = rv_inst_mret;
      break;
    default:
      return false;
    }
    break;
  case 1:  // CSRRW
    out.type = rv_inst_csrrw;
    break;
  case 2:  // CSRRS
    out.type = rv_inst_csrrs;
    break;
  case 3:  // CSRRC
    out.type = rv_inst_csrrc;
    break;
  case 5:  // CSRRWI
    out.type = rv_inst_csrrwi;
    break;
  case 6:  // CSRRSI
    out.type = rv_inst_csrrsi;
    break;
  case 7:  // CSRRCI
    out.type = rv_inst_csrrci;
    break;
  default:
    return false;
  }

  switch (funct3) {
  case 1:  // CSRRW
  case 2:  // CSRRS
  case 3:  // CSRRC
  case 5:  // CSRRWI
  case 6:  // CSRRSI
  case 7:  // CSRRCI
    out.rd = dec_rd(raw);
    out.imm = dec_imm_i(raw);
    out.rs1 = dec_rs1(raw);
    break;
  }
  return true;
}

static bool dec_comp_quad0(uint32_t raw, rv_inst_t &out) {
  out.size = 2;
  const uint32_t funct3 = dec_funct3(raw);
  switch (funct3) {
  case 2:  // lw
    out.type = rv_inst_lw;
    out.rd   = (raw >> 2) & 7;
    out.rs1  = (raw >> 7) & 7;
    out.imm  = (((raw >>  6) & 1) << 2) |
               (((raw >>  5) & 1) << 6) |
               (((raw >> 10) & 7) << 3);
    return true;
  case 3:  // ld
    out.type = rv_inst_ld;
    out.rd   = (raw >> 2) & 7;
    out.rs1  = (raw >> 7) & 7;
    out.imm  = (((raw >>  5) & 3) << 6) |
               (((raw >> 10) & 7) << 3);
    return true;
  case 6:  // sw
    out.type = rv_inst_sw;
    out.rs2  = (raw >> 2) & 7;
    out.rs1  = (raw >> 7) & 7;
    out.imm  = (((raw >>  6) & 1) << 2) |
               (((raw >>  5) & 1) << 6) |
               (((raw >> 10) & 7) << 3);
    return true;
  case 7:  // sd
    out.type = rv_inst_sd;
    out.rs2  = (raw >> 2) & 7;
    out.rs1  = (raw >> 7) & 7;
    out.imm  = (((raw >>  5) & 3) << 6) |
               (((raw >> 10) & 7) << 3);
    return true;
  default:
    return false;
  }
}

static bool dec_comp_quad1(uint32_t raw, rv_inst_t &out) {
  out.size = 2;
  return false;
}

static bool dec_comp_quad2(uint32_t raw, rv_inst_t &out) {
  out.size = 2;
  const uint32_t funct3 = dec_funct3(raw);
  switch (funct3) {
  case 0:  // C.SLLI, C.SLLI64
  case 1:  // C.FLDSP
  case 2:  // C.LWSP
  case 3:  // C.FLWSP, C.LDSP
  case 4:  // C.JR, C.MV, C.EBREAK, C.JALR, C.ADD
  case 5:  // C.FSDSP
  case 6:  // C.SWSP
  case 7:  // C.FSWSP, C.SDSP
    break;
  }
  return false;
}

bool rv_decode(uint32_t raw, rv_inst_t &out) {
  switch (raw & 3) {
#if 0
  case 0: // comp. quad. 0
    return dec_comp_quad0(raw, out);
  case 1: // comp. quad. 1
    return dec_comp_quad1(raw, out);
  case 2: // comp. quad. 2
    return dec_comp_quad2(raw, out);
#endif
  case 3: // uncompressed
    out.size = 4;
    const uint32_t index = (raw >> 2) & 0x1F;
    switch (index) {
    case 0: // LOAD
      return dec_load(raw, out);
    case 3: // MISC-MEM
      return dec_misc_mem(raw, out);
    case 4: // OP-IMM
      return dec_op_imm(raw, out);
    case 5: // AUIPC
      return dec_auipc(raw, out);
    case 6: // OP-IMM-32
      return dec_op_imm_32(raw, out);
    case 8: // STORE
      return dec_store(raw, out);
    case 12: // OP
      return dec_op(raw, out);
    case 13: // LUI
      return dec_lui(raw, out);
    case 14: // OP-32
      return dec_op_32(raw, out);
    case 24: // BRANCH
      return dec_branch(raw, out);
    case 25: // JALR
      return dec_jalr(raw, out);
    case 27: // JAL
      return dec_jal(raw, out);
    case 28: // SYSTEM
      return dec_system(raw, out);
    }
  }
  return false;
}

// return the uncompressed instruction opcodes
static uint32_t get_opcode(rv_inst_type_t inst) {
  switch (inst) {
  case rv_inst_lui:         return 0b0110111;
  case rv_inst_auipc:       return 0b0010111;
  case rv_inst_jal:         return 0b1101111;
  case rv_inst_jalr:        return 0b1100111;
  case rv_inst_beq: 
  case rv_inst_bne: 
  case rv_inst_blt: 
  case rv_inst_bge: 
  case rv_inst_bltu:
  case rv_inst_bgeu:        return 0b1100011;
  case rv_inst_lb: 
  case rv_inst_lh: 
  case rv_inst_lw: 
  case rv_inst_lbu:
  case rv_inst_lwu:
  case rv_inst_ld: 
  case rv_inst_lhu:         return 0b0000011;
  case rv_inst_sb:
  case rv_inst_sh:
  case rv_inst_sw:
  case rv_inst_sd:          return 0b0100011;
  case rv_inst_addi: 
  case rv_inst_slti: 
  case rv_inst_sltiu:
  case rv_inst_xori: 
  case rv_inst_ori:  
  case rv_inst_andi: 
  case rv_inst_slli: 
  case rv_inst_srli: 
  case rv_inst_srai:        return 0b0010011;
  case rv_inst_add: 
  case rv_inst_sub: 
  case rv_inst_sll: 
  case rv_inst_slt: 
  case rv_inst_sltu:
  case rv_inst_xor: 
  case rv_inst_srl: 
  case rv_inst_sra: 
  case rv_inst_or:  
  case rv_inst_and:         return 0b0110011;
  case rv_inst_fencei:
  case rv_inst_fence:       return 0b0001111;
  case rv_inst_slliw:
  case rv_inst_srliw:
  case rv_inst_sraiw:
  case rv_inst_addiw:       return 0b0011011;
  case rv_inst_addw:
  case rv_inst_subw:
  case rv_inst_sllw:
  case rv_inst_srlw:
  case rv_inst_sraw:        return 0b0111011;
  case rv_inst_ecall:
  case rv_inst_mret:
  case rv_inst_ebreak:
  case rv_inst_csrrw: 
  case rv_inst_csrrs: 
  case rv_inst_csrrc: 
  case rv_inst_csrrwi:
  case rv_inst_csrrsi:
  case rv_inst_csrrci:      return 0b1110011;
  default:
    assert(!"unreachable");
    return 0;
  }
}

bool rv_encode(const rv_inst_t &i, uint32_t &out) {
  if (i.size == 2) {
    assert(!"todo");
  }

  out = 0;
  out |= get_opcode(i.type);

  switch (i.type) {
  case rv_inst_lui:
  case rv_inst_auipc:
    out |= enc_rd(i.rd) | enc_imm_u(i.imm);
    break;
  case rv_inst_jal:
    out |= enc_rd(i.rd) | enc_imm_j(i.imm);
    break;
  case rv_inst_jalr:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_beq:
  case rv_inst_bne:
  case rv_inst_blt:
  case rv_inst_bge:
  case rv_inst_bltu:
  case rv_inst_bgeu:
    out |= enc_imm_b(i.imm) | enc_rs1(i.rs1) | enc_rs2(i.rs2);
    break;
  case rv_inst_lb:
  case rv_inst_lbu:
  case rv_inst_lh:
  case rv_inst_lhu:
  case rv_inst_lw:
  case rv_inst_lwu:
  case rv_inst_ld:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_sb:
  case rv_inst_sh:
  case rv_inst_sw:
  case rv_inst_sd:
    out |= enc_rs1(i.rs1) | enc_rs2(i.rs2) | enc_imm_s(i.imm);
    break;
  case rv_inst_addi:
  case rv_inst_slti:
  case rv_inst_sltiu:
  case rv_inst_xori:
  case rv_inst_ori:
  case rv_inst_andi:
  case rv_inst_slli:
  case rv_inst_srli:
  case rv_inst_srai:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_add:
  case rv_inst_sub:
  case rv_inst_sll:
  case rv_inst_slt:
  case rv_inst_sltu:
  case rv_inst_xor:
  case rv_inst_srl:
  case rv_inst_sra:
  case rv_inst_or:
  case rv_inst_and:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_rs2(i.rs2);
    break;
  case rv_inst_fence:
  case rv_inst_fencei:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_ecall:
    out |= enc_imm_i(0);
    break;
  case rv_inst_ebreak:
    out |= enc_imm_i(1);
    break;
  case rv_inst_addiw:
  case rv_inst_slliw:
  case rv_inst_srliw:
  case rv_inst_sraiw:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_addw:
  case rv_inst_subw:
  case rv_inst_sllw:
  case rv_inst_srlw:
  case rv_inst_sraw:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_rs2(i.rs2);
    break;
  case rv_inst_csrrw:
  case rv_inst_csrrs:
  case rv_inst_csrrc:
  case rv_inst_csrrwi:
  case rv_inst_csrrsi:
  case rv_inst_csrrci:
    out |= enc_rd(i.rd) | enc_rs1(i.rs1) | enc_imm_i(i.imm);
    break;
  case rv_inst_mret:
    out |= enc_rs2(2);
    break;
  default:
    assert(!"unreachable");
  }

  // set funct3 / funct7 field if needed
  switch (i.type) {
  case rv_inst_jalr:   out |= enc_funct3(0b000); break;

  case rv_inst_beq:    out |= enc_funct3(0b000); break;
  case rv_inst_bne:    out |= enc_funct3(0b001); break;
  case rv_inst_blt:    out |= enc_funct3(0b100); break;
  case rv_inst_bge:    out |= enc_funct3(0b101); break;
  case rv_inst_bltu:   out |= enc_funct3(0b110); break;
  case rv_inst_bgeu:   out |= enc_funct3(0b111); break;

  case rv_inst_sb:     out |= enc_funct3(0b000); break;
  case rv_inst_sh:     out |= enc_funct3(0b001); break;
  case rv_inst_sw:     out |= enc_funct3(0b010); break;
  case rv_inst_sd:     out |= enc_funct3(0b011); break;

  case rv_inst_addi:   out |= enc_funct3(0b000); break;
  case rv_inst_slti:   out |= enc_funct3(0b010); break;
  case rv_inst_sltiu:  out |= enc_funct3(0b011); break;
  case rv_inst_xori:   out |= enc_funct3(0b100); break;
  case rv_inst_ori:    out |= enc_funct3(0b110); break;
  case rv_inst_andi:   out |= enc_funct3(0b111); break;
  case rv_inst_slli:   out |= enc_funct3(0b001); break;
  case rv_inst_srli:   out |= enc_funct3(0b101) | enc_funct7(0b0000000); break;
  case rv_inst_srai:   out |= enc_funct3(0b101) | enc_funct7(0b0100000); break;

  case rv_inst_add:    out |= enc_funct3(0b000) | enc_funct7(0b0000000); break;
  case rv_inst_sub:    out |= enc_funct3(0b000) | enc_funct7(0b0100000); break;
  case rv_inst_sll:    out |= enc_funct3(0b001) | enc_funct7(0b0000000); break;
  case rv_inst_slt:    out |= enc_funct3(0b010) | enc_funct7(0b0000000); break;
  case rv_inst_sltu:   out |= enc_funct3(0b011) | enc_funct7(0b0000000); break;
  case rv_inst_xor:    out |= enc_funct3(0b100) | enc_funct7(0b0000000); break;
  case rv_inst_srl:    out |= enc_funct3(0b101) | enc_funct7(0b0000000); break;
  case rv_inst_sra:    out |= enc_funct3(0b101) | enc_funct7(0b0100000); break;
  case rv_inst_or:     out |= enc_funct3(0b110) | enc_funct7(0b0000000); break;
  case rv_inst_and:    out |= enc_funct3(0b111) | enc_funct7(0b0000000); break;

  case rv_inst_lb:     out |= enc_funct3(0b000); break;
  case rv_inst_lh:     out |= enc_funct3(0b001); break;
  case rv_inst_lw:     out |= enc_funct3(0b010); break;
  case rv_inst_ld:     out |= enc_funct3(0b011); break;
  case rv_inst_lbu:    out |= enc_funct3(0b100); break;
  case rv_inst_lhu:    out |= enc_funct3(0b101); break;
  case rv_inst_lwu:    out |= enc_funct3(0b110); break;

  case rv_inst_fence:  out |= enc_funct3(0b000); break;
  case rv_inst_fencei: out |= enc_funct3(0b001); break;

  case rv_inst_addw:   out |= enc_funct3(0b000) | enc_funct7(0b0000000); break;
  case rv_inst_subw:   out |= enc_funct3(0b000) | enc_funct7(0b0100000); break;
  case rv_inst_sllw:   out |= enc_funct3(0b001) | enc_funct7(0b0000000); break;
  case rv_inst_srlw:   out |= enc_funct3(0b101) | enc_funct7(0b0000000); break;
  case rv_inst_sraw:   out |= enc_funct3(0b101) | enc_funct7(0b0100000); break;

  case rv_inst_csrrw:  out |= enc_funct3(0b001); break;
  case rv_inst_csrrs:  out |= enc_funct3(0b010); break;
  case rv_inst_csrrc:  out |= enc_funct3(0b011); break;
  case rv_inst_csrrwi: out |= enc_funct3(0b101); break;
  case rv_inst_csrrsi: out |= enc_funct3(0b110); break;
  case rv_inst_csrrci: out |= enc_funct3(0b111); break;

  case rv_inst_addiw:  out |= enc_funct3(0b000) | enc_funct7(0b0000000); break;
  case rv_inst_slliw:  out |= enc_funct3(0b001) | enc_funct7(0b0000000); break;
  case rv_inst_srliw:  out |= enc_funct3(0b101) | enc_funct7(0b0000000); break;
  case rv_inst_sraiw:  out |= enc_funct3(0b101) | enc_funct7(0b0100000); break;

  case rv_inst_mret:   out |= enc_funct7(0b0011000);
  }

  return true;
}

static const char *regname(uint32_t i) {
  static const char *name[32] = {
    "zero",
    "ra",
    "sp",
    "gp",
    "tp",
    "t0", "t1", "t2",
    "fp", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6",
  };
  return name[i & 0x1f];
}

static const char *mnemonic(const rv_inst_t &in) {
  switch (in.type) {
  case rv_inst_lui:    return "lui";
  case rv_inst_auipc:  return "auipc";
  case rv_inst_jal:    return "jal";
  case rv_inst_jalr:   return "jalr";
  case rv_inst_beq:    return "beq";
  case rv_inst_bne:    return "bne";
  case rv_inst_blt:    return "blt";
  case rv_inst_bge:    return "bge";
  case rv_inst_bltu:   return "bltu";
  case rv_inst_bgeu:   return "bgeu";
  case rv_inst_lb:     return "lb";
  case rv_inst_lh:     return "lh";
  case rv_inst_lw:     return "lw";
  case rv_inst_lbu:    return "lbu";
  case rv_inst_lhu:    return "lhu";
  case rv_inst_sb:     return "sb";
  case rv_inst_sh:     return "sh";
  case rv_inst_sw:     return "sw";
  case rv_inst_addi:   return "addi";
  case rv_inst_slti:   return "slti";
  case rv_inst_sltiu:  return "sltiu";
  case rv_inst_xori:   return "xori";
  case rv_inst_ori:    return "ori";
  case rv_inst_andi:   return "andi";
  case rv_inst_slli:   return "slli";
  case rv_inst_srli:   return "srli";
  case rv_inst_srai:   return "srai";
  case rv_inst_add:    return "add";
  case rv_inst_sub:    return "sub";
  case rv_inst_sll:    return "sll";
  case rv_inst_slt:    return "slt";
  case rv_inst_sltu:   return "sltu";
  case rv_inst_xor:    return "xor";
  case rv_inst_srl:    return "srl";
  case rv_inst_sra:    return "sra";
  case rv_inst_or:     return "or";
  case rv_inst_and:    return "and";
  case rv_inst_fence:  return "fence";
  case rv_inst_ecall:  return "ecall";
  case rv_inst_ebreak: return "ebreak";
  case rv_inst_lwu:    return "lwu";
  case rv_inst_ld:     return "ld";
  case rv_inst_sd:     return "sd";
  case rv_inst_addiw:  return "addiw";
  case rv_inst_slliw:  return "slliw";
  case rv_inst_srliw:  return "srliw";
  case rv_inst_sraiw:  return "sraiw";
  case rv_inst_addw:   return "addw";
  case rv_inst_subw:   return "subw";
  case rv_inst_sllw:   return "sllw";
  case rv_inst_srlw:   return "srlw";
  case rv_inst_sraw:   return "sraw";
  case rv_inst_fencei: return "fencei";
  case rv_inst_csrrw:  return "csrrw";
  case rv_inst_csrrs:  return "csrrs";
  case rv_inst_csrrc:  return "csrrc";
  case rv_inst_csrrwi: return "csrrwi";
  case rv_inst_csrrsi: return "csrrsi";
  case rv_inst_csrrci: return "csrrci";
  case rv_inst_mret:   return "mret";
  default:
    assert(!"unreachable");
    return "";
  }
}

void rv_print(uint32_t pc, const struct rv_inst_t &i, char *dst, size_t max) {
  const char *m = mnemonic(i);
  assert(m);
  switch (i.type) {
  case rv_inst_lui:
  case rv_inst_auipc:
    snprintf(dst, max, "%s %s %xh", m, regname(i.rd), i.imm);
    break;
  case rv_inst_jal:
    if (i.rd == 0) {
      snprintf(dst, max, "j %xh", i.imm);
    }
    else {
      snprintf(dst, max, "%s %s %xh", m, regname(i.rd), i.imm);
    }
    break;
  case rv_inst_jalr:
    if (i.imm == 0 && i.rd == 0 && i.rs1 == 1) {
      snprintf(dst, max, "ret");
    }
    else {
      snprintf(dst, max, "%s %s %s %xh", m, regname(i.rd), regname(i.rs1), i.imm);
    }
    break;
  case rv_inst_beq:
  case rv_inst_bne:
  case rv_inst_blt:
  case rv_inst_bge:
  case rv_inst_bltu:
  case rv_inst_bgeu:
    snprintf(dst, max, "%s %s %s %xh", m, regname(i.rs1), regname(i.rs2), i.imm);
    break;
  case rv_inst_lb:
  case rv_inst_lbu:
  case rv_inst_lh:
  case rv_inst_lhu:
  case rv_inst_lw:
  case rv_inst_lwu:
  case rv_inst_ld:
    snprintf(dst, max, "%s %s %s %d", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_sb:
  case rv_inst_sh:
  case rv_inst_sw:
  case rv_inst_sd:
    snprintf(dst, max, "%s %s %s %d", m, regname(i.rs1), regname(i.rs2), i.imm);
    break;
  case rv_inst_addi:
    if (i.imm == 0) {
      snprintf(dst, max, "mv %s %s", regname(i.rd), regname(i.rs1));
    }
    else if (i.rs1 == 0) {
      snprintf(dst, max, "li %s %d", regname(i.rd), i.imm);
    }
    else {
      snprintf(dst, max, "%s %s %s %d", m, regname(i.rd), regname(i.rs1), i.imm);
    }
    break;
  case rv_inst_slti:
  case rv_inst_sltiu:
  case rv_inst_xori:
  case rv_inst_ori:
  case rv_inst_andi:
  case rv_inst_slli:
  case rv_inst_srli:
  case rv_inst_srai:
    snprintf(dst, max, "%s %s %s %d", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_add:
  case rv_inst_sub:
  case rv_inst_sll:
  case rv_inst_slt:
  case rv_inst_sltu:
  case rv_inst_xor:
  case rv_inst_srl:
  case rv_inst_sra:
  case rv_inst_or:
  case rv_inst_and:
    snprintf(dst, max, "%s %s %s %s", m, regname(i.rd), regname(i.rs1), regname(i.rs2));
    break;
  case rv_inst_fence:
    snprintf(dst, max, "%s %s %s %04x", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_ecall:
  case rv_inst_ebreak:
    snprintf(dst, max, "%s", m);
    break;
  case rv_inst_addiw:
  case rv_inst_slliw:
  case rv_inst_srliw:
  case rv_inst_sraiw:
    snprintf(dst, max, "%s %s %s %d", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_addw:
  case rv_inst_subw:
  case rv_inst_sllw:
  case rv_inst_srlw:
  case rv_inst_sraw:
    snprintf(dst, max, "%s %s %s %s", m, regname(i.rd), regname(i.rs1), regname(i.rs2));
    break;
  case rv_inst_fencei:
    snprintf(dst, max, "%s %s %s %04x", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_csrrw:
  case rv_inst_csrrs:
  case rv_inst_csrrc:
    snprintf(dst, max, "%s %s %s %d", m, regname(i.rd), regname(i.rs1), i.imm);
    break;
  case rv_inst_csrrwi:
  case rv_inst_csrrsi:
  case rv_inst_csrrci:
    snprintf(dst, max, "%s %s %d %d", m, regname(i.rd), i.rs1, i.imm);
    break;
  case rv_inst_mret:
    snprintf(dst, max, "%s", m);
    break;
  default:
    assert(!"unreachable");
  }
}
