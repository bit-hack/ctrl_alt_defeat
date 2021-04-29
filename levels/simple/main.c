// password "bovine"

#include <stdint.h>
#include <stdbool.h>

#include "../common.h"

bool compare(const char *a, const char *b) {
  for (int i=0; i<6; ++i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

int main() {
  char pass[8];
  const char *x = "bovine";
  for (;;) {
    get_password(pass, sizeof(pass));
    if (compare(pass, x)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
