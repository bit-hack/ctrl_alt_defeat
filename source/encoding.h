#pragma once
#include <cstdint>

enum rv_inst_type_t : uint8_t {
  rv_inst_unknown,

  // RV32I
  rv_inst_lui,
  rv_inst_auipc,
  rv_inst_jal,
  rv_inst_jalr,
  rv_inst_beq,
  rv_inst_bne,
  rv_inst_blt,
  rv_inst_bge,
  rv_inst_bltu,
  rv_inst_bgeu,
  rv_inst_lb,
  rv_inst_lh,
  rv_inst_lw,
  rv_inst_lbu,
  rv_inst_lhu,
  rv_inst_sb,
  rv_inst_sh,
  rv_inst_sw,
  rv_inst_addi,
  rv_inst_slti,
  rv_inst_sltiu,
  rv_inst_xori,
  rv_inst_ori,
  rv_inst_andi,
  rv_inst_slli,
  rv_inst_srli,
  rv_inst_srai,
  rv_inst_add,
  rv_inst_sub,
  rv_inst_sll,
  rv_inst_slt,
  rv_inst_sltu,
  rv_inst_xor,
  rv_inst_srl,
  rv_inst_sra,
  rv_inst_or,
  rv_inst_and,
  rv_inst_fence,
  rv_inst_ecall,
  rv_inst_ebreak,

  // RV64I
  rv_inst_lwu,
  rv_inst_ld,
  rv_inst_sd,
  rv_inst_addiw,
  rv_inst_slliw,
  rv_inst_srliw,
  rv_inst_sraiw,
  rv_inst_addw,
  rv_inst_subw,
  rv_inst_sllw,
  rv_inst_srlw,
  rv_inst_sraw,

  // Zifencei
  rv_inst_fencei,

  // Zicsr
  rv_inst_csrrw,
  rv_inst_csrrs,
  rv_inst_csrrc,
  rv_inst_csrrwi,
  rv_inst_csrrsi,
  rv_inst_csrrci,

  // privileged
  rv_inst_mret,
};

struct rv_inst_t {

  rv_inst_t()
    : imm(0)
    , type(rv_inst_unknown)
    , rd(0)
    , rs1(0)
    , rs2(0)
    , size(0)
  {}

  // immediate operand
  int32_t imm;
  // opcode
  rv_inst_type_t type;
  // field operands
  uint8_t rd, rs1, rs2;
  // instruction size
  uint8_t size;
};

bool rv_decode(uint32_t raw, rv_inst_t &out);
bool rv_encode(const rv_inst_t &in, uint32_t &out);
void rv_print(uint32_t pc, const rv_inst_t &i, char *dst, size_t max);
