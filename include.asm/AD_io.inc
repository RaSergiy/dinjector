


CommandLineArg proc
LOCAL	rv:DWORD
	pusha
	invoke  GetCommandLine
	mov	edi, eax
	mov	ebx, eax
	invoke  lstrlen, edi
	test	eax, eax
	jz	lclerr
	inc	edi
	mov	ecx, eax
	mov	edx, eax
	mov	al,22h
	repnz	scasb
	jnz	lcl1
	inc	edi
	jmp	lcl2
lcl1:	mov	edi, ebx
	mov	ecx, edx
	mov	al,20h
	repnz	scasb
	jnz	lclerr
	mov	rv, edi
	jmp	lcl2
lclerr:	mov	rv, 0
lcl2:	popa
	mov	eax, rv
	ret
CommandLineArg endp

;------------------------------------------------

paraAlign proc Number:DWORD, nAlign:DWORD
	push	ebx
	push	edx
	mov	eax, Number
	push	eax
	mov	ebx, nAlign
	xor	edx, edx
	div	ebx
	test	edx, edx
	jz	lpala1
	add	esp, 4
	inc	eax
	mul	bx
	push	dx
	push	ax
lpala1:	pop	eax
	pop	edx
	pop	ebx
	ret
paraAlign endp

;------------------------------------------------

ioOpenFile proc pHandle:DWORD, pPath:DWORD, pdqSize:DWORD
	pusha
	invoke	CreateFile, pPath, GENERIC_READ or GENERIC_WRITE,   \
				FILE_SHARE_READ, 0, OPEN_EXISTING,  \
				FILE_ATTRIBUTE_NORMAL, 0
	mov	edi, pHandle
	stosd
	cmp	eax, INVALID_HANDLE_VALUE
	jz	lioof1
	mov	ebx, pdqSize
	push	ebx
	add	ebx, 4
	invoke	GetFileSize, eax, ebx
	pop	edi
	stosd
lioof1:	popa
	ret
ioOpenFile endp

;------------------------------------------------

ioRead proc	Handle:DWORD, pBuf:DWORD, FilePos:DWORD, nBytes:DWORD
	pushad
	invoke	memSetZero, offset __io_overlapped, SIZEOF (OVERLAPPED)
	mov	eax, FilePos
	mov	__io_overlapped.loffset, eax
	invoke	ReadFile, Handle, pBuf, nBytes, 0, offset __io_overlapped
	mov	pBuf,eax
	popad
	mov	eax,pBuf
	ret
ioRead endp

;------------------------------------------------

ioWrite proc 	Handle:DWORD, pBuf:DWORD, FilePos:DWORD, nBytes:DWORD
	pushad
	invoke	memSetZero, offset __io_overlapped, SIZEOF (OVERLAPPED)
	mov	eax, FilePos
	mov	__io_overlapped.loffset, eax
	invoke	WriteFile, Handle, pBuf, nBytes, 0, offset __io_overlapped
	mov	pBuf, eax
	popad
	mov	eax, pBuf
	ret
ioWrite endp

;------------------------------------------------

ioReadAsciiz proc Handle:DWORD, pBuf:DWORD, FilePos:DWORD
	pushad
	mov	ebx, pBuf
	mov	esi, ebx
	mov	edi, ebx
	mov	edx, FilePos
liraf1:	invoke	ioRead, Handle, ebx , edx , 1
		test	eax, eax
		jz	liraf2
		inc	ebx
		inc	edx
		lodsb
		test	al, al
		jnz	liraf1
	sub	ESI, EDI
	mov	EAX, ESI
liraf2:	mov	pBuf, eax
	popad
	mov	eax, pBuf
	ret
ioReadAsciiz endp

;------------------------------------------------

ioReadDwordz proc Handle:DWORD, pBuf:DWORD, FilePos:DWORD
	pushad
	mov	ebx, pBuf
	mov	esi, ebx
	mov	edi, ebx
	mov	edx, FilePos
	mov	ecx, 4
lirdf1:		invoke	ioRead, Handle, ebx , edx , ecx
		test	eax, eax
		jz	lirdf2
		add	ebx, ecx
		add	edx, ecx
		lodsd
		test	eax, eax
		jnz	lirdf1
	sub	ESI, EDI
	mov	EAX, ESI
lirdf2:	mov	pBuf, eax
	popad
	mov	eax, pBuf
	ret
ioReadDwordz endp


;------------------------------------------------
