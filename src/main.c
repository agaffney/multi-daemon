#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ "debug", no_argument, NULL, 'd' },
	{ "type", required_argument, NULL, 't' },
	{ "port", required_argument, NULL, 'p' },
	{ NULL, 0, NULL, 0 }
};

static const char help_string[] =
	"usage: " PROJECT_BINARY_NAME " [OPTION]...\n\n"
	"-h, --help       display this help and exit\n"
	"-v, --version    display version information and exit\n"
	"-d, --debug      don't fork into the background\n"
	"-t, --type       the server type to start\n"
	"-p, --port       the port to run the server on, if not the default\n";

void usage()
{
	printf(help_string);
}

int main(int argc, char *argv[])
{
	int optc;
	int debug = 0;
	char servertype[20];
	int port = 0;

	while ((optc = getopt_long(argc, argv, "hvdt:p:", longopts, NULL)) != -1) {
		switch (optc){
		case 'h':
			usage();
			return 0;
		case 'v':
			printf("%s %s\n", PROJECT_NAME, PROJECT_VERSION);
			return 0;
		case 'd':
			debug = 1;
			break;
		case 't':
			strncpy(servertype, optarg, sizeof(servertype));
			servertype[sizeof(servertype)-1] = 0;
			break;
		case 'p':
			port = atoi(optarg);
			if (port <= 0 || port >= 65535)
			{
				fprintf(stderr, "Invalid port specification: %s\n\n", optarg);
				usage();
				return 1;
			}
			break;
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

	if (port == 0)
	{
		printf("Would start server of type %s on default port %s debug\n", servertype, (debug ? "with" : "without"));
	}
	else
	{
		printf("Would start server of type %s on port %d %s debug\n", servertype, port, (debug ? "with" : "without"));
	}

	return 0;
}
