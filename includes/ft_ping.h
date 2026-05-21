#ifndef FT_PING_H
#define FT_PING_H

// libc
#include <math.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

// System & Network Libraries */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define PING_PACKET_SIZE 64
#define PING_DATA_FILL 42

extern char *NAME;
extern int8_t verbose;

// STRUCT DEFINITIONS

typedef struct {
  struct icmphdr header;
  char payload[PING_PACKET_SIZE - sizeof(struct icmphdr)];
} icmp_message_t;

typedef struct {
  struct iphdr *ip_header;
  struct icmphdr *icmp_header;
  struct sockaddr_in addr;
  uint32_t ip_header_size;
  char buffer[1024];
  ssize_t recv_bytes;
} ping_reply_t;

typedef struct {
  uint16_t sequence;
  uint16_t received;
  uint16_t sent;
  double min;
  double max;
  double total_rtt;
  double total_rtt_squared;
  double stddev;
} ping_stats_t;

typedef struct {
  int sockfd;
  icmp_message_t icmp_message;
  char ip_str[INET_ADDRSTRLEN];
  char *hostname;
  struct addrinfo *res;
  uint16_t sequence;
  ping_stats_t stats;
  ping_reply_t reply;
} ping_data_t;

// FUNCTION PROTOTYPES

/* [checksum.c] */
uint16_t get_checksum(const void *ptr, size_t count);

/* [icmp.c] */
void set_icmp_message(icmp_message_t *icmp_message);

/* [host.c] */
struct addrinfo *get_host_info(const char *hostname);

/* [loop.c] */
void ping_loop(ping_data_t *ping_data);

/* [exit.c] */
void exit_ping(ping_data_t *ping_data, const char *message);

/* [utils.c] */
void to_upper(char *str);

/* [stats.c] */
void set_stddev(ping_data_t *ping_data);
void display_stats(const ping_stats_t *ping_stats, const char *hostname);
void set_ping_rtt_stats(ping_data_t *ping_data, double rtt);

/* [replies.c] */
void ping_reply(ping_data_t *ping_data);
void ping_reply_ttl_expired(ping_data_t *ping_data);

/* [args.c] */
void parse_args(int argc, char *argv[]);

#endif
