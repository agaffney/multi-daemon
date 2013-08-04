#include "hash.h"

#include <getopt.h>

#define INIT_MAX_OPTS 255

struct _cmdline_opt {
	struct option opt;
	char helpstring[150];
};

typedef struct _cmdline_opt cmdline_opt;

int init_parse_config_error(char *, char *, int);
int init_parse_config_file(char *, char *, Hash *);
int init_parse_config_line(char *, Hash *);
int init_parse_commandline(cmdline_opt * opts, int argc, char **argv, void (*callback)(void *, int, char *, cmdline_opt *), void * callback_arg);
void init_usage(char *, cmdline_opt *);

