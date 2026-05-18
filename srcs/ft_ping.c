#include "../includes/ft_ping.h"

#define TARGET_IP "127.0.0.1"

int main() {
  int sockfd = get_socket();

  char *icmp_message;
  size_t icmp_message_size = set_icmp_message(&icmp_message);

  //
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  inet_pton(AF_INET, TARGET_IP, &dest.sin_addr);

  ssize_t sent_bytes = sendto(sockfd, icmp_message, icmp_message_size, 0,
                              (struct sockaddr *)&dest, sizeof(dest));
  if (sent_bytes < 0) {
    perror("sendto");
    return 1;
  }

  printf("Sent %zd bytes\n", sent_bytes);

  return 0;
}
