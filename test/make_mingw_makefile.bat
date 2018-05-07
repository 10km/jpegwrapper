echo off 
echo make test MinGW project
where gcc
if errorlevel 1 (
	echo MinGW/gcc NOT FOUND.
	exit /B -1
)
echo MinGW/gcc found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit /B -1
)
echo cmake found
set sh_folder=%~dp0
pushd %sh_folder%\..
rem 判断是否能编译64位程序
gcc --version |findstr "sjlj seh"
if errorlevel 1 (
	echo unsupported x86_64 build
	paush 
	exit /B -1
	)
if exist test.mingw rmdir test.mingw /s/q
mkdir test.mingw
pushd test.mingw
set sh_folder=%sh_folder:\=/%
echo creating x86_64 Project for MinGW ...
cmake -G "Eclipse CDT4 - MinGW Makefiles" ^
	-DCMAKE_MODULE_PATH=%sh_folder%../cmake/Modules ^
	-DCMAKE_BUILD_TYPE=DEBUG ^
	-DCMAKE_PREFIX_PATH=%sh_folder%../release/jpegwrapper-windows-gcc-x86_64;^%sh_folder%../dependencies/libjpeg-turbo-windows-gcc-x86_64;%sh_folder%../dependencies/openjpeg-windows-gcc-x86_64 ^
	%sh_folder%
	
popd
popd

goto :eof
