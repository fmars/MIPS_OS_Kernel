#include <ctype.h>
#include <string.h>
#include "string.h"

char *
trim(char *buffer)
{
	int i;
	while (isspace(*buffer))
		buffer++;

	i = strlen(buffer);
	i--;

	while (i >= 0) {
		if (isspace(buffer[i])) {
			buffer[i] = 0;
			i--;
		} else {
			break;
		}
	}

	return buffer;
}

int
str_start(char *buffer, int start)
{
	while (isspace(buffer[start]))
		start++;

	return start;
}

/* use (int is?????(int c)) to find the index of the first matched char. */
int
str_match(char *buffer, int (*test) (int))
{
	int i = 0;
	while (buffer[i]) {
		if (test(buffer[i])) {
			return i;
		}
		i++;
	}
	return -1;
}

int
str_not_match(char *buffer, int (*test) (int))
{
	int i = 0;
	while (buffer[i]) {
		if (!test(buffer[i])) {
			return i;
		}
		i++;
	}
	return -1;
}
