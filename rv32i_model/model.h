#pragma once
#include <cstdint>
#include <memory>

#include "elf.h"
#include "memory.h"

// riscv register file enums
enum {
  rv_reg_zero = 0,
  rv_reg_ra,
  rv_reg_sp,
  rv_reg_gp,
  rv_reg_tp,
  rv_reg_t0,
  rv_reg_t1,
  rv_reg_t2,
  rv_reg_s0,
  rv_reg_s1,
  rv_reg_a0,
  rv_reg_a1,
  rv_reg_a2,
  rv_reg_a3,
  rv_reg_a4,
  rv_reg_a5,
  rv_reg_a6,
  rv_reg_a7,
  rv_reg_s2,
  rv_reg_s3,
  rv_reg_s4,
  rv_reg_s5,
  rv_reg_s6,
  rv_reg_s7,
  rv_reg_s8,
  rv_reg_s9,
  rv_reg_s10,
  rv_reg_s11,
  rv_reg_t3,
  rv_reg_t4,
  rv_reg_t5,
  rv_reg_t6,
};

struct rv32i_model_t {

  rv32i_model_t();
  ~rv32i_model_t();

  void set_pc(uint32_t pc);
  uint32_t get_pc() const;

  uint32_t get_reg(uint32_t index) const;
  void set_reg(uint32_t index, uint32_t value);

  void reset();
  void step();

  bool load_elf(const char *path);
  bool load_hex(const char *path, uint32_t base);

  memory_t &memory() {
    return mem;
  }

  elf_t &program() {
    return elf;
  }

  bool ecall_pending;

protected:

  uint32_t get_phi() const;

  elf_t elf;
  memory_t mem;
  std::unique_ptr<struct Vrv32i_cpu_t> rtl;
};
