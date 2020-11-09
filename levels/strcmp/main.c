#include <stdbool.h>
#include <stdint.h>

#define NO_INLINE __attribute__ ((noinline))

char temp[16] = "unknown";
char ref[] = "Rosebud";

enum {
  syscall_input,
  syscall_output,
  syscall_unlock
};

inline void syscall(long n, long _a0) {
  register long a0 asm("a0") = _a0;
  register long syscall_id asm("a7") = n;
  asm volatile ("scall"
		: : "r"(a0), "r"(syscall_id));
}

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

void NO_INLINE get_password() {
  syscall(syscall_input, (long)temp);
}

void NO_INLINE bad_password() {
  syscall(syscall_output, (long)"Wrong password!");
}

void NO_INLINE unlock() {
  syscall(syscall_unlock, 0);
}

int main() {
  while (true) {
    get_password();
    if (compare(temp, ref)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
