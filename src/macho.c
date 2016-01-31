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

#include "xmalloc.h"
#include "macho.h"
#include "log.h"

static char *cmd_name(unsigned int cmd)
{
	static char buf[64];

	switch (cmd) {
	case LC_SEGMENT_64:
		strncpy(buf, "LC_SEGMENT_64", sizeof(buf));
		break;
	case LC_VERSION_MIN_MACOSX:
		strncpy(buf, "LC_VERSION_MIN_MACOSX", sizeof(buf));
		break;
	case LC_SYMTAB:
		strncpy(buf, "LC_SYMTAB", sizeof(buf));
		break;
	case LC_DYSYMTAB:
		strncpy(buf, "LC_DYSYMTAB", sizeof(buf));
		break;
	default:
		snprintf(buf, sizeof(buf), "Unknown %#x", cmd);
		break;
	}

	return buf;
}

#define __off(__p) ((unsigned long)((void *)__p - mem))

static int __parse_macho(const char *fname, void *mem, size_t size)
{
	mach_header_64_t *hdr = mem;
	mach_load_command_t *cmds;
	void *p;
	size_t i;

	mach_dysymtab_command_t *dysymtab = NULL;
	mach_version_min_command_t *ver = NULL;
	mach_segment_command_64_t *seg = NULL;
	mach_symtab_command_t *symtab = NULL;

	macho_nlist_64_t *nlist;
	char *strtab;

	pr_info("Parsing %s (at %p size %zu)\n", fname, mem, size);

	if (size < sizeof(*hdr)) {
		pr_err("Too small size\n");
		return -1;
	}

	if (hdr->magic != MH_MAGIC_64) {
		pr_err("Unknown magic\n");
		return -1;
	}

	pr_info("%08x | Header (MachO 64)\n"
		"         |  magic                %#x\n"
		"         |  cputype              %#x\n"
		"         |  cpusubtype           %#x\n"
		"         |  filetype             %#x\n"
		"         |  ncmds                %u\n"
		"         |  sizeofcmds           %u\n"
		"         |  flags                %#x\n",
		__off(hdr), hdr->magic, hdr->cputype, hdr->cpusubtype,
		hdr->filetype, hdr->ncmds, hdr->sizeofcmds, hdr->flags);

	cmds = mem + sizeof(*hdr);
	pr_info("%08x | Commands\n", __off(cmds));
	for (i = 0; i < hdr->ncmds; i++) {
		switch (cmds->cmd) {
		case LC_SEGMENT_64:
			seg = (void *)cmds;
			pr_info("%08x |  %-20s cmdsize %d\n"
				"         |   segname             %s\n"
				"         |   vmaddr              %#lx\n"
				"         |   vmsize              %#lx\n"
				"         |   fileoff             %#lx\n"
				"         |   filesize            %#lx\n"
				"         |   maxprot             %#x\n"
				"         |   initprot            %#x\n"
				"         |   nsects              %d\n"
				"         |   flags               %#x\n",
				__off(seg), cmd_name(LC_SEGMENT_64), seg->cmdsize,
				seg->segname, seg->vmaddr, seg->vmsize,
				seg->fileoff, seg->filesize, seg->maxprot,
				seg->initprot, seg->nsects, seg->flags);
			cmds = (void *)cmds + seg->cmdsize;
			break;
		case LC_VERSION_MIN_MACOSX:
			ver = (void *)cmds;
			pr_info("%08x |  %-20s cmdsize %d\n"
				"         |   version             %#lx\n"
				"         |   sdk                 %#lx\n",
				__off(ver), cmd_name(LC_VERSION_MIN_MACOSX), ver->cmdsize,
				ver->version, ver->sdk);
			cmds = (void *)cmds + ver->cmdsize;
			break;
		case LC_SYMTAB:
			symtab = (void *)cmds;
			pr_info("%08x |  %-20s cmdsize %d\n"
				"         |   symoff              %#lx\n"
				"         |   nsyms               %#lx\n"
				"         |   stroff              %#x\n"
				"         |   strsize             %#lx\n",
				__off(symtab), cmd_name(LC_SYMTAB), symtab->cmdsize,
				symtab->symoff, symtab->nsyms,
				symtab->stroff, symtab->strsize);
			cmds = (void *)cmds + symtab->cmdsize;
			break;
		case LC_DYSYMTAB:
			dysymtab = (void *)cmds;
			pr_info("%08x |  %-20s cmdsize %d\n",
				__off(dysymtab), cmd_name(LC_DYSYMTAB), dysymtab->cmdsize);
			cmds = (void *)cmds + dysymtab->cmdsize;
			break;
		default:
			pr_info("%08x |  cmd %20s cmdsize %d\n",
				__off(cmds), cmd_name(cmds->cmd), cmds->cmdsize);
			return -1;
			break;
		}
	}

	if (!symtab) {
		pr_err("No symbol table found\n");
		return -1;
	}

	pr_info("------------------------------\n");

	if (seg) {
		mach_section_64_t *s = (void *)seg + sizeof(*seg);
		for (i = 0; i < seg->nsects; i++, s++) {
			pr_info("%08x | Section \n", __off(s));
			pr_info("         |   sectname            %-16s\n"
				"         |   segname             %-16s\n"
				"         |   addr                %#lx\n"
				"         |   size                %#lx\n"
				"         |   offset              %#lx\n"
				"         |   align               %#lx\n"
				"         |   reloff              %#lx\n"
				"         |   nreloc              %#lx\n"
				"         |   flags               %#lx\n",
				s->sectname, s->segname, (long)s->addr,
				(unsigned long)s->size, (long)s->offset,
				(unsigned long)s->align, (long)s->reloff,
				(unsigned long)s->nreloc, (long)s->flags);
		}
	}

	nlist = mem + symtab->symoff;
	strtab = mem + symtab->stroff;

	pr_info("%08x | Symbol table\n", __off(nlist));
	for (i = 0; i < symtab->nsyms; i++) {
		if (!nlist[i].n_strx)
			continue;
		pr_info("%08x |  n_strx %#8x -> %s\n"
			"         |    n_type %#4x n_sect  %#4x\n"
			"         |    n_desc %#4x n_value %#lx\n",
			__off(&nlist[i]), nlist[i].n_strx,
			strtab + nlist[i].n_strx,
			nlist[i].n_type, nlist[i].n_sect,
			nlist[i].n_desc, nlist[i].n_value);
	}

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
