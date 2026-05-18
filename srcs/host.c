#include "../includes/ft_ping.h"

struct addrinfo *get_host_info(const char *hostname) {
  struct addrinfo hints;
  struct addrinfo *res;

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  int status = getaddrinfo(hostname, NULL, &hints, &res);
  if (status != 0) {
    fprintf(stderr, "%s: unknown host\n", NAME);
    exit(EXIT_FAILURE);
  }

  return res;
}
