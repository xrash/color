#include "colors.h"

#include <string.h>
#include <stdlib.h>

char *FG_COLORS[32]  = {
	"black", FG_BLACK,
	"red", FG_RED,
	"green", FG_GREEN,
	"brown", FG_BROWN,
	"blue", FG_BLUE,
	"purple", FG_PURPLE,
	"cyan", FG_CYAN,
	"light-gray", FG_LIGHT_GRAY,
	"dark-gray", FG_DARK_GRAY,
	"light-red", FG_LIGHT_RED,
	"light-green", FG_LIGHT_GREEN,
	"yellow", FG_YELLOW,
	"light-blue", FG_LIGHT_BLUE,
	"light-purple", FG_LIGHT_PURPLE,
	"light-cyan", FG_LIGHT_CYAN,
	"white", FG_WHITE
};

int FG_COLORS_LEN = 32;

char *BG_COLORS[16] = {
	"black", BG_BLACK,
	"red", BG_RED,
	"green", BG_GREEN,
	"brown", BG_BROWN,
	"blue", BG_BLUE,
	"purple", BG_PURPLE,
	"cyan", BG_CYAN,
	"light-gray", BG_LIGHT_GRAY
};

int BG_COLORS_LEN = 16;

/* 
 * Return the corresponding code for the specified color.
 * Expects an array of colors in the format of FG_COLORS or BG_COLORS.
 */
char* color2code(char *array[], int len, char* color) {
	int i;

	if (color == NULL || array == NULL) {
		return NULL;
	}

	for (i = 0; i < len; i = i + 2) {
		if (strcmp(array[i], color) == 0) {
			return array[i+1];
		}
	}

	return NULL;
}

/* Return the corresponding code for the specified foreground color. */
char* fg2code(char* color) {
	return color2code(FG_COLORS, FG_COLORS_LEN, color);
}

/* Return the corresponding code for the specified background color. */
char* bg2code(char* color) {
	return color2code(BG_COLORS, BG_COLORS_LEN, color);
}

/* Allocate and return a new color. */
color* new_color(char* foreground, char* background) {
	color* c = malloc(sizeof(color));
	c->foreground = fg2code(foreground);

	if (c->foreground == NULL) {
		return NULL;
	}

	c->background = bg2code(background);
	return c;
}
