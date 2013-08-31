#include "init.h"
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
	config_opts->set(config_opts, key, value, LIST_TYPE_STRING);
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

void init_usage(char * prog, cmdline_opt * opts)
{
	printf("Usage: %s [options]\n\n", prog);
	for (int i = 0;; i++)
	{
		if (opts[i].opt.name == NULL)
		{
			break;
		}
		printf("-%c, --%-20s %s\n", opts[i].opt.val, opts[i].opt.name, opts[i].helpstring);
	}
}

int init_parse_commandline(cmdline_opt * opts, int argc, char **argv, void (*callback)(void *, int, char *, cmdline_opt *), void * callback_arg)
{
	int num_opts = 0;
	int optc;
	int last_optind = 1;
	char short_opts[300] = "";
	struct option longopts[INIT_MAX_OPTS + 1] = {
		{ NULL, 0, NULL, 0 }
	};
	// Fill out longopts
	if (opts != NULL)
	{
		for (int i = 0; num_opts < INIT_MAX_OPTS; i++)
		{
			if (opts[num_opts].opt.name == NULL)
			{
				break;
			}
			memcpy(&longopts[num_opts], &opts[num_opts].opt, sizeof(struct option));
			memset(&longopts[++num_opts], 0, sizeof(struct option));
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
	// Disable getopt's error output
	opterr = 0;
	while ((optc = getopt_long(argc, argv, short_opts, longopts, NULL)) != -1) {
		if (optc == '?')
		{
			// Invalid option
			if (optopt)
			{
				// Bad short option
				printf("Invalid option: -%c\n\n", optopt);
			}
			else
			{
				// Bad long option
				printf("Invalid option: %s\n\n", argv[last_optind]);
			}
		}
		last_optind = optind;
		callback(callback_arg, optc, optarg, opts);
	}
	return 1;
}
