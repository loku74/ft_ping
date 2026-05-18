#include "../includes/ft_ping.h"

#define TARGET_IP "127.0.0.1"

int main() {
  int sockfd;
  icmp_header_t icmp_header = {.type = ICMP_ECHO, .code = 0, .checksum = 0};

  char icmp_message[PING_PACKET_SIZE];
  bzero(icmp_message, sizeof(icmp_message));

  memcpy(icmp_message, &icmp_header, sizeof(icmp_header));
  memset(icmp_message + sizeof(icmp_header), PING_DATA_FILL,
         PING_PACKET_SIZE - sizeof(icmp_header));

  icmp_header.checksum = get_checksum(icmp_message, PING_PACKET_SIZE);

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

  ssize_t sent_bytes = sendto(sockfd, icmp_message, sizeof(icmp_message), 0,
                              (struct sockaddr *)&dest, sizeof(dest));
  if (sent_bytes < 0) {
    perror("sendto");
    return 1;
  }

  printf("Sent %zd bytes\n", sent_bytes);

  return 0;
}
