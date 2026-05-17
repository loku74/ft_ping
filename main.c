#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#define TARGET_IP "127.0.0.1"

typedef struct {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;

} icmp_header_t;

uint16_t get_checksum(const void *addr, size_t count) {
  uint32_t sum = 0;
  const uint16_t *char_addr = addr;

  while (count > 1) {
    sum += *char_addr++;
    count -= 2;
  }

  if (count > 0) {
    sum += *(uint8_t *)char_addr;
  }

  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return ~sum;
}

int main() {
  int sockfd;
  icmp_header_t icmp_header = {.type = ICMP_ECHO, .code = 0, .checksum = 0};

  char icmp_message[64];
  bzero(icmp_message, sizeof(icmp_message));
  char text[32] = "test";

  memcpy(icmp_message, &icmp_header, sizeof(icmp_header));
  memcpy(icmp_message + sizeof(icmp_header), text, strlen(text));

  uint32_t checksum = get_checksum(icmp_message, sizeof(icmp_header) + strlen(text));

  icmp_header.checksum = checksum;

  memcpy(icmp_message, &icmp_header, sizeof(icmp_header));

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  //
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  inet_pton(AF_INET, TARGET_IP, &dest.sin_addr);

  ssize_t sent_bytes = sendto(sockfd, icmp_message, sizeof(icmp_header) + strlen(text), 0, (struct sockaddr *)&dest, sizeof(dest));
  if (sent_bytes < 0) {
    perror("sendto");
    return 1;
  }

  printf("Sent %zd bytes\n", sent_bytes);

  return 0;
}
