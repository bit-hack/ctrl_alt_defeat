#include <stdint.h>
#include <stdbool.h>

#include "../common.h"

bool compare(const char *input, int max) {
  return false;
}

void NO_INLINE loop() {
  char input[8];
  get_password(input, sizeof(input));
  if (compare(input, sizeof(input))) {
    unlock();
  }
  else {
    bad_password();
  }
}

int main() {
  for (;;) {
    loop();
  }
}
