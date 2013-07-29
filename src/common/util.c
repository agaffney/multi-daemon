#include "util.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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

int urldecode(char * dst, char * src)
{
	char tmpcode[3] = "";
	// Loop until we hit a 0
	while (*src)
	{
		if (*src == '%')
		{
			memcpy(tmpcode, ++src, 2);
			int ord = strtoul(tmpcode, NULL, 16);
			if (ord <= 0)
			{
				return 1;
			}
			*dst++ = (char)ord;
			// Advance the src pointer by 2 to skip the characters we decoded
			src += 2;
		}
		else if (*src == '+')
		{
			*dst++ = ' ';
			src++;
		}
		else
		{
			*dst++ = *src++;
		}
	}
	return 0;
}

int is_true(char * value)
{
	if (atoi(value) == 1)
	{
		return 1;
	}
	char * tmp = (char *)calloc(1, strlen(value) + 1);
	for (int i = 0;; i++)
	{
		if (value[i] == 0)
		{
			// end of string
			break;
		}
		tmp[i] = tolower(value[i]);
	}
	if (!strcmp(tmp, "true"))
	{
		return 1;
	}
	if (!strcmp(tmp, "on"))
	{
		return 1;
	}
	if (!strcmp(tmp, "yes"))
	{
		return 1;
	}
	return 0;
}
