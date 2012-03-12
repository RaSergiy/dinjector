ADDFILENAME ="INJECT.EXE"
VS =soft/vs_2005
MASM =soft\asm\masm32\bin
MASM_INC_SYS =prog\include.asm\masm32\include
MASM_INC =prog\include.asm
MASM_LIB =prog\include.asm\masm32\lib
FORG = prog/include.cpp
INCS = $(VS)/include
LIBVC = $(VS)/lib/vc
LIBSDK = $(VS)/lib/sdk
LIBATLMFC = $(VS)/lib/atlmfc
BIN2SRC =prog\bin2src\bin2src.exe
LIBRARYSVC = $(LIBVC)/kernel32.lib $(LIBVC)/libcpmt.lib $(LIBVC)/libcmt.lib $(LIBVC)/oldnames.lib $(LIBVC)/uuid.lib
LIBRARYSSDK = $(LIBSDK)/user32.lib $(LIBSDK)/shlwapi.lib $(LIBSDK)/shell32.lib $(LIBSDK)/oleaut32.lib $(LIBSDK)/ole32.lib $(LIBSDK)/advapi32.lib
LIBRARYS = $(LIBRARYSVC) $(LIBRARYSSDK) $(LIBATLMFC)/atls.lib
FORGSRCS = $(FORG)/CCFile.cpp $(FORG)/CCFilePE.cpp $(FORG)/CCBase.cpp
INCLUDES = /I"$(INCS)/vc" /I"$(INCS)/sdk" /I"$(INCS)/atlmfc" /I"$(FORG)"
COMPILEROPTS = /EHsc /GL /Od /link"/MANIFEST:NO /INCREMENTAL:NO /SAFESEH:NO /SUBSYSTEM:CONSOLE"
din.exe: preclear
	$(BIN2SRC) asm injected_exe $(ADDFILENAME) prog\dinject\DINLOADER.INC
	$(MASM)\ml /c /coff /I$(MASM_INC) /I$(MASM_INC) /I$(MASM_INC_SYS) prog\dinject\dinloader.asm
	$(MASM)\rc prog\dinject\dinloader.rc
	$(MASM)\Link /SUBSYSTEM:WINDOWS /LIBPATH:$(MASM_LIB) dinloader.obj prog\dinject\dinloader.res
	$(BIN2SRC) cpp dinloader dinloader.exe prog\dinject\dinloader.h
	soft\vs_2005\bin\cl.exe prog\dinject\din.cpp $(FORGSRCS) $(LIBRARYS) $(INCLUDES) $(COMPILEROPTS)
	@del *.obj
	@del prog\dinject\dinloader.h
	@del prog\dinject\dinloader.inc
	@del dinloader.exe
	@del package\din.exe
	@copy din.exe package\din.exe
preclear:
	@del din.exe
	@del prog\dinject\din.exe
