#include "../includes/ft_ping.h"
#include <netinet/ip_icmp.h>
#include <sys/socket.h>

static ssize_t send_ping(ping_data_t *ping_data) {
  struct timeval timestamp = {0};

  gettimeofday(&timestamp, NULL);
  memcpy(ping_data->icmp_message.payload, &timestamp, sizeof(timestamp));

  ping_data->icmp_message.header.un.echo.sequence = htons(ping_data->sequence);
  ping_data->icmp_message.header.checksum = 0;
  ping_data->icmp_message.header.checksum =
      get_checksum(&ping_data->icmp_message, sizeof(ping_data->icmp_message));

  ssize_t sent_bytes =
      sendto(ping_data->sockfd, &ping_data->icmp_message, PING_PACKET_SIZE, 0,
             ping_data->res->ai_addr, ping_data->res->ai_addrlen);
  if (sent_bytes < 0)
    exit_ping(ping_data, "sendto");

  ping_data->stats.sent++;
  ping_data->sequence++;

  return sent_bytes;
}

static void handle_ping_reply(ping_data_t *ping_data, ssize_t bytes) {
  struct iphdr *ip = (struct iphdr *)ping_data->reply.buffer;
  size_t ip_header_size = ip->ihl * 4;
  struct icmphdr *icmp_reply =
      (struct icmphdr *)(ping_data->reply.buffer + ip_header_size);

  ping_data->reply.ip_header = ip;
  ping_data->reply.icmp_header = icmp_reply;
  ping_data->reply.ip_header_size = ip_header_size;
  ping_data->reply.recv_bytes = bytes;

  switch (icmp_reply->type) {
  case ICMP_ECHOREPLY:
    ping_reply(ping_data);
    break;
  case ICMP_TIMXCEED:
    ping_reply_ttl_expired(ping_data);
    break;
  default:
    break;
  }
}

static void receive_ping(ping_data_t *ping_data) {
  socklen_t recv_addr_len = sizeof(ping_data->reply.addr);

  ssize_t recv_bytes =
      recvfrom(ping_data->sockfd, ping_data->reply.buffer,
               sizeof(ping_data->reply.buffer), 0,
               (struct sockaddr *)&ping_data->reply.addr, &recv_addr_len);

  if (recv_bytes < 0)
    exit_ping(ping_data, "recvfrom");

  handle_ping_reply(ping_data, recv_bytes);
}

volatile int stop = 0;

void ping_signal_handler(int sig) {
  (void)sig;
  stop = 1;
}

void ping_loop(ping_data_t *ping_data) {
  fd_set readfds;
  struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

  signal(SIGINT, ping_signal_handler);

  printf("FT_PING %s (%s): %zd data bytes\n", ping_data->hostname,
         ping_data->ip_str, sizeof(ping_data->icmp_message.payload));

  send_ping(ping_data);

  while (!stop) {
    FD_ZERO(&readfds);
    FD_SET(ping_data->sockfd, &readfds);

    int select_result =
        select(ping_data->sockfd + 1, &readfds, NULL, NULL, &timeout);

    if (select_result < 0 && errno != EINTR)
      exit_ping(ping_data, "select");
    else if (select_result > 0) {
      receive_ping(ping_data);
    } else if (!stop) {
      timeout = (struct timeval){.tv_sec = 1, .tv_usec = 0};
      send_ping(ping_data);
    }
  }

  set_stddev(ping_data);
}
