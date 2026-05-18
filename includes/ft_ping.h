#ifndef FT_PING_H
#define FT_PING_H

#define NAME "ft_ping"

/* ========================================================================== */
/*                                 INCLUDES                                   */
/* ========================================================================== */

/* Standard C Libraries */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

/* System & Network Libraries */
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

/* ========================================================================== */
/*                                  MACROS                                    */
/* ========================================================================== */

#define PING_PACKET_SIZE 64
#define PING_DATA_FILL 42

/* ========================================================================== */
/*                            STRUCT DEFINITIONS                              */
/* ========================================================================== */

typedef struct {
  struct icmphdr header;
  char data[PING_PACKET_SIZE - sizeof(struct icmphdr)];
} icmp_message_t;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

/* [checksum.c] */
uint16_t get_checksum(const void *ptr, size_t count);

/* [icmp.c] */
void set_icmp_message(icmp_message_t *icmp_message);

#endif
