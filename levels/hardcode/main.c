#include <stdbool.h>
#include <stdint.h>

#include "../common.h"

bool NO_INLINE compare(uint8_t *a) {
  if (a[0] == (uint8_t)'o')
    if (a[1] == (uint8_t)'c')
      if (a[2] == (uint8_t)'t')
        if (a[3] == (uint8_t)'o')
          if (a[4] == (uint8_t)'p')
            if (a[5] == (uint8_t)'u')
              if (a[6] == (uint8_t)'s')
                return true;
  return false;
}

int main() {
  char temp[16];
  while (true) {
    get_password(temp, 16);
    if (compare(temp)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
