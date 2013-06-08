#include "config.h"
#include "main.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

int parse_config_file(char *configfile, char *service, config_opt config_opts[], int config_opts_len)
{
	int config_opts_idx = 0;
	FILE *config_fd;

	config_fd = fopen(configfile, "r");
	if (config_fd == NULL)
	{
		fprintf(stderr, "Could not open config file %s: %s\n", configfile, strerror(errno));
		return -1;
	}

	// Actually read the config file here

	fclose(config_fd);

	return config_opts_idx;
}
