#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ NULL, 0, NULL, 0 }
};

static const char help_string[] =
        "usage: " PROJECT_BINARY_NAME " [OPTION]...\n\n"
        "-h, --help          display this help and exit\n"
        "-v, --version       display version information and exit\n";

void usage()
{
	printf(help_string);
}

int main(int argc, char *argv[])
{
	int optc;
	int invarg = 0;

	while ((optc = getopt_long(argc, argv, "hv", longopts, NULL)) != -1) {
		switch (optc){
		case 'h':
			usage();
			return 0;
		case 'v':
			printf("%s %s\n", PROJECT_NAME, PROJECT_VERSION);
			return 0;
		case '?':
			printf("\n");
			usage();
			return 1;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "%s: extra operand: %s\n\n",
			argv[0], argv[optind]);
		usage();
		return 1;
	}

	return 0;
}
