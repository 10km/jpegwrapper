echo off 
echo build jpeg-turbo by VS2015
if not defined VS140COMNTOOLS (
	echo vs2015 NOT FOUND.
	exit -1
)
echo vs2015 found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit -1
)
echo cmake found.
where nasm
if errorlevel 1 (
	echo nasm NOT FOUND.
	exit -1
)
echo nasm found.
set sh_folder=%~sdp0

pushd %sh_folder%\libjpeg-turbo-1.4.2

:msvc_x86
echo build x86 
if exist build rmdir build /s/q
mkdir build
cd build
echo make MSVC x86 environment ...
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86

cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86-mt ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%\cmake\compiler_flag_overrides.cmake ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DWITH_CRT_DLL=TRUE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86 ^
	..
nmake clean install
cd ..

:msvc_x86_64
echo build x86_64 
if exist build rmdir build /s/q
mkdir build
cd build
echo make MSVC amd64 environment ...
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64

cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86_64-mt ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%\cmake\compiler_flag_overrides.cmake ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DWITH_CRT_DLL=TRUE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86_64 ^
	..
nmake clean install
cd ..

rmdir build/s/q

popd