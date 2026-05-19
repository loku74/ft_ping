#include "../includes/ft_ping.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%s: missing host operand\n", argv[0]);
    return 1;
  }

  ping_data_t ping_data = {0};

  ping_data.hostname = argv[1];

  ping_data.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (ping_data.sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  set_icmp_message(&ping_data.icmp_message);
  ping_data.res = get_host_info(ping_data.hostname);
  inet_ntop(AF_INET, &((struct sockaddr_in *)ping_data.res->ai_addr)->sin_addr,
            ping_data.ip_str, sizeof(ping_data.ip_str));

  ping_loop(&ping_data);
  freeaddrinfo(ping_data.res);

  return EXIT_SUCCESS;
}
