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

using namespace std;

//--------------------------------------------------------------------
#define SRCASM 1
#define SRCCPP 2

int main ( int argc, char* argv[] )
{
	int srctype;
	CCErrorTo = CCERROR_TO_CONSOLE;
	cout << "\nbin2src {FOURG@km.ru}\n";
	if ( argc < 5 )
	{
		cout << "USAGE: bin2src.exe <src_type> <data_name> <input_binary_file> <output_src_file>\n";
		cout << "AVAILABLE SOURCE TYPES: asm, cpp\n";
		return 0;
	}
	CString sType=argv[1];	sType.MakeUpper();
	if ( sType == "ASM" ) srctype=SRCASM; else
	if ( sType == "CPP" ) srctype=SRCCPP; else
        	{ cout << "ERROR: Unknown source type\n"; return 1; }

	CCFile fbin  ( argv[3], GENERIC_READ );
	CCFile fsrc  ( argv[4], GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS );
	CString str;
	CString wstr;
	cout << "PROCESSING " << sType << " File ("<<fbin.Size()<< " bytes)";

	switch (srctype)
        {      	case SRCASM: wstr.Format("%s_size equ %d\n%s:\n", argv[2], fbin.Size(), argv[2]); break;
        	case SRCCPP: wstr.Format("#define %s_size %d\nunsigned char %s [ %s_size ] = {",
        				  argv[2], fbin.Size(), argv[2], argv[2]); break;
        }
	fbin.Read  ( );
	fbin.Close ( );

	int line=256;
	for (int i=0; i<fbin.count; i++ )
	{
		switch (srctype)
		{	case SRCASM: str.Format("0%.2xh", fbin[i]);	break;
			case SRCCPP: str.Format("0x%.2x", fbin[i]);	break;
		}

		if (++line > 15 )
		{	line=0;
			switch (srctype)
			{	case SRCASM: wstr += "\ndb ";	break;
				case SRCCPP: wstr += "\n";	break;
			}
		}
		wstr += str;
		switch (srctype)
		{	case SRCASM: if ( (line!=15) && ((i+1)<fbin.count) ) wstr+=",";	break;
			case SRCCPP: if (((i+1)<fbin.count) ) wstr+=","; break;
		}
	}
	switch (srctype)
	{	case SRCASM: wstr += "\n";	break;
		case SRCCPP: wstr += "};";	break;
	}


	fsrc.WriteS(&wstr);
	fsrc.Close ( );
	cout << "\nOk :)\n";
	return 0;
}
