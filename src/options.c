#include "list.h"
#include "options.h"
#include "colors.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

/* Print the help message and exit. */
void help() {
	/* Usage*/
	printf(
"Usage:\n"
"    color [-h] [-f <filename>] [-c <foreground[/<background>]>...] <pattern...>\n"
"\n");

	/* Description */
	printf(
"Description:\n"
"    color reads stdin and output a colorized version applying specified colors to every regular expression <pattern...> matched.\n"
"    Patterns and colors are matched by order of declaration: if you declare two colors and then two patterns, the first color matches the first pattern and so on. If you declare less colors than patterns, the last color will repeat. See Examples for more information.\n"
"\n");

	/* Options */
	printf(
"Options:\n"
"    -h\n"
"        Print this message and exit.\n"
"    -f <filename>\n"
"        Read <filename> instead of stdin.\n"
"    -c <foreground>[/<background>]\n"
"        Specify the foreground and background colors. Background is optional. See Supported colors and Examples for more information.\n"
		);
	printf(
"    -m <mode>\n"
"        Supported modes are \"char\" and \"line\". Default is line.\n"
"        Line mode executes every pattern line by line.\n"
"        Char mode advances the text from left to right executing every pattern each character.\n"
"        Nested matches are NOT supported by any of the two modes. They will be ignored. First matches have priority.\n"
"\n");

	/* Supported colors */
	printf(
"Supported colors:\n"
"    Foreground:\n"
"        black         red          green         brown\n"
"        blue          purple       cyan          light-gray\n"
"        dark-gray     light-red    light-green   yellow\n"
"        light-blue    light-purple light-cyan    white\n"
"    Background:\n"
"        black         red          green         brown\n"
"        blue          purple       cyan          light-gray\n"
"\n"
);

	/* Examples */
	printf(
"Examples:\n"
"    echo \"Hello, world\" | color world\n"
"    echo \"Hello, world\" | color -c green world\n"
"    echo \"Hello, world\" | color -c blue/green world\n"
"    echo \"Hello, world\" | color -c green Hello world\n"
"    echo \"Hello, world\" | color -c red/light-gray -c green Hello world\n"
"    color -f /path/to/file -c red WARNING\n"
		);
	printf(
"    color -m char \\\n"
"        'if(?= ?\\()' -c cyan \\\n"
"        'true|false' -c cyan \\\n"
"        '\\(|\\)' -c yellow \\\n"
"        'then|else' -c green \\\n"
"        '\".*\"' -c red \\\n"
"    <<EOF\n"
"    if (true) then \"if (\" else false\n"
"    EOF\n"
"\n"
);

	exit(0);
}

/* Allocate and return a new pattern. */
pattern* new_pattern(char* string, pcre* code) {
	pattern* p = malloc(sizeof(pattern));
	p->string = string;
	p->code = code;
	return p;
}

/*
 * Return a pointer to filename if it's valid. Return stdin otherwise.
 */
FILE* selectfile(char* filename) {
	FILE* file = stdin;

	if (filename != NULL) {
		file = fopen(filename, "rb");
		if (file == NULL) {
			printf("Could not open file: %s\n", filename);
			exit(1);
		}
	}

	return file;
}

/*
 * Read filename if it's a valid readable filename. Read stdin otherwise.
 * Returns a pointer to a buffer containing all the contents of the read file.
 */
char* readfile(FILE* file, int* string_size) {
	char* buffer = NULL;
	char* bufferp = NULL;
	char* tmp = NULL;
	long fsize = 0;
	int c = 0;
	unsigned int size = 1024 * sizeof(char);

	if (file != stdin) {
		fsize = ftell(file);
		rewind(file);
		buffer = malloc(sizeof(char) * size + 1);
		memset(buffer, 0, size + 1);
		fread(buffer, fsize, sizeof(char), file);
		*string_size = fsize;

		return buffer;
	}

	buffer = malloc(size);
	memset(buffer, 0, size);
	bufferp = buffer;
	while ((c = fgetc(file)) != EOF) {
		/* Double the buffer size if it's needed. */
		if ((buffer - bufferp) == size) {
			size = size*2;
			tmp = malloc(size);
			strcpy(tmp, bufferp);
			free(bufferp);
			buffer = tmp + (size/2);
			bufferp = tmp;
		}

		*buffer++ = c;
	}

	*string_size = strlen(bufferp);
	return bufferp;
}

/* Just a wrapper to pcre_compile. */
pcre* compile_pcre(char* pattern) {
	pcre* code = NULL;
	int options = 0;
	const char* errptr = NULL;
	int* erroffset = malloc(sizeof(int));
	unsigned char *tableptr = NULL;
	memset(erroffset, 0, sizeof(int));

	code = pcre_compile(pattern, options, &errptr, erroffset, tableptr);

	if (code == NULL) {
		printf("PCRE could not compile pattern: %s.\n", errptr);
		printf("Pattern follows: %s.\n", pattern);
		exit(1);
	}

	return code;
}

/*
 * Compile each pattern from the list of patterns
 * and return a new list, containing the compiled ones.
 */
list* build_patterns(list* patterns) {
	list* result = list_new();
	list_node* n = patterns->head;
	pcre* code = NULL;

	while ((n = n->next) != NULL) {
		code = compile_pcre(n->element);
		list_add(result, new_pattern(n->element, code));
	}

	return result;
}

/*
 * Concatenate every pattern from the patterns list
 * into one regular expression and compile it.
 */
pcre* concatenate(list* patterns) {
	char* pattern = malloc(1);
	char* string = NULL;
	char* tmp = NULL;
	list_node* n = patterns->head;
	unsigned int size = 0;
	memset(pattern, 0, 1);
	while ((n = n->next) != NULL) {
		string = n->element;
		size += strlen(string) + 3;
		tmp = malloc(sizeof(char) * size + 1);
		strcpy(tmp, pattern);
		strcat(tmp, "(");
		strcat(tmp, string);
		strcat(tmp, ")|");
		free(pattern);
		pattern = tmp;
	}

	/* Remove the trailing pipe. */
	pattern[size-1] = '\0';

	return compile_pcre(pattern);
}

/*
 * Allocates and returns a new options.
 */
options* new_options() {
	options* opt = malloc(sizeof(options));
	return opt;
}

/*
 * Given a list of colors and the quantity of targets,
 * return an array in the form (index of target) => (color).
 *
 * This allows for fast lookup later.
 */
color** organize(list* colors, int targets_length) {
	int i, j;
	list_node* n = NULL;
	color* c = NULL;
	color** array = malloc(sizeof(color*) * targets_length);

	for (i = 0; i < targets_length; i++) {
		j = 0;
		n = colors->head;
		while ((n = n->next) != NULL) {
			c = n->element;
			if (j++ == i) {
				break;
			}
		}

		array[i] = c;
	}

	return array;
}

/*
 * Parse the command-line options and returns an options representing them.
 */
options* parse_options(int argc, char* argv[]) {
	list* colors = list_new();
	list* patterns = list_new();
	char* foreground = NULL;
	char* background = NULL;
	char* current = NULL;
	char* filename = NULL;
	color* color = NULL;
	options* opt = new_options();
	int option, c, i;
	int mode = MODE_LINE;
	FILE* file;

    while ((option = getopt(argc, argv, "hc:f:m:")) != -1) {
        switch (option) {
			/* Add a color to the list of colors. */
			case 'c' :
				foreground = malloc(MAX_COLOR_SIZE);
				background = malloc(MAX_COLOR_SIZE);
				memset(foreground, 0, MAX_COLOR_SIZE);
				memset(background, 0, MAX_COLOR_SIZE);

				/* Start reading the foreground color. */
				current = foreground;
				c = 0;
				i = 0;
				while ((c = *optarg++) != '\0') {
					/* If found a /, skip it and start reading the background color. */
					if (c == '/') {
						current[i++] = '\0';
						current = background;
						i = 0;
						continue;
					}

					current[i++] = c;
				}
				/* Finish the color (whether it's fg or bg) with a \0 byte */
				current[i] = '\0';

				/* Allocate a new color instance based on foreground and background */
				color = new_color(foreground, background);
				if (color == NULL) {
					printf("Color %s is not supported.\n", foreground);
					exit(1);
				}
				list_add(colors, color);

				/* Free stuff out. */
				free(foreground);
				free(background);

				break;
			case 'f':
				filename = optarg;
				break;
			case 'm':
				if (strcmp(optarg, "line") == 0) {
					mode = MODE_LINE;
				} else if (strcmp(optarg, "char") == 0) {
					mode = MODE_CHAR;
				} else {
					printf("%s is not a valid mode. Use -h if you need help.\n", optarg);
					exit(1);
				}
				break;
			case 'h':
			default:
				/* Print the help message and exit. */
				help();
				break;
        }
    }

	/* If there was actually no color specified, cyan is the default color. */
	if (colors->length < 1) {
		list_add(colors, new_color("cyan", NULL));
	}

	/* Read the arguments after the options. */
	for (i = optind; i < argc; i++) {
		list_add(patterns, argv[i]);
	}

	/* If, for some reason, we got no patterns... */
	if (patterns->length < 1) {
		help();
	}

	/* Decide if the input is a user-defined file or stdin. */
	file = selectfile(filename);

	/* Build the opt stuff. */
	opt->mode = mode;
	opt->file = file;
	if (opt->mode == MODE_CHAR) {
		opt->string = readfile(file, &opt->string_size);
		opt->code = concatenate(patterns);
	} else {
		opt->patterns = build_patterns(patterns);
	}
	opt->colors = organize(colors, patterns->length);

	return opt;
}
