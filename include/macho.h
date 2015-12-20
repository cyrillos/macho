#ifndef MACHO_H__
#define MACHO_H__

#include <inttypes.h>

#define MH_MAGIC			(0xfeedface)
#define MH_MAGIC_64			(0xfeedfacf)
#define FAT_MAGIC			(0xcafebabe)

#define LC_SEGMENT			(0x00000001)
#define LC_SYMTAB			(0x00000002)
#define LC_SEGMENT_64			(0x00000019)
#define LC_VERSION_MIN_MACOSX		(0x00000024)
#define LC_DYSYMTAB			(0x0000000b)

#define SECTION_TYPE			(0x000000ff)
#define SECTION_ATTRIBUTES_SYS		(0x00ffff00)

#define S_REGULAR			(0x00)
#define S_ZEROFILL			(0x01)

#define S_ATTR_PURE_INSTRUCTIONS	(0x80000000)
#define S_ATTR_SOME_INSTRUCTIONS	(0x00000400)
#define S_ATTR_EXT_RELOC		(0x00000200)
#define S_ATTR_LOC_RELOC		(0x00000100)

#define R_ABS				(0)
#define R_SCATTERED			(0x80000000)

#define N_TYPE				(0x0e)
#define N_EXT				(0x01)

#define N_UNDF				(0x0)
#define N_ABS				(0x2)
#define N_SECT				(0xe)

#define NO_SECT				(0)
#define MAX_SECT			(0xff)

#define VM_PROT_NONE			(0x0)
#define VM_PROT_READ			(0x1)
#define VM_PROT_WRITE			(0x2)
#define VM_PROT_EXECUTE			(0x4)

#define CPU_ARCH_ABI64			(0x01000000)

#define CPU_TYPE_X86			(7)
#define CPU_TYPE_I386			(CPU_TYPE_X86)
#define CPU_TYPE_X86_64			(CPU_TYPE_X86 | CPU_ARCH_ABI64)

#define CPU_SUBTYPE_I386_ALL		(3)

typedef struct mach_header {
	uint32_t	magic;
	uint32_t	cputype;
	uint32_t	cpusubtype;
	uint32_t	filetype;
	uint32_t	ncmds;
	uint32_t	sizeofcmds;
	uint32_t	flags;
} mach_header_t;

typedef struct mach_header_64 {
	uint32_t	magic;
	uint32_t	cputype;
	uint32_t	cpusubtype;
	uint32_t	filetype;
	uint32_t	ncmds;
	uint32_t	sizeofcmds;
	uint32_t	flags;
	uint32_t	reserved;
} mach_header_64_t;

typedef struct mach_load_command {
	uint32_t	cmd;
	uint32_t	cmdsize;
} mach_load_command_t;

typedef struct mach_segment_command {
	uint32_t	cmd;
	uint32_t	cmdsize;
	char		segname[16];
	uint32_t	vmaddr;
	uint32_t	vmsize;
	uint32_t	fileoff;
	uint32_t	filesize;
	uint32_t	maxprot;
	uint32_t	initprot;
	uint32_t	nsects;
	uint32_t	flags;
} mach_segment_command_t;

typedef struct mach_segment_command_64 {
	uint32_t	cmd;
	uint32_t	cmdsize;
	char		segname[16];
	uint64_t	vmaddr;
	uint64_t	vmsize;
	uint64_t	fileoff;
	uint64_t	filesize;
	uint32_t	maxprot;
	uint32_t	initprot;
	uint32_t	nsects;
	uint32_t	flags;
} mach_segment_command_64_t;

typedef struct mach_version_min_command {
	uint32_t	cmd;
	uint32_t	cmdsize;
	uint32_t	version;
	uint32_t	sdk;
} mach_version_min_command_t;

typedef struct mach_dysymtab_command {
	uint32_t	cmd;
	uint32_t	cmdsize;
	uint32_t	ilocalsym;
	uint32_t	nlocalsym;
	uint32_t	iextdefsym;
	uint32_t	nextdefsym;
	uint32_t	iundefsym;
	uint32_t	nundefsym;
	uint32_t	tocoff;
	uint32_t	ntoc;
	uint32_t	modtaboff;
	uint32_t	nmodtab;
	uint32_t	extrefsymoff;
	uint32_t	nextrefsyms;
	uint32_t	indirectsymoff;
	uint32_t	nindirectsyms;
	uint32_t	extreloff;
	uint32_t	nextrel;
	uint32_t	locreloff;
	uint32_t	nlocrel;
} mach_dysymtab_command_t;

typedef struct mach_section {
	char		sectname[16];
	char		segname[16];
	uint32_t	addr;
	uint32_t	size;
	uint32_t	offset;
	uint32_t	align;
	uint32_t	reloff;
	uint32_t	nreloc;
	uint32_t	flags;
	uint32_t	reserved1;
	uint32_t	reserved2;
} mach_section_t;

typedef struct mach_section_64 {
	char		sectname[16];
	char		segname[16];
	uint64_t	addr;
	uint64_t	size;
	uint32_t	offset;
	uint32_t	align;
	uint32_t	reloff;
	uint32_t	nreloc;
	uint32_t	flags;
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
} mach_section_64_t;

typedef struct mach_symtab_command {
	uint32_t	cmd;
	uint32_t	cmdsize;
	uint32_t	symoff;
	uint32_t	nsyms;
	uint32_t	stroff;
	uint32_t	strsize;
} mach_symtab_command_t;

typedef struct macho_nlist {
	uint32_t	n_strx;
	uint8_t		n_type;
	uint8_t		n_sect;
	int16_t		n_desc;
	uint32_t	n_value;
} macho_nlist_t;

typedef struct macho_nlist_64 {
	uint32_t	n_strx;
	uint8_t		n_type;
	uint8_t		n_sect;
	uint16_t	n_desc;
	uint64_t	n_value;
} macho_nlist_64_t;

typedef struct macho_relocation_info {
	int32_t		r_address;
	uint32_t	r_symbolnum:24,
			r_pcrel:1,
			r_length:2,
			r_extern:1,
			r_type:4;
} macho_relocation_info_t;

#endif /* MACHO_H__ */
