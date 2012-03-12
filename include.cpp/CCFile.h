#ifndef CCFILE_H
#define CCFILE_H

#include <cstring>
#include <time.h>
#include "CCBase.h"

#define CCFILE_BUFFER_SIZE 16384	// 16kb buffer

class CCFile : public CCBaseBuffer
{

public:
	HANDLE		file_handle;

	unsigned int	file_access;
	unsigned int	file_attrib;
	unsigned int	file_creation;


	CString		file_path;
	CString		file_dir;
	CString		file_name;

	enum
	{
		CCFILE_OPERATION_READ			= 0,
		CCFILE_OPERATION_WRITE			= 1,
		CCFILE_OPERATION_SEEK			= 2,
		CCFILE_OPERATION_OPEN			= 4,
		CCFILE_OPERATION_GETFILESIZE		= 5,
		CCFILE_OPERATION_GETFULLPATHNAME	= 6,
		CCFILEPE_ERROR_NOTMZ			= 0x20,
		CCFILEPE_ERROR_NOTPE			= 0x21,
		CCFILEPE_ERROR_NE_NOTSUPPORTED		= 0x22,
		CCFILE_OPERATION_NOTSUPPORTED		= 0xFFFF,
	};

//====================================================================

	virtual CString ClassName ( );
	void	CCError ( int error_code );	// Override of CCBaseBuffer::CCError (char* msg)

	~CCFile ( );
	 CCFile ( );
	 CCFile ( CString filepath,
		  int	access   = GENERIC_READ | GENERIC_WRITE,
		  int	creation = OPEN_EXISTING,
		  int	attrib   = FILE_ATTRIBUTE_NORMAL );

//--------------------------------------------------------------------

	BOOL Open ( CString  path,
		    unsigned int access = GENERIC_READ | GENERIC_WRITE,
	            unsigned int creation = OPEN_EXISTING,
	            unsigned int attrib = FILE_ATTRIBUTE_NORMAL );
	BOOL ReOpen ( );
	BOOL OpenCopy ( CString  path_src, CString path_copy );

	BOOL CreateTemp ( );

	void Close ( );
	BOOL Delete ();

//--------------------------------------------------------------------


	long int Size ( );

//--------------------------------------------------------------------
// READ/WRITE:	Если nbytes=-1 то nbytes=FileSize
// 		Если pbuf=0 то pbuf=CCBase::buffer


	BOOL Read	( void* pbuf=0, int nbytes=-1 );
	BOOL Write	( void* pbuf=0, int nbytes=-1 );
	BOOL WriteS	( CString* write_string, BOOL Asciiz = false );

//--------------------------------------------------------------------

	BOOL WriteFromFile	( CCFile* src, int nbytes=-1);
	BOOL WriteZeroes	( int nbytes );
	void ShiftData		( int src_pos, int dest_pos, int data_size = -1 );	// default data_size = EOF - src_pos

//--------------------------------------------------------------------

	BOOL Seek ( unsigned long int filepos = 0, unsigned int move_method = FILE_BEGIN );
	void Truncate ( int position = 0 );

};



#endif				// CCFILE_H
