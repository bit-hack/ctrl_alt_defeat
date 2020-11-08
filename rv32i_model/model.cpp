#include "obj_dir/Vrv32i_cpu_t.h"

#include "model.h"


rv32i_model_t::rv32i_model_t()
  : active(false)
  , rtl(new Vrv32i_cpu_t)
{
}

rv32i_model_t::~rv32i_model_t()
{
}

void rv32i_model_t::set_pc(uint32_t addr) {
  rtl->rv32i_cpu_t__DOT__pc = addr;
  rtl->eval();
}

uint32_t rv32i_model_t::get_pc() const {
  return rtl->rv32i_cpu_t__DOT__pc;
}

uint32_t rv32i_model_t::get_phi() const {
  return rtl->rv32i_cpu_t__DOT__phi;
}

uint32_t rv32i_model_t::get_reg(uint32_t index) const {
  return rtl->rv32i_cpu_t__DOT__X[index & 31];
}

void rv32i_model_t::set_reg(uint32_t index, uint32_t value) {
  rtl->rv32i_cpu_t__DOT__X[index & 31] = value;
}

void rv32i_model_t::reset() {
  active = true;
  rtl->hold = 0;
  rtl->reset = 1;
  for (int i = 0; i < 4; ++i) {
    rtl->clk ^= 1;
    rtl->eval();
  }
  rtl->reset = 0;
  rtl->eval();

  // TODO: set the real reset PC
  if (elf.is_valid()) {
    set_pc(elf.get_entry_point());
  }
  else {
    set_pc(0x1000);
    set_reg(rv_reg_sp, 0x1fff);
  }
}

void rv32i_model_t::step() {
  static const uint32_t ecall_inst = 0x00000073;
  while (active) {

    // update logic
    rtl->eval();

    // update memory on the negative clock edge
    if (rtl->clk == 0) {
      const uint32_t addr = rtl->out_mem_addr;
      const uint32_t read = mem.read_w(addr & ~3u);
      const uint32_t out = rtl->out_data;
      const uint32_t mask = rtl->out_write_mask;

      rtl->in_data = read;
      rtl->eval();

      if (mask) {
        const uint32_t write =
          ((mask & 1) ? (out & 0x000000ff) : (read & 0x000000ff)) |
          ((mask & 2) ? (out & 0x0000ff00) : (read & 0x0000ff00)) |
          ((mask & 4) ? (out & 0x00ff0000) : (read & 0x00ff0000)) |
          ((mask & 8) ? (out & 0xff000000) : (read & 0xff000000));

        mem.write(addr & ~3u, (const uint8_t*)&write, 4);
      }
    }

    // check if we just fetched and ecall instruction
    if (rtl->rv32i_cpu_t__DOT__inst == ecall_inst) {
      active = false;
    }

    // toggle the clock
    rtl->clk ^= 1;
    // exit on next instruction fetch
    if (rtl->clk && get_phi() == 0) {
      break;
    }
  }
}

bool rv32i_model_t::is_active() const {
  return active;
}

bool rv32i_model_t::load_elf(const char *path) {
  if (!elf.load(path)) {
    return false;
  }
  mem.reset();
  if (!elf.upload(mem)) {
    return false;
  }
  reset();
  set_pc(elf.get_entry_point());
  return true;
}

bool rv32i_model_t::load_hex(const char *path, uint32_t base) {
  reset();
  set_pc(base);
  FILE *fd = fopen(path, "r");
  if (!fd) {
    return false;
  }
  for (;; base += 4) {
    int32_t val = 0;
    if (fscanf(fd, "%x\n", &val) != 1) {
      break;
    }
    mem.write(base, (const uint8_t*)&val, 4);
  }
  fclose(fd);
  return true;
}
