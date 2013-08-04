#include "main.h"
#include "init.h"
#include "echo/echo.h"
#include "http/http.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const dispatch_table_entry dispatch_table[] = {
	{ "echo", echo_entry },
	{ "http", http_entry },
	{ NULL, NULL }
};

int main(int argc, char *argv[])
{
	int i;
	pid_t child_pid;
	char service[20] = "";

	// Disable output buffering
	setbuf(stdout, NULL);

	Hash * config_opts = Hash_init();
	init_parse_commandline(config_opts, NULL, NULL, argc, argv);

/*
	while ((optc = getopt_long(argc, argv, "hvdc:p:o:", longopts, NULL)) != -1) {
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
		case 'o':
			if(!init_parse_config_line(optarg, config_opts))
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", optarg);
				usage();
			}
			break;
		case '?':
			printf("\n");
			usage();
			return 1;
		}
	}
*/

	// Grab service from arguments
	if (optind < argc) {
		strncpy(service, argv[optind], sizeof(service));
		service[sizeof(service)-1] = 0;
		optind++;
	}

	// Parse the config file, if specified
	if (config_opts->has_key(config_opts, "config"))
	{
		if (init_parse_config_file(config_opts->get(config_opts, "config"), service, config_opts) <= 0)
		{
			// error parsing config
			return 1;
		}
		config_opts->unset(config_opts, "config");
	}

	// Lookup the function to call in the dispatch table
	for(i=0;;i++)
	{
		if (dispatch_table[i].service == NULL)
		{
			fprintf(stderr, "Invalid service type: %s\n\n", service);
			//usage();
			return 1;
		}
		if (!strcmp(service, dispatch_table[i].service))
		{
			if (!config_opts->has_key(config_opts, "debug"))
			{
				child_pid = fork();
				if (child_pid > 0)
				{
					// Parent
					return 0;
				}
			}
			config_opts->unset(config_opts, "debug");
			return (*dispatch_table[i].func)(config_opts);
		}
	}
}
