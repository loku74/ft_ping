#include "../includes/ft_ping.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%s: missing host operand\n", NAME);
    return 1;
  }

  char *hostname = argv[1];

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  icmp_message_t icmp_message;
  set_icmp_message(&icmp_message);

  struct addrinfo hints;
  struct addrinfo *res;

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  int status = getaddrinfo(hostname, NULL, &hints, &res);
  if (status != 0) {
    fprintf(stderr, "%s: unknown host\n", NAME);
    return EXIT_FAILURE;
  }

  char ip_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ip_str,
            sizeof(ip_str));

  printf("%s %s (%s): %zd data bytes\n", NAME, hostname, ip_str,
         sizeof(icmp_message.data));

  while (1) {
    ssize_t sent_bytes = sendto(sockfd, &icmp_message, PING_PACKET_SIZE, 0,
                                res->ai_addr, res->ai_addrlen);
    printf("%zd bytes from %s: icmp_seq=%u\n", sent_bytes, ip_str,
           icmp_message.header.un.echo.sequence);
    sleep(1);
    icmp_message.header.un.echo.sequence++;
    if (sent_bytes < 0) {
      perror("sendto");
      freeaddrinfo(res);
      return EXIT_FAILURE;
    }
  }

  freeaddrinfo(res);

  return EXIT_SUCCESS;
}
