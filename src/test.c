#include "main.h"
#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_entry(type_server_opt server_opts[])
{
	int i;
	for(i=0;;i++)
	{
		if (!strcmp(server_opts[i].name, ""))
		{
			break;
		}
		printf("test_entry(): %s = %s\n", server_opts[i].name, server_opts[i].value);
	}

	return 0;
}
