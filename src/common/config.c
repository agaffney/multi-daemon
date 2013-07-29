#include "config.h"
#include "main.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

int config_parse_error(char *configfile, char *line, int linenum)
{
	fprintf(stderr, "Could not parse file '%s'. Error on line %d:\n\n%s", configfile, linenum, line);
	return -1;
}

int config_parse_file(char *configfile, char *service, Hash * config_opts)
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
				return config_parse_error(configfile, buf, linenum);
			}
			continue;
		}

		// Try to parse as a key=value pair
		char *key = strtok(buf, "=");
		if (key == NULL)
		{
			return config_parse_error(configfile, buf, linenum);
		}
		char *value = strtok(NULL, "");
		if (value == NULL)
		{
			return config_parse_error(configfile, buf, linenum);
		}
		if (!strcmp(header, "common") || !strcmp(header, service))
		{
			// Strip off trailing whitespace
			rtrim(value);
			config_opts->set(config_opts, key, value);
		}
	}

	fclose(config_fd);

	return 1;
}
