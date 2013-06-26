#include "util.h"

#include <ctype.h>
#include <string.h>

char * trim(char * str)
{
	int len = strlen(str);
	while (1)
	{
		if (isspace(str[len-1]))
		{
			str[len-1] = 0;
			continue;
		}
		break;
	}
	return str;
}
