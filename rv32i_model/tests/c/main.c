#if 0
static inline void finish() {
  asm volatile ("ecall\n\t");
}

int main() {
  volatile unsigned char *leds = (unsigned char*)0x80000000;
  *leds = 0;
  for (int i=0; ; ++i) {
    *leds += 1;
  }
  finish();
}
#else
#include <stdint.h>

uint32_t x = 0xcafebabe;

int main() {
  volatile uint8_t *leds = (uint8_t*)0x80000000;
  
  for (;;) {
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
    *leds = x & 0xff;
  }
}
#endif
