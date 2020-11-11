#include <stdbool.h>
#include <stdint.h>

#include "../common.h"

volatile char password[8];
char expected[8] = "octopii";

bool NO_INLINE check_password(volatile const char *a,
                              const char *b) {
  for (;*a; ++a, ++b) {
    if (*a != *b) {
      return false;
    }
  }
  return true;
}

int main() {
  bool passed = false;
  while (true) {
    get_password(password);
    passed = check_password(password, expected) ? 1 : 0;
    if (passed) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
