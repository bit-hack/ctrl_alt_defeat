#include <stdbool.h>
#include <stdint.h>

#define NO_INLINE __attribute__ ((noinline))

enum {
  syscall_input,
  syscall_output,
  syscall_unlock
};

inline void syscall(long n, long _a0, long _a1=0) {
  register long a0 asm("a0") = _a0;
  register long a1 asm("a1") = _a1;
  register long syscall_id asm("a7") = n;
  asm volatile ("scall"
		: : "r"(a0), "r"(a1), "r"(syscall_id));
}

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

void NO_INLINE get_password(char *dst) {
  syscall(syscall_input, (long)dst);
}

void NO_INLINE bad_password() {
  syscall(syscall_output, (long)"Wrong password!");
}

void NO_INLINE unlock() {
  syscall(syscall_unlock, 0);
}

int main() {
  char temp[16];
  while (true) {
    get_password(temp);
    if (compare(temp)) {
      unlock();
    }
    else {
      bad_password();
    }
  }
}
