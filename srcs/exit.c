#include "../includes/ft_ping.h"

void exit_ping(ping_data_t *ping_data, const char *message) {
  perror(message);
  freeaddrinfo(ping_data->res);
  close(ping_data->sockfd);
  exit(EXIT_FAILURE);
}
