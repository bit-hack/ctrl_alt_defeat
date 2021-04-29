#pragma once
#include <cstdint>
#include <array>

struct level_t {
  // reputation required
  uint32_t rep_req;
  // mission briefing
  const char *brief;
  // path to the executable
  const char *elf_path;
};

const level_t levels[] = {
{
  0,
  R"(The Simple Level
)",
  "../levels/simple/out.elf"
},
{
  1,
  R"(The Needle Level
)",
  "../levels/needle/out.elf"
},
{
  2,
  R"(The Offset Level
)",
  "../levels/offset/out.elf"
}
};
