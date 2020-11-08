#include <stdint.h>

uint32_t x = 0xcafebabe;

int main() {
  volatile uint8_t *leds = (uint8_t*)0x1fff;
  
  for (;;) {
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
    *leds = x & 0xff;
  }
}
