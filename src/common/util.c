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
