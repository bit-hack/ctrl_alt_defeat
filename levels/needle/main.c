// password "growth"

#include <stdint.h>
#include <stdbool.h>

#include "../common.h"

const char *words[] = {
  "chosen",
  "season",
  "silent",
  "agenda",
  "switch",
  "domain",
  "extend",
  "factor",
  "combat",
  "resort",
  "option",
  "define",
  "ruling",
  "former",
  "growth",
  "window",
  "agenda",
  "bottle",
  "except",
  "castle",
  "survey",
  "origin",
  "saying",
  "timely",
  "hardly",
  "income",
  "retain",
  "doctor",
  "latter",
  "danger"};

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
  const char *x = words[14];
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
