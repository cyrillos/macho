#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>

#include "macho.h"
#include "log.h"

int main(int argc, char *argv[])
{
	char *fname;
	int ret = -1;
	int opt, idx;

	static const char short_opts[] = "f:h";
	static struct option long_opts[] = {
		{ "file",			required_argument,	0, 'f'	},
		{ "help",			no_argument,		0, 'h'	},
		{ },
	};

	if (argc < 1)
		goto usage;

	while (1) {
		idx = -1;
		opt = getopt_long(argc, argv, short_opts, long_opts, &idx);
		if (opt == -1)
			break;

		switch (opt) {
		case 'f':
			fname = optarg;
			break;
		case 'h':
		default:
			goto usage;
		}
	}
	return 0;

usage:
	pr_msg("\n"
"Usage:\n"
"  macho -f file [<options>]\n");
	return 1;
}
