#include <stdint.h>
#include <string.h>

uint16_t get_checksum(const void *ptr, size_t count) {
  uint32_t sum = 0;
  const uint16_t *char_ptr = ptr;

  while (count > 1) {
    sum += *char_ptr++;
    count -= 2;
  }

  if (count > 0) {
    sum += *(uint8_t *)char_ptr;
  }

  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return ~sum;
}
