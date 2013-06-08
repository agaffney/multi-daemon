#include "config.h"
#include "main.h"
#include "test.h"
#include "echo.h"

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

static const dispatch_table_entry dispatch_table[] = {
	{ "test", test_entry },
	{ "echo", echo_entry },
	{ NULL, NULL }
};

static const char help_string[] =
	"usage: multi-daemon [OPTIONS] <service>\n\n"
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
	int optc, i;
	pid_t child_pid;
	int debug = 0;
	char service[20] = "";
	char pidfile[255] = "";
	char configfile[255] = "";
	config_opt config_opts[255];
	int config_opts_idx;

	while ((optc = getopt_long(argc, argv, "hvdc:p:", longopts, NULL)) != -1) {
		switch (optc){
		case 'h':
			usage();
			return 0;
		case 'v':
			printf("%s %s\n", "multi-daemon", "0.1");
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

	// Parse the config file, if specified
	if(!strcmp(configfile, ""))
	{
		config_opts_idx = parse_config_file(configfile, service, config_opts, sizeof(config_opts));
	}

	if (optind < argc) {
		strncpy(service, argv[optind], sizeof(service));
		service[sizeof(service)-1] = 0;
		optind++;
		// Parse remaining arguments as key/value pairs
		for(;optind < argc;optind++)
		{
			char *firstpart = strtok(argv[optind], "=");
			if (firstpart == NULL)
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", argv[optind]);
				usage();
				return 1;
			}
			strncpy(config_opts[config_opts_idx].name, firstpart, sizeof(config_opts[config_opts_idx].name));
			config_opts[config_opts_idx].name[sizeof(config_opts[config_opts_idx].name)-1] = 0;
			char *secondpart = strtok(NULL, "");
			if (secondpart == NULL)
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", argv[optind]);
				usage();
				return 1;
			}
			strncpy(config_opts[config_opts_idx].value, secondpart, sizeof(config_opts[config_opts_idx].value));
			config_opts[config_opts_idx].value[sizeof(config_opts[config_opts_idx].value)-1] = 0;
			config_opts_idx++;
		}
		config_opts[config_opts_idx].name[0] = 0;
		config_opts[config_opts_idx].value[0] = 0;
	}

	// Lookup the function to call in the dispatch table
	for(i=0;;i++)
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
			return (*dispatch_table[i].func)(config_opts);
		}
	}
}
