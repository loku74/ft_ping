#include "../includes/ft_ping.h"

void set_icmp_message(icmp_message_t *icmp_message) {
  bzero(icmp_message, sizeof(icmp_message_t));

  icmp_message->header.type = ICMP_ECHO;
  icmp_message->header.code = 0;
  icmp_message->header.checksum = 0;
  icmp_message->header.un.echo.id = htons(getpid());
  icmp_message->header.un.echo.sequence = htons(0);

  memset(&icmp_message->payload, PING_DATA_FILL, sizeof(icmp_message->payload));
}
