#include "CCFile.h"

 CCFile ::
~CCFile ( )
{
	Close ( );
	BufferFree ( );
}



CCFile ::
CCFile ( ) : CCBaseBuffer ( )
{

	file_handle = 0;
}



CCFile ::
CCFile(	CString	filepath, int access, int creation, int attrib)
{
	file_handle = 0;
	Open ( filepath, access, creation, attrib );
}



CString CCFile::ClassName ( )
{
	return "CCFile";
}


//--------------------------------------------------------------------

void CCFile::CCError ( int error_code )
{
	CString errfn;
	switch ( error_code )
	{
		case CCFILE_OPERATION_READ:
			errfn = "ReadFile";
			break;
		case CCFILE_OPERATION_WRITE:
			errfn = "WriteFile";
			break;
		case CCFILE_OPERATION_SEEK:
			errfn = "SetFilePos";
			break;
		case CCFILE_OPERATION_OPEN:
			errfn = "OpenFile";
			break;
		case CCFILE_OPERATION_GETFILESIZE:
			errfn = "GetFileSize";
			break;
		case CCFILE_OPERATION_GETFULLPATHNAME:
			errfn = "GetFullPathName";
			break;
		case CCFILEPE_ERROR_NOTMZ:
			errfn = "MZ_MAGIC_NOT_FOUND:NOT_EXE_FILE";
			break;
		case CCFILEPE_ERROR_NOTPE:
			errfn = "PE_MAGIC_NOT_FOUND:NOT_EXE_FILE";
			break;
		case CCFILEPE_ERROR_NE_NOTSUPPORTED:
			errfn = "NE-File detected. NE-Files not supported in the current version";
			break;
		default:
			errfn = "UNKNOWN!";
	}
	errfn += " ( \""+file_path+"\" ) ";
	CCBaseBuffer::CCError ( errfn );
}

//====================================================================

BOOL CCFile::Open ( CString  path, unsigned int access, unsigned int creation, unsigned int attrib )
{
	file_access = access;
	file_creation= creation;
	file_attrib = attrib;
	if ( file_handle ) Close ( );
	file_handle = CreateFile
		(
			path, 		// lpFileName			pointer to name of the file
			access, 	// dwDesiredAccess		access (read-write) mode
			0, 		// dwShareMode			share mode
			0,		// lpSecurityAttributes		pointer to security attributes
			creation,	// dwCreationDistribution	how to create
			attrib,		// dwFlagsAndAttributes		file attributes
			0	   	// hTemplateFile 		handle to file with attributes to copy
		);
	if ( file_handle == INVALID_HANDLE_VALUE)
	{
		file_path   = path;
		CCError ( CCFILE_OPERATION_OPEN );
		return false;		// Только если будет возврат из CCError
	}
	file_path   = path;
	int pthlen = GetFullPathName ( path, 0, 0, 0 );
	if ( ! pthlen )
		CCError ( CCFILE_OPERATION_GETFULLPATHNAME );
	LPTSTR pfilepart;
	LPTSTR pfilefullpath = file_path.GetBuffer( pthlen );
	pthlen = GetFullPathName ( path, pthlen, pfilefullpath, &pfilepart);
	if ( ! pthlen )
		CCError ( CCFILE_OPERATION_GETFULLPATHNAME );
	file_name = pfilepart;
	file_path.ReleaseBuffer  ( );
	file_dir = file_path.Mid ( 0, (int)pfilepart - (int)pfilefullpath );
	return true;
}

//--------------------------------------------------------------------

BOOL CCFile::ReOpen ( )
{
	file_creation = OPEN_EXISTING;
	if ( file_handle ) Close ( );
	file_handle = CreateFile ( file_path, file_access, 0, 0, file_creation, file_attrib, 0 );
	if ( file_handle == INVALID_HANDLE_VALUE )
			CCError ( CCFILE_OPERATION_OPEN );
		else
			return true;
	return false;
}

//--------------------------------------------------------------------

BOOL CCFile::Read ( void* pbuf, int nbytes )
{
	unsigned int readed;

	if ( nbytes == -1 )
	{
		this->Seek ( );
		nbytes = this->Size ( );
	}
	if ( !pbuf )
        {
		BufferAlloc ( nbytes );
		pbuf = buffer;
	}
	if ( ! ReadFile ( file_handle, pbuf, nbytes, (LPDWORD)&readed, NULL ) || nbytes != readed )
	{
		CCError ( CCFILE_OPERATION_READ );
		return false;
	}
	return true;
}

//--------------------------------------------------------------------

BOOL CCFile::Write( void* pbuf, int nbytes )
{
	unsigned int w;
	if ( ! nbytes )
		return true;
	if ( nbytes == -1 )
		nbytes = this->count;
	if ( ! pbuf )
		pbuf = buffer;
	if (! WriteFile	(	file_handle,	//	HANDLE	hFile			handle to file to write to
				pbuf,		//	LPCVOID	lpBuffer		pointer to data to write to file
				nbytes,		//	DWORD	nNumberOfBytesToWrite	number of bytes to write
				(LPDWORD)&w,	//	LPDWORD	lpNumberOfBytesWritten,	pointer to number of bytes written
				NULL		//	LPOVERLAPPED lpOverlapped 	pointer to structure needed for overlapped I/O
			) )
		{
			CCError ( CCFILE_OPERATION_WRITE );
			return false;
		}
	if ( w == nbytes ) return true;
	CCError ( CCFILE_OPERATION_WRITE );
	return false;
}

//--------------------------------------------------------------------

BOOL CCFile::WriteS ( CString* write_string, BOOL Asciiz )
{
	if ( ! Write ( write_string->GetBuffer ( ), write_string->GetLength() ) )
		return false;
	if ( Asciiz )
		return this->WriteZeroes( 1 );
	return true;
}

//--------------------------------------------------------------------

BOOL CCFile::Seek ( unsigned long int filepos, unsigned int move_method )
{
	unsigned int movehighdw = (filepos & 0xFFFFFFFF00000000) >> 32;
	unsigned int ret = SetFilePointer
		(
			file_handle,
			(unsigned int) filepos,
			(PLONG)&movehighdw,
			move_method
		);
	if ( ( ret == 0xFFFFFFFF ) && (GetLastError ( ) != NO_ERROR ) )
	{
		CCError ( CCFILE_OPERATION_SEEK );
		return false;
	}
	return true;
}

//--------------------------------------------------------------------

BOOL CCFile::WriteFromFile ( CCFile* src, int nbytes )
{
	if ( nbytes == -1 )
	{
		src->Seek ( 0 );
		nbytes= src->Size( );
	}
	int n;
	BufferAlloc ( CCFILE_BUFFER_SIZE );

	do {
		n = nbytes;
		if ( n > CCFILE_BUFFER_SIZE) n = CCFILE_BUFFER_SIZE;
		if ( ! src->Read ( buffer, n ) ) return false;
		if ( ! Write   ( buffer, n ) ) return false;
		nbytes -= n;
	} while ( nbytes != 0 );
	return true;
}


//--------------------------------------------------------------------

BOOL CCFile::WriteZeroes ( int nbytes )
{
	int n;
	if ( ! nbytes ) return true;
	BufferAlloc ( CCFILE_BUFFER_SIZE );
	ZeroMemory ( buffer, CCFILE_BUFFER_SIZE );
	do {
		n = nbytes;
		if ( n > CCFILE_BUFFER_SIZE) n = CCFILE_BUFFER_SIZE;
		if ( ! Write   ( buffer, n ) ) return false;
		nbytes -= n;
	} while ( nbytes != 0 );
	return true;
}


//--------------------------------------------------------------------

void CCFile::ShiftData ( int src_pos, int dest_pos, int data_size )
{
	if ( data_size == 0 ) return;
	if ( data_size == -1 )
		data_size = Size( ) - src_pos;
	BufferAlloc ( data_size );
	Seek ( src_pos );
	Read ( buffer, data_size );
	Seek ( dest_pos );
	Write ( buffer, data_size );
	BufferAlloc (0);
}


//--------------------------------------------------------------------

void CCFile::Truncate ( int position )
{
	Seek ( position );
	SetEndOfFile ( file_handle );
}


//--------------------------------------------------------------------

long int CCFile::Size ( )
{
	int high,ret;
	ret = GetFileSize ( file_handle, (LPDWORD)&high );
	if ( ( ret == 0xFFFFFFFF ) && ( GetLastError ( ) != NO_ERROR ) )
		CCError ( CCFILE_OPERATION_GETFILESIZE);
	return ( ret + high );
}

//--------------------------------------------------------------------

void CCFile::Close ( )
{
	if ( file_handle )
	{
		CloseHandle ( file_handle );
		file_handle = 0;
	}
};

//--------------------------------------------------------------------

BOOL CCFile::OpenCopy ( CString  path_src, CString path_copy )
{
	CCFile original_file;

	if ( ! original_file.Open (path_src, GENERIC_READ ) ) return false;
	if ( ! Open ( path_copy, GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS ) ) return false;
	Truncate();
	BOOL res = WriteFromFile(&original_file, -1);
	original_file.Close ();
	return res;
}

//--------------------------------------------------------------------

BOOL CCFile::CreateTemp ( )
{
	CString temp_path;
	srand( (unsigned)time ( NULL ) );
	temp_path.GetEnvironmentVariable ( "TEMP" );
	if ( temp_path.GetAt ( temp_path.GetLength ( ) ) != '\\' )
		temp_path += "\\";
	for (int i=0; i < 8; i++)
		temp_path += (char)( rand( )%26 + 0x41 );
	temp_path += ".tmp";
	if ( ! Open ( temp_path, GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS ) ) return false;
	Truncate ( );
	return true;
}

//--------------------------------------------------------------------


BOOL CCFile::Delete ()
{
	Close ( );
	return DeleteFile ( file_path );
}
