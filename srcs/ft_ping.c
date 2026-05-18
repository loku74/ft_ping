#include "../includes/ft_ping.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%s: missing host operand\n", NAME);
    return 1;
  }

  char *hostname = argv[1];
  int sockfd;
  icmp_message_t icmp_message;
  struct addrinfo *res;
  char ip_str[INET_ADDRSTRLEN];
  uint16_t sequence;

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  set_icmp_message(&icmp_message);
  res = get_host_info(hostname);
  inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ip_str,
            sizeof(ip_str));

  printf("%s %s (%s): %zd data bytes\n", NAME, hostname, ip_str,
         sizeof(icmp_message.data));

  sequence = 0;
  while (1) {
    icmp_message.header.un.echo.sequence = htons(sequence);
    icmp_message.header.checksum = 0;
    icmp_message.header.checksum =
        get_checksum(&icmp_message, sizeof(icmp_message));

    ssize_t sent_bytes = sendto(sockfd, &icmp_message, PING_PACKET_SIZE, 0,
                                res->ai_addr, res->ai_addrlen);

    printf("%zd bytes from %s: icmp_seq=%u\n", sent_bytes, ip_str, sequence);
    sleep(1);
    sequence++;
    if (sent_bytes < 0) {
      perror("sendto");
      freeaddrinfo(res);
      return EXIT_FAILURE;
    }
  }

  freeaddrinfo(res);

  return EXIT_SUCCESS;
}
