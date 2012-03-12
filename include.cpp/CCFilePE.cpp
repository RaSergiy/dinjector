#include "CCFilePE.h"

using namespace std;

//====================================================================
//====================================================================
//====================================================================

//CCSection::CCSection ( ) { Unmovable=false; }
CString CCSection  :: ClassName ( ) { return "CCSection";  };
CString CCSections :: ClassName ( ) { return "CCSections"; };

//--------------------------------------------------------------------

CString CCSection::GetName ( )
{	CString str;	int i=0;
	while ( (i<8) && (descriptor.SectionName[i]) )
        	str+=descriptor.SectionName[i++];
	return str;
}
//--------------------------------------------------------------------

void CCSection::SetName ( CString name )
{	ZeroMemory ( &descriptor.SectionName , 8 );
	int namelen = name.GetLength() % 9;
	for (int i=0; i < namelen; i++)
		descriptor.SectionName[i] = name.GetAt(i);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

void CCSections:: Delete ( CCSection* section )
{	for (int i=0; i<count; i++)
		if (buffer[i] == section )
                	{ Cut ( i ); return; }
}

CCSection* CCSections::FindByRVA ( int RVA )
{
	for ( int i=0; i < this->count; i++)
        {
        	int sVA = this->buffer[i]->descriptor.VirtualAddress;
        	int sVAE = sVA + this->buffer[i]->descriptor.VirtualSize;
		if ( (RVA >= sVA ) && (RVA <= sVAE ) ) return this->buffer[i];
        }
        return 0;
}

//--------------------------------------------------------------------

CCSection* CCSections::FindLastByVA ( )
{
	CCSection* section = 0;
	int vmax = 0;
	for (int i=0; i < this->count; i++)
        {      	int v=this->buffer[i]->descriptor.VirtualAddress;
		if ( v > vmax)
		{	section = this->buffer[i];
			vmax = v;
		}
	}
	return section;
}

//--------------------------------------------------------------------

CCSection* CCSections::FindLastByRaw ( )
{
	CCSection* section = 0;
	int rmax = 0;
	for (int i=0; i < this->count; i++)
        {      	int r = this->buffer[i]->descriptor.PointerToRawData;
		if ( r > rmax)
		{	section = this->buffer[i];
			rmax = r;
		}
	}
	return section;
}

//--------------------------------------------------------------------

CCSection* CCSections::FindFirstByRaw ( )
{
	CCSection* section = 0;
	int rmin = 0xFFFFFFF;
	for (int i=0; i < this->count; i++)
	{	int r = this->buffer[i]->descriptor.PointerToRawData;
		if ( ( this->buffer[i]->descriptor.SizeOfRawData > 0) && ( r < rmin) )
			{	section = this->buffer[i];
				rmin = r;
			}
	}
	return section;
}


//====================================================================
//====================================================================
//====================================================================

CString CCFilePE::ClassName ( )	{ return "CCFilePE"; }

//--------------------------------------------------------------------

int CCFilePE::SizeOfRaw ( )
{
	CCSection* section = sections.FindLastByRaw ( );
	return section->descriptor.PointerToRawData + section->descriptor.SizeOfRawData; // ALIGN? header_pe.FileAlignment
}


//--------------------------------------------------------------------

BOOL CCFilePE::ReadPE ( )
{

	// ---------------------------------------------------
	// 	DOS-Stub
	// ---------------------------------------------------

	Seek ( );
	if ( ! Read ( &header_mz, sizeof(CCFILEPE_MZ) ) ) return false;
	if ( header_mz.e_magic != CCFILEPE_MAGIC_MZ )
	{	SetLastError ( NO_ERROR );
		CCError ( CCFILEPE_ERROR_NOTMZ );
		return false;
	}
	dosstub.BufferResize ( header_mz.e_lfanew - sizeof (header_mz) );
	Seek ( sizeof(header_mz) );
	Read ( dosstub.buffer, dosstub.count );
	// ---------------------------------------------------
	// 	PE
	// ---------------------------------------------------
	if ( ! Seek ( header_mz.e_lfanew ) ) return false;
	if ( ! Read ( &header_pe, sizeof(header_pe) ) ) return false;
	if ( header_pe.e_magic == CCFILEPE_MAGIC_NE )
	{	SetLastError ( NO_ERROR );
		CCError ( CCFILEPE_ERROR_NE_NOTSUPPORTED );
		return false;
	}
	if ( header_pe.e_magic != CCFILEPE_MAGIC_PE )
	{	SetLastError ( NO_ERROR );
		CCError ( CCFILEPE_ERROR_NOTPE );
		return false;
	}
	// ---------------------------------------------------
	// 	SECTIONS TABLE
	// ---------------------------------------------------
	for (int i=0; i< header_pe.NumberOfSections; i++)
	{
		CCSection* psection = new (CCSection);
//		psection->Unmovable=false;
		Read ( &psection->descriptor, sizeof(psection->descriptor) );
		sections.Add (psection);
	}

	return true;
}

//--------------------------------------------------------------------

BOOL CCFilePE::ReadImage ( )
{
	overlay.BufferFree ( );
	if ( ! ReadSections ( ) ) return false;
	int rawsize  = SizeOfRaw ( );
	int oversize = Size ( ) - rawsize;
	if ( oversize )
        {      	overlay.BufferAlloc ( oversize );
        	if ( !Seek ( rawsize ) ) return false;
        	if ( !Read ( overlay.buffer, oversize ) ) return false;
        }
        return true;
}


//--------------------------------------------------------------------

BOOL CCFilePE::ReadSection ( CCSection* section )
{	if ( ! section->BufferAlloc ( section->descriptor.SizeOfRawData ) ) return false;
	if ( ! Seek ( section->descriptor.PointerToRawData ) ) return false;
	return Read ( section->buffer, section->descriptor.SizeOfRawData );
}

//--------------------------------------------------------------------

BOOL CCFilePE::ReadSections ( )
{
	for ( int i=0; i<sections.count; i++)
		if ( ! ReadSection( sections[i]) ) return false;
	return true;
}

//====================================================================
//====================================================================
//====================================================================


BOOL CCFilePE::WritePE ( )
{
	header_pe.NumberOfSections = sections.count;
	CCSection* last_section = sections.FindLastByVA ( );
	header_pe.SizeOfImage = last_section->descriptor.VirtualAddress + last_section->descriptor.VirtualSize;


	Seek  ( );
	if ( !Write ( &header_mz, sizeof(header_mz) ) ) return false;
	if ( !Write ( dosstub.buffer, dosstub.count ) ) return false;
	return Write ( &header_pe, sizeof(header_pe) );

}
//--------------------------------------------------------------------
BOOL CCFilePE::WriteSectionsTable ( )
{
	Seek ( header_mz.e_lfanew + sizeof(header_pe) );
	for ( int i=0; i<sections.count; i++)
		if ( !Write ( &sections[i]->descriptor, sizeof(CCFILEPE_SECTION_DESCRIPTOR) ) ) return false;
	return true;
}
//--------------------------------------------------------------------
BOOL CCFilePE::WriteSectionsImage ( )
{
	int imageof = header_mz.e_lfanew + sizeof(header_pe) + sections.count * sizeof(CCFILEPE_SECTION_DESCRIPTOR);
	imageof = ALIGN (imageof, header_pe.FileAlignment);
	Seek ( imageof );
	for ( int i=0; i<sections.count; i++)
        {	CCSection* ps = sections[i];
		if ( ps->descriptor.SizeOfRawData > 0 )
		{	ps->descriptor.PointerToRawData = imageof;
			ps->descriptor.SizeOfRawData = ALIGN(ps->descriptor.SizeOfRawData, header_pe.FileAlignment);
			if ( !Write ( ps->buffer, ps->count ) ) return false;
			imageof += ps->descriptor.SizeOfRawData;
			Seek ( imageof );
		}
	}
	return true;
}

//--------------------------------------------------------------------

BOOL CCFilePE::WriteOverlay ( )
{
	Seek ( SizeOfRaw ( ) );
	if ( overlay.count ) return Write ( overlay.buffer, overlay.count );
	return true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------

BOOL CCFilePE::WriteImage ( )
{
	Truncate ( );
	if ( !WriteSectionsImage ( ) ) return false;
	if ( !WriteSectionsTable ( ) ) return false;
	if ( !WriteOverlay ( ) ) return false;
	return WritePE  ( );
}



//=================================================================EOF
