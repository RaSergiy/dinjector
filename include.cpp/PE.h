#ifndef PE_H
#define PE_H

#define	CCFILEPE_MAGIC_MZ	0x5A4D
#define	CCFILEPE_MAGIC_PE	0x4550
#define	CCFILEPE_MAGIC_NE	0x454E

struct CCFILEPE_MZ
{
	short	e_magic;
	short	e_cblp;
	short	e_cp;
	short	e_crlc;
	short	e_cparhdr;
	short	e_minalloc;
	short	e_maxalloc;
	short	e_ss;
	short	e_sp;
	short	e_csum;
	short	e_ip;
	short	e_cs;
	short	e_lfarlc;
	short	e_ovno;
	short	e_res [4];
	short	e_oemid;
	short	e_oeminfo;
	short	e_res2 [10];
	int	e_lfanew;
};


//====================================================================
//		PE.Characteristics (word)
/*--------------------------------------------------------------------

    0  IMAGE_FILE_RELOCS_STRIPPED - if there is no relocation
    information in the file. This refers to relocation information per
    section in the sections themselves; it is not used for executables,
    which have relocation information in the 'base relocation' directory
    described below.

    1 IMAGE_FILE_EXECUTABLE_IMAGE - if the file is executable
    (i.e. it is not an object file or a library).

    IMAGE_FILE_LINE_NUMS_STRIPPED - is set if the line number
    information is stripped; this is not used for executable files.

    3 IMAGE_FILE_LOCAL_SYMS_STRIPPED - if there is no
    information about local symbols in the file (this is not used
    for executable files).

    4 IMAGE_FILE_AGGRESIVE_WS_TRIM - if the operating system
    is supposed to trim the working set of the running process (the
    amount of RAM the process uses) aggressivly by paging it out. This
    should be set if it is a demon-like application that waits most of
    the time and only wakes up once a day, or the like.

    7. IMAGE_FILE_BYTES_REVERSED_LO & IMAGE_FILE_BYTES_REVERSED_HI (15)
    are set if the endianess of the file is
    not what the machine would expect, so it must swap bytes before
    reading. This is unreliable for executable files (the OS expects
    executables to be correctly byte-ordered).

    8 IMAGE_FILE_32BIT_MACHINE - if the machine is expected
    to be a 32 bit machine. This is always set.

    9 IMAGE_FILE_DEBUG_STRIPPED - if there is no debugging
    information in the file. This is unused for executable files.

    10 IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP - if the application
    may not run from a removable medium such as a floppy or a CD-ROM. In
    this case, the operating system is advised to copy the file to the
    swapfile and execute it from there.

    11 IMAGE_FILE_NET_RUN_FROM_SWAP - if the application may
    not run from the network. In this case, the operating system is
    advised to copy the file to the swapfile and execute it from there.

    12 IMAGE_FILE_SYSTEM - if the file is a system file such
    as a driver. This is unused for executable files.

    13 IMAGE_FILE_DLL - if the file is a DLL.

    14 IMAGE_FILE_UP_SYSTEM_ONLY - is set if the file is not
    designed to run on multiprocessor systems (that is, it will crash
    there because it relies in some way on exactly one processor).

    15 IMAGE_FILE_BYTES_REVERSED_HI (see bit 7)
*/



//--------------------------------------------------------------------

struct CCFILEPE_DATADIR
{
	int		VirtualAddress;
	int		isize;
};

//--------------------------------------------------------------------

struct CCFILEPE_PE
{
	int	e_magic;
	// -----------------------------< File Header
	short	Machine;
	short	NumberOfSections;
	int	TimeDateStamp;
	int	PointerToSymbolTable;
	int	NumberOfSymbols;
	short	SizeOfOptionalHeader;
	short	Characteristics;
	// -----------------------------< Optional Header
	short	Magic;
	char	MajorLinkerVersion;
	char	MinorLinkerVersion;
	int	SizeOfCode;
	int	SizeOfInitializedData;
	int	SizeOfUninitializedData;
	int	AddressOfEntryPoint;
	int	BaseOfCode;
	int	BaseOfData;
	int	ImageBase;
	int	SectionAlignment;		// !
	int	FileAlignment;			// !
	short	MajorOperatingSystemVersion;
	short	MinorOperatingSystemVersion;
	short	MajorImageVersion;
	short	MinorImageVersion;
	short	MajorSubsystemVersion;
	short	MinorSubsystemVersion;
	int	Win32VersionValue;
	int	SizeOfImage;
	int	SizeOfHeaders;
	int	CheckSum;
	short	Subsystem;
	short	DllCharacteristics;
	int	SizeOfStackReserve;
	int	SizeOfStackCommit;
	int	SizeOfHeapReserve;
	int	SizeOfHeapCommit;
	int	LoaderFlags;
	int	NumberOfRvaAndSizes;
	// -----------------------------< Data directories
	CCFILEPE_DATADIR	DD_ExportTable;
	CCFILEPE_DATADIR	DD_ImportTable;
	CCFILEPE_DATADIR	DD_ResourceTable;
	CCFILEPE_DATADIR	DD_ExceptionTable;
	CCFILEPE_DATADIR	DD_CertificateTable;
	CCFILEPE_DATADIR	DD_RelocationTable;
	CCFILEPE_DATADIR	DD_DebugData;
	CCFILEPE_DATADIR	DD_ArchitectureSpecificData;
	CCFILEPE_DATADIR	DD_MachineValue;
	CCFILEPE_DATADIR	DD_TLS_Table;
	CCFILEPE_DATADIR	DD_LoadConfigurationTable;
	CCFILEPE_DATADIR	DD_BoundImportTable;
	CCFILEPE_DATADIR	DD_ImportAddressTable;
	CCFILEPE_DATADIR	DD_DelayImportDescriptor;
	CCFILEPE_DATADIR	DD_ComRunTimeHeader;
	CCFILEPE_DATADIR	DD_Reserved;
};

//================================================================================
//			SECTION CHARACTERISTICS
/*--------------------------------------------------------------------------------

	BITS

	0 IMAGE_SCN_TYPE_DSECT ?

	1 IMAGE_SCN_TYPE_NOLOAD ?

	2 IMAGE_SCN_TYPE_GROUP ?

	3 IMAGE_SCN_TYPE_NO_PAD ?

	4 IMAGE_SCN_TYPE_COPY ?

	5 IMAGE_SCN_CNT_CODE - the section contains
	executable code.

	6 IMAGE_SCN_CNT_INITIALIZED_DATA  - the section
	contains data that gets a defined value before execution starts. In
	other words: the section's data in the file is meaningful.

	7 IMAGE_SCN_CNT_UNINITIALIZED_DATA - this section's
	data is uninitialized data and will be initialized to all-0-bytes
	before execution starts. This is normally the BSS.

	8 IMAGE_SCN_LNK_OTHER ?

	9 IMAGE_SCN_LNK_INFO - the section doesn't contain
	image data but comments, description or other documentation.

	10 IMAGE_SCN_TYPE_OVER ?

	11 IMAGE_SCN_LNK_REMOVE - the data is part of an
	object file's section that is supposed to be left out when the
	executable file is linked.

	12 IMAGE_SCN_LNK_COMDAT - the section contains
	"common block data", which are packaged functions of some sort.

	13

	14

	15 IMAGE_SCN_MEM_FARDATA - we have far data -
	whatever that means. This bit's meaning is unsure.

	16

	17 IMAGE_SCN_MEM_PURGEABLE - the section's data
	is purgeable - but I don't think that this is the same as
	"discardable", which has a bit of its own, see below.
	The same bit is apparently used to indicate 16-bit-information as
	there is also a define IMAGE_SCN_MEM_16BIT for it.
	This bit's meaning is unsure.

	18 IMAGE_SCN_MEM_LOCKED - the section should not be
	moved in memory? This bit's meaning is unsure.

	19 IMAGE_SCN_MEM_PRELOAD - the section should be
	paged in before execution starts? This bit's meaning is unsure.

	20 - 23 ALIGN GROUP  - specify an alignment that I have no information
	about. There are #defines IMAGE_SCN_ALIGN_16BYTES and the like. The
	only value I've ever seen used is 0, for the default 16-byte-
	alignment. I suspect that this is the alignment of objects in a
	library file or the like.

	24 IMAGE_SCN_LNK_NRELOC_OVFL - the section contains
	some extended relocations that I don't know about.

	25 IMAGE_SCN_MEM_DISCARDABLE - the section's data is
	not needed after the process has started. This is the case,
	for example, with the relocation information. I've seen it also for
	startup routines of drivers and services that are only executed
	once.

	26 IMAGE_SCN_MEM_NOT_CACHED - the section's data
	should not be cached. Don't ask my why not. Does this mean to switch
	off the 2nd-level-cache?

	27 IMAGE_SCN_MEM_NOT_PAGED - the section's data
	should not be paged out. This may be interesting for drivers.

	28 IMAGE_SCN_MEM_SHARED - the section's data is
	shared among all running instances of the image. If it is e.g. the
	initialized data of a DLL, all running instances of the DLL will at
	any time have the same variable contents.
	Note that only the first instance's section is initialized.
	Sections containing code are always shared.

	29 IMAGE_SCN_MEM_EXECUTE - the process gets
	'execute'-access to the section's memory.

	30 IMAGE_SCN_MEM_READ -  the process gets
	'read'-access to the section's memory.

	31 IMAGE_SCN_MEM_WRITE - the process gets
	'write'-access to the section's memory.
--------------------------------------------------------------------------------*/


struct CCFILEPE_SECTION_DESCRIPTOR
{
	char	SectionName[8];
	int	VirtualSize;		// In .OBJ files: PhysicalAddress
	int	VirtualAddress;
	int	SizeOfRawData;
        int	PointerToRawData;
        int	PointerToRelocations;	// |
        int	PointerToLinenumbers;	// |	This fields used only in
        short	NumberOfRelocations;	// |	object files
        short	NumberOfLinenumbers;	// |
        int	Characteristics;
};

//--------------------------------------------------------------------

struct CCFILEPE_IMPORT_DESCRIPTOR
{
	int	OriginalFirstThunk;		// = Characteristics [?]
	int	TimeDateStamp;			// Usually ignored: 0 or FFFFFFFF
	int	ForwarderChain;			// Usually ignored: 0 or FFFFFFFF
	int	Name1;
	int	FirstThunk;
};

//--------------------------------------------------------------------

struct CCFILEPE_IMAGE_IMPORT_BY_NAME
{
	short	Hint;	// Index in dll (can be ignored)
	signed char	Name1;
};


#endif