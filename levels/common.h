#pragma once

enum {
  syscall_input,
  syscall_output,
  syscall_unlock
};

#define NO_INLINE __attribute__ ((noinline))

inline void syscall1(long n, volatile void* _a0) {
  register volatile void* a0 asm("a0") = _a0;
  register long syscall_id asm("a7") = n;
  asm volatile ("scall"
		: : "r"(a0), "r"(syscall_id));
}

inline void syscall2(long n, volatile void* _a0, volatile void* _a1) {
  register volatile void* a0 asm("a0") = _a0;
  register volatile void* a1 asm("a1") = _a1;
  register long syscall_id asm("a7") = n;
  asm volatile ("scall"
		: : "r"(a0), "r"(a1), "r"(syscall_id));
}

void NO_INLINE get_password(volatile char *dst, uint32_t max) {
  syscall2(syscall_input, dst, &max);
}

void NO_INLINE bad_password() {
  syscall1(syscall_output, "Wrong password!");
}

void NO_INLINE unlock() {
  syscall1(syscall_unlock, 0);
}
