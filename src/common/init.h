#include "hash.h"

#include <getopt.h>

#define INIT_MAX_OPTS 255

int init_parse_config_error(char *, char *, int);
int init_parse_config_file(char *, char *, Hash *);
int init_parse_config_line(char *, Hash *);
int init_parse_commandline(Hash *, struct option *, int, char **);
