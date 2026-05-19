#ifndef FT_PING_H
#define FT_PING_H

// INCLUDES

// libc
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>

// System & Network Libraries */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

// MACROS

#define NAME "ft_ping"
#define NAME_UPPER "FT_PING"

#define PING_PACKET_SIZE 64
#define PING_DATA_FILL 42

// STRUCT DEFINITIONS

typedef struct {
  struct icmphdr header;
  char payload[PING_PACKET_SIZE - sizeof(struct icmphdr)];
} icmp_message_t;

typedef struct {
  int sockfd;
  icmp_message_t icmp_message;
  char ip_str[INET_ADDRSTRLEN];
  char *hostname;
  struct addrinfo *res;
  uint16_t sequence;
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

#endif
