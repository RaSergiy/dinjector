@del bin2src.exe
%SYSTEMBASE%\soft\vs_2005\bin\cl.exe bin2src.cpp ../include.cpp/CCBase.cpp ../include.cpp/CCFile.cpp ^
 "%SYSTEMBASE%\soft\vs_2005\lib\vc\kernel32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\user32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\advapi32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\ole32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\oleaut32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\shell32.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\sdk\shlwapi.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\vc\libcpmt.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\vc\libcmt.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\vc\oldnames.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\vc\uuid.lib" ^
 "%SYSTEMBASE%\soft\vs_2005\lib\atlmfc\atls.lib" ^
 /I"%SYSTEMBASE%\soft\vs_2005\include\sdk" ^
 /I"%SYSTEMBASE%\soft\vs_2005\include\vc" ^
 /I"%SYSTEMBASE%\soft\vs_2005\include\atlmfc" ^
 /EHsc /GL /Ox /Os /GA /arch:SSE2 ^
 /link"/MANIFEST:NO /INCREMENTAL:NO /SAFESEH:NO /SUBSYSTEM:CONSOLE"
@del *.obj
