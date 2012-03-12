#ifndef CCFILEPE_H
#define CCFILEPE_H

#include "CCFile.h"
#include "PE.h"



//====================================================================

class CCSection : public CCBaseBuffer
{
public:
	CCFILEPE_SECTION_DESCRIPTOR descriptor;

//	BOOL	Unmovable;
//	CCSection ( );
	virtual CString ClassName ( );

	CString GetName ( );
	void	SetName	( CString name );

};
//--------------------------------------------------------------------

class CCSections : public CCBase <CCSection*>
{
public:
	virtual CString ClassName ( );

	CCSection* FindByRVA		( int RVA );
	CCSection* FindLastByVA		( );
	CCSection* FindLastByRaw	( );
	CCSection* FindFirstByRaw	( );

	void	   Delete	( CCSection* section );
};



//====================================================================


class CCFilePE : public CCFile

{

public:

	CCBaseBuffer	dosstub;
	CCBaseBuffer	overlay;

	CCFILEPE_MZ	header_mz;
	CCFILEPE_PE	header_pe;
	CCSections	sections;

//--------------------------------------------------------------------

	virtual CString ClassName ( );

//--------------------------------------------------------------------

	int	SizeOfRaw	( );

	BOOL	ReadPE 		( );
	BOOL	ReadImage	( );
	BOOL	ReadSection	( CCSection* section );
	BOOL	ReadSections	( );

	BOOL	WritePE ( );
	BOOL	WriteSectionsTable ( );
	BOOL	WriteSectionsImage ( );
	BOOL	WriteOverlay ( );
	BOOL	WriteImage ( );

};

#endif
