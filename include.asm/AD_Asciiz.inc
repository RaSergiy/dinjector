;------------------------------------------------

asciizCompareL proc pStr1:DWORD, pStr2:DWORD, nLength:DWORD
	pushad
	mov	ecx, nLength
	mov	esi, pStr1
	mov	edi, pStr2
	repz	cmpsb
	popad
	ret
asciizCompareL endp



;------------------------------------------------

asciizCopy proc pSrc:DWORD, pDest:DWORD
	push	esi
	push	edi
	mov	esi, pSrc
	mov	edi, pDest
ascl1:	lodsb
	stosb
	test	al, al
	jnz	ascl1
	mov	eax, edi
	sub	eax, pDest
	pop	edi
	pop	esi
	ret
asciizCopy endp

;------------------------------------------------

ConsoleInit proc
	pushad
	invoke	GetStdHandle, STD_OUTPUT_HANDLE
	mov	__hConsole, eax
	popad
	ret
ConsoleInit endp

;------------------------------------------------

ConsoleClose proc
	pushad
	invoke	CloseHandle, __hConsole
	popad
	ret
ConsoleClose endp

;------------------------------------------------

asciizLength proc pString:DWORD
	push	esi
	push	ecx
	xor	ecx, ecx
	mov	esi, pString
aslf1:		inc	ecx
		lodsb
		test	al,al
		jnz	aslf1
	mov	eax, ecx
	pop	ecx
	pop	esi
	ret
asciizLength endp

;------------------------------------------------

asciizPrint proc	pString:DWORD, nChars:DWORD
	pushad
	xor	edx, edx
	invoke	WriteConsoleA, __hConsole, pString, nChars, edx, edx
	popad
	ret
asciizPrint endp

;------------------------------------------------


asciizPrintC proc	pString:DWORD
	push	eax
	invoke	asciizLength, pString
	dec	eax
	invoke	asciizPrint, pString, eax
	pop	eax
	ret
asciizPrintC endp

;------------------------------------------------

asciizPrintN proc	pString:DWORD
	invoke	asciizPrintC, pString
	invoke	asciizPrint,  offset __CRLF, 2
	ret
asciizPrintN endp


;------------------------------------------------

Bin2Hex32 proc	dwNumber:DWORD, pString:DWORD
	pushad
	pushf
	std
	mov	ebx, offset __hex_xlat
	mov	edi, pString
	mov	ecx, 8
	add	edi, ecx
	dec	edi
	mov	eax, dwNumber
abh32:		push	eax
		and	eax, 0Fh
		xlat
		stosb
		pop	eax
		shr	eax, 04h
		loop	abh32
		xchg	eax,edi
	popf
	popad
	ret
Bin2Hex32 endp

;------------------------------------------------

PrintHex32 proc	dwNumber:DWORD
	invoke	Bin2Hex32,   dwNumber, offset __hex_build
	invoke	asciizPrint, offset __hex_build, 8
	ret
PrintHex32 endp


;------------------------------------------------

asciizPrintHex32 proc	pString:DWORD, dwNumber:DWORD
	invoke	asciizPrintC, pString
	invoke	PrintHex32,   dwNumber
	invoke	asciizPrint, offset __CRLF, 2
	ret
asciizPrintHex32 endp


;------------------------------------------------

memIsZero proc	pMem:DWORD, Count:DWORD
	push	ecx
	push	edi
	mov	edi, pMem
	mov	ecx, Count
	xor	eax, eax
	repz	scasb
	jz	lbmisz1
	inc	eax
lbmisz1:pop	edi
	pop	ecx
	ret
memIsZero endp

;------------------------------------------------

memCopy proc	pSrc:DWORD, pDest:DWORD, nBytes:DWORD
	pushad
	mov	esi, pSrc
	mov	edi, pDest
	mov	ecx, nBytes
	rep	movsb
	popad
	ret
memCopy endp


;------------------------------------------------

memSetZero proc	pMem:DWORD, Count:DWORD
	pushad
	mov	edi, pMem
	mov	ecx, Count
	xor	AX, AX
	rep	stosb
	popad
	ret
memSetZero endp














