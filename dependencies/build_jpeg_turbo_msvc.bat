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
	echo download from http://cmake.org/ ,extract to disk 
	echo add installation path to environment variable PATH
	pause
	exit -1
)
echo cmake found.
where nasm
if errorlevel 1 (
	echo nasm NOT FOUND.
	echo download from https://www.nasm.us/ ,extract to disk 
	echo add installation path to environment variable PATH, version above 2.13.03 required.
	pause
	exit -1
)
echo nasm found.
set sh_folder=%~sdp0

set source_folder=libjpeg-turbo-1.5.90
if not exist %source_folder% (
	echo not found source folder: %source_folder%,please unzip %source_folder%.zip in current folder
	pause
	exit -1
	)
pushd %sh_folder%\%source_folder%
:msvc_x86
echo build x86 
if exist build rmdir build /s/q
mkdir build
cd build
echo make MSVC x86 environment ...
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86

cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86-mt ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DWITH_CRT_DLL=TRUE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86 ..
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
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/libjpeg-turbo-windows-vc-x86_64-mt ..
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