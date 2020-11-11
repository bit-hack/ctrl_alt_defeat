#pragma once
#include "encoding.h"

static const char *rv_inst_desc(const rv_inst_t &i) {
  switch (i.type) {
  case rv_inst_lui:    return "Load Upper Immediate";
  case rv_inst_auipc:  return "Add Upper Immediate to PC";
  case rv_inst_jal:
    if (i.rd == 0)
      return "Jump";
    return "Jump and Link";
  case rv_inst_jalr:
    if (i.rd == 0 && i.imm == 0 && i.rs1 == 1)
      return "Return from subroutine";
    return "Jump and Link Relative";
  case rv_inst_beq:    return "Branch if Equal";
  case rv_inst_bne:    return "Branch if Not Equal";
  case rv_inst_blt:    return "Branch less than";
  case rv_inst_bge:    return "Branch Greater or Equal";
  case rv_inst_bltu:   return "Branch Less Than Unsigned";
  case rv_inst_bgeu:   return "Branch Greater or Equal Unsigned";
  case rv_inst_lb:     return "Load Byte";
  case rv_inst_lh:     return "Load Half";
  case rv_inst_lw:     return "Load Word";
  case rv_inst_lbu:    return "Load Byte Unsigned";
  case rv_inst_lhu:    return "Load Half Unsigned";
  case rv_inst_sb:     return "Store Byte";
  case rv_inst_sh:     return "Store Half";
  case rv_inst_sw:     return "Store Word";
  case rv_inst_addi:
    if (i.rd == 0)
      return "No Operation";
    if (i.imm == 0)
      return "Move";;
    if (i.rs1 == 0)
      return "Load Immediate";
    return "Add Immediate";
  case rv_inst_slti:   return "Set Less Than Immediate";
  case rv_inst_sltiu:  return "Set Less Than Immediate Unsigned";
  case rv_inst_xori:   return "Exclusive Or Immediate";
  case rv_inst_ori:    return "Or Immediate";
  case rv_inst_andi:   return "And Immediate";
  case rv_inst_slli:   return "Shift Logical Left Immediate";
  case rv_inst_srli:   return "Shift Right Logical Immediate";
  case rv_inst_srai:   return "Shift Right Arithmetic Immediate";
  case rv_inst_add:    return "Add";
  case rv_inst_sub:    return "Subtract";
  case rv_inst_sll:    return "Shift Left Logical";
  case rv_inst_slt:    return "Set Less Than";
  case rv_inst_sltu:   return "Set Less Than Unsigned";
  case rv_inst_xor:    return "Exclusive Or";
  case rv_inst_srl:    return "Shift Right Logical";
  case rv_inst_sra:    return "Shift Right Arithmetic";
  case rv_inst_or:     return "Or";
  case rv_inst_and:    return "And";
  case rv_inst_fence:  return "Memory Fence";
  case rv_inst_ecall:  return "System Call";
  case rv_inst_ebreak: return "Breakpoint";
  default:             return nullptr;
  }
}

static const char *rv_inst_info(const rv_inst_t &i) {
  switch (i.type) {
  case rv_inst_lui:
    return "rd = imm";
  case rv_inst_auipc:
    return "rd = imm + pc";
  case rv_inst_jal:
    return "tmp = pc; pc += imm; rd = tmp + 4";
  case rv_inst_jalr:
    return "tmp = pc; pc = (rs1 + imm) & ~1u; rd = tmp + 4";
  case rv_inst_beq:
    return "pc += (rs1 == rs2) ? imm : 4";
  case rv_inst_bne:
    return "pc += (rs1 != rs2) ? imm : 4";
  case rv_inst_blt:
    return "pc += (rs1 < rs2) ? imm : 4";
  case rv_inst_bge:
    return "pc += (rs1 >= rs2) ? imm : 4";
  case rv_inst_bltu:
    return "pc += (rs1 < rs2) ? imm : 4";
  case rv_inst_bgeu:
    return "pc += (rs1 >= rs2) ? imm : 4";
  case rv_inst_lb:
    return "rd = (char)[rs1 + imm]";
  case rv_inst_lh:
    return "rd = (short)[rs1 + imm]";
  case rv_inst_lw:
    return "rd = (int)[rs1 + imm]";
  case rv_inst_lbu:
    return "rd = (unsigned char)[rs1 + imm]";
  case rv_inst_lhu:
    return "rd = (unsigned short)[rs1 + imm]";
  case rv_inst_sb:
    return "(byte)[rs1 + imm] = rs2)";
  case rv_inst_sh:
    return "(short)[rs1 + imm] = rs2)";
  case rv_inst_sw:
    return "(int)[rs1 + imm] = rs2)";
  case rv_inst_addi:
    return "rd = rs1 + imm";
  case rv_inst_slti:
    return "rd = (rs1 < imm) ? 1 : 0";
  case rv_inst_sltiu:
    return "rd = (rs1 < (uint32_t)imm) ? 1 : 0";
  case rv_inst_xori:
    return "rd = rs1 ^ imm";
  case rv_inst_ori:
    return "rd = rs1 | imm";
  case rv_inst_andi:
    return "rd = rs1 & imm";
  case rv_inst_slli:
    return "rd = rs1 << (imm & 31)";
  case rv_inst_srli:
    return "rd = rs1 >> (imm & 31)";
  case rv_inst_srai:
    return "rd = rs1 >> (imm & 31)";
  case rv_inst_add:
    return "rd = rs1 + rs2";
  case rv_inst_sub:
    return "rd = rs1 - rs2";
  case rv_inst_sll:
    return "rd = rs1 << (rs2 & 31)";
  case rv_inst_slt:
    return "rd = (rs1 < rs2) ? 1 : 0";
  case rv_inst_sltu:
    return "rd = (rs1 < rs2) ? 1 : 0";
  case rv_inst_xor:
    return "rd = rs1 ^ rs2";
  case rv_inst_srl:
    return "rd = rs1 >> (rs2 & 31)";
  case rv_inst_sra:
    return "rd = rs1 >> (rs2 & 31)";
  case rv_inst_or:
    return "rd = rs1 | rs2";
  case rv_inst_and:
    return "rd = rs1 & rs2";
  case rv_inst_fence:
    return "Ensure all memory operations are complete";
  case rv_inst_ecall:
    return "ecall()";
  case rv_inst_ebreak:
    return "ebreak();";
  default: return nullptr;
  }
}
