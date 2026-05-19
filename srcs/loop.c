#include "../includes/ft_ping.h"

static ssize_t send_ping(ping_data_t *ping_data) {
  static uint16_t sequence = 0;
  struct timeval timestamp = {0};

  gettimeofday(&timestamp, NULL);
  memcpy(ping_data->icmp_message.payload, &timestamp, sizeof(timestamp));

  ping_data->icmp_message.header.un.echo.sequence = htons(sequence);
  ping_data->icmp_message.header.checksum = 0;
  ping_data->icmp_message.header.checksum =
      get_checksum(&ping_data->icmp_message, sizeof(ping_data->icmp_message));

  ssize_t sent_bytes =
      sendto(ping_data->sockfd, &ping_data->icmp_message, PING_PACKET_SIZE, 0,
             ping_data->res->ai_addr, ping_data->res->ai_addrlen);
  if (sent_bytes < 0)
    exit_ping(ping_data, "sendto");

  sequence++;

  return sent_bytes;
}

static void handle_reply(ping_data_t *ping_data, char *buffer, ssize_t bytes) {
  struct iphdr *ip = (struct iphdr *)buffer;
  size_t ip_header_size = ip->ihl * 4;
  struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + ip_header_size);

  if (icmp_reply->type == ICMP_ECHOREPLY &&
      icmp_reply->un.echo.id == ping_data->icmp_message.header.un.echo.id) {

    struct timeval *timestamp_sent, timestamp_received;
    gettimeofday(&timestamp_received, NULL);

    timestamp_sent =
        (struct timeval *)(buffer + ip_header_size + sizeof(struct icmphdr));
    size_t diff =
        (timestamp_received.tv_sec - timestamp_sent->tv_sec) * 1000000 +
        (timestamp_received.tv_usec - timestamp_sent->tv_usec);

    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n",
           bytes - ip_header_size, ping_data->ip_str,
           ntohs(icmp_reply->un.echo.sequence), ip->ttl, diff / 1000.0);
  }
}

static void receive_ping(ping_data_t *ping_data) {
  char recv_buffer[1024];
  struct sockaddr_in recv_addr;
  socklen_t recv_addr_len = sizeof(recv_addr);

  ssize_t recv_bytes =
      recvfrom(ping_data->sockfd, recv_buffer, sizeof(recv_buffer), 0,
               (struct sockaddr *)&recv_addr, &recv_addr_len);

  if (recv_bytes < 0)
    exit_ping(ping_data, "recvfrom");

  handle_reply(ping_data, recv_buffer, recv_bytes);
}

volatile int stop = 0;

void signal_handler(int sig) {
  (void)sig;
  stop = 1;
}

void ping_loop(ping_data_t *ping_data) {
  fd_set readfds;
  uint16_t packet_sent, packet_received;
  struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

  signal(SIGINT, signal_handler);

  printf("%s %s (%s): %zd data bytes\n", NAME_UPPER, ping_data->hostname,
         ping_data->ip_str, sizeof(ping_data->icmp_message.payload));

  send_ping(ping_data);

  packet_sent = packet_received = 0;
  while (!stop) {
    FD_ZERO(&readfds);
    FD_SET(ping_data->sockfd, &readfds);

    int select_result =
        select(ping_data->sockfd + 1, &readfds, NULL, NULL, &timeout);

    if (select_result < 0 && errno != EINTR)
      exit_ping(ping_data, "select");
    else if (select_result > 0) {
      receive_ping(ping_data);
      packet_received++;
    } else {
      timeout = (struct timeval){.tv_sec = 1, .tv_usec = 0};
      send_ping(ping_data);
      packet_sent++;
    }
  }

  printf("--- %s %s statistics ---\n", ping_data->hostname, NAME);
  printf("%u packets transmitted, %u packets received, %u%% packet loss\n",
         packet_sent, packet_received,
         (packet_sent - packet_received) * 100 / packet_sent);
}
