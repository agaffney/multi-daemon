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

void cmdline_callback(void * cbarg, int optc, char * optarg, cmdline_opt * opts)
{
	Hash * config = (Hash *) cbarg;
	switch (optc)
	{
		case '?':
			// Invalid option
			init_usage("program", opts);
			exit(1);
		case 'h':
			init_usage("program", opts);
			exit(0);
		case 'v':
			printf("%s %s\n", "multi-daemon", "0.1");
			exit(0);
		case 'd':
			config->set(config, "debug", "1");
			break;
		case 'c':
			config->set(config, "configfile", optarg);
			break;
		case 'p':
			config->set(config, "pidfile", optarg);
			break;
		case 'o':
			if(!init_parse_config_line(optarg, config))
			{
				fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", optarg);
				init_usage("program", opts);
				exit(1);
			}
			break;
	}
}

int main(int argc, char *argv[])
{
	int i;
	pid_t child_pid;
	char service[20] = "";

	// Disable output buffering
	setbuf(stdout, NULL);

	Hash * config = Hash_init();

	cmdline_opt opts[INIT_MAX_OPTS] = {
		{ { "help", no_argument, NULL, 'h' }, "display usage information and exit" },
		{ { "version", no_argument, NULL, 'v' }, "display version information and exit" },
		{ { "debug", no_argument, NULL, 'd' }, "don't fork into the background" },
		{ { "config", required_argument, NULL, 'c' }, "specifies path to config file" },
		{ { "pidfile", required_argument, NULL, 'p' }, "specifies path to PID file" },
		{ { "option", required_argument, NULL, 'o' }, "specifies config option in key=value format" },
		{ { NULL, 0, NULL, 0 }, "" },
	};

	init_parse_commandline(opts, argc, argv, cmdline_callback, (void *) config);

	// Grab service from arguments
	if (optind < argc) {
		strncpy(service, argv[optind], sizeof(service));
		service[sizeof(service)-1] = 0;
		optind++;
	}

	// Parse the config file, if specified
	if (config->has_key(config, "configfile"))
	{
		if (init_parse_config_file(config->get(config, "configfile"), service, config) <= 0)
		{
			// error parsing config
			return 1;
		}
		config->unset(config, "configfile");
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
			if (!config->has_key(config, "debug"))
			{
				child_pid = fork();
				if (child_pid > 0)
				{
					// Parent
					return 0;
				}
			}
			config->unset(config, "debug");
			return (*dispatch_table[i].func)(config);
		}
	}
}
