#include "../includes/ft_ping.h"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>

static struct iphdr *print_ip_header(struct iphdr *ip, size_t size) {
  printf("IP Hdr Dump:\n ");

  uint16_t *ip_words = (uint16_t *)ip;

  int num_words = size / sizeof(uint16_t);

  for (int i = 0; i < num_words; i++) {
    printf("%04x", ntohs(ip_words[i]));

    if (i < num_words - 1) {
      printf(" ");
    }
  }
  printf("\n");

  printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
  printf(" %1x  %1x  %02x",
         ip->version, // version
         ip->ihl,     // ip header length
         ip->tos      // type of service
  );

  printf(" %04x %04x",
         (ip->tot_len > 0x2000) ? ntohs(ip->tot_len)
                                : ip->tot_len, // total ip header length
         ntohs(ip->id));                       // id)
  printf("   %1x %04x",
         (ntohs(ip->frag_off) & 0xe000) >> 13, // flag (top 3 bits)
         ntohs(ip->frag_off) & 0x1FFF);        // offset (bottom 13 bits))
  printf("  %02x  %02x %04x", ip->ttl,         // ttl
         ip->protocol,                         // protocol
         ntohs(ip->check));                    // checksum (cks))
  printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->saddr)); // source ip
  printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->daddr)); // destination ip
  printf("\n");

  return ip;
}

static void print_icmp_header(struct icmphdr *icmp, size_t size) {
  printf("ICMP: type %d, code %u, size %zd, id 0x%04x, sequence 0x%04x\n",
         icmp->type, icmp->code, size, ntohs(icmp->un.echo.id),
         ntohs(icmp->un.echo.sequence));
}

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

  if (verbose) {
    struct iphdr *ip = (struct iphdr *)(ping_data->reply.buffer +
                                        ping_data->reply.ip_header_size +
                                        sizeof(struct icmphdr));

    size_t ip_header_size = ip->ihl * 4;
    size_t icmp_header_size = ntohs(ip->tot_len) - ip_header_size;

    struct icmphdr *icmp = (struct icmphdr *)((char *)ip + ip_header_size);

    print_ip_header(ip, ip_header_size);
    print_icmp_header(icmp, icmp_header_size);
  }
}
