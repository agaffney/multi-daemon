#include "config.h"
#include "main.h"

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

char * config_get_option(char * opt, config_opt config_opts[])
{
	for (int i = 0;; i++)
	{
		if (!strcmp(config_opts[i].name, ""))
		{
			// End of options
			break;
		}
		if (!strcmp(config_opts[i].name, opt))
		{
			return config_opts[i].value;
		}
	}
	return NULL;
}

int config_parse_file(char *configfile, char *service, config_opt config_opts[], int config_opts_len)
{
	int config_opts_idx = 0;
	FILE *config_fd;

	config_fd = fopen(configfile, "r");
	if (config_fd == NULL)
	{
		fprintf(stderr, "Could not open config file %s: %s\n", configfile, strerror(errno));
		return -1;
	}

	char *buf = malloc(1024);
	char *header = malloc(50);
	if (buf == NULL || header == NULL)
	{
		fprintf(stderr, "Panic!\n");
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
		if (!strcmp(header, "main") || !strcmp(header, service))
		{
			strncpy(config_opts[config_opts_idx].name, key, sizeof(config_opts[config_opts_idx].name));
			config_opts[config_opts_idx].name[sizeof(config_opts[config_opts_idx].name)-1] = 0;
			// Strip off trailing whitespace
			while (1)
			{
				if (isspace(value[strlen(value)-1]))
				{
					value[strlen(value)-1] = 0;
					continue;
				}
				break;
			}
			strncpy(config_opts[config_opts_idx].value, value, sizeof(config_opts[config_opts_idx].value));
			config_opts[config_opts_idx].value[sizeof(config_opts[config_opts_idx].value)-1] = 0;
			config_opts_idx++;
		}
	}

	free(buf);
	free(header);
	fclose(config_fd);

	return config_opts_idx;
}
