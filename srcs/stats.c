#include "../includes/ft_ping.h"

void set_ping_rtt_stats(ping_data_t *ping_data, double rtt) {
  if (ping_data->stats.min == 0 || rtt < ping_data->stats.min)
    ping_data->stats.min = rtt;
  if (rtt > ping_data->stats.max)
    ping_data->stats.max = rtt;

  ping_data->stats.total_rtt += rtt;
  ping_data->stats.total_rtt_squared += rtt * rtt;
}

void set_stddev(ping_data_t *ping_data) {
  ping_data->stats.stddev =
      sqrt(ping_data->stats.total_rtt_squared / ping_data->stats.received -
           pow(ping_data->stats.total_rtt / ping_data->stats.received, 2));
}

void display_stats(const ping_stats_t *ping_stats, const char *hostname) {
  printf("--- %s ft_ping statistics ---\n", hostname);
  printf("%u packets transmitted, %u packets received, %u%% packet loss\n",
         ping_stats->sent, ping_stats->received,
         (ping_stats->sent - ping_stats->received) * 100 / ping_stats->sent);
  if (ping_stats->received) {
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           ping_stats->min, ping_stats->total_rtt / ping_stats->received,
           ping_stats->max, ping_stats->stddev);
  }
}
