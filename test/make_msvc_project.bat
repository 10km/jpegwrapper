echo off 
echo make test VS2015 project
if not defined VS140COMNTOOLS (
	echo vs2015 NOT FOUND.
	exit /B -1
)
echo vs2015 found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit /B -1
)
echo cmake found
set sh_folder=%~dp0
pushd %sh_folder%

rem 需要先编译feature_se
if exist project.vs2015 rmdir project.vs2015 /s/q
mkdir project.vs2015
pushd project.vs2015
if not defined VisualStudioVersion (
	echo make MSVC environment ...
	call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
)
set sh_folder=%sh_folder:\=/%
echo creating x86_64 Project for Visual Studio 2015 ...
cmake -G "Visual Studio 14 2015 Win64" ^
	-DCMAKE_MODULE_PATH=%sh_folder%../cmake/Modules ^
	-DCMAKE_PREFIX_PATH=%sh_folder%../release/jpegwrapper-windows-vc-x86_64-mt;^
%sh_folder%../dependencies/libjpeg-turbo-windows-vc-x86_64-mt;%sh_folder%../dependencies/openjpeg-windows-vc-x86_64-mt ^
	-DWITH_STATIC_CRT=ON ^
	-DWITH_DEBUG_Z7=ON ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%../cmake/compiler_flag_overrides.cmake ^
	..
	
popd
popd