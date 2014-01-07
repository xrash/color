#ifndef COLORS_H
#define COLORS_H

typedef struct _color {
	char* foreground;
	char* background;
} color;

#define FG_BLACK        "\x1b[0;30m"
#define FG_RED          "\x1b[0;31m"
#define FG_GREEN        "\x1b[0;32m"
#define FG_BROWN        "\x1b[0;33m"
#define FG_BLUE         "\x1b[0;34m"
#define FG_PURPLE       "\x1b[0;35m"
#define FG_CYAN         "\x1b[0;36m"
#define FG_LIGHT_GRAY   "\x1b[0;37m"
#define FG_DARK_GRAY    "\x1b[1;30m"
#define FG_LIGHT_RED    "\x1b[1;31m"
#define FG_LIGHT_GREEN  "\x1b[1;32m"
#define FG_YELLOW       "\x1b[1;33m"
#define FG_LIGHT_BLUE   "\x1b[1;34m"
#define FG_LIGHT_PURPLE "\x1b[1;35m"
#define FG_LIGHT_CYAN   "\x1b[1;36m"
#define FG_WHITE        "\x1b[1;37m"

#define BG_BLACK        "\x1b[40m"
#define BG_RED          "\x1b[41m"
#define BG_GREEN        "\x1b[42m"
#define BG_BROWN        "\x1b[43m"
#define BG_BLUE         "\x1b[44m"
#define BG_PURPLE       "\x1b[45m"
#define BG_CYAN         "\x1b[46m"
#define BG_LIGHT_GRAY   "\x1b[47m"

#define COLOR_RESET     "\x1b[0m"

#define MAX_COLOR_SIZE  32

extern color* new_color(char* foreground, char* background);

#endif /* COLORS_H */
