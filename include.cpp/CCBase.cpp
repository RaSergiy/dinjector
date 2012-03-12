#include "CCBase.h"

int	CCErrorTo	= CCERROR_TO_CONSOLE;

//--------------------------------------------------------------------

CString CCBaseBuffer::ClassName ( )
{	return "CCBaseBuffer";
}

CCBaseBuffer::CCBaseBuffer ( )
{	count	= 0;
	buffer	= 0;
}

CCBaseBuffer::CCBaseBuffer ( int size )
{	CCBaseBuffer ();
	BufferAlloc ( size );
}

CCBaseBuffer::~CCBaseBuffer ( )
{	BufferFree ( );
}
