

#ifndef CCBase_H
#define	CCBase_H

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <strstream>
#include <stdlib.h>
#include <fstream>
#include <atlstr.h>
#include <windows.h>

using namespace std;


#define ALIGN(variable,align)	(variable) % (align) ? (align) * ( 1 + (variable) / (align)) : (variable)

//====================================================================
//====================================================================
//====================================================================
/*
	ќбъ€вл€ютс€ классы
		1: CCBase	: template <class CCBaseObject>
		2: CCBaseBuffer : public CCBase <unsigned char>

*/



//--------------------------------------------------------------------

enum
{
	CCERROR_TO_MSGBOX	= 1,
	CCERROR_TO_CONSOLE	= 2,

};

extern int   CCErrorTo;


//====================================================================
//====================================================================
//====================================================================


template <class CCBaseObject>
class CCBase
{
public:

//--------------------------------------------------------------------


	int		count;
	CCBaseObject*	buffer;

/*
	virtual CString ClassName ( )
	virtual void	CCError		( char* error_message );
	CCBaseObject*	BufferAlloc	( int size );
	CCBaseObject*	BufferResize	( int size );
	void		BufferCopyTo	( CCBase <CCBaseObject>* destination );
	void		BufferFree	( );
	virtual void	Add		( CCBaseObject object )

*/

//====================================================================
//====================================================================
//====================================================================


//--------------------------------------------------------------------

CCBase ( )
{
	count = 0;
	buffer = 0;

}

virtual CString ClassName ( )
{
	return "CCBase";
}

//--------------------------------------------------------------------

//virtual
void CCError ( CString error_function )
{
	CString note = "FATAL ERROR < " + ClassName( ) + "::" + error_function + ">";
	CString str1,str2;
	int errcode = GetLastError();
	if (errcode != NO_ERROR )
	{
		void*   errget;
		FormatMessage
		(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			errcode,
			0,			// Default language
			(LPTSTR) &errget,
			0,
			NULL
		);
		str1 = (LPTSTR) errget;
		LocalFree (errget);
	}

	switch ( CCErrorTo )
	{
		case CCERROR_TO_CONSOLE:
			CharToOem(str1, str2.GetBuffer(str1.GetLength()+1) );
			str2.ReleaseBuffer();
			cout << "\n" << note << "\n" << str2 << "\n";
			break;
		case CCERROR_TO_MSGBOX:
		default:
			MessageBox ( NULL, str2, note, MB_OK | MB_ICONERROR );
	};
	ExitProcess ( -1 );
}


void CCErrorMalloc ( )
{
	if ( GetLastError ( ) == NO_ERROR ) SetLastError ( ERROR_NOT_ENOUGH_MEMORY );
	CCError("malloc");
}

//====================================================================
//====================================================================
//====================================================================

CCBaseObject* BufferAlloc ( int size )
{
	if ( buffer )
		free ( buffer );
	buffer = (CCBaseObject*) malloc ( size * sizeof(CCBaseObject) );
	if ( ! buffer )
		CCErrorMalloc( );
	count = size;
	return buffer;
}

//--------------------------------------------------------------------

CCBaseObject* BufferResize ( int size )
{
	if ( ! size )
	{
		BufferFree ( );
		return 0;
	}

	if ( size == count ) return buffer;
	if ( ! buffer )	return BufferAlloc ( size );

	int newsize = size*sizeof(CCBaseObject);
	int oldsize = count*sizeof(CCBaseObject);
	int copycnt = newsize > oldsize ? oldsize : newsize;

	void* tmpptr = malloc(newsize);
	memset(tmpptr, 0, newsize);
	memcpy(tmpptr, (void*)buffer, copycnt);
	free(buffer);
	buffer = (CCBaseObject*)tmpptr;

	count = size;
	return buffer;
}

//--------------------------------------------------------------------

void BufferFree ( )
{
	free ( buffer );
	buffer = 0;
	count  = 0;
}

//--------------------------------------------------------------------

void BufferCopyTo ( CCBase <CCBaseObject>* destination )
{
	destination->BufferFree();
	if ( ! buffer ) return;
	memcpy ( destination->BufferResize(count), buffer, count * sizeof(CCBaseObject) );
}

//--------------------------------------------------------------------

virtual
void Add ( CCBaseObject object )
{
	BufferResize ( count + 1 );
	buffer [ count - 1 ] = object;
}

CCBaseObject Cut ( int index )
{
	CCBaseObject RET = buffer [ index ];
	for ( int i=index; i < (count-1); i++ )
		buffer [i] = buffer [i+1];
	BufferResize( count - 1);
	return RET;
}

//====================================================================
//=========================== OP OVERLOADS ===========================
//====================================================================

CCBaseObject operator [ ] ( int index )
{
	return buffer [ index ];
}


}; // End of template



//--------------------------------------------------------------------
//--------------------------------------------------------------------

//====================================================================
//====================================================================
//====================================================================

//--------------------------------------------------------------------
//--------------------------------------------------------------------


class CCBaseBuffer : public CCBase <unsigned char>
{
public:

	virtual CString ClassName ( );
	CCBaseBuffer ( );
	CCBaseBuffer ( int size );
	~CCBaseBuffer ( );

};

#endif		// CCBase_H
