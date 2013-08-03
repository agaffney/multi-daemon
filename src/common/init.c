#include "init.h"
#include "main.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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

int init_parse_commandline(Hash * config, struct option * opts, int argc, char **argv)
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
	for (int i = 0;; i++)
	{
		if (opts[i].name == NULL)
		{
			break;
		}
		longopts[num_opts++] = opts[i];
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
		if (optc == '?')
		{
			// Unknown option
			return 0;
		}
		else
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
		}
	}
	return 1;
}
