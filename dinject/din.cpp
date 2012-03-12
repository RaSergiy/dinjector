#ifndef WINVER			// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410	// Change this to the appropriate value to target Windows Me or later.
#endif

#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <strstream>
#include <stdlib.h>
#include <fstream>
#include <atlstr.h>
#include <windows.h>

#include "..\\include.cpp\CCBase.h"
#include "..\\include.cpp\CCFile.h"
#include "..\\include.cpp\CCFilePE.h"

#include "dinloader.h"

using namespace std;

//--------------------------------------------------------------------

#define		DIN_LANGID 1033

struct STR_RESITM
{
	BOOL	IsStr;
	CString	*Str;
};

struct DINHDR
{
	unsigned int	loader_size;	// |
	unsigned int	origin_size;	// |
	unsigned int	machine_id;
	short	 int	machines_counter;
	short	 int	machines_counter_limit;
};


class CResArray : public CCBase < STR_RESITM* > { };


//--------------------------------------------------------------------

CResArray	GroupIconList;
CResArray	IconList;


//--------------------------------------------------------------------
//			MISC SHIT
//--------------------------------------------------------------------

BOOL CALLBACK ResEnum ( HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam )
{
	STR_RESITM* pRes = new STR_RESITM;
	if ( (ULONG)lpszName & 0xFFFF0000 )
	{
		pRes->IsStr = true;
		pRes->Str   = new CString ( lpszName );
	} else
	{
		pRes->IsStr = false;
		pRes->Str   = (CString*)lpszName;
	}
        if ( lpszType == RT_GROUP_ICON ) GroupIconList.Add(pRes);
		 else if ( lpszType == RT_ICON ) IconList.Add(pRes);
        return TRUE;
}

//-----------------------------------------------------------------

BOOL ResUpd ( CString update_path, HMODULE hModule, STR_RESITM* pResitm, LPCTSTR ResType )
{
	HGLOBAL	hResLoad;
	HRSRC	hRes;
	HANDLE	hUpdateRes;
	LPVOID	lpResLock;
	LPVOID	lpRes;
	BOOL	result;
	if ( pResitm->IsStr )	hRes = FindResource ( hModule, pResitm->Str->GetBuffer(), ResType );
		else 		hRes = FindResource ( hModule, (LPCSTR)pResitm->Str, ResType );
	if ( !hRes ) return false;
	hResLoad = LoadResource ( hModule, hRes );
	if ( !hResLoad ) return false;
	lpResLock = LockResource(hResLoad);
	if ( !lpResLock ) return false;
	hUpdateRes = BeginUpdateResource(update_path, false);
	if ( !hUpdateRes ) return false;
	if ( pResitm->IsStr )	lpRes = pResitm->Str->GetBuffer();
		else  		lpRes = (LPVOID)pResitm->Str;
	result = UpdateResource ( hUpdateRes, ResType, (LPCSTR)lpRes, DIN_LANGID, lpResLock, SizeofResource(hModule, hRes) );
	if ( !result ) return false;
	if ( !EndUpdateResource ( hUpdateRes, FALSE ) ) return false;
	return true;
}

//-----------------------------------------------------------------

BOOL ResUpdIcon ( CString path_to, CString path_from )
{
	HMODULE	hModule;
	BOOL	result;
	hModule = LoadLibrary ( path_from );
	if ( !hModule ) return false;
	EnumResourceNames ( hModule, RT_GROUP_ICON,	(ENUMRESNAMEPROC)ResEnum, NULL );
	EnumResourceNames ( hModule, RT_ICON,		(ENUMRESNAMEPROC)ResEnum, NULL );
	for ( int i=0; i<IconList.count; i++ )
	{
		result = ResUpd ( path_to, hModule, IconList[i], RT_ICON);
		if ( !result ) break;
	}
	if (result)
		for ( int i=0; i<GroupIconList.count; i++ )
		{
			result = ResUpd ( path_to,hModule, GroupIconList[i], RT_GROUP_ICON );
			if ( !result ) break;
		}
	if ( !FreeLibrary(hModule) ) return false;
	return result;
}



// ===================================================================
//			MAIN
// ===================================================================



int main ( int argc, char* argv[] )
{

	BOOL	oInject	  = true;
	int	oMachines = 1;
	int	nPatchSize;
	int	nPatchOffs;
	printf("\nDiNJECTOR-0.2 {FOURG@km.ru}");
	if ( argc == 1 )
	{	printf("\
\nUSAGE : din.exe [options] <source_file_1> ... [options] ... [source_file_N]\
\n  /I[n] - Inject mode. Where optional [n] = Machine limiter counter\
\n  /R    - Repair mode\
\nDEFAULT MODE: I1 (inject, machine_limiter=1)\n\
");

		return 1;
	}

	for ( int argi=1; argi < argc; argi++ )
        {
         	CString str (argv[argi]);
         	str.MakeUpper();
  		if ( str == "/R" ) { oInject=false; printf("\n.+.MODE:REPAIR"); continue; }
  		if ( (str[0]=='/') && (str[1]=='I') )
  		{	oInject=true;
  			printf("\n.+.MODE:INJECT");
			CString om (str.Mid(2));
			if (om != "")
			{
				oMachines=StrToInt( om );
				printf("\n.+.MACHINES_LIMITER=%d",oMachines);
			}
  			continue;
  		}

		if (oInject) printf("\n...INJECT: %s",argv[argi]);
			else printf("\n...REPAIR: %s",argv[argi]);

  		DINHDR	 dinhdr;
		CCFilePE forigin;
		forigin.Open( argv [argi] );
		forigin.Seek ( 0x40 );
		forigin.Read ( &dinhdr, sizeof (DINHDR) );

		if (!oInject) // ---------- REPAIR ---------
                {
			if ( (dinhdr.loader_size + dinhdr.origin_size) != forigin.Size ( ) )
			{	printf("\n WARNING: File cannot be repaired. Skipped.\a");
				forigin.Close( );
				continue;
			}
			CString str_id;
			CString str_cn;
			if (dinhdr.machine_id)
			{	str_id.Format(" ID:%.8X",dinhdr.machine_id);
				str_cn.Format(" counter:%d/%d",dinhdr.machines_counter,dinhdr.machines_counter_limit);
			}
			printf("   [-%Xh%s%s]",dinhdr.loader_size,str_id,str_cn);
			nPatchOffs  = dinhdr.loader_size > dinhdr.origin_size ?
						dinhdr.loader_size : dinhdr.origin_size;
			nPatchSize  = dinhdr.loader_size > dinhdr.origin_size ?
						dinhdr.origin_size : dinhdr.loader_size;
			forigin.Seek ( nPatchOffs );
			forigin.Read ( 0, nPatchSize );
			forigin.Seek ( 0 );
			forigin.Write( 0, nPatchSize );
			forigin.Truncate ( dinhdr.origin_size );
			forigin.Close( );
			printf(" Ok");
			continue;
                }


		if ( (dinhdr.loader_size + dinhdr.origin_size) == forigin.Size ( ) )
		{	printf("\n WARNING: File already injected! Skipped.\a");
			forigin.Close( );
			continue;
		}
		forigin.Close( );

		CCFilePE floader;
		floader.CreateTemp ( );
		floader.Write ( &dinloader, dinloader_size );
		floader.Close ( );

		CString str_ic("no");
		if ( ! ResUpdIcon ( floader.file_path, forigin.file_path ) )
                {	printf ("\n WARNING: Icons update failed!\a");
                	IconList.count = -1;
                } else str_ic.Format("%d",IconList.count);

                CString str_mc("no");
                if (oMachines) str_mc.Format("%d",oMachines);

		floader.ReOpen ( );
		forigin.ReOpen ( );
		forigin.ReadPE ( );
		floader.ReadPE ( );
		dinhdr.machine_id	= 0;
		dinhdr.loader_size = floader.Size();
		dinhdr.origin_size = forigin.Size();
		dinhdr.machines_counter_limit = oMachines;
		dinhdr.machines_counter = 0;


		if ( IconList.count > 0 )
			printf("   [+%Xh counter:%s icons:%s]",dinhdr.loader_size,str_mc,str_ic);
		else	// Strip loader's icons
		{
			floader.ReadImage ( );
			CCSection* res_section = floader.sections.FindByRVA
				( floader.header_pe.DD_ResourceTable.VirtualAddress );
			floader.sections.Delete(res_section);
			floader.header_pe.DD_ResourceTable.VirtualAddress = 0;
			floader.header_pe.DD_ResourceTable.isize = 0;
			floader.WriteImage ( );
		}

		nPatchSize  = dinhdr.loader_size > dinhdr.origin_size ?
					dinhdr.origin_size : dinhdr.loader_size;
		nPatchOffs  = dinhdr.loader_size > dinhdr.origin_size ?
					dinhdr.loader_size : dinhdr.origin_size;

		floader.Seek  ( 0x40 );
		floader.Write ( &dinhdr, sizeof (DINHDR) );
		floader.Read  ( );

		forigin.Seek  ( );
		forigin.Read  ( 0, nPatchSize );
		forigin.Seek  ( nPatchOffs );
		forigin.Write ( forigin.buffer, nPatchSize );

		forigin.Seek  ( );
		forigin.Write ( floader.buffer, floader.Size( ) );

		forigin.Close  ( );
		floader.Close  ( );
		floader.Delete ( );
		printf(" Ok");
	}
	printf("\nDONE.\n");
	return 0;
}
