#include "util.h"

#include <ctype.h>
#include <string.h>

char * rtrim(char * str)
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

char * ltrim(char * str)
{
	while (1)
	{
		if (isspace(str[0]))
		{
			int len = strlen(str);
			int i;
			for (i = 1; i < len; i++)
			{
				str[i-1] = str[i];
			}
			str[i-1] = 0;
			continue;
		}
		break;
	}
	return str;
}
