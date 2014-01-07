#include "options.h"
#include "scanner.h"

int main(int argc, char* argv[]) {
	options* opt;

	opt = parse_options(argc, argv);

	switch (opt->mode) {
		case MODE_CHAR:
			return scanchar(opt->string, opt->string_size, opt->code, opt->colors);
			break;
		case MODE_LINE:
			return scanline(opt->file, opt->patterns, opt->colors);
			break;
	}

	return 0;
}
