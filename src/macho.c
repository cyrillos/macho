#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "macho.h"
#include "log.h"

static int __parse_macho(const char *fname, void *mem, size_t size)
{
	mach_header_64_t *hdr = mem;

	pr_info("Parsing %s (at %p size %zu)\n", fname, mem, size);

	if (size < sizeof(*hdr)) {
		pr_err("Too small size\n");
		return -1;
	}

	if (hdr->magic != MH_MAGIC_64) {
		pr_err("Unknown magic\n");
		return -1;
	}

	pr_info("Header (MachO 64)\n"
		" magic       %#x\n"
		" cputype     %#x\n"
		" cpusubtype  %#x\n"
		" filetype    %#x\n"
		" ncmds       %u\n"
		" sizeofcmds  %u\n"
		" flags       %#x\n",
		hdr->magic, hdr->cputype, hdr->cpusubtype,
		hdr->filetype, hdr->ncmds, hdr->sizeofcmds, hdr->flags);

	return 0;
}

static int parse_macho(const char *fname)
{
	int fd = open(fname, O_RDONLY);
	struct stat st;
	size_t size;
	void *mem;
	int ret;

	if (fd < 0) {
		pr_perror("Can't open %s", fname);
		return -1;
	}

	if (fstat(fd, &st)) {
		pr_perror("Can't stat %s", fname);
		close(fd);
		return -1;
	}

	mem = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (mem == MAP_FAILED) {
		pr_perror("Can't mmap %s", fname);
		close(fd);
		return -1;
	}

	ret = __parse_macho(fname, mem, st.st_size);
	munmap(mem, st.st_size);
	close(fd);

	return ret;
}

int main(int argc, char *argv[])
{
	struct stat st;
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

	return parse_macho(fname);

usage:
	pr_msg("\n"
"Usage:\n"
"  macho -f file [<options>]\n");
	return 1;
}
