echo off 
echo build jpegwrapper by VS2015
if not defined VS140COMNTOOLS (
	echo vs2015 NOT FOUND.
	pause
	exit -1
)
echo vs2015 found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	pause
	exit -1
)
echo cmake found
set sh_folder=%~dp0
pushd %sh_folder%

call:msvc_x86
call:msvc_x86_64
goto :end

:msvc_x86
echo build x86 
if exist build rmdir build /s/q
mkdir build
pushd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86 ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%cmake\compiler_flag_overrides.cmake ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86-mt ..
nmake install

popd
goto:eof 

:msvc_x86_64
echo build x86_64 
if exist build rmdir build /s/q
mkdir build
pushd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86_64 ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%cmake\compiler_flag_overrides.cmake ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86_64-mt ..
nmake install

popd
rmdir build/s/q
goto:eof 

:end
popd