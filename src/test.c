#include "config.h"
#include "main.h"
#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_entry(config_opt config_opts[])
{
	int i;
	for(i=0;;i++)
	{
		if (!strcmp(config_opts[i].name, ""))
		{
			break;
		}
		printf("test_entry(): %s = %s\n", config_opts[i].name, config_opts[i].value);
	}

	return 0;
}
