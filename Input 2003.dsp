# Microsoft Developer Studio Project File - Name="Input 2003" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Input 2003 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Input 2003.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Input 2003.mak" CFG="Input 2003 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Input 2003 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Input 2003 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Input 2003 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Input 2003 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Input 2003 - Win32 Release"
# Name "Input 2003 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\consol.cpp
# End Source File
# Begin Source File

SOURCE=.\dbmi.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\glgui.cpp
# End Source File
# Begin Source File

SOURCE=.\HUD.CPP
# End Source File
# Begin Source File

SOURCE=.\palman.cpp
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glwinapi.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\qpctimer.cpp
# End Source File
# End Group
# Begin Group "Utilities"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ioUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\mtultilities.cpp
# End Source File
# Begin Source File

SOURCE=.\stringUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\textDisplay.cpp
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\di8input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputman.cpp
# End Source File
# Begin Source File

SOURCE=.\strinproc.cpp
# End Source File
# Begin Source File

SOURCE=.\textInput.cpp
# End Source File
# End Group
# Begin Group "Data Types"

# PROP Default_Filter ""
# Begin Group "Adv Data Types"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Graphics Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\consol.h
# End Source File
# Begin Source File

SOURCE=.\dbmi.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\glgui.h
# End Source File
# Begin Source File

SOURCE=.\hud.h
# End Source File
# Begin Source File

SOURCE=.\palman.h
# End Source File
# End Group
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glwinapi.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\qpctimer.h
# End Source File
# End Group
# Begin Group "Utility Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ioUtilities.h
# End Source File
# Begin Source File

SOURCE=.\mtutilities.h
# End Source File
# Begin Source File

SOURCE=.\stdmacros.h
# End Source File
# Begin Source File

SOURCE=.\stringUtilities.h
# End Source File
# Begin Source File

SOURCE=.\textDisplay.h
# End Source File
# End Group
# Begin Group "Input Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\di8input.h
# End Source File
# Begin Source File

SOURCE=.\inputman.h
# End Source File
# Begin Source File

SOURCE=.\strinproc.h
# End Source File
# Begin Source File

SOURCE=.\textInput.h
# End Source File
# End Group
# Begin Group "Data Type Headers"

# PROP Default_Filter ""
# Begin Group "Adv Data Type Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\advdtypes\advdtypes.h
# End Source File
# Begin Source File

SOURCE=.\arrays.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Text Files"

# PROP Default_Filter ".txt"
# Begin Source File

SOURCE=.\dump.txt
# End Source File
# Begin Source File

SOURCE=".\Progress Notes.txt"
# End Source File
# Begin Source File

SOURCE=.\t1.txt
# End Source File
# End Group
# Begin Group "Linked Libraries"

# PROP Default_Filter ".LIB"
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\DINPUT8.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\DXGUID.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\GLAUX.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\OPENGL32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\GLU32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\ijl15.lib"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\ImageLib\lib\DevIL.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\ImageLib\lib\ilu.lib
# End Source File
# End Group
# End Target
# End Project
