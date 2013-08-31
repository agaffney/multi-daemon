#include "common/init.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
			config->set(config, "debug", "1", LIST_TYPE_STRING);
			break;
		case 'c':
			config->set(config, "configfile", optarg, LIST_TYPE_STRING);
			break;
		case 'p':
			config->set(config, "pidfile", optarg, LIST_TYPE_STRING);
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
	pid_t child_pid;

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

	// Parse the config file, if specified
	if (config->has_key(config, "configfile"))
	{
		if (init_parse_config_file(config->get(config, "configfile"), "http", config) <= 0)
		{
			// error parsing config
			return 1;
		}
		config->unset(config, "configfile");
	}
	if (!config->has_key(config, "debug"))
	{
		child_pid = fork();
		if (child_pid > 0)
		{
			// Parent
			return 0;
		}
	}
	else
	{
		config->unset(config, "debug");
	}
	return http_start(config);
}
