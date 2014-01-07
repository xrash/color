#ifndef OPTIONS_H
#define OPTIONS_H

#include "list.h"
#include "colors.h"

#include <pcre.h>
#include <stdio.h>

#define MODE_LINE 0
#define MODE_CHAR 1

typedef struct {
	char* string;
	pcre* code;
} pattern;

typedef struct {
	FILE* file;
	char* string;
	int string_size;
	list* patterns;
	pcre* code;
	color** colors;
	int mode;
} options;

extern options* parse_options(int argc, char* argv[]);

#endif /* OPTIONS_H */
