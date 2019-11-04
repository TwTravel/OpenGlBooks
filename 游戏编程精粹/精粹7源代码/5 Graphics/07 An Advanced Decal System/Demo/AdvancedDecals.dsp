# Microsoft Developer Studio Project File - Name="AdvancedDecals" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AdvancedDecals - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AdvancedDecals.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AdvancedDecals.mak" CFG="AdvancedDecals - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AdvancedDecals - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AdvancedDecals - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AdvancedDecals - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "dxut" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "STRSAFE_NO_DEPRECATE" /D "__noop" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib dxguid.lib dxerr9.lib dsound.lib comctl32.lib winmm.lib bufferoverflowu.lib /nologo /subsystem:windows /machine:I386 /out:"AdvancedDecals.exe" /libpath:"vc6"

!ELSEIF  "$(CFG)" == "AdvancedDecals - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "dxut" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "__noop" /D "STRSAFE_NO_DEPRECATE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib dxguid.lib dxerr9.lib dsound.lib comctl32.lib winmm.lib bufferoverflowu.lib /nologo /subsystem:windows /debug /machine:I386 /out:"AdvancedDecals.exe" /pdbtype:sept /libpath:"vc6"

!ENDIF 

# Begin Target

# Name "AdvancedDecals - Win32 Release"
# Name "AdvancedDecals - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AdvancedDecals.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AdvancedDecals.rc
# End Source File
# Begin Source File

SOURCE=.\dxut\directx.ico
# End Source File
# End Group
# Begin Group "dxut"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dxut\dxstdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\dxstdafx.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUT.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUT.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTenum.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTenum.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTgui.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTgui.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTMesh.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTmisc.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTRes.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTRes.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTsound.cpp
# End Source File
# Begin Source File

SOURCE=.\dxut\DXUTsound.h
# End Source File
# End Group
# End Target
# End Project
