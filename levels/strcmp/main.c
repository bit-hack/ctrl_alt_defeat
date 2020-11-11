#include <stdbool.h>
#include <stdint.h>

#include "../common.h"

char temp[16] = "unknown";
char ref[] = "Rosebud";

bool NO_INLINE compare(char *a, char *b) {
  for (;; ++a, ++b) {
    if (*a != *b) {
      return false;
    }
    if (*a == '\0') {
      return true;
    }
  }
}

int main() {
  while (true) {
    get_password(temp);
    if (compare(temp, ref)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
