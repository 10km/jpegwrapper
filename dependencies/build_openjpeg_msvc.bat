echo off 
echo build jpegwrapper by VS2015
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

set sh_folder=%~sdp0

set source_folder=openjpeg-version.2.1
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
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/openjpeg-windows-vc-x86-mt ^
	-DBUILD_SHARED_LIBS=OFF ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%\..\cmake\compiler_flag_overrides.cmake ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/openjpeg-windows-vc-x86 ^
	-DBUILD_SHARED_LIBS=OFF ^
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
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/openjpeg-windows-vc-x86_64-mt ^
	-DBUILD_SHARED_LIBS=OFF ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%\..\cmake\compiler_flag_overrides.cmake ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=RELEASE ^
	-DCMAKE_INSTALL_PREFIX=%sh_folder%/release/openjpeg-windows-vc-x86_64 ^
	-DBUILD_SHARED_LIBS=OFF ^
	..
nmake clean install
cd ..
rmdir build/s/q

:end
popd

