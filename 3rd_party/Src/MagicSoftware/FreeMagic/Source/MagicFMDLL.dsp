# Microsoft Developer Studio Project File - Name="MagicFMDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MagicFMDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MagicFMDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MagicFMDLL.mak" CFG="MagicFMDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MagicFMDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MagicFMDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MagicFMDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MagicFMDLL___Win32_Release"
# PROP BASE Intermediate_Dir "MagicFMDLL___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAGICFMDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I ".\\" /I "Approximation2D" /I "Approximation3D" /I "Containment2D" /I "Containment3D" /I "Core" /I "Core2D" /I "Core3D" /I "Curve2D" /I "Curve3D" /I "Distance2D" /I "Distance3D" /I "ImageAnalysis" /I "Interpolation1D" /I "Interpolation2D" /I "Interpolation3D" /I "Intersection2D" /I "Intersection3D" /I "Numerics" /I "Projection" /I "Surface" /I "Tessellation2D" /I "Tessellation3D" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAGICFMDLL_EXPORTS" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\Library\ReleaseDLL\MagicFM.dll"
# Begin Special Build Tool
OutDir=.\ReleaseDLL
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Approximation2D\*.h ..\Include	copy Approximation2D\*.pkg ..\Include	copy Approximation3D\*.h ..\Include	copy Approximation3D\*.pkg ..\Include	copy Containment2D\*.h ..\Include	copy Containment2D\*.inl ..\Include	copy Containment2D\*.pkg ..\Include	copy Containment3D\*.h ..\Include	copy Containment3D\*.inl ..\Include	copy Containment3D\*.pkg ..\Include	copy Core\*.h ..\Include	copy Core\*.inl ..\Include	copy Core\*.pkg ..\Include	copy Core2D\*.h ..\Include	copy Core2D\*.inl ..\Include	copy Core2D\*.pkg ..\Include	copy Core3D\*.h ..\Include	copy Core3D\*.inl ..\Include	copy Core3D\*.pkg ..\Include	copy Curve2D\*.h ..\Include	copy Curve2D\*.inl ..\Include	copy Curve2D\*.pkg ..\Include	copy Curve3D\*.h ..\Include	copy Curve3D\*.inl ..\Include	copy Curve3D\*.pkg ..\Include	copy Distance2D\*.h ..\Include	copy Distance2D\*.pkg ..\Include	copy Distance3D\*.h ..\Include	copy Distance3D\*.pkg ..\Include	copy ImageAnalysis\*.h ..\Include	copy ImageAnalysis\*.inl ..\Include	copy ImageAnalysis\*.pkg ..\Include	copy Interpolation1D\*.h ..\Include	copy Interpolation1D\*.inl ..\Include	copy Interpolation1D\*.pkg ..\Include	copy Interpolation2D\*.h ..\Include	copy\
                          Interpolation2D\*.inl ..\Include	copy Interpolation2D\*.pkg ..\Include	copy Interpolation3D\*.h ..\Include	copy Interpolation3D\*.inl ..\Include	copy Interpolation3D\*.pkg ..\Include	copy Intersection2D\*.h ..\Include	copy Intersection2D\*.pkg ..\Include	copy Intersection3D\*.h ..\Include	copy Intersection3D\*.pkg ..\Include	copy Numerics\*.h ..\Include	copy Numerics\*.inl ..\Include	copy Numerics\*.pkg ..\Include	copy Projection\*.h ..\Include	copy Projection\*.inl ..\Include	copy Projection\*.pkg ..\Include	copy Surface\*.h ..\Include	copy Surface\*.inl ..\Include	copy Surface\*.pkg ..\Include	copy Tessellation2D\*.h ..\Include	copy Tessellation2D\*.inl ..\Include	copy Tessellation2D\*.pkg ..\Include	copy Tessellation3D\*.h ..\Include	copy Tessellation3D\*.inl ..\Include	copy Tessellation3D\*.pkg ..\Include	copy MgcRTLib.h ..\Include	copy MagicFMLibType.h ..\Include	copy MagicFM.pkg  ..\Include	copy $(OutDir)\MagicFM.lib ..\Library\$(OutDir)	copy ..\Include\*.h ..\..\Sdk\Include	copy ..\Include\*.inl ..\..\Sdk\Include	copy ..\Include\*.pkg ..\..\Sdk\Include	copy ..\Library\$(OutDir)\MagicFM.lib ..\..\Sdk\Library\$(OutDir)	copy ..\Library\$(OutDir)\MagicFM.dll\
     ..\..\Sdk\Library\$(OutDir)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MagicFMDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MagicFMDLL___Win32_Debug"
# PROP BASE Intermediate_Dir "MagicFMDLL___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAGICFMDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "Approximation2D" /I "Approximation3D" /I "Containment2D" /I "Containment3D" /I "Core" /I "Core2D" /I "Core3D" /I "Curve2D" /I "Curve3D" /I "Distance2D" /I "Distance3D" /I "ImageAnalysis" /I "Interpolation1D" /I "Interpolation2D" /I "Interpolation3D" /I "Intersection2D" /I "Intersection3D" /I "Numerics" /I "Projection" /I "Surface" /I "Tessellation2D" /I "Tessellation3D" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAGICFMDLL_EXPORTS" /D "STRICT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\Library\DebugDLL\MagicFM.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\DebugDLL
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Approximation2D\*.h ..\Include	copy Approximation2D\*.pkg ..\Include	copy Approximation3D\*.h ..\Include	copy Approximation3D\*.pkg ..\Include	copy Containment2D\*.h ..\Include	copy Containment2D\*.inl ..\Include	copy Containment2D\*.pkg ..\Include	copy Containment3D\*.h ..\Include	copy Containment3D\*.inl ..\Include	copy Containment3D\*.pkg ..\Include	copy Core\*.h ..\Include	copy Core\*.inl ..\Include	copy Core\*.pkg ..\Include	copy Core2D\*.h ..\Include	copy Core2D\*.inl ..\Include	copy Core2D\*.pkg ..\Include	copy Core3D\*.h ..\Include	copy Core3D\*.inl ..\Include	copy Core3D\*.pkg ..\Include	copy Curve2D\*.h ..\Include	copy Curve2D\*.inl ..\Include	copy Curve2D\*.pkg ..\Include	copy Curve3D\*.h ..\Include	copy Curve3D\*.inl ..\Include	copy Curve3D\*.pkg ..\Include	copy Distance2D\*.h ..\Include	copy Distance2D\*.pkg ..\Include	copy Distance3D\*.h ..\Include	copy Distance3D\*.pkg ..\Include	copy ImageAnalysis\*.h ..\Include	copy ImageAnalysis\*.inl ..\Include	copy ImageAnalysis\*.pkg ..\Include	copy Interpolation1D\*.h ..\Include	copy Interpolation1D\*.inl ..\Include	copy Interpolation1D\*.pkg ..\Include	copy Interpolation2D\*.h ..\Include	copy\
                          Interpolation2D\*.inl ..\Include	copy Interpolation2D\*.pkg ..\Include	copy Interpolation3D\*.h ..\Include	copy Interpolation3D\*.inl ..\Include	copy Interpolation3D\*.pkg ..\Include	copy Intersection2D\*.h ..\Include	copy Intersection2D\*.pkg ..\Include	copy Intersection3D\*.h ..\Include	copy Intersection3D\*.pkg ..\Include	copy Numerics\*.h ..\Include	copy Numerics\*.inl ..\Include	copy Numerics\*.pkg ..\Include	copy Projection\*.h ..\Include	copy Projection\*.inl ..\Include	copy Projection\*.pkg ..\Include	copy Surface\*.h ..\Include	copy Surface\*.inl ..\Include	copy Surface\*.pkg ..\Include	copy Tessellation2D\*.h ..\Include	copy Tessellation2D\*.inl ..\Include	copy Tessellation2D\*.pkg ..\Include	copy Tessellation3D\*.h ..\Include	copy Tessellation3D\*.inl ..\Include	copy Tessellation3D\*.pkg ..\Include	copy MgcRTLib.h ..\Include	copy MagicFMLibType.h ..\Include	copy MagicFM.pkg  ..\Include	copy $(OutDir)\MagicFM.lib ..\Library\$(OutDir)	copy ..\Include\*.h ..\..\Sdk\Include	copy ..\Include\*.inl ..\..\Sdk\Include	copy ..\Include\*.pkg ..\..\Sdk\Include	copy ..\Library\$(OutDir)\MagicFM.lib ..\..\Sdk\Library\$(OutDir)	copy ..\Library\$(OutDir)\MagicFM.dll\
     ..\..\Sdk\Library\$(OutDir)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MagicFMDLL - Win32 Release"
# Name "MagicFMDLL - Win32 Debug"
# Begin Group "Approximation2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Approximation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DCircleFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DEllipseByArcs.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DEllipseFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DGaussPointsFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DLineFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DQuadraticFit.cpp
# End Source File
# End Group
# Begin Group "Header Files (Approximation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DCircleFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DEllipseByArcs.h
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DEllipseFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DGaussPointsFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DLineFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation2D\MgcAppr2DQuadraticFit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Approximation2D\MgcApproximation2D.pkg
# End Source File
# End Group
# Begin Group "Approximation3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Approximation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DEllipsoidFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DGaussPointsFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DLineFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DParaboloidFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DPlaneFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DQuadraticFit.cpp
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DSphereFit.cpp
# End Source File
# End Group
# Begin Group "Header Files (Approximation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DEllipsoidFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DGaussPointsFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DLineFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DParaboloidFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DPlaneFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DQuadraticFit.h
# End Source File
# Begin Source File

SOURCE=.\Approximation3D\MgcAppr3DSphereFit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Approximation3D\MgcApproximation3D.pkg
# End Source File
# End Group
# Begin Group "Containment2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Containment2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DCircleScribe.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DEllipse.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DMinBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DMinCircle.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DPointInPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcConvexHull2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcMinAreaEllipseCR.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcSeparatePointSets2D.cpp
# End Source File
# End Group
# Begin Group "Header Files (Containment2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DBox.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DCircleScribe.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DEllipse.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DMinBox.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DMinCircle.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcCont2DPointInPolygon.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcConvexHull2D.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcMinAreaEllipseCR.h
# End Source File
# Begin Source File

SOURCE=.\Containment2D\MgcSeparatePointSets2D.h
# End Source File
# End Group
# Begin Group "Inline Files (Containment2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment2D\MgcConvexHull2D.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Containment2D\MgcContainment2D.pkg
# End Source File
# End Group
# Begin Group "Containment3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Containment3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCapsule.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCircleSphereScribe.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DEllipsoid.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DLozenge.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DMinBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DMinSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcConvexHull3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcMinVolumeEllipsoidCR.cpp
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcSeparatePointSets3D.cpp
# End Source File
# End Group
# Begin Group "Header Files (Containment3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DBox.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCapsule.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCircleSphereScribe.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DCylinder.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DEllipsoid.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DLozenge.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DMinBox.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DMinSphere.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcCont3DSphere.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcConvexHull3D.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcMinVolumeEllipsoidCR.h
# End Source File
# Begin Source File

SOURCE=.\Containment3D\MgcSeparatePointSets3D.h
# End Source File
# End Group
# Begin Group "Inline Files (Containment3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Containment3D\MgcConvexHull3D.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Containment3D\MgcContainment3D.pkg
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Source Files (Core)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\MgcColorRGB.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcEdgeMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcIntervalIntersection.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMath.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcPolynomial.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MgcTriangleMesh.cpp
# End Source File
# End Group
# Begin Group "Header Files (Core)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\MgcColorRGB.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcCommand.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcEdgeMesh.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcIntervalIntersection.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMath.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTEdge.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTIEdge.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTITriangle.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTIVertex.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTMesh.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTTriangle.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTVertex.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcPolynomial.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcSmallSet.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=.\Core\MgcTUnorderedSet.h
# End Source File
# End Group
# Begin Group "Inline Files (Core)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\MgcColorRGB.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcEdgeMesh.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMath.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTEdge.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTIEdge.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTITriangle.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTIVertex.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTMesh.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTTriangle.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcMTVertex.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcSmallSet.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcTriangleMesh.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MgcTUnorderedSet.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Core\MgcCore.pkg
# End Source File
# End Group
# Begin Group "Core2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Core2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core2D\MgcBox2.cpp
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcMatrix2.cpp
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcVector2.cpp
# End Source File
# End Group
# Begin Group "Header Files (Core2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core2D\MgcArc2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcBox2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcCircle2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcDisk2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcEllipse2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcLine2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcMatrix2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcRay2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcSegment2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcTriangle2.h
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcVector2.h
# End Source File
# End Group
# Begin Group "Inline Files (Core2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core2D\MgcArc2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcBox2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcCircle2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcDisk2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcEllipse2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcLine2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcMatrix2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcRay2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcSegment2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcTriangle2.inl
# End Source File
# Begin Source File

SOURCE=.\Core2D\MgcVector2.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Core2D\MgcCore2D.pkg
# End Source File
# End Group
# Begin Group "Core3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Core3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core3D\MgcBox3.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcExtraSpin.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTetrahedron.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTorus.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcVector3.cpp
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcXFormInterp.cpp
# End Source File
# End Group
# Begin Group "Header Files (Core3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core3D\MgcBox3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcCapsule.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcCircle3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcCone3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcCylinder.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcDisk3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcEllipsoid.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcExtraSpin.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcFrustum.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcLine3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcLozenge.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcMatrix3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcParallelogram3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcPlane.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcRay3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcRectangle3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcSegment3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcSphere.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTetrahedron.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTorus.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTriangle3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcVector3.h
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcXFormInterp.h
# End Source File
# End Group
# Begin Group "Inline Files (Core3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core3D\MgcMatrix3.inl
# End Source File
# Begin Source File

SOURCE=.\Core3D\MgcTetrahedron.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Core3D\MgcCore3D.pkg
# End Source File
# End Group
# Begin Group "Curve2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Curve2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve2D\MgcBezierCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCubicPolynomialCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcMultipleCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcNaturalSpline2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcPolynomialCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcSingleCurve2.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcTCBSpline2.cpp
# End Source File
# End Group
# Begin Group "Header Files (Curve2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve2D\MgcBezierCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCubicPolynomialCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcMultipleCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcNaturalSpline2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcPolynomialCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcSingleCurve2.h
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcTCBSpline2.h
# End Source File
# End Group
# Begin Group "Inline Files (Curve2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve2D\MgcBezierCurve2.inl
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCubicPolynomialCurve2.inl
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcCurve2.inl
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcNaturalSpline2.inl
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcPolynomialCurve2.inl
# End Source File
# Begin Source File

SOURCE=.\Curve2D\MgcTCBSpline2.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Curve2D\MgcCurve2D.pkg
# End Source File
# End Group
# Begin Group "Curve3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Curve3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve3D\MgcBezierCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCubicPolynomialCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcMultipleCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcNaturalSpline3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcPolynomialCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcSingleCurve3.cpp
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcTCBSpline3.cpp
# End Source File
# End Group
# Begin Group "Header Files (Curve3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve3D\MgcBezierCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCubicPolynomialCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcMultipleCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcNaturalSpline3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcPolynomialCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcSingleCurve3.h
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcTCBSpline3.h
# End Source File
# End Group
# Begin Group "Inline Files (Curve3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve3D\MgcBezierCurve3.inl
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCubicPolynomialCurve3.inl
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcCurve3.inl
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcNaturalSpline3.inl
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcPolynomialCurve3.inl
# End Source File
# Begin Source File

SOURCE=.\Curve3D\MgcTCBSpline3.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Curve3D\MgcCurve3D.pkg
# End Source File
# End Group
# Begin Group "Distance2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Distance2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Distance2D\MgcDist2DVecElp.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance2D\MgcDist2DVecQdr.cpp
# End Source File
# End Group
# Begin Group "Header Files (Distance2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Distance2D\MgcDist2DVecElp.h
# End Source File
# Begin Source File

SOURCE=.\Distance2D\MgcDist2DVecQdr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Distance2D\MgcDistance2D.pkg
# End Source File
# End Group
# Begin Group "Distance3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Distance3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DCirCir.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinCir.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinLin.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinPgm.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinRct.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DPgmPgm.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DRctRct.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriPgm.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriRct.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecCir.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecElp.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecLin.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecPgm.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecPln.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecQdr.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecRct.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcEllipsoidGeodesicDist.cpp
# End Source File
# End Group
# Begin Group "Header Files (Distance3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DCirCir.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinBox.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinCir.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinLin.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinPgm.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinRct.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DLinTri.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DPgmPgm.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DRctRct.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriPgm.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriRct.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DTriTri.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecBox.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecCir.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecElp.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecFrustum.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecLin.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecPgm.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecPln.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecQdr.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecRct.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcDist3DVecTri.h
# End Source File
# Begin Source File

SOURCE=.\Distance3D\MgcEllipsoidGeodesicDist.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Distance3D\MgcDistance3D.pkg
# End Source File
# End Group
# Begin Group "ImageAnalysis"

# PROP Default_Filter ""
# Begin Group "Source Files (ImageAnalysis)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageAnalysis\MgcBinary2D.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcElement.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcImageConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcImages.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcLattice.cpp
# End Source File
# End Group
# Begin Group "Header Files (Image Analysis)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageAnalysis\MgcBinary2D.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcElement.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcImageConvert.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcImages.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcLattice.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage2D.h
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage3D.h
# End Source File
# End Group
# Begin Group "Inline Files (ImageAnalysis)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageAnalysis\MgcElement.inl
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcLattice.inl
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage.inl
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage2D.inl
# End Source File
# Begin Source File

SOURCE=.\ImageAnalysis\MgcTImage3D.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\ImageAnalysis\MgcImageAnalysis.pkg
# End Source File
# End Group
# Begin Group "Interpolation1D"

# PROP Default_Filter ""
# Begin Group "Source Files (Interpolation1D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkima.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaNonuniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DBSplineUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterpBSplineUniform.cpp
# End Source File
# End Group
# Begin Group "Header Files (Interpolation1D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkima.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaNonuniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DBSplineUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterpBSplineUniform.h
# End Source File
# End Group
# Begin Group "Inline Files (Interpolation1D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkima.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaNonuniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DAkimaUniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterp1DBSplineUniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterpBSplineUniform.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interpolation1D\MgcInterpolation1D.pkg
# End Source File
# End Group
# Begin Group "Interpolation2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Interpolation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DAkimaUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBicubic.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBSplineUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DLinearNonuniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DQdrNonuniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DThinPlateSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DVectorField.cpp
# End Source File
# End Group
# Begin Group "Header Files (Interpolation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DAkimaUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBicubic.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBSplineUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DLinearNonuniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DQdrNonuniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DSphere.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DThinPlateSpline.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DVectorField.h
# End Source File
# End Group
# Begin Group "Inline Files (Interpolation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DAkimaUniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBicubic.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterp2DBSplineUniform.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interpolation2D\MgcInterpolation2D.pkg
# End Source File
# End Group
# Begin Group "Interpolation3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Interpolation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DAkimaUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DBSplineUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DThinPlateSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp4DBSplineUniform.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterpNDBSplineUniform.cpp
# End Source File
# End Group
# Begin Group "Header Files (Interpolation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DAkimaUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DBSplineUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DThinPlateSpline.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp4DBSplineUniform.h
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterpNDBSplineUniform.h
# End Source File
# End Group
# Begin Group "Inline Files (Interpolation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DAkimaUniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp3DBSplineUniform.inl
# End Source File
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterp4DBSplineUniform.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interpolation3D\MgcInterpolation3D.pkg
# End Source File
# End Group
# Begin Group "Intersection2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Intersection2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DBoxBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DCirBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DCirCir.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DElpElp.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DLinCir.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DLinLin.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DTriTri.cpp
# End Source File
# End Group
# Begin Group "Header Files (Intersection2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DBoxBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DCirBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DCirCir.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DElpElp.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DLinCir.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DLinLin.h
# End Source File
# Begin Source File

SOURCE=.\Intersection2D\MgcIntr2DTriTri.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Intersection2D\MgcIntersection2D.pkg
# End Source File
# End Group
# Begin Group "Intersection3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Intersection3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DBoxBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DBoxFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DCapCap.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCap.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCone.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCyln.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinElp.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinLoz.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinSphr.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinTorus.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLozLoz.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnCap.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnCyln.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnElp.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnLoz.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnPln.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnSphr.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrCone.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrSphr.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTetrTetr.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriCone.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriSphr.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriTri.cpp
# End Source File
# End Group
# Begin Group "Header Files (Intersection3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DBoxBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DBoxFrustum.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DCapCap.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCap.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCone.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinCyln.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinElp.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinLoz.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinSphr.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinTorus.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLinTri.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DLozLoz.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnCap.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnCyln.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnElp.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnLoz.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnPln.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DPlnSphr.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrCone.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrFrustum.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DSphrSphr.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTetrTetr.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriBox.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriCone.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriSphr.h
# End Source File
# Begin Source File

SOURCE=.\Intersection3D\MgcIntr3DTriTri.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Intersection3D\MgcIntersection3D.pkg
# End Source File
# End Group
# Begin Group "Numerics"

# PROP Default_Filter ""
# Begin Group "Source Files (Numerics)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Numerics\MgcBisect1.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcBisect2.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcBisect3.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcEigen.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcEuler.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcFastFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcIntegrate.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcLinearSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMidpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimize1D.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimizeND.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcODE.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4Adapt.cpp
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcSpecialFunction.cpp
# End Source File
# End Group
# Begin Group "Header Files (Numerics)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Numerics\MgcBisect1.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcBisect2.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcBisect3.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcEigen.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcEuler.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcFastFunction.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcIntegrate.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcLinearSystem.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMidpoint.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimize1D.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimizeND.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcODE.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4Adapt.h
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcSpecialFunction.h
# End Source File
# End Group
# Begin Group "Inline Files (Numerics)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Numerics\MgcEigen.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcEuler.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcLinearSystem.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMidpoint.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimize1D.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcMinimizeND.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcODE.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4.inl
# End Source File
# Begin Source File

SOURCE=.\Numerics\MgcRK4Adapt.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Numerics\MgcNumerics.pkg
# End Source File
# End Group
# Begin Group "Projection"

# PROP Default_Filter ""
# Begin Group "Source Files (Projection)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Projection\MgcConvexClipper.cpp
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcConvexPolyhedron.cpp
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcPerspProjEllipsoid.cpp
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcQuadToQuadTransforms.cpp
# End Source File
# End Group
# Begin Group "Header Files (Projection)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Projection\MgcConvexClipper.h
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcConvexPolyhedron.h
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcPerspProjEllipsoid.h
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcQuadToQuadTransforms.h
# End Source File
# End Group
# Begin Group "Inline Files (Projection)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Projection\MgcConvexClipper.inl
# End Source File
# Begin Source File

SOURCE=.\Projection\MgcConvexPolyhedron.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Projection\MgcProjection.pkg
# End Source File
# End Group
# Begin Group "Surface"

# PROP Default_Filter ""
# Begin Group "Source Files (Surface)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinder2G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinder3G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinderG.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierPatchG.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangle2G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangle3G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangleG.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangle2G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangle3G.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangleG.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcImplicitSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcParametricSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcQuadricSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcTubeSurfaceG.cpp
# End Source File
# End Group
# Begin Group "Header Files (Surface)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinder2G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinder3G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinderG.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierPatchG.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangle2G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangle3G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierRectangleG.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangle2G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangle3G.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierTriangleG.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcImplicitSurface.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcParametricSurface.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcQuadricSurface.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcSurface.h
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcTubeSurfaceG.h
# End Source File
# End Group
# Begin Group "Inline Files (Surface)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Surface\MgcBezierCylinderG.inl
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcBezierPatchG.inl
# End Source File
# Begin Source File

SOURCE=.\Surface\MgcTubeSurfaceG.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Surface\MgcSurface.pkg
# End Source File
# End Group
# Begin Group "Tessellation2D"

# PROP Default_Filter ""
# Begin Group "Source Files (Tessellation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation2D\MgcDelaunay2D.cpp
# End Source File
# End Group
# Begin Group "Header Files (Tessellation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation2D\MgcDelaunay2D.h
# End Source File
# End Group
# Begin Group "Inline Files (Tessellation2D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation2D\MgcDelaunay2D.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Tessellation2D\MgcTessellation2D.pkg
# End Source File
# End Group
# Begin Group "Tessellation3D"

# PROP Default_Filter ""
# Begin Group "Source Files (Tessellation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation3D\MgcDelaunay3D.cpp
# End Source File
# End Group
# Begin Group "Header Files (Tessellation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation3D\MgcDelaunay3D.h
# End Source File
# End Group
# Begin Group "Inline Files (Tessellation3D)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tessellation3D\MgcDelaunay3D.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\Tessellation3D\MgcTessellation3D.pkg
# End Source File
# End Group
# Begin Source File

SOURCE=.\MagicFM.pkg
# End Source File
# Begin Source File

SOURCE=.\MagicFMLibType.h
# End Source File
# Begin Source File

SOURCE=.\MgcRTLib.h
# End Source File
# End Target
# End Project
