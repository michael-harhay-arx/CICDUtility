@set CVIDIR=c:\program files (x86)\national instruments\cvi2019
@set CVISHAREDDIR=C:\Program Files (x86)\National Instruments\Shared\CVI
@set CVIPUBDOCSDIR=C:\Users\Public\Documents\National Instruments\CVI
@set CVIPROJPATH=c:\Users\Michael_Harhay\Documents\CICDUtility\Source\UnitTest.prj
@set CVIPROJDIR=c:\Users\Michael_Harhay\Documents\CICDUtility\Source
@set CVIPROJNAME=UnitTest.prj
@set CVITARGETPATH=c:\Users\Michael_Harhay\Documents\CICDUtility\Source\UnitTest.dll
@set CVITARGETDIR=c:\Users\Michael_Harhay\Documents\CICDUtility\Source
@set CVITARGETNAME=UnitTest.dll
@set CVIBUILDCONFIG=Debug
@set CVITARGETFILEVER=1.0.0.0
@set CVITARGETPRODVER=1.0.0.0
@set CVIVXIPNPDIR=C:\Program Files (x86)\IVI Foundation\VISA\winnt
@set CVIIVIDIR=C:\Program Files (x86)\IVI Foundation\IVI
@set CVIWINDIR=C:\WINDOWS
@set CVISYSDIR=C:\WINDOWS\system32
@pushd "c:\Users\Michael_Harhay\Documents\CICDUtility\Source"
@setlocal
@set _some_build_step_failed=0
copy "%CVITARGETPATH%"  "C:\Arxtron\Libraries\"
@if %errorlevel% neq 0 (@set _some_build_step_failed=1)
@exit /b %_some_build_step_failed%
