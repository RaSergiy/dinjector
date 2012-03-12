
CommandLineArg	PROTO		; -> EAX=0 if no arg, EAX=ptr to arg
paraAlign	PROTO	Number:DWORD,  nAlign:DWORD	; -> EAX = Aligned number
ioOpenFile	PROTO	pHandle:DWORD, pPath:DWORD, pdqSize:DWORD		; -> pHandle=file_handle, pdqSize=filesize
ioRead		PROTO	Handle:DWORD,  pBuf:DWORD,  FilePos:DWORD, nBytes:DWORD	; -> EAX=result
ioWrite		PROTO 	Handle:DWORD,  pBuf:DWORD,  FilePos:DWORD, nBytes:DWORD ; -> EAX=result
ioReadAsciiz	PROTO	Handle:DWORD,  pBuf:DWORD,  FilePos:DWORD		; -> EAX=bytes readen, EAX=0 if eror
ioReadDwordz	PROTO	Handle:DWORD,  pBuf:DWORD,  FilePos:DWORD		; -> EAX=bytes readen, EAX=0 if eror


__io_overlapped	OVERLAPPED	<0>