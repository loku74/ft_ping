#include "../includes/ft_ping.h"

void ping_loop(ping_data_t *ping_data) {

  printf("%s %s (%s): %zd data bytes\n", NAME, ping_data->hostname,
         ping_data->ip_str, sizeof(ping_data->icmp_message.data));

  uint16_t sequence = 0;
  while (1) {
    ping_data->icmp_message.header.un.echo.sequence = htons(sequence);
    ping_data->icmp_message.header.checksum = 0;
    ping_data->icmp_message.header.checksum =
        get_checksum(&ping_data->icmp_message, sizeof(ping_data->icmp_message));

    ssize_t sent_bytes =
        sendto(ping_data->sockfd, &ping_data->icmp_message, PING_PACKET_SIZE, 0,
               ping_data->res->ai_addr, ping_data->res->ai_addrlen);
    if (sent_bytes < 0) {
      perror("sendto");
      freeaddrinfo(ping_data->res);
      exit(EXIT_FAILURE);
    }

    printf("%zd bytes from %s: icmp_seq=%u\n", sent_bytes, ping_data->ip_str,
           sequence);
    sleep(1);
    sequence++;
  }

  freeaddrinfo(ping_data->res);
}
