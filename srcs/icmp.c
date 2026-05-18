#include "../includes/ft_ping.h"

void set_icmp_message(icmp_message_t *icmp_message) {
  bzero(icmp_message, sizeof(icmp_message_t));

  icmp_message->header.type = ICMP_ECHO;
  icmp_message->header.code = 0;
  icmp_message->header.checksum = 0;
  icmp_message->header.un.echo.id = getpid();
  icmp_message->header.un.echo.sequence = 0;

  memset(&icmp_message->data, PING_DATA_FILL, sizeof(icmp_message->data));

  icmp_message->header.checksum = get_checksum(icmp_message, sizeof(icmp_message_t));
}
