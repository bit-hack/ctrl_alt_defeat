#include <stdbool.h>
#include <stdint.h>

#define NO_INLINE __attribute__ ((noinline))

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

bool NO_INLINE check_password(char *a) {
  uint32_t hash = 0;
  for (;; ++a) {
    if (*a == '\0') {
      return hash == 0xc001babe;
    }
    hash += *a;
  }
}

void NO_INLINE get_password(char *dst) {
  syscall(syscall_input, (long)dst);
}

void NO_INLINE bad_password() {
  syscall(syscall_output, (long)"Wrong password!");
}

void NO_INLINE unlock() {
  syscall(syscall_unlock, 0);
}

volatile int32_t passed = 0;
volatile char password[16];

int main() {
  while (true) {
    get_password(password);
    passed |= check_password(password) ? 1 : 0;
    if (passed) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
