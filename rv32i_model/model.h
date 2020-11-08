#pragma once
#include <cstdint>
#include <memory>

#include "elf.h"
#include "memory.h"


struct rv32i_model_t {

  rv32i_model_t();
  ~rv32i_model_t();

  void set_pc(uint32_t pc);
  uint32_t get_pc() const;

  uint32_t get_reg(uint32_t index) const;
  void set_reg(uint32_t index, uint32_t value);

  void reset();
  void step();

  bool is_active() const;

  bool load_elf(const char *path);

  bool load_hex(const char *path, uint32_t base);

  memory_t &memory() {
    return mem;
  }

  const memory_t &memory() const {
    return mem;
  }

protected:

  uint32_t get_phi() const;

  bool active;
  elf_t elf;
  memory_t mem;
  std::unique_ptr<struct Vrv32i_cpu_t> rtl;
};