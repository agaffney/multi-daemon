#include "config.h"

#include "main.h"
#include "test.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ "debug", no_argument, NULL, 'd' },
	{ "config", required_argument, NULL, 'c' },
	{ "pidfile", required_argument, NULL, 'p' },
	{ NULL, 0, NULL, 0 }
};

static const struct dispatch_table_entry dispatch_table[] = {
	{ "test", test_entry },
	{ NULL, NULL }
};

static const char help_string[] =
	"usage: " PROJECT_BINARY_NAME " [OPTIONS] <service>\n\n"
	"-h, --help       display this help and exit\n"
	"-v, --version    display version information and exit\n"
	"-d, --debug      don't fork into the background\n"
	"-c, --config     the server type to start\n"
	"-p, --pidfile    the port to run the server on, if not the default\n";

void usage()
{
	printf(help_string);
}

int main(int argc, char *argv[])
{
	int optc;
	pid_t child_pid;
	int debug = 0;
	char service[20] = "";
	char pidfile[255] = "";
	char configfile[255] = "";
	type_server_opt server_opts[255];

	while ((optc = getopt_long(argc, argv, "hvdc:p:", longopts, NULL)) != -1) {
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
		case 'c':
			strncpy(configfile, optarg, sizeof(configfile));
			configfile[sizeof(configfile)-1] = 0;
			break;
		case 'p':
			strncpy(pidfile, optarg, sizeof(pidfile));
			pidfile[sizeof(pidfile)-1] = 0;
			break;
		case '?':
			printf("\n");
			usage();
			return 1;
		}
	}

	if (optind < argc) {
		strncpy(service, argv[optind], sizeof(service));
		service[sizeof(service)-1] = 0;
		optind++;
		// Parse remaining arguments as key/value pairs
		int server_opts_idx = 0;
		for(;optind < argc;optind++)
		{
			char *firstpart = strtok(argv[optind], "=");
			if (firstpart == NULL)
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", argv[optind]);
				usage();
				return 1;
			}
			strncpy(server_opts[server_opts_idx].name, firstpart, sizeof(server_opts[server_opts_idx].name));
			server_opts[server_opts_idx].name[sizeof(server_opts[server_opts_idx].name)-1] = 0;
			char *secondpart = strtok(NULL, "");
			if (secondpart == NULL)
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", argv[optind]);
				usage();
				return 1;
			}
			strncpy(server_opts[server_opts_idx].value, secondpart, sizeof(server_opts[server_opts_idx].value));
			server_opts[server_opts_idx].value[sizeof(server_opts[server_opts_idx].value)-1] = 0;
			server_opts_idx++;
		}
		server_opts[server_opts_idx].name[0] = 0;
		server_opts[server_opts_idx].value[0] = 0;
	}

	// Lookup the function to call in the dispatch table
	int i = 0;
	while(1)
	{
		if (dispatch_table[i].service == NULL)
		{
			fprintf(stderr, "Invalid service type: %s\n\n", service);
			usage();
			return 1;
		}
		if (!strcmp(service, dispatch_table[i].service))
		{
			if (!debug)
			{
				child_pid = fork();
				if (child_pid > 0)
				{
					// Parent
					return 0;
				}
			}
			return (*dispatch_table[i].func)(server_opts);
		}
		i++;
	}
}
