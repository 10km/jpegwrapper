echo off 
echo make test MinGW project
where gcc
if errorlevel 1 (
	echo MinGW/gcc NOT FOUND.
	exit -1
)
echo MinGW/gcc found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit -1
)
echo cmake found
set sh_folder=%~dp0
pushd %sh_folder%
rem 判断是否能编译64位程序
gcc --version |findstr "sjlj seh"
if errorlevel 1 (
	echo unsupported x86_64 build
	paush 
	exit -1
	)
if exist build.gcc rmdir build.gcc /s/q
mkdir build.gcc
pushd build.gcc
set sh_folder=%sh_folder:\=/%
echo creating x86_64 Project for MinGW ...
cmake -G "MinGW Makefiles" ^
	-DCMAKE_MODULE_PATH=%sh_folder%../cmake/Modules ^
	-DCMAKE_BUILD_TYPE=DEBUG ^
	-DCMAKE_PREFIX_PATH=%sh_folder%../release/jpegwrapper-windows-gcc-x86_64;^%sh_folder%../dependencies/libjpeg-turbo-windows-gcc-x86_64;%sh_folder%../dependencies/openjpeg-windows-gcc-x86_64 ^
	..
	
popd
popd