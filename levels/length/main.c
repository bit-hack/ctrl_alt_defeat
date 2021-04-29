#include <stdbool.h>
#include <stdint.h>

#include "../common.h"

bool NO_INLINE compare(uint8_t *a) {
  uint32_t i=0;
  for (;*a ;++a, ++i);
  return i == 7;
}

int main() {
  char temp[16];
  while (true) {
    get_password(temp, sizeof(temp));
    if (compare(temp)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
