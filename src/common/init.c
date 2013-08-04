#include "init.h"
#include "main.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int init_parse_config_error(char *configfile, char *line, int linenum)
{
	fprintf(stderr, "Could not parse file '%s'. Error on line %d:\n\n%s", configfile, linenum, line);
	return -1;
}

int init_parse_config_line(char * line, Hash * config_opts)
{
	char key[100];
	char value[1024];
	if (sscanf(line, "%99[^=]=%1023s", key, value) < 2)
	{
		return 0;
	}
	// Strip off trailing whitespace
	rtrim(value);
	config_opts->set(config_opts, key, value);
	return 1;
}

int init_parse_config_file(char *configfile, char *service, Hash * config_opts)
{
	FILE *config_fd;
	char buf[1024], header[50];

	config_fd = fopen(configfile, "r");
	if (config_fd == NULL)
	{
		fprintf(stderr, "Could not open config file %s: %s\n", configfile, strerror(errno));
		return -1;
	}

	int linenum;
	for (linenum = 1;; linenum++)
	{
		if (fgets(buf, 1024, config_fd) == NULL)
		{
			break;
		}
		if (buf[0] == ';')
		{
			// Comment line
			continue;
		}
		if (!strcmp(buf, "\n"))
		{
			// Empty line
			continue;
		}
		if (buf[0] == '[')
		{
			// Section header
			if (!sscanf(buf, "[%49[a-z]]", header))
			{
				return init_parse_config_error(configfile, buf, linenum);
			}
			continue;
		}
		if (!strcmp(header, "common") || !strcmp(header, service))
		{
			if (!init_parse_config_line(buf, config_opts))
			{
				return init_parse_config_error(configfile, buf, linenum);
			}
		}
	}

	fclose(config_fd);

	return 1;
}

void init_usage(char * prog, struct option * opts, Hash * usage)
{
	printf("Usage: %s [options]\n\n", prog);
	for (int i = 0;; i++)
	{
		if (opts[i].name == NULL)
		{
			break;
		}
		printf("-%c, --%-20s %s\n", opts[i].val, opts[i].name, usage->get(usage, (char *) opts[i].name));
	}
}

int init_parse_commandline(Hash * config, struct option * extra_opts, Hash * extra_usage, int argc, char **argv)
{
	int num_opts = 0;
	char short_opts[300];
	struct option longopts[INIT_MAX_OPTS] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ "debug", no_argument, NULL, 'd' },
		{ "config", required_argument, NULL, 'c' },
		{ "pidfile", required_argument, NULL, 'p' },
		{ "option", required_argument, NULL, 'o' },
		{ NULL, 0, NULL, 0 }
	};
	Hash * usage = Hash_init();
	usage->set(usage, "help", "display usage information and exit");
	usage->set(usage, "version", "display version information and exit");
	usage->set(usage, "debug", "don't fork into the background");
	usage->set(usage, "config", "specifies path to the config file");
	usage->set(usage, "pidfile", "path to write PID file");
	usage->set(usage, "option", "specifies a key=value pair configuration option");
	// Count the default opts
	while (1)
	{
		if (longopts[num_opts].name == NULL)
		{
			break;
		}
		num_opts++;
	}
	// Add additional opts
	if (extra_opts != NULL)
	{
		for (int i = 0;; i++)
		{
			if (extra_opts[i].name == NULL)
			{
				break;
			}
			// use memcpy
			longopts[num_opts++] = extra_opts[i];
		}
	}
	// Build short opts string
	for (int i = 0; i < num_opts; i++)
	{
		if (longopts[i].name == NULL)
		{
			break;
		}
		switch (longopts[i].has_arg)
		{
			case no_argument:
				sprintf(short_opts, "%s%c", short_opts, longopts[i].val);
				break;
			case required_argument:
				sprintf(short_opts, "%s%c:", short_opts, longopts[i].val);
				break;
			case optional_argument:
				sprintf(short_opts, "%s%c::", short_opts, longopts[i].val);
				break;
		}
	}
	int optc;
	while ((optc = getopt_long(argc, argv, short_opts, longopts, NULL)) != -1) {
		switch (optc)
		{
			case '?':
				// Unknown option
				init_usage("program", longopts, usage);
				_exit(1);
			case 'h':
				init_usage("program", longopts, usage);
				_exit(0);
			case 'o':
				if(!init_parse_config_line(optarg, config))
				{
					fprintf(stderr, "Argument does not appear to be a key/value pair: %s\n\n", optarg);
					init_usage("program", longopts, usage);
				}
				break;
			default:
			{
				int i = 0;
				for (; i < num_opts; i++)
				{
					if (longopts[i].val == optc)
					{
						switch (longopts[i].has_arg)
						{
							case no_argument:
								config->set(config, (char *) longopts[i].name, "1");
								break;
							case required_argument:
								config->set(config, (char *) longopts[i].name, optarg);
								break;
							case optional_argument:
								// This will probably break
								config->set(config, (char *) longopts[i].name, optarg);
								break;
						}
						break;
					}
				}
				break;
			}
		}
	}
	return 1;
}
