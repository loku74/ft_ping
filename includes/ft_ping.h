#ifndef FT_PING_H
#define FT_PING_H

/* ========================================================================== */
/*                                 INCLUDES                                   */
/* ========================================================================== */

/* Standard C Libraries */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

/* System & Network Libraries */
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/types.h>

/* ========================================================================== */
/*                                  MACROS                                    */
/* ========================================================================== */

#define PING_PACKET_SIZE 64
#define PING_DATA_FILL 67

/* ========================================================================== */
/*                            STRUCT DEFINITIONS                              */
/* ========================================================================== */

typedef struct {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;

} icmp_header_t;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

/* [checksum.c] */
uint16_t get_checksum(const void *ptr, size_t count);

#endif
