#ifndef SCANNER_H
#define SCANNER_H

#include "list.h"
#include "colors.h"

#include <stdio.h>
#include <pcre.h>

/* Represents one point to insert color inside a string. */
typedef struct  {
	char* color;
	int reset;
} c_point;

/* Represents one regex match. */
typedef struct {
	int start;
	int end;
	color* color;
	char* string;
} o_match;

extern int scanline(FILE* file, list* patterns, color** colors);
extern int scanchar(char* string, int string_size, pcre* code, color** colors);

#endif /* SCANNER_H */
