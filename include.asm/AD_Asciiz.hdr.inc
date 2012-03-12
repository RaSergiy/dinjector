
ConsoleInit		PROTO
ConsoleClose		PROTO

asciizCompareL		PROTO	pStr1:DWORD,    \
				pStr2:DWORD,    \
				nLength:DWORD	; -> Z-Flag
asciizCopy		PROTO	pSrc:DWORD,     \
				pDest:DWORD	; -> EAX = Bytes copied (with 0)
asciizLength		PROTO	pString:DWORD	; -> EAX = Length (with 0)

asciizPrint		PROTO	pString:DWORD,  \
				nChars:DWORD	; Print N Chars
asciizPrintC		PROTO	pString:DWORD
asciizPrintN		PROTO	pString:DWORD
asciizPrintHex32	PROTO	pString:DWORD,	\
				dwNumber:DWORD

Bin2Hex32		PROTO	dwNumber:DWORD, pString:DWORD
PrintHex32		PROTO	dwNumber:DWORD

memIsZero		PROTO	pMem:DWORD,     Count:DWORD	; -> EAX=0 if mem is zero
memCopy			PROTO	pSrc:DWORD,     pDest:DWORD,    nBytes:DWORD
memSetZero		PROTO	pMem:DWORD,     Count:DWORD



__hConsole		dd	0
__CRLF			db	13,10
__hex_xlat		db	"0123456789ABCDEF"
__hex_build		db	8 dup ("X")