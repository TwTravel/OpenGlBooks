# Microsoft Developer Studio Project File - Name="mathglpp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mathglpp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mathglpp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mathglpp.mak" CFG="mathglpp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mathglpp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathglpp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mathglpp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathglpp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gi /GX /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"mathglpp.lib"

!ENDIF 

# Begin Target

# Name "mathglpp - Win32 Release"
# Name "mathglpp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="src\libmathglpp\GLBernsteinPolynomial.cpp"
# End Source File
# Begin Source File

SOURCE=.\src\libmathglpp\GLFunctions.cpp
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLMatrix.cpp"
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLVector.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="src\libmathglpp\GLBernsteinPolynomial.h"
# End Source File
# Begin Source File

SOURCE=.\src\libmathglpp\GLCubicBezierCurve.h
# End Source File
# Begin Source File

SOURCE=.\src\libmathglpp\GLCubicBezierSpline.h
# End Source File
# Begin Source File

SOURCE=.\src\libmathglpp\GLCubicHermiteCurve.h
# End Source File
# Begin Source File

SOURCE=.\src\libmathglpp\GLFunctions.h
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLMatrix.h"
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLPlane.h"
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLQuaternion.h"
# End Source File
# Begin Source File

SOURCE="src\libmathglpp\GLVector.h"
# End Source File
# End Group
# End Target
# End Project
