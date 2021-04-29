#include <stdint.h>
#include <stdbool.h>

#include "../common.h"

bool compare(const char *input, int max) {
  const char password[] = { 'm'-'a', 'o'-'a', 'n'-'a', 'k'-'a', 'e'-'a', 'y'-'a' };
  for (int i=0; i<6; ++i) {
    if (input[i] != (password[i]+'a')) {
      return false;
    }
  }
  return true;
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
