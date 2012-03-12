;=======================================
;
; Заголовки PE-файлов и прочее выдранное
; из Windows.inc
;
;=======================================

HDR_MZ STRUCT
	e_magic		WORD	?
	e_cblp		WORD	?
	e_cp		WORD	?
	e_crlc		WORD	?
	e_cparhdr	WORD	?
	e_minalloc	WORD	?
	e_maxalloc	WORD	?
	e_ss		WORD	?
	e_sp		WORD	?
	e_csum		WORD	?
	e_ip		WORD	?
	e_cs		WORD	?
	e_lfarlc	WORD	?
	e_ovno		WORD	?
	e_res		WORD	4 dup(?)
	e_oemid		WORD	?
	e_oeminfo	WORD	?
	e_res2		WORD	10 dup(?)
	e_lfanew	DWORD	?
HDR_MZ ENDS


HDR_DATADIR STRUCT
	VirtualAddress	DWORD	?
	isize		DWORD	?
HDR_DATADIR ENDS


HDR_PE STRUCT
	e_magic				DWORD	?
	; ----------------------------- File Header
	Machine				WORD	?
	NumberOfSections		WORD	?
	TimeDateStamp			DWORD	?
	PointerToSymbolTable		DWORD	?
	NumberOfSymbols			DWORD	?
	SizeOfOptionalHeader		WORD	?
	Characteristics			WORD	?
	;------------------------------ Optional Header
	Magic				WORD	?
	MajorLinkerVersion		BYTE	?
	MinorLinkerVersion		BYTE	?
	SizeOfCode			DWORD	?	; !
	SizeOfInitializedData		DWORD	?	; !
	SizeOfUninitializedData		DWORD	?	; !
	AddressOfEntryPoint		DWORD	?
	BaseOfCode			DWORD	?
	BaseOfData			DWORD	?
	ImageBase			DWORD	?
	SectionAlignment		DWORD	?
	FileAlignment			DWORD	?
	MajorOperatingSystemVersion	WORD	?
	MinorOperatingSystemVersion	WORD	?
	MajorImageVersion		WORD	?
	MinorImageVersion		WORD	?
	MajorSubsystemVersion		WORD	?
	MinorSubsystemVersion		WORD	?
	Win32VersionValue		DWORD	?
	SizeOfImage			DWORD	?	; !
	SizeOfHeaders			DWORD	?
	CheckSum			DWORD	?	; ?
	Subsystem			WORD	?
	DllCharacteristics		WORD	?
	SizeOfStackReserve		DWORD	?
	SizeOfStackCommit		DWORD	?
	SizeOfHeapReserve		DWORD	?
	SizeOfHeapCommit		DWORD	?
	LoaderFlags			DWORD	?
	NumberOfRvaAndSizes		DWORD	?
	;------------------------------ Data directories
	DD_ExportTable			HDR_DATADIR <>
	DD_ImportTable			HDR_DATADIR <>
	DD_ResourceTable		HDR_DATADIR <>
	DD_ExceptionTable		HDR_DATADIR <>
	DD_CertificateTable		HDR_DATADIR <>
	DD_RelocationTable		HDR_DATADIR <>
	DD_DebugData			HDR_DATADIR <>
	DD_ArchitectureSpecificData	HDR_DATADIR <>
	DD_MachineValue			HDR_DATADIR <>
	DD_TLS_Table			HDR_DATADIR <>
	DD_LoadConfigurationTable	HDR_DATADIR <>
	DD_BoundImportTable		HDR_DATADIR <>
	DD_ImportAddressTable		HDR_DATADIR <>
	DD_DelayImportDescriptor	HDR_DATADIR <>
	DD_ComRunTimeHeader		HDR_DATADIR <>
	DD_Reserved			HDR_DATADIR <>
HDR_PE ENDS


SECTION_DESCRIPTOR STRUCT
	SectionName		db	8 dup (?)
	VirtualSize		dd	?	; In .OBJ files: PhysicalAddress
	VirtualAddress		dd	?
	SizeOfRawData		dd	?
        PointerToRawData	dd	?
        PointerToRelocations	dd	?	; ──┐
        PointerToLinenumbers	dd	?	;   ├── This fields used only in
        NumberOfRelocations	dw	?	;   │   object files
        NumberOfLinenumbers	dw	?	; ──┘
        Characteristics		dd	?
SECTION_DESCRIPTOR ENDS


HDR_IMPORT_DESCRIPTOR STRUCT
	OriginalFirstThunk	dd	?	; = Characteristics [?]
	TimeDateStamp		dd	?	; Usually ignored: 0 or FFFFFFFF
	ForwarderChain		dd	?	; Usually ignored: 0 or FFFFFFFF
	Name1			dd	?
	FirstThunk		dd	?
HDR_IMPORT_DESCRIPTOR ENDS


IMAGE_IMPORT_BY_NAME STRUCT
	Hint	dw	?	; Index in dll (can be ignored)
	Name1	db	?
IMAGE_IMPORT_BY_NAME ENDS


OVERLAPPED STRUCT
	Internal	DWORD	?
	InternalHigh	DWORD	?
	loffset		DWORD	?
	OffsetHigh	DWORD	?
	hEvent		DWORD	?
OVERLAPPED ENDS
