#include <stdbool.h>
#include <stdint.h>

#include "../common.h"

volatile char password[8];
uint32_t expect = 0xdfca312d;  // "testme12"

bool NO_INLINE check_password(const char *b) {
  uint32_t hash = 0;
  for (int i=0; i<8; ++i) {
    hash ^= (hash << 4) + b[i];
  }
  return hash == expect;
}

int main() {
  bool passed = false;
  while (true) {
    get_password(password, 8);
    passed = check_password(password) ? 1 : 0;
    if (passed) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
