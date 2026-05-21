#include "../includes/ft_ping.h"
#include <getopt.h>

int8_t verbose = 0;

#define VERBOSE_FLAG 'v'
#define VERBOSE_LONG "verbose"
#define HELP_FLAG '?'
#define HELP_LONG "help"

static void help_message() {
  printf("Usage: %s [OPTION...] DESTINATION\n", NAME);
  printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
  printf("Options valid for all request types:\n");
  printf("  -%c, --%s    Verbose output\n", VERBOSE_FLAG, VERBOSE_LONG);
}

static void error_message() {
  fprintf(stderr, "Try '%s -?' or '%s --help' for more information.\n", NAME,
          NAME);
}

void parse_args(int argc, char *argv[]) {
  int opt;
  int option_index = 0;

  static struct option long_options[] = {
      {VERBOSE_LONG, no_argument, 0, VERBOSE_FLAG},
      {HELP_LONG, no_argument, 0, HELP_FLAG},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "v?", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'v':
      verbose = 1;
      break;
    case '?':
      if (optopt != 0 && optopt != '?') {
        error_message();
        exit(1);
      }
      help_message();
      exit(0);
    default:
      exit(1);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "%s: missing host operand\n", NAME);
    error_message();
    exit(64);
  }
}
