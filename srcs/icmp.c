#include "../includes/ft_ping.h"

size_t set_icmp_message(char **icmp_message) {
  icmp_header_t icmp_header = {.type = ICMP_ECHO, .code = 0, .checksum = 0};

  *icmp_message = malloc(PING_PACKET_SIZE);
  if (icmp_message == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  bzero(*icmp_message, PING_PACKET_SIZE);

  memcpy(*icmp_message, &icmp_header, sizeof(icmp_header));
  memset(*icmp_message + sizeof(icmp_header), PING_DATA_FILL,
         PING_PACKET_SIZE - sizeof(icmp_header));

  icmp_header.checksum = get_checksum(*icmp_message, PING_PACKET_SIZE);

  memcpy(*icmp_message, &icmp_header, sizeof(icmp_header));

  return PING_PACKET_SIZE;
}
