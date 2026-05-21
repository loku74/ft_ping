#include "../includes/ft_ping.h"

void ping_reply(ping_data_t *ping_data) {
  if (ping_data->reply.icmp_header->un.echo.id ==
      ping_data->icmp_message.header.un.echo.id) {
    ping_data->stats.received++;

    struct timeval *timestamp_sent, timestamp_received;
    if (gettimeofday(&timestamp_received, NULL) != 0) {
      exit_ping(ping_data, "gettimeofday failed");
    }

    timestamp_sent = (struct timeval *)(ping_data->reply.buffer +
                                        ping_data->reply.ip_header_size +
                                        sizeof(struct icmphdr));

    size_t diff = (timestamp_received.tv_sec - timestamp_sent->tv_sec) * 10e6 +
                  (timestamp_received.tv_usec - timestamp_sent->tv_usec);

    double rtt_time = diff / 1000.;
    set_ping_rtt_stats(ping_data, rtt_time);

    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n",
           ping_data->reply.recv_bytes - ping_data->reply.ip_header_size,
           ping_data->ip_str,
           ntohs(ping_data->reply.icmp_header->un.echo.sequence),
           ping_data->reply.ip_header->ttl, rtt_time);
  }
}

void ping_reply_ttl_expired(ping_data_t *ping_data) {
  char host_buffer[NI_MAXHOST];
  char ip_str[INET_ADDRSTRLEN];
  ssize_t bytes = ping_data->reply.recv_bytes - ping_data->reply.ip_header_size;

  if (!inet_ntop(AF_INET, &ping_data->reply.addr.sin_addr, ip_str,
                 sizeof(ip_str))) {
    exit_ping(ping_data, "inet_ntop failed");
  }

  if (getnameinfo((struct sockaddr *)&ping_data->reply.addr,
                  sizeof(ping_data->reply.addr), host_buffer,
                  sizeof(host_buffer), NULL, 0, 0)) {
    exit_ping(ping_data, "getnameinfo failed");
  } else if (strcmp(host_buffer, ip_str) != 0) {
    printf("%zd bytes from %s (%s): Time to live exceeded\n", bytes,
           host_buffer, ip_str);
  } else {
    printf("%zd bytes from %s: Time to live exceeded\n", bytes, ip_str);
  }
}
