#include "options.h"
#include "scanner.h"
#include "colors.h"
#include "list.h"

#include <pcre.h>
#include <stdio.h>
#include <string.h>

/*
 * Print size bytes from buffer with colored output.
 * color can't be NULL.
 */
void print_buffer(char* buffer, int size, color* color) {
	fputs(color->foreground, stdout);
	if (color->background != NULL) {
		fputs(color->background, stdout);
	}

	while (size-- > 0) {
		fputc(*buffer++, stdout);
	}

	fputs(COLOR_RESET, stdout);
}

/*
 * Read the next line from file into buffer.
 * Buffer should be NULL, it will be allocated and grow accordingly.
 * Returns 1 if there are more lines to be read and EOF otherwise.
 */
int nextline(FILE* file, char** buffer) {
	int c = 0;
	int linesize = 8;
	char* bufferp;
	char* tmp;

	/*
	 * Return EOF if there are no more lines to be read.
	 */
	if (feof(file)) {
		return EOF;
	}

	/* Allocate the buffer and save it's initial address. */
	*buffer = malloc(sizeof(char) * linesize + 1);
	memset(*buffer, 0, sizeof(char) * linesize + 1);
	bufferp = *buffer;

	/* Read file until EOF or a newline. */
	while ((c = fgetc(file)) != EOF && c != '\n') {
		*(*buffer)++ = c;

		/* Grow the buffer accordingly. */
		if ((*buffer - bufferp) == linesize) {
			linesize *= 2;
			tmp = malloc(sizeof(char) * linesize + 1);
			memset(tmp, 0, sizeof(char) * linesize + 1);
			strcpy(tmp, bufferp);
			free(bufferp);
			bufferp = tmp;
			*buffer = bufferp + (linesize/2);
		}
	}

	/* If the loop ended in a newline, we have to add it now. */
	if (c == '\n') {
		*(*buffer)++ = '\n';
	}

	/* Always finish the buffer with a \0. */
	**buffer = '\0';

	/* Point the buffer to it's initial position. */
	*buffer = bufferp;

	return 1;
}

/*
 * Just a wrapper to pcre_exec.
 */
int* match(pcre* code, char* subject, int length, int startoffset, int ovecsize) {
	int r = 0;
	pcre_extra* extra = NULL;
	int options = PCRE_NOTEMPTY;
	int *ovector = malloc(sizeof(int) * ovecsize);
	memset(ovector, 0, sizeof(int) * ovecsize);

	r = pcre_exec(code, extra, subject, length, startoffset, options, ovector, ovecsize);

	/* If there is actually an error, we should stop execution. */
	if (r < -1) {
		printf("PCRE error: %d\n", r);
		exit(1);
	}

	return ovector;
}

/*
 * Print buffer considering the list of matches.
 */
void print_colored_buffer(char* buffer, list* matches) {
	int bufferlen = strlen(buffer);
	char** start;
	char** end;
	o_match* m = NULL;
	list_node* n = NULL;
	int len = 0;
	int c = 0;
	int i = 0;
	int j = 0;

	/*
	 * Allocate two arrays of colors in the format (index) => (color)
	 * so we can lookup it at every character while traversing the buffer
	 * to check if there is some color to be output at that index.
	 *
	 * The lookups are constant time but they happen at every
	 * index plus we need to allocate O(buffer) memory, which can
	 * be very expensive.
	 *
	 * Remember: premature optimization is the root of all evils.
	 */
	start = malloc(sizeof(char*) * (bufferlen + 1));
	end = malloc(sizeof(char*) * (bufferlen + 1));
	memset(start, 0, sizeof(char*) * (bufferlen + 1));
	memset(end, 0, sizeof(char*) * (bufferlen + 1));

	/* Build the colors array. */
	n = matches->head;
	while ((n = n->next) != NULL) {
		m = n->element;

		/* Keep the first match and discard the later. */
		if (start[m->start] != NULL || end[m->end] != NULL) {
			continue;
		}

		/* Do not allow nested matches. */
		for (j = m->start + 1; j < m->end; j++) {
			if (start[j] != NULL || end[j] != NULL) {
				break;
			}
		}

		if (j != m->end) {
			continue;
		}

		len = strlen(m->color->foreground);
		if (m->color->background != NULL) {
			len += strlen(m->color->background);
		}

		start[m->start] = malloc(len + 1);
		memset(start[m->start], 0, len + 1);
		strcat(start[m->start], m->color->foreground);
		if (m->color->background != NULL) {
			strcat(start[m->start], m->color->background);
		}

		end[m->end] = malloc(strlen(COLOR_RESET) + 1);
		strcpy(end[m->end], COLOR_RESET);
	}

	/* Loop through the buffer while checking the colored array. */
	while ((c = *buffer++) != '\0') {
		if (end[i] != NULL) {
			fputs(end[i], stdout);
		}

		if (start[i] != NULL) {
			fputs(start[i], stdout);
		}

		i++;
		fputc(c, stdout);
	}

	/* The last index has to run outside the loop */
	if (end[i] != NULL) {
		fputs(end[i], stdout);
	}
	if (start[i] != NULL) {
		fputs(start[i], stdout);
	}
}

/*
 * Scan through file line by line executing every pattern
 * individually as many times as needed each line.
 */
int scanline(FILE* file, list* patterns, color** colors) {
	int i = 0;
	char* buffer = NULL;
	int ovecsize = patterns->length * 3 + 3;
	int* ovector = NULL;
	list_node* n = NULL;
	list* matches = NULL;
	o_match* m = NULL;
	int adv = 0;
	unsigned int len = 0;
	pattern* p = NULL;
	ovecsize = 30;

	/* Read the file line by line. */
	while ((nextline(file, &buffer)) != EOF) {
		n = patterns->head;
		i = 0;
		matches = list_new();
		len = strlen(buffer);

		/* Try to match every pattern with this line. */
		while ((n = n->next) != NULL) {
			adv = 0;
			p = n->element;

			/*
			 * Try to match the same pattern as many times as possible.
			 * PCRE will only match one time, so we need to loop through
			 * the string in order to match every possibility.
			 */
			while (adv >= 0) {
				ovector = match(p->code, buffer, len, adv, ovecsize);

				/* If the pattern matches, add the match to a list of matches. */
				if (ovector[0] >= 0) {
					m = malloc(sizeof(o_match));
					m->start = ovector[0];
					m->end = ovector[1];
					m->color = colors[i];
					m->string = p->string;
					list_add(matches, m);
					adv = ovector[1];
				} else {
					adv = -1;
				}

				free(ovector);
			}
			i++;
		}

		if (matches->length < 1) {
			fputs(buffer, stdout);
		} else {
			print_colored_buffer(buffer, matches);
		}

		free(matches);
		free(buffer);
	}

	return 0;
}

/*
 * Advance string character by character executing pattern at the beggining
 * every iteration.
 *
 * When pattern is found, print it out with the corresponding colors.
 */
int scanchar(char* string, int string_size, pcre* code, color** colors) {
	int i = 0;
	int length = string_size;
	int ovecsize = 30;
	int *ovector = NULL;

	do {
		ovector = match(code, string, length, 0, ovecsize);

		if (ovector[0] != 0) {
			fputc(*string++, stdout);
			length--;
			continue;
		}

		/* Loop through the first 2/3 of the ovector. */
		for (i = 2; i < ovecsize/3*2 - 2; i = i+2) {
			/*
			 * pair (ovector[i], ovector[i+1]) is set to
			 * (start, end) of the matching. It is < 0 in case of not matching.
			 */
			if (ovector[i] == 0) {
				print_buffer(string, ovector[i+1], colors[i/2-1]);
				break;
			}
		}

		/* Increase the string pointer. */
		string += ovector[1];
		length -= ovector[1];

		free(ovector);
	} while(length > 0);

	return 0;
}
